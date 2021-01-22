//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmHostPinger.hh 9482 $
// @(#) $Id: DwmHostPinger.hh 9482 2017-06-09 05:14:31Z dwm $
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
//!  \file DwmHostPinger.hh
//!  \brief Dwm::HostPinger class definition
//---------------------------------------------------------------------------

#ifndef _DWMHOSTPINGER_HH_
#define _DWMHOSTPINGER_HH_

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include "DwmIpv4Address.hh"
#include "DwmPcap.hh"
#include "DwmSocket.hh"
#include "DwmTimeValue.hh"

#include "DwmPingDestinationEntry.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a 'pinger', which can 'ping' one or more hosts
  //!  using TCP or ICMP.
  //--------------------------------------------------------------------------
  class HostPinger
  {
  public:
    //------------------------------------------------------------------------
    //!  Data callback interface.  @c destination is the destination being
    //!  pinged.  @c recvTime is the time of reception (or the time at which 
    //!  we timed out the ping).  @c rtt is the round trip time.  If rtt.Secs()
    //!  is 0xFFFFFFFF, we did not receive a response to the ping.
    //!  To use this interface, you must derive from this class and implement
    //!  the () operator, then register it via SetReceiver().
    //------------------------------------------------------------------------
    class Receiver
    {
    public:
      //----------------------------------------------------------------------
      //!  Operator ().  Must be implemented in a derived class.
      //----------------------------------------------------------------------
      virtual void operator () (const PingDestination & destination,
                                const TimeValue & recvTime,
                                const TimeValue & rtt) = 0;
    };
    
    //------------------------------------------------------------------------
    //!  Construct with the given @c identifier (normally getpid() & 0xFFFF).
    //------------------------------------------------------------------------
    HostPinger(uint16_t identifier, const std::string & device = "lo0");

    //------------------------------------------------------------------------
    //!  Returns the Receiver registered to receive ping data.
    //------------------------------------------------------------------------
    static Receiver *GetReceiver();
    
    //------------------------------------------------------------------------
    //!  Registers @c receiver to receive ping data.
    //------------------------------------------------------------------------
    static Receiver *SetReceiver(Receiver *receiver);
    
    //------------------------------------------------------------------------
    //!  Adds a destination.  Returns true on success.
    //------------------------------------------------------------------------
    bool AddDestination(const PingDestination & destination);

    //------------------------------------------------------------------------
    //!  Returns the packet rate.  This is divided across all destinations;
    //!  if the packet rate is 10 and there are 5 destinations, each
    //!  destination will get 10/5 == 2 packets/sec.
    //------------------------------------------------------------------------
    uint16_t PacketRate() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the packet rate in packets/sec.  This is divided
    //!  across all destinations; if the packet rate is 10 and there are 5
    //!  destinations, each destination will get 10/5 == 2 packets/sec.
    //------------------------------------------------------------------------
    uint16_t PacketRate(uint16_t pps);

    //------------------------------------------------------------------------
    //!  Returns the number of packets sent to the given @c destination.
    //------------------------------------------------------------------------
    uint32_t PacketsSent(const PingDestination & destination) const;

    //------------------------------------------------------------------------
    //!  Returns the number of packets received from the given @c 
    //!  destination.
    //------------------------------------------------------------------------
    uint32_t PacketsReceived(const PingDestination & destination) const;

    //------------------------------------------------------------------------
    //!  Returns the percent packet loss for the given @c destination.
    //------------------------------------------------------------------------
    float PacketLoss(const PingDestination & destination) const;
    
    //------------------------------------------------------------------------
    //!  Starts the pinger.  Returns true on success.
    //------------------------------------------------------------------------
    bool Start(int count = 0);

    //------------------------------------------------------------------------
    //!  Stops the pinger.  Returns true on success.
    //------------------------------------------------------------------------
    bool Stop();
    
    //------------------------------------------------------------------------
    //!  Returns true if we have not yet received replies for some of the
    //!  packets we sent.
    //------------------------------------------------------------------------
    bool Outstanding() /* const */;

    bool SenderDone() const;
    bool StopSender();

  protected:
    typedef std::map<PingDestination,PingDestinationEntry>  DestinationMap;

    std::string                   _device;
    Ipv4Address                   _myAddress;
    std::thread                   _watcherThread;
    std::thread                   _senderThread;
    std::atomic<bool>             _senderDone;
    std::mutex                    _watcherReadyMutex;
    std::unique_lock<std::mutex>  _watcherReadyLock;
    std::condition_variable       _watcherReady;
    std::atomic<bool>             _runSender;
    std::atomic<bool>             _runWatcher;
    uint16_t                      _identifier;
    Socket                        _socket;
    uint16_t                      _packetRate;
    Pcap                          _pcap;
    int                           _count;
    int                           _packetsDesired;
    bool                          _forever;
    DestinationMap                _destinations;
    static Receiver              *_receiver;
    
    void GetTcpPacket(const PingDestination & destination,
                      uint16_t seq, std::string & s);
    
    static void PcapCallback(uint8_t *user, const struct pcap_pkthdr *pchdr, 
                             const uint8_t *pkt);
    void RunWatcher();
    void RunSender();
    bool StopWatcher();
    bool AllPacketsSent() const;
  };
  
}  // namespace Dwm

#endif  // _DWMHOSTPINGER_HH_
