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

#include <map>
#include <vector>

#include "DwmIpv4AddrMap.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmReadLockedReference.hh"
#include "DwmWriteLockedReference.hh"

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
      _map.max_load_factor(.25);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv4Prefix & pfx, const T & value)
    {
      std::unique_lock  lck(_mtx);
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
      bool  rc = false;
      std::shared_lock  lck(_mtx);
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
    bool FindLongest(const Ipv4Address & addr,
                     std::pair<Ipv4Prefix,T> & value) const
    {
      bool  rc = false;
      Ipv4Prefix  pfx(addr, 32);
      std::shared_lock  lck(_mtx);
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
    bool FindMatches(const Ipv4Address & addr,
                     std::vector<std::pair<Ipv4Prefix,T>> & values) const
    {
      values.clear();
      std::pair<Ipv4Prefix,T>  value;
      std::shared_lock  lck(_mtx);
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
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv4Prefix & pfx)
    {
      bool  rc = false;
      std::unique_lock  lck(_mtx);
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
    ReadLockedReference<Ipv4PrefixMap<T>,MapType> ReadLockedRef() const
    {
      return ReadLockedReference<Ipv4PrefixMap<T>,MapType>(_mtx, _map);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    WriteLockedReference<Ipv4PrefixMap<T>,MapType> WriteLockedRef()
    {
      return WriteLockedReference<Ipv4PrefixMap<T>,MapType>(_mtx, _map);
    }
    

  private:
    mutable std::shared_mutex    _mtx;
    MapType                      _map;
    std::map<uint8_t,uint64_t>   _lengthCounters;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4PREFIXMAP_HH_
