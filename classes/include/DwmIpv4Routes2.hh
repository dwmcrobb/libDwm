//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016, 2021
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
//!  \file DwmIpv4Routes2.hh
//!  \brief Dwm::Ipv4Routes2 declaration and functions
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ROUTES2_HH_
#define _DWMIPV4ROUTES2_HH_

#include <algorithm>
#include <cassert>
#include <map>

#include "DwmIpv4Prefix.hh"


namespace Dwm {

  struct Routes2KeyComp {
    using is_transparent = std::true_type;
    
    bool operator () (const Ipv4Prefix & p, const Ipv4Address & a) const
    {
      // std::cout << "Comparing pfx " << p << " < " << a
      //          << " (i.e. " << p << " < " << Ipv4Prefix(a, p.MaskLength())
      //          << ")\n";
      return (p < Ipv4Prefix(a, p.MaskLength()));
      // return (p.Network() < a);
    }
    bool operator () (const Ipv4Prefix & p1, const Ipv4Prefix & p2) const
    {
      // std::cout << "comparing " << p1 << " < " << p2 << '\n';
      return (p1 < p2);
    }
    bool operator () ( const Ipv4Address & a, const Ipv4Prefix & p) const
    {
      // std::cout << "comparing " << a << " < " << p << " (i.e. "
      //          << Ipv4Prefix(a, p.MaskLength()) << " < " << p << ")\n";
      return (Ipv4Prefix(a, p.MaskLength()) < p);
      // return (a < p.Network());
    }
  };

  //--------------------------------------------------------------------------
  //!  Just a std::map keyed by Ipv4Prefix with values of type T.
  //--------------------------------------------------------------------------
  template <typename T>
  using Ipv4Routes2 = std::map<Ipv4Prefix, T, Routes2KeyComp>;

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  typename Ipv4Routes2<T>::iterator
  SearchRoute(Ipv4Routes2<T> & routes, const Ipv4Address & addr)
  {
    typename Ipv4Routes2<T>::iterator  it;
    Ipv4Prefix  pfx(addr, 32);
    for (int i = 32; i > 0; --i) {
      pfx.MaskLength(i);
      it = routes.find(pfx);
      if (it != routes.end()) {
        break;
      }
    }
    return it;
  }
  
  //--------------------------------------------------------------------------
  //!  Find the longest prefix match in @c routes for the given IPv4 address
  //!  @c addr.  Returns an iterator for the matching entry if found, else
  //!  returns routes.end().
  //!
  //!  May seem surprising, but... this is roughly a decimal order of
  //!  magnitude faster than Ipv4Routes::FindLongest().  I'm seeing 65
  //!  million lookups/sec in unit tests on a Threadripper 3960X running
  //!  Ubuntu 20.04 when using 901,114 IPv4 prefixes from routeviews as of
  //!  Jan 21, 2021.  This is single-threaded code.
  //!
  //!  Seems like a trivial amount of code, which it is... it's allowed to
  //!  be trivial due to the Ipv4Prefix class providing the less-than
  //!  operator that enables us to use upper_bound.
  //--------------------------------------------------------------------------
  template <typename T>
  typename Ipv4Routes2<T>::iterator
  FindRoute(Ipv4Routes2<T> & routes, const Ipv4Address & addr)
  {
    return SearchRoute(routes, addr);
    
    auto  eqr = routes.equal_range(addr);
    if (eqr.first != routes.end()) {
      std::cout << "eqr.first(" << addr << "): "
                << eqr.first->first << '\n';
    }
    else {
      std::cout << "eqr.first(" << addr << "): end\n";
    }
    if (eqr.second != routes.end()) {
      std::cout << "eqr.second(" << addr << "): "
                << eqr.second->first << '\n';
    }
    else {
      std::cout << "eqr.second(" << addr << "): end\n";
    }
    
    typename Ipv4Routes2<T>::iterator  lit = routes.lower_bound(addr);
    if (lit != routes.end()) {
      std::cout << "routes.lower_bound(" << addr << "): "
                << lit->first << '\n';
      if (lit != routes.begin()) {
        auto  tmpit = lit;
        --tmpit;
        std::cout << "routes.lower_bound(" << addr << ") - 1: "
                  << tmpit->first << '\n';
      }
    }
    else {
      std::cout << "routes.lower_bound(" << addr << "): end\n";
    }
    typename Ipv4Routes2<T>::iterator  uit = routes.upper_bound(addr);
    if (uit != routes.end()) {
      std::cout << "routes.upper_bound(" << addr << "): "
                << uit->first << '\n';
    }
    else {
      std::cout << "routes.upper_bound(" << addr << "): end\n";
    }

    auto  it = routes.end();
    for ( ; lit != uit; ++lit) {
      if (lit->first.Contains(addr)) {
        std::cout << "matched " << lit->first << " to " << addr << '\n';
        it = lit;
      }
    }
    if (it == routes.end()) {
      if ((uit != routes.end()) && uit->first.Contains(addr)) {
        it = uit;
      }
    }
    
    return it;

#if 0
    if (it != routes.begin()) {
      --it;
    }
    

    typename Ipv4Routes2<T>::iterator  previt = routes.end();
    for ( ; it != routes.begin(); --it) {
      if (it->first.Contains(addr)) {
        break;
      }
      if (previt != routes.end()) {
        if (! it->first.Contains(previt->first.Network())) {
          return SearchRoute(routes, addr);
        }
      }
      previt = it;
    }
    if (it == routes.begin()) {
      if (! it->first.Contains(addr)) {
        it = routes.end();
      }
    }
    return it;
#endif
  }
    
  //--------------------------------------------------------------------------
  //!  Same as above but for const container.
  //--------------------------------------------------------------------------
  template <typename T>
  typename Ipv4Routes2<T>::const_iterator
  FindRoute(const Ipv4Routes2<T> & routes, const Ipv4Address & addr)
  {
    assert(1 == 0);
    return routes.find(addr);

    Ipv4Prefix  pfx(addr, 32);
    auto  it = routes.upper_bound(pfx);
    if (it != routes.begin()) {
      --it;
    }
#if 1
    auto  eit = routes.lower_bound(pfx);
    std::cerr << "eit: " << eit->first << '\n';
    for ( ; it != eit; --it) {
      if (it->first.Contains(addr)) {
        break;
      }
    }
    if (it == eit) {
      if (! it->first.Contains(addr)) {
        it = routes.end();
      }
    }
#else    
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
#endif
    return it;
  }

  
}  // namespace Dwm

#endif  // _DWMIPV4ROUTES2_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
