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
//!  \file DwmIpv4AddrMap.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4AddrMap class template declaration
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ADDRMAP_HH_
#define _DWMIPV4ADDRMAP_HH_

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "DwmIpv4Address.hh"
#include "DwmReadLockedReference.hh"
#include "DwmWriteLockedReference.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4AddrMap
  {
  public:
    using MapType = std::unordered_map<ipv4addr_t,T>;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv4AddrMap()
        : _mtx(), _map()
    {
      _map.max_load_factor(0.15);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv4Address & addr, const T & value)
    {
      std::unique_lock  lck(_mtx);
      _map[addr.Raw()] = value;
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Address & addr, T & value) const
    {
      std::shared_lock  lck(_mtx);
      auto iter = _map.find(addr.Raw());
      if (iter != _map.end()) {
        value = iter->second;
        return true;
      }
      return false;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv4Address & addr)
    {
      bool  rc = false;
      std::unique_lock  lck(_mtx);
      auto  it = _map.find(addr.Raw());
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
    ReadLockedReference<Ipv4AddrMap<T>,MapType> ReadLockedRef() const
    {
      return ReadLockedReference<Ipv4AddrMap<T>,MapType>(_mtx, _map);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    WriteLockedReference<Ipv4AddrMap<T>,MapType> WriteLockedRef()
    {
      return WriteLockedReference<Ipv4AddrMap<T>,MapType>(_mtx, _map);
    }
    
  private:
    mutable std::shared_mutex  _mtx;
    MapType                    _map;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4ADDRMAP_HH_
