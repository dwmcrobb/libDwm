//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmIpv4IcmpDestinationUnreachable.hh
//!  \brief Dwm::Ipv4IcmpDestinationUnreachable class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ICMPDESTINATIONUNREACHABLE_HH_
#define _DWMIPV4ICMPDESTINATIONUNREACHABLE_HH_


extern "C" {
#include <inttypes.h>
}

#include <string>

#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4IcmpMessage.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an ICMP destination unreachable message.
  //--------------------------------------------------------------------------
  class Ipv4IcmpDestinationUnreachable
    : public Ipv4IcmpMessage
  {
  public:
    //------------------------------------------------------------------------
    //!  Known destination unreachable codes
    //------------------------------------------------------------------------
    typedef enum {
      e_networkUnreachable        = 0,
      e_hostUnreachable           = 1,
      e_protocolUnreachable       = 2,
      e_portUnreachable           = 3,
      e_tooBig                    = 4,
      e_sourceRouteFailed         = 5,
      e_networkUnknown            = 6,
      e_hostUnknown               = 7,
      e_sourceHostIsolated        = 8,
      e_networkProhibited         = 9,
      e_hostProhibited            = 10,
      e_tosNetworkUnreachable     = 11,
      e_tosHostUnreachable        = 12,
      e_communicatonProhibited    = 13,
      e_hostPrecedenceViolation   = 14,
      e_precedenceCutoffInEffect  = 15
    } CodeEnum;

    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    Ipv4IcmpDestinationUnreachable();
    
    //------------------------------------------------------------------------
    //!  Constrct from the given @c code, @c nexthopMTU, @c ipHeader and
    //!  @c data.
    //------------------------------------------------------------------------
    Ipv4IcmpDestinationUnreachable(uint8_t code, uint16_t nexthopMTU, 
                                   const Ipv4PacketHeader & ipHeader,
                                   const std::string & data);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv4IcmpDestinationUnreachable(uint8_t *buf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Returns the next hop MTU.
    //------------------------------------------------------------------------
    uint16_t NexthopMTU() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the next hop MTU.
    //------------------------------------------------------------------------
    uint16_t NexthopMTU(uint16_t nexthopMTU);

    //------------------------------------------------------------------------
    //!  Returns a copy of the IPv4 packet header.
    //------------------------------------------------------------------------
    Ipv4PacketHeader IpHeader() const;

    //------------------------------------------------------------------------
    //!  Sets the IPv4 packet header and returns a copy of it.
    //------------------------------------------------------------------------
    Ipv4PacketHeader IpHeader(const Ipv4PacketHeader & ipHeader);

    //------------------------------------------------------------------------
    //!  Returns the data as a string.
    //------------------------------------------------------------------------
    std::string Data() const;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4ICMPDESTINATIONUNREACHABLE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
