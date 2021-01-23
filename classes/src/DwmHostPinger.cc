//===========================================================================
// @(#) $Name$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//     endorse or promote products derived from this software without
//     specific prior written permission.
//
//  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
//  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
//  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
//  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
//  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
//  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER
//  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmHostPinger.cc
//!  \brief HostPinger class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in_systm.h>
  #include <netinet/in.h>
  #include <netinet/ip.h>
  #include <errno.h>
  #include <netdb.h>
  #include <arpa/inet.h>
}

#include <cstring>
#include <sstream>

#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4IcmpEchoReply.hh"
#include "DwmIpv4IcmpEchoRequest.hh"
#include "DwmIpv4TcpHeader.hh"
#include "DwmSignal.hh"
#include "DwmStringUtils.hh"
#include "DwmHostPinger.hh"
#include "DwmPacer.hh"

using namespace std;

namespace Dwm {

  HostPinger::Receiver  *HostPinger::_receiver = nullptr;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  HostPinger::HostPinger(uint16_t identifier, const std::string & device)
      : _myAddress(), _device(device), _watcherThread(), _senderThread(),
        _senderDone(false), _watcherReadyMutex(),
        _watcherReadyLock(_watcherReadyMutex, std::defer_lock),
        _watcherReady(), _runSender(false), _runWatcher(false),
        _identifier(identifier), _socket(), _packetRate(1),
        _pcap(), _count(0), _forever(true), _destinations()
  {
    char  myhostname[256];
    memset(myhostname, 0, 256);
    gethostname(myhostname, 255);
    struct addrinfo  hints, *addrResult = 0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(myhostname, nullptr, &hints, &addrResult) == 0) {
      const struct addrinfo  *addr;
      for (addr = addrResult; addr; addr = addr->ai_next) {
        const struct sockaddr_in  *sain =
          (const struct sockaddr_in *)addr->ai_addr;
        if ((sain->sin_addr.s_addr & inet_addr("127.0.0.0")) !=
            inet_addr("127.0.0.0")) {
          _myAddress = sain->sin_addr.s_addr;
          struct in_addr  inAddr;
          inAddr.s_addr = _myAddress.Raw();
          break;
        }
      }
    }
    freeaddrinfo(addrResult);
    struct in_addr  inAddr;
    inAddr.s_addr = _myAddress.Raw();
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  HostPinger::Receiver *HostPinger::GetReceiver()
  {
    return _receiver;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  HostPinger::Receiver *HostPinger::SetReceiver(Receiver *receiver)
  {
    _receiver = receiver;
    return _receiver;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::AddDestination(const PingDestination & destination)
  {
    bool  rc = false;
    map<PingDestination,PingDestinationEntry>::const_iterator  i = 
      _destinations.find(destination);
    if (i == _destinations.end()) {
      _destinations[destination].Destination(destination);
      rc = true;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t HostPinger::PacketRate() const
  {
    return(_packetRate);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t HostPinger::PacketRate(uint16_t pps)
  {
    _packetRate = pps;
    return(_packetRate);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t HostPinger::PacketsSent(const PingDestination & destination) const
  {
    map<PingDestination,PingDestinationEntry>::const_iterator  i = 
      _destinations.find(destination);
    return(i == _destinations.end() ? 0 : i->second.PacketsSent());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t 
  HostPinger::PacketsReceived(const PingDestination & destination) const
  {
    map<PingDestination,PingDestinationEntry>::const_iterator  i =
      _destinations.find(destination);
    return(i == _destinations.end() ? 0 : i->second.PacketsReceived());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  float HostPinger::PacketLoss(const PingDestination & destination) const
  {
    float  pktLoss = 0;
    map<PingDestination,PingDestinationEntry>::const_iterator  i = 
      _destinations.find(destination);
    if (i != _destinations.end()) {
      if (i->second.PacketsSent()) {
        pktLoss = (i->second.PacketsSent() - i->second.PacketsReceived())
          * 100.0 / i->second.PacketsSent();
      }
    }
    return(pktLoss);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::Start(int count)
  {
    bool  rc = false;
    _packetsDesired = count;
    _count = count;
    if (_count > 0) {
      _forever = false;
    }
    if (! _pcap.OpenLive(_device, 128, false, 500)) {
      cerr << "_pcap.OpenLive() failed: " << _pcap.LastError() << endl;
      return(false);
      // _pcap.SetBufferSize(4096*1024);
    }
    
    ostringstream  filter;
    filter << "(tcp port 1)"
           << " or (icmp[4:2] == " << _identifier << ")";
    _pcap.SetFilter(filter.str(), true);
    _runWatcher = true;
    _watcherThread = std::thread(&HostPinger::RunWatcher, this);
    std::mutex  mtx;
    _watcherReadyLock.lock();
    _watcherReady.wait(_watcherReadyLock);
    _watcherReadyLock.unlock();
    if (_socket.Open(PF_INET, SOCK_RAW, IPPROTO_TCP)) {
      int  hdrincl = 1;
      _socket.Setsockopt(IPPROTO_IP, IP_HDRINCL, &hdrincl, sizeof(hdrincl));
      _runSender = true;
      _senderThread = std::thread(&HostPinger::RunSender, this);
      rc = true;
    }
    else {
      StopWatcher();
      cerr << "Failed to open socket: " << strerror(errno) << endl;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::Stop()
  {
    bool  rc = false;
#if 0
    if (StopWatcher()) {
      if (StopSender()) {
        rc = true;
      }
    }
#else
    if (! StopWatcher()) {
      cerr << "Failed to stop watcher\n";
    }
    if (! StopSender()) {
      cerr << "Failed to stop sender\n";
    }
#endif
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::Outstanding() // const
  {
    bool  rc = false;
    for (auto i : _destinations) {
      if (i.second.Sequences().Outstanding()) {
        rc = true;
        break;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::StopWatcher()
  {
    bool  rc = false;
    _runWatcher = false;
    if (_watcherThread.joinable()) {
      _pcap.BreakLoop();
#ifdef __linux__
      //  pcap_breakloop() nor timeouts work on linux (which makes it a crappy
      //  operating system for pcap programs).
      pthread_cancel(_watcherThread.native_handle());
#endif
      _watcherThread.join();
      rc = true;
      _pcap.Close();
    }
    else {
      //  Watcher not running
      rc = true;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::StopSender()
  {
    bool  rc = false;
    _runSender = false;
    if (_senderThread.joinable()) {
      _senderThread.join();
      rc = true;
      if (_socket) {
        _socket.Close();
      }
    }
    else {
      //  Sender not running
      rc = true;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void HostPinger::PcapCallback(uint8_t *user,
                                const struct pcap_pkthdr *pchdr, 
                                const uint8_t *pkt)
  {
    HostPinger  *pinger = (HostPinger *)user;
    if (pchdr->caplen < (sizeof(struct ip) + pinger->_pcap.DataLinkOffset())) {
      cerr << "IP packet too short\n";
      return;
    }
    struct ip  *ipptr = (struct ip *)(pkt + pinger->_pcap.DataLinkOffset());
    Ipv4PacketHeader  ip(ipptr);
    if (ip.Protocol() == IPPROTO_TCP) {
      if (pchdr->caplen <
          ip.HeaderLength() + 20 + pinger->_pcap.DataLinkOffset()) {
        cerr << "TCP packet too short\n";
        return;
      }
      uint8_t  *ptr = (uint8_t *)pkt + pinger->_pcap.DataLinkOffset();
      ptr += ip.HeaderLength();
      Ipv4TcpHeader  tcpHeader((struct tcphdr *)ptr);
      if (tcpHeader.SourcePort() == 1) {
        PingDestination  destination(ip.DestinationAddress(), 
                                     tcpHeader.DestinationPort());
        auto iter = pinger->_destinations.find(destination);
        if (iter == pinger->_destinations.end())
          return;
        //  if the sequence number is 0, it's a TCP SYN we sent to prime
        //  pf and ipfw rules.  Ignore it.
#if 0
        if (tcpHeader.SequenceNumber() == 0)
          return;
#endif
        if (ip.Id() == pinger->_identifier) {
          iter->second.Sequences().AddSent(tcpHeader.SequenceNumber(),
                                           pchdr->ts);
          iter->second.PacketsSent(iter->second.PacketsSent() + 1);
        }
      }
      else if (tcpHeader.DestinationPort() == 1) {
        PingDestination  destination(ip.SourceAddress(), tcpHeader.SourcePort());
        auto  iter = pinger->_destinations.find(destination);
        if (iter == pinger->_destinations.end()) {
          return;
        }
        if (tcpHeader.Flags() & (TH_SYN|TH_ACK)) {
          tcpHeader.SequenceNumber(0);
        }
        
        TimeValue  rtt;
        if (iter->second.Sequences().ProcessReceive(tcpHeader.SequenceNumber(),
                                                    pchdr->ts, rtt)) {
          iter->second.PacketsReceived(iter->second.PacketsReceived() + 1);
        }
      }
    }
    else if (ip.Protocol() == IPPROTO_ICMP) {
      if (pchdr->caplen < ip.HeaderLength() + 16 + pinger->_pcap.DataLinkOffset())
        return;
      uint8_t  *ptr = (uint8_t *)pkt + pinger->_pcap.DataLinkOffset();
      ptr += ip.HeaderLength();
      if (*ptr == 8) {  // ICMP echo request
        PingDestination  destination(ip.DestinationAddress());
        auto  iter = pinger->_destinations.find(destination);
        if (iter == pinger->_destinations.end())
          return;
        uint16_t  icmpLen = 
          pchdr->caplen - (ip.HeaderLength() + pinger->_pcap.DataLinkOffset());
        Ipv4IcmpEchoRequest  echoRequest(ptr, icmpLen);
        if (echoRequest.Identifier() == pinger->_identifier) {
          iter->second.Sequences().AddSent(echoRequest.SequenceNumber(),
                                           pchdr->ts);
          iter->second.PacketsSent(iter->second.PacketsSent() + 1);
        }
      }
      else if (*ptr == 0) {  // ICMP echo reply
        PingDestination  destination(ip.SourceAddress());
        auto  iter = pinger->_destinations.find(destination);
        if (iter == pinger->_destinations.end())  
          return;
        uint16_t  icmpLen =
          pchdr->caplen - (ip.HeaderLength() + pinger->_pcap.DataLinkOffset());
        Ipv4IcmpEchoReply  echoReply(ptr, icmpLen);
        if (echoReply.Identifier() == pinger->_identifier) {
          TimeValue  rtt;
          if (iter->second.Sequences().ProcessReceive(echoReply.SequenceNumber(),
                                                      pchdr->ts, rtt)) {
            iter->second.PacketsReceived(iter->second.PacketsReceived() + 1);
          }
        }
      }
    }
    
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void HostPinger::RunWatcher()
  {
    _watcherReady.notify_all();
    while (_runWatcher) {
      if (_pcap.Dispatch(-1, HostPinger::PcapCallback, (uint8_t *)this) < 0) {
        break;
      }
    }
    return;
  }

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void HostPinger::GetTcpPacket(const PingDestination & destination, 
                              uint16_t seq, string & s)
  {
    Ipv4PacketHeader  packetHeader;
    packetHeader.HeaderLength(20);
    packetHeader.TypeOfService(0);
    packetHeader.TotalLength(40);
    packetHeader.Id(_identifier);
    packetHeader.FragmentOffset(0);
    packetHeader.TimeToLive(250);
    packetHeader.Protocol(6);
    packetHeader.SourceAddress(_myAddress);
    packetHeader.DestinationAddress(destination.Address());
    packetHeader.SetChecksum();
    
    Ipv4TcpHeader  tcpHeader;
    tcpHeader.SourcePort(1);
    tcpHeader.DestinationPort(destination.Port());
    tcpHeader.SequenceNumber(seq);
    tcpHeader.AckNumber(seq);
    tcpHeader.DataOffset(20);
    if (! _destinations[destination].Primed()) {
      tcpHeader.Flags(TH_SYN);
      // tcpHeader.Flags(TH_RST);
      _destinations[destination].Primed(true);
    }
    else {
      tcpHeader.Flags(TH_ACK);
    }
    tcpHeader.Window(1024);
    tcpHeader.SetChecksum(packetHeader, "");
    
    ostringstream  os;
    packetHeader.Write(os);
    tcpHeader.Write(os);
    s = os.str();
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void HostPinger::RunSender()
  {
    _senderDone = false;
    
    Pacer  pacer(_packetRate);
    uint16_t  seqno = 0;
    
    while (_runSender || (_runSender && (_count > 0))) {
      string  packet;
      map<PingDestination,PingDestinationEntry>::iterator  i;
      for (i = _destinations.begin(); 
           i != _destinations.end(); ++i) {
        if (! i->first.SendTo(_socket, _myAddress, _identifier)) {
          cerr << "SendTo(" << i->first << ") failed\n";
        }
        pacer.Pace();
      }
      if (! _forever) {
        --_count;
        if (_count <= 0) {
          int  numWaits = 5;
          while ((! AllPacketsSent()) || Outstanding()) {
            usleep(250000);
            --numWaits;
            if (numWaits <= 0) {
              break;
            }
          }
          _senderDone = true;
          return;
        }
      }
      ++seqno;
    }
    _senderDone = true;
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::SenderDone() const
  {
    return _senderDone;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool HostPinger::AllPacketsSent() const
  {
    bool  rc = true;
    for (auto i = _destinations.begin(); i != _destinations.end(); ++i) {
      if (_packetsDesired > i->second.PacketsSent()) {
        rc = false;
        break;
      }
    }
    return rc;
  }
  
}  // namespace Dwm
