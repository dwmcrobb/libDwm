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
//!  \file DwmPingDestinationEntry.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::PingDestinationEntry class implementation
//---------------------------------------------------------------------------

#include <string>

#include "DwmHostPinger.hh"
#include "DwmPingDestinationEntry.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t  PingSequencesInFlight::_timeoutSecs = 5;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingSequencesInFlight::PingSequencesInFlight()
      : _destination(), _sequences()
  {}
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void PingSequencesInFlight::Destination(const PingDestination & destination)
  {
    _destination = destination;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool
  PingSequencesInFlight::AddSent(uint16_t seq, const TimeValue & sendTime)
  {
    _sequences[seq] = sendTime;
    if (_sequences.size() > 20) {
      CleanupOld();
    }
    return(true);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingSequencesInFlight::ProcessReceive(uint16_t seq,
                                             const TimeValue & recvTime, 
                                             TimeValue & rtt)
  {
    bool  rc = false;
    map<uint16_t,TimeValue>::iterator  i = _sequences.find(seq);
    if (i != _sequences.end()) {
      rtt = recvTime;
      rtt -= i->second;
      if (HostPinger::GetReceiver()) {
        (*HostPinger::GetReceiver())(_destination, recvTime, rtt);
      }
      _sequences.erase(i);
      rc = true;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingSequencesInFlight::Outstanding() // const
  {
    CleanupOld();
    return(! _sequences.empty());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void PingSequencesInFlight::CleanupOld()
  {
    TimeValue  old(time((time_t *)0) - _timeoutSecs, 0);
    
    map<uint16_t,TimeValue>::iterator  i = _sequences.begin();
    while (i != _sequences.end()) {
      if (i->second < old) {
        if (HostPinger::GetReceiver()) {
          (*HostPinger::GetReceiver())(_destination, i->second,
                                       TimeValue(0xFFFFFFFF, 0));
        }
        _sequences.erase(i++);
      }
      else {
        ++i;
      }
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t PingSequencesInFlight::TimeoutSecs()
  {
    return _timeoutSecs;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t PingSequencesInFlight::TimeoutSecs(uint32_t secs)
  {
    _timeoutSecs = secs;
    return _timeoutSecs;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingDestinationEntry::PingDestinationEntry()
      : _destination(), _sequences(), _packetsSent(0), _packetsReceived(0),
        _primed(false)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void PingDestinationEntry::Destination(const PingDestination & destination)
  {
    _destination = destination;
    _sequences.Destination(destination);
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const PingSequencesInFlight & PingDestinationEntry::Sequences() const
  {
    return(_sequences);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  PingSequencesInFlight & PingDestinationEntry::Sequences()
  {
    return(_sequences);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t PingDestinationEntry::PacketsSent() const
  {
    return(_packetsSent);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t PingDestinationEntry::PacketsSent(uint64_t packetsSent)
  {
    _packetsSent = packetsSent;
    return(_packetsSent);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t PingDestinationEntry::PacketsReceived() const
  {
    return(_packetsReceived);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t PingDestinationEntry::PacketsReceived(uint64_t packetsReceived)
  {
    _packetsReceived = packetsReceived;
    return(_packetsReceived);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingDestinationEntry::Primed() const
  {
    return(_primed);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PingDestinationEntry::Primed(bool primed)
  {
    _primed = primed;
    return(_primed);
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
