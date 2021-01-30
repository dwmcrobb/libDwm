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
//!  \file DwmIpv4PrefixMap.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMIPV4PREFIXMAP_HH_
#define _DWMIPV4PREFIXMAP_HH_

#include <cassert>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

// #include "DwmIpv4AddrMap.hh"
#include "DwmIpv4Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  struct OurIpv4PrefixHash
  {
    inline size_t operator () (const Dwm::Ipv4Prefix & pfx) const
    {
      return pfx.NetworkRaw();
    }
  };
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T, typename Hash = OurIpv4PrefixHash>
  class Ipv4PrefixMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    using MapType = std::unordered_map<Ipv4Prefix,T,Hash>;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv4PrefixMap()
        : _mtx(), _map(), _lengthCounters()
    {
      _map.max_load_factor(.15);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv4Prefix & pfx, const T & value)
    {
      std::unique_lock  lck(_mtx);
      return Add(lck, pfx, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(std::unique_lock<std::shared_mutex> & lck,
             const Ipv4Prefix & pfx, const T & value)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      if (_map.insert_or_assign(pfx, value).second) {
        _lengthCounters[pfx.MaskLength()]++;
      }
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & pfx, T & value) const
    {
      std::shared_lock  lck(_mtx);
      return Find(lck, pfx, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(std::shared_lock<std::shared_mutex> & lck,
              const Ipv4Prefix & pfx, T & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindNoLock(pfx, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(std::unique_lock<std::shared_mutex> & lck,
              const Ipv4Prefix & pfx, T & value)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindNoLock(pfx, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(const Ipv4Address & addr,
                     std::pair<Ipv4Prefix,T> & value) const
    {
      std::shared_lock  lck(_mtx);
      return FindLongest(lck, addr, value);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(std::shared_lock<std::shared_mutex> & lck,
                     const Ipv4Address & addr,
                     std::pair<Ipv4Prefix,T> & value) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindLongestNoLock(addr, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongest(std::unique_lock<std::shared_mutex> & lck,
                     const Ipv4Address & addr,
                     std::pair<Ipv4Prefix,T> & value)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindLongestNoLock(addr, value);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindMatches(const Ipv4Address & addr,
                     std::vector<std::pair<Ipv4Prefix,T>> & values) const
    {
      std::shared_lock  lck(_mtx);
      return FindMatches(lck, addr, values);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindMatches(std::shared_lock<std::shared_mutex> & lck,
                     const Ipv4Address & addr,
                     std::vector<std::pair<Ipv4Prefix,T>> & values) const
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindMatchesNoLock(addr, values);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindMatches(std::unique_lock<std::shared_mutex> & lck,
                     const Ipv4Address & addr,
                     std::vector<std::pair<Ipv4Prefix,T>> & values)
    {
      assert((lck.mutex() == &_mtx) && lck.owns_lock());
      return FindMatchesNoLock(addr, values);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv4Prefix & pfx)
    {
      std::unique_lock  lck(_mtx);
      return Remove(lck, pfx);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(std::unique_lock<std::shared_mutex> & lck,
                const Ipv4Prefix & pfx)
    {
      bool  rc = false;
      auto  it = _map.find(pfx);
      if (it != _map.end()) {
        _map.erase(it);
        auto lit = _lengthCounters.find(pfx.MaskLength());
        lit->second--;
        if (0 == lit->second) {
          _lengthCounters.erase(lit);
        }
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Rehash(typename MapType::size_type count)
    {
      _map.rehash(count);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::shared_lock<std::shared_mutex> SharedLock() const
    {
      return std::shared_lock(_mtx);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::unique_lock<std::shared_mutex> UniqueLock()
    {
      return std::unique_lock(_mtx);
    }
    
  private:
    mutable std::shared_mutex    _mtx;
    MapType                      _map;
    std::map<uint8_t,uint64_t>   _lengthCounters;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindNoLock(const Ipv4Prefix & pfx, T & value) const
    {
      bool  rc = false;
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
    bool FindLongestNoLock(const Ipv4Address & addr,
                           std::pair<Ipv4Prefix,T> & value) const
    {
      bool  rc = false;
      Ipv4Prefix  pfx(addr, 32);
      for (auto lit = _lengthCounters.rbegin();
           lit != _lengthCounters.rend(); ++lit) {
        pfx.MaskLength(lit->first);
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
    bool
    FindMatchesNoLock(const Ipv4Address & addr,
                      std::vector<std::pair<Ipv4Prefix,T>> & values) const
    {
      values.clear();
      std::pair<Ipv4Prefix,T>  value;
      for (auto lit = _lengthCounters.rbegin();
           lit != _lengthCounters.rend(); ++lit) {
        Ipv4Prefix  pfx(addr, lit->first);
        auto  it = _map.find(pfx);
        if (it != _map.end()) {
          value.first = pfx;
          value.second = it->second;
          values.push_back(value);
        }
      }
      return (! values.empty());
    }

  };
  
}  // namespace Dwm

#endif  // _DWMIPV4PREFIXMAP_HH_
