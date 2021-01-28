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
//!  \file DwmIpv6PrefixMap.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMIPV6PREFIXMAP_HH_
#define _DWMIPV6PREFIXMAP_HH_

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

#define XXH_INLINE_ALL
#include <xxhash.h>

#include "DwmIpv6Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv6PrefixMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv6PrefixMap()
        : _mtx(), _map(), _lengthCounters()
    {
      _map.max_load_factor(.25);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv6Prefix & pfx, const T & value)
    {
      std::lock_guard<std::mutex>  lck(_mtx);
      _map[pfx] = value;
      _lengthCounters[pfx.MaskLength()]++;
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv6Prefix & pfx, T & value) const
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lck(_mtx);
      auto  it = _map.find(pfx);
      if (it != _map.end()) {
        value = it->second;
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv6Address & addr,
                     std::pair<Ipv6Prefix,T> & value) const
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lck(_mtx);
      for (auto lit = _lengthCounters.rbegin();
           lit != _lengthCounters.rend(); ++lit) {
        Ipv6Prefix  pfx(addr, lit->first);
        auto  it = _map.find(pfx);
        if (it != _map.end()) {
          value.first = pfx;
          value.second = it->second;
          rc = true;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindMatches(const Ipv6Address & addr,
                     std::vector<std::pair<Ipv6Prefix,T>> & values) const
    {
      values.clear();
      std::pair<Ipv6Prefix,T>      value;
      std::lock_guard<std::mutex>  lck(_mtx);
      for (auto lit = _lengthCounters.rbegin();
           lit != _lengthCounters.rend(); ++lit) {
        Ipv6Prefix  pfx(addr, lit->first);
        auto  it = _map.find(pfx);
        if (it != _map.end()) {
          value.first = pfx;
          value.second = it->second;
          values.push_back(value);
        }
      }
      return (! values.empty());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv6Prefix & pfx)
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lck(_mtx);
      auto  it = _map.find(pfx);
      if (it != _map.end()) {
        _map.erase(it);
        auto lit = _lengthCounters.find(pfx.MaskLength());
        lit->second--;
        if (0 == lit->second) {
          _lengthCounters.erase(lit);
        }
      }
      return rc;
    }
    
  private:
    struct OurHash
    {
      inline size_t operator () (const Dwm::Ipv6Prefix & pfx) const
      {
        return (XXH64(pfx.In6Addr().s6_addr, (pfx.MaskLength() + 7) >> 3, 0));
      }
    };
    
    mutable std::mutex                        _mtx;
    std::unordered_map<Ipv6Prefix,T,OurHash>  _map;
    std::map<uint8_t,uint64_t>                _lengthCounters;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV6PREFIXMAP_HH_
