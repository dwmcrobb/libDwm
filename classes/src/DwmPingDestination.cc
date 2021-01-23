//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008
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
//!  \file DwmPingDestination.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::PingDestination class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in_systm.h>
  #include <netinet/in.h>
  #include <netinet/ip.h>
  #include <errno.h>
  #include <netdb.h>
  #include <string.h>  // for memset()
}

#include <sstream>
#include <string>

#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4IcmpEchoRequest.hh"
#include "DwmIpv4TcpHeader.hh"
#include "DwmStringUtils.hh"
#include "DwmTimeValue.hh"
#include "DwmPingDestination.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingDestination::PingDestination()
      : _addr(), _port(0), _primed(false), _sequenceNumber(0)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingDestination::PingDestination(const string & destination)
      : _addr(), _port(0), _primed(false), _sequenceNumber(0)
  {
    istringstream  is(destination);
    string         s;
    if (getline(is, s, ':')) {
      _addr = s;
      if (getline(is, s)) {
        uint16_t  port;
        if (StringUtils::StringTo(s, port)) {
          _port = port;
        }
      }
    }
    if (_addr == Ipv4Address("255.255.255.255")) {
      is.clear();
      is.seekg(0, ios::beg);
      string  hostName;
      if (getline(is, hostName, ':')) {
        struct hostent  *hostEntry = gethostbyname(hostName.c_str());
        if (hostEntry) {
          Address(*(ipv4addr_t *)hostEntry->h_addr_list[0]);
          _hostname = hostName;
        }
        else {
          cerr << "host '" << hostName << "' not found" << endl;
        }
      }
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingDestination::PingDestination(const Ipv4Address & addr, uint16_t port)
      : _addr(addr), _port(port), _primed(false), _sequenceNumber(0)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4Address & PingDestination::Address() const
  {
    return(_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4Address & PingDestination::Address(const Ipv4Address & address)
  {
    _addr = address;
    return(_addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t PingDestination::Port() const
  {
    return(_port);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & PingDestination::Hostname() const
  {
    return(_hostname);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & PingDestination::Hostname(const std::string & hostname)
  {
    _hostname = hostname;
    return(_hostname);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingDestination::SendTo(Socket & socket, const Ipv4Address & myAddress,
                               uint16_t identifier) const
  {
    string  packet;
    GetPacket(packet, myAddress, identifier);
    struct sockaddr_in  destination;
    memset(&destination, 0, sizeof(destination));
    destination.sin_family = PF_INET;
#ifndef __linux__
    destination.sin_len = sizeof(struct sockaddr_in);
#endif
    destination.sin_addr.s_addr = _addr.Raw();

    return(socket.SendTo(packet.c_str(), packet.length(), 0, destination));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingDestination::operator < (const PingDestination & pd) const
  {
    return((string)(*this) < (string)(pd));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingDestination::operator == (const PingDestination & pd) const
  {
    return((string)(*this) == (string)(pd));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingDestination::operator string () const
  {
    ostringstream  os;
    os << *this;
    return(os.str());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const PingDestination & pd)
  {
    if (os) {
      os << pd._addr;
      if (pd._port) {
        os << ':' << pd._port;
      }
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void PingDestination::GetPacket(std::string & packet,
                                  const Ipv4Address & myAddress,
                                  uint16_t identifier) const
  {
    if (_port) {
      GetTcpPacket(packet, myAddress, identifier);
    }
    else {
      GetIcmpPacket(packet, myAddress, identifier);
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void PingDestination::GetIcmpPacket(std::string & packet,
                                      const Ipv4Address & myAddress,
                                      uint16_t identifier) const
  {
    Ipv4PacketHeader  packetHeader;
    packetHeader.HeaderLength(20);
    packetHeader.TypeOfService(0);
    packetHeader.Id(identifier);
    packetHeader.FragmentOffset(0);
    packetHeader.TimeToLive(250);
    packetHeader.Protocol(1);
    packetHeader.SourceAddress(myAddress);
    packetHeader.DestinationAddress(_addr);

    Ipv4IcmpEchoRequest  echoRequest;
    echoRequest.Identifier(identifier);
    echoRequest.SequenceNumber(++_sequenceNumber);
    if (! _sequenceNumber) {
      ++_sequenceNumber;  // handle rollover; we only want to send a 0 once.
    }
    echoRequest.SetData(TimeValue(true));
    echoRequest.SetChecksum();
      
    packetHeader.TotalLength(packetHeader.StreamedLength() + 
                             echoRequest.StreamedLength());
    packetHeader.SetChecksum();

    ostringstream  os;
    packetHeader.Write(os);
    echoRequest.Write(os);
    packet = os.str();
    
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void PingDestination::GetTcpPacket(std::string & packet, 
                                     const Ipv4Address & myAddress, 
                                     uint16_t identifier) const
  {
    Ipv4PacketHeader  packetHeader;
    packetHeader.HeaderLength(20);
    packetHeader.TypeOfService(0);
    packetHeader.TotalLength(40);
    packetHeader.Id(identifier);
    packetHeader.FragmentOffset(0);
    packetHeader.TimeToLive(250);
    packetHeader.Protocol(6);
    packetHeader.SourceAddress(myAddress);
    packetHeader.DestinationAddress(_addr);
    packetHeader.SetChecksum();

    Ipv4TcpHeader  tcpHeader;
    tcpHeader.SourcePort(1);
    tcpHeader.DestinationPort(_port);
    if (_sequenceNumber == 0xFFFF) {
      _sequenceNumber = 1;  // handle rollover.  We only want to send 0 once.
    }
    tcpHeader.SequenceNumber(_sequenceNumber);
    tcpHeader.AckNumber(_sequenceNumber);
    ++_sequenceNumber;
    tcpHeader.DataOffset(20);
    if (! _primed) {
      tcpHeader.Flags(TH_SYN);
      _primed = true;
    }
    else {
      tcpHeader.Flags(TH_ACK);
    }
    tcpHeader.Window(1024);
    tcpHeader.SetChecksum(packetHeader, "");

    ostringstream  os;
    packetHeader.Write(os);
    tcpHeader.Write(os);
    packet = os.str();
    return;
  }
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
