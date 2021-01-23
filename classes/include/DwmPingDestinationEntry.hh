//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008, 2016
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
//!  \file DwmPingDestinationEntry.hh
//!  \brief Dwm::PingDestinationEntry class definition
//---------------------------------------------------------------------------

#ifndef _DWMPINGDESTINATIONENTRY_HH_
#define _DWMPINGDESTINATIONENTRY_HH_

#include <map>

#include "DwmTimeValue.hh"
#include "DwmPingDestination.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Used to cache information about packets whose responses have not
  //!  yet been received.  We keep a map of send times keyed by identifiers.
  //!  The identifier for a destination is incremented each time we send
  //!  a packet to the destination, after which we insert a map entry with
  //!  AddSent().  ProcessReceive() is called when we receive a packet,
  //!  to look up the time at which the request packet was sent and hence
  //!  calculate a round-trip time.
  //--------------------------------------------------------------------------
  class PingSequencesInFlight
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    PingSequencesInFlight();
    
    //------------------------------------------------------------------------
    //!  Sets the destination.
    //------------------------------------------------------------------------
    void Destination(const PingDestination & destination);
    
    //------------------------------------------------------------------------
    //!  Adds to the cache.
    //------------------------------------------------------------------------
    bool AddSent(uint16_t seq, const TimeValue & sendTime);
    
    //------------------------------------------------------------------------
    //!  Process a received packet.
    //------------------------------------------------------------------------
    bool ProcessReceive(uint16_t seq, const TimeValue & recvTime, 
                        TimeValue & rtt);
    
    //------------------------------------------------------------------------
    //!  Returns true if we have sent packets for which we have not yet
    //!  received a response.
    //------------------------------------------------------------------------
    bool Outstanding();

    static uint32_t TimeoutSecs();

    static uint32_t TimeoutSecs(uint32_t secs);
    
  protected:
    static uint32_t                _timeoutSecs;
    PingDestination                _destination;
    std::map<uint16_t,TimeValue>   _sequences;

    void CleanupOld();
  };
  
  //--------------------------------------------------------------------------
  //!  Holds state information for a destination being pinged.
  //--------------------------------------------------------------------------
  class PingDestinationEntry
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    PingDestinationEntry();

    //------------------------------------------------------------------------
    //!  Sets the destination.
    //------------------------------------------------------------------------
    void Destination(const PingDestination & destination);

    //------------------------------------------------------------------------
    //!  Returns a const reference to the contained sequences in flight.
    //------------------------------------------------------------------------
    const PingSequencesInFlight & Sequences() const;

    //------------------------------------------------------------------------
    //!  Returns a mutable reference to the contained sequences in flight.
    //------------------------------------------------------------------------
    PingSequencesInFlight & Sequences();

    //------------------------------------------------------------------------
    //!  Returns the number of packets sent to the destination.
    //------------------------------------------------------------------------
    uint64_t PacketsSent() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the number of packets sent to the destination.
    //------------------------------------------------------------------------
    uint64_t PacketsSent(uint64_t packetsSent);

    //------------------------------------------------------------------------
    //!  Returns the number of packets received from the destination.
    //------------------------------------------------------------------------
    uint64_t PacketsReceived() const;

    //------------------------------------------------------------------------
    //!  Sets and Returns the number of packets received from the destination.
    //------------------------------------------------------------------------
    uint64_t PacketsReceived(uint64_t packetsReceived);

    //------------------------------------------------------------------------
    //!  Returns true if the destination has been primed.
    //!  When using TCP, we 'prime' the destination with a SYN packet.
    //!  This lets us get through weak stateful firewall rules such as those
    //!  typically used with ipfw and pf.
    //------------------------------------------------------------------------
    bool Primed() const;

    //------------------------------------------------------------------------
    //!  Sets whether or not the destination has been primed.
    //!  When using TCP, we 'prime' the destination with a SYN packet.
    //!  This lets us get through weak stateful firewall rules such as those
    //!  typically used with ipfw and pf.
    //------------------------------------------------------------------------
    bool Primed(bool primed);
    
  protected:
    PingDestination          _destination;
    PingSequencesInFlight    _sequences;
    uint64_t                 _packetsSent;
    uint64_t                 _packetsReceived;
    bool                     _primed;
  };
  
}  // namespace Dwm

#endif  // _DWMPINGDESTINATIONENTRY_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
