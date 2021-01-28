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
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ADDRMAP_HH_
#define _DWMIPV4ADDRMAP_HH_

#include <iostream>
#include <mutex>
#include <unordered_map>

#include "DwmIpv4Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4AddrMap
  {
  public:
    
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
      std::lock_guard<std::mutex>  lck(_mtx);
      _map[addr.Raw()] = value;
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Address & addr, T & value) const
    {
      std::lock_guard<std::mutex>  lck(_mtx);
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
      std::lock_guard<std::mutex>  lck(_mtx);
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
      std::lock_guard<std::mutex>  lck(_mtx);
      return _map.empty();
    }
    
  private:
    mutable std::mutex                _mtx;
    std::unordered_map<ipv4addr_t,T>  _map;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4ADDRMAP_HH_
