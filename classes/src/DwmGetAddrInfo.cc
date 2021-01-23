//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmGetAddrInfo.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::GetAddrInfo function implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netdb.h>
}

#include <cstring>

#include "DwmGetAddrInfo.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetAddrInfo(const std::string & hostName,
                   std::set<Ipv4Address> & ip4Addrs,
                   std::set<Ipv6Address> & ip6Addrs,
                   int flags)
  {
    struct  addrinfo   hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags    = flags;
    struct  addrinfo  *addrInfo;

    ip4Addrs.clear();
    ip6Addrs.clear();
    if (getaddrinfo(hostName.c_str(), 0, &hints, &addrInfo) == 0) {
      for (struct addrinfo *res = addrInfo; res; res = res->ai_next) {
        switch (res->ai_family) {
          case AF_INET:
            {
              auto  *inAddr = (const struct sockaddr_in *)res->ai_addr;
              ip4Addrs.insert(Dwm::Ipv4Address(inAddr->sin_addr.s_addr));
            }
            break;
          case AF_INET6:
            {
              auto  *in6Addr = (const struct sockaddr_in6 *)res->ai_addr;
              ip6Addrs.insert(Dwm::Ipv6Address(in6Addr->sin6_addr));
            }
            break;
          default:
            break;
        }
      }
    }
    freeaddrinfo(addrInfo);
    return ((! ip4Addrs.empty()) || (! ip6Addrs.empty()));
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
