//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmGetAddrInfo.hh 9936 $
// @(#) $Id: DwmGetAddrInfo.hh 9936 2018-01-05 21:43:42Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2017
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
//!  \file DwmGetAddrInfo.hh
//!  \brief Dwm::GetAddrInfo function definition
//---------------------------------------------------------------------------

#ifndef _DWMGETADDRINFO_HH_
#define _DWMGETADDRINFO_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netdb.h>
}

#include <set>
#include <string>

#include "DwmIpv4Address.hh"
#include "DwmIpv6Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetAddrInfo(const std::string & hostName,
                   std::set<Ipv4Address> & ip4Addrs,
                   std::set<Ipv6Address> & ip6Addrs,
                   int flags = AI_ADDRCONFIG);
  
}  // namespace Dwm

#endif  // _DWMGETADDRINFO_HH_
