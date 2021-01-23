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
//!  \file DwmPingDestination.hh
//!  \brief Dwm::PingDestination class definition
//---------------------------------------------------------------------------

#ifndef _DWMPINGDESTINATION_HH_
#define _DWMPINGDESTINATION_HH_

#include <string>

#include "DwmIpv4Address.hh"
#include "DwmSocket.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Common interface for an IPv4 destination we wish to ping with ICMP
  //!  or TCP.
  //--------------------------------------------------------------------------
  class PingDestination
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    PingDestination();

    //------------------------------------------------------------------------
    //!  Construct from a string of the form 'a.b.c.d:n' where a.b.c.d is
    //!  the IP address and n is the port number.  If only an IP address
    //!  is given, we will use ICMP to probe the destination.  If a 
    //!  non-zero port number is given, we will use TCP to probe the 
    //!  destination.
    //------------------------------------------------------------------------
    PingDestination(const std::string & destination);

    //------------------------------------------------------------------------
    //!  Construct from an IP address (for an ICMP destination), or
    //!  an IP address and port number (for a TCP destination).
    //------------------------------------------------------------------------
    PingDestination(const Ipv4Address & addr, uint16_t port = 0);
    
    //------------------------------------------------------------------------
    //!  Returns the destination IP address.
    //------------------------------------------------------------------------
    const Ipv4Address & Address() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the destination IP address.
    //------------------------------------------------------------------------
    const Ipv4Address & Address(const Ipv4Address & address);

    //------------------------------------------------------------------------
    //!  Returns the port number (0 for ICMP).
    //------------------------------------------------------------------------
    uint16_t Port() const;

    //------------------------------------------------------------------------
    //!  Returns the hostname (an empty string if it was not set).
    //------------------------------------------------------------------------
    const std::string & Hostname() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the hostname.
    //------------------------------------------------------------------------
    const std::string & Hostname(const std::string & hostname);
    
    //------------------------------------------------------------------------
    //!  Sends a 'ping' to the destination.  For ICMP, sends an echo
    //!  request.  For TCP, sends a pure ACK.
    //------------------------------------------------------------------------
    bool SendTo(Socket & socket, const Ipv4Address & myAddress,
                uint16_t identifier) const;

    //------------------------------------------------------------------------
    //!  operator <
    //------------------------------------------------------------------------
    bool operator < (const PingDestination & pd) const;

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const PingDestination & pd) const;
    
    //------------------------------------------------------------------------
    //!  Returns the destination as a string (IP address in dotted-decimal
    //!  for an ICMP destination, IP address and port separated by a ':'
    //!  for a TCP destination).
    //------------------------------------------------------------------------
    virtual operator std::string () const;

    //------------------------------------------------------------------------
    //!  Prints a PingDestination to an ostream in human-readable form.
    //------------------------------------------------------------------------
    friend std::ostream & 
    operator << (std::ostream & os, const PingDestination & pd);
    
  protected:
    Ipv4Address       _addr;
    uint16_t          _port;
    std::string       _hostname;
    mutable bool      _primed;
    mutable uint16_t  _sequenceNumber;

    void GetPacket(std::string & packet, const Ipv4Address & myAddress, 
                   uint16_t identifier) const;
    void GetIcmpPacket(std::string & packet, const Ipv4Address & myAddress, 
                       uint16_t identifier) const;
    void GetTcpPacket(std::string & packet, const Ipv4Address & myAddress, 
                      uint16_t identifier) const;
  };
  
}  // namespace Dwm

#endif  // _DWMPINGDESTINATION_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
