//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2021
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
//!  \file DwmIpv6AddrMap.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMIPV6ADDRMAP_HH_
#define _DWMIPV6ADDRMAP_HH_

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "DwmIpv6Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  struct OurIpv6AddressHash
  {
    inline size_t operator () (const Dwm::Ipv6Address & addr) const
    {
      return addr.Hash();
    }
  };

  //--------------------------------------------------------------------------
  //!  A thin wrapper around std::unordered_map<Ipv6Address,T,Hash>.  Mainly
  //!  provides thread safety (shared and unique locks).
  //--------------------------------------------------------------------------
  template <typename T, typename Hash = OurIpv6AddressHash>
  class Ipv6AddrMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv6AddrMap()
        : _mtx(), _map()
    {
      _map.max_load_factor(0.25);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv6Address & addr, const T & value)
    {
      std::unique_lock  lck(_mtx);
      _map[addr] = value;
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv6Address & addr, T & value) const
    {
      std::shared_lock  lck(_mtx);
      auto iter = _map.find(addr);
      if (iter != _map.end()) {
        value = iter->second;
        return true;
      }
      return false;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv6Address & addr)
    {
      bool  rc = false;
      std::unique_lock  lck(_mtx);
      auto  it = _map.find(addr);
      if (it != _map.end()) {
        _map.erase(it);
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Empty() const
    {
      std::shared_lock  lck(_mtx);
      return _map.empty();
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Clear()
    {
      std::unique_lock  lck(_mtx);
      _map.clear();
      return;
    }
    
  private:
    mutable std::shared_mutex               _mtx;
    std::unordered_map<Ipv6Address,T,Hash>  _map;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV6ADDRMAP_HH_
