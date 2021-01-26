//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1999-2005, 2016
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
//!  \file DwmIpv6Routes2.hh
//!  \brief Dwm::Ipv6Routes2 class template definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV6ROUTES2_HH_
#define _DWMIPV6ROUTES2_HH_

extern "C" {
#include <assert.h>
}

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "DwmPortability.hh"
#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmIpv6Prefix.hh"
#include "DwmOperators.hh"

namespace Dwm {

  template <typename T>
  using Ipv6Routes2 = std::map<Ipv6Prefix, T>;

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  typename Ipv6Routes2<T>::const_iterator
  FindRoute(const Ipv6Routes2<T> & routes, const Ipv6Address & addr)
  {
    Ipv6Prefix  pfx(addr, 128);
    auto  it = routes.upper_bound(pfx);
    if (it != routes.begin()) {
      --it;
    }
    for ( ; it != routes.begin(); --it) {
      if (it->first.Contains(addr)) {
        break;
      }
    }
    if (it == routes.begin()) {
      if (! it->first.Contains(addr)) {
        it = routes.end();
      }
    }
    return it;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  typename Ipv6Routes2<T>::iterator
  FindRoute(Ipv6Routes2<T> & routes, const Ipv6Address & addr)
  {
    Ipv6Prefix  pfx(addr, 128);
    auto  it = routes.upper_bound(pfx);
    if (it != routes.begin()) {
      --it;
    }
    for ( ; it != routes.begin(); --it) {
      if (it->first.Contains(addr)) {
        break;
      }
    }
    if (it == routes.begin()) {
      if (! it->first.Contains(addr)) {
        it = routes.end();
      }
    }
    return it;
  }
  
  
}  // namespace Dwm

#endif  // _DWMIPV6ROUTES2_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
