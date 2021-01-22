//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmIpv4IcmpRedirect.hh 8401 $
// @(#) $Id: DwmIpv4IcmpRedirect.hh 8401 2016-04-17 06:44:31Z dwm $
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
//!  \file DwmIpv4IcmpRedirect.hh
//!  \brief Dwm::Ipv4IcmpRedirect class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ICMPREDIRECT_HH_
#define _DWMIPV4ICMPREDIRECT_HH_

extern "C" {
#include <inttypes.h>
}

#include <string>

#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4IcmpMessage.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an ICMP source quench message.  See RFC792, RFC1016 and
  //!  RFC1122.
  //--------------------------------------------------------------------------
  class Ipv4IcmpRedirect
    : public Ipv4IcmpMessage
  {
  public:
    //------------------------------------------------------------------------
    //!  Known redirect codes.
    //------------------------------------------------------------------------
    typedef enum {
      e_networkRedirect    = 0,
      e_hostRedirect       = 1,
      e_tosNetworkRedirect  = 2,
      e_tosHostRedirect    = 3
    } CodeEnum;

    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Ipv4IcmpRedirect();
    
    //------------------------------------------------------------------------
    //!  Construct from a redirect IP address, IP header and data.
    //------------------------------------------------------------------------
    Ipv4IcmpRedirect(uint8_t code, const Ipv4Address & ipAddr,
                     const Ipv4PacketHeader & ipHeader,
                     const std::string & data);

    //------------------------------------------------------------------------
    //!  Construct from a pointer to an ICMP redirect message inside
    //!  an existing IP packet, and the length of the message.  We don't
    //!  deep copy, so @c buf must be around until the new 
    //!  Ipv4IcmpRedirect object is no longer needed.
    //------------------------------------------------------------------------
    Ipv4IcmpRedirect(uint8_t *buf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Returns a copy of the contained redirect IP address.
    //------------------------------------------------------------------------
    Ipv4Address IpAddr() const;

    //------------------------------------------------------------------------
    //!  Sets the contained redirect IP address.
    //------------------------------------------------------------------------
    Ipv4Address IpAddr(const Ipv4Address & ipAddr);
    
    //------------------------------------------------------------------------
    //!  Returns a copy of the contained IP header.
    //------------------------------------------------------------------------
    Ipv4PacketHeader IpHeader() const;

    //------------------------------------------------------------------------
    //!  Sets the contained IP header.
    //------------------------------------------------------------------------
    Ipv4PacketHeader IpHeader(const Ipv4PacketHeader & ipHeader);

    //------------------------------------------------------------------------
    //!  Returns the contained data beyond the contained IP header.
    //------------------------------------------------------------------------
    std::string Data() const;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4ICMPREDIRECT_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
