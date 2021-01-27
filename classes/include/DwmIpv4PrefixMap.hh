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

#include "DwmIpv4AddrMap.hh"
#include "DwmIpv4Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  class Ipv4PrefixMap
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Ipv4PrefixMap()
        : _mtx(), _map()
    {}
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Add(const Ipv4Prefix & pfx, const T & value)
    {
      std::lock_guard<std::mutex>  lck(_mtx);
      _map[pfx.MaskLength()].Add(pfx.Network(), value);
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Find(const Ipv4Prefix & pfx, T & value) const
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lck(_mtx);
      auto  it = _map.find(pfx.MaskLength());
      if (it != _map.end()) {
        rc = it->second.Find(pfx.Network(), value);
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
      std::lock_guard<std::mutex>  lck(_mtx);
      for (auto it = _map.rbegin(); it != _map.rend(); ++it) {
        Ipv4Prefix  pfx(addr, it->first);
        if (it->second.Find(pfx.Network(), value.second)) {
          value.first = pfx;
          rc = true;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Remove(const Ipv4Prefix & pfx)
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lck(_mtx);
      auto  it = _map.find(pfx.MaskLength());
      if (it != _map.end()) {
        rc = it->second.Remove(pfx.Network());
        if (it->second.Empty()) {
          _map.erase(it);
        }
      }
      return rc;
    }
    
  private:
    mutable std::mutex                _mtx;
    std::map<uint8_t,Ipv4AddrMap<T>>  _map;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4PREFIXMAP_HH_
