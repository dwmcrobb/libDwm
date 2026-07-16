//===========================================================================
//  Copyright (c) Daniel W. McRobb 2026
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
//!  @file DwmIpv6Routes.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::Ipv6Routes class template
//---------------------------------------------------------------------------

#ifndef _DWMIPV6ROUTES_HH_
#define _DWMIPV6ROUTES_HH_

#include <map>
#include <unordered_map>

#include "DwmIpv6Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  struct Ipv6RoutesPrefixHash
  {
    inline size_t operator () (const Dwm::Ipv6Prefix & pfx) const
    {
      return pfx.Hash();
    }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename ValueType>
  class Ipv6Routes
  {
  public:
    using  value_type = std::pair<Ipv6Prefix,ValueType>;
    using  hash_iterator = std::unordered_map<Ipv6Prefix,ValueType,Ipv6RoutesPrefixHash>::iterator;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ValueType & operator [] (const Ipv6Prefix & pfx)
    {
      return _hashes[pfx.MaskLength()][pfx];
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void clear()
    { _hashes.clear(); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool empty() const
    {
      bool  rc = true;
      for (const auto & h : _hashes) {
        if (! h.second.empty()) {
          rc = false;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    size_t size() const
    {
      size_t  rc = 0;
      for (const auto & h : _hashes) {
        rc += h.second.size();
      }
      return rc;
    }
#if 0
    find_longest()
    {}
#endif
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool FindLongestMatch(const Ipv6Address & addr, value_type & result) const
    {
      bool  rc = false;
      auto  pfx = Ipv6Prefix(addr, 128);
      for (auto it = _hashes.crbegin(); it != _hashes.crend(); ++it) {
        pfx.MaskLength(it->first);
        auto fit = it->second.find(pfx);
        if (fit != it->second.end()) {
          result = *fit;
          rc = true;
          break;
        }
      }
      return rc;
    }
    
  private:
    std::map<uint8_t,std::unordered_map<Ipv6Prefix,ValueType,Ipv6RoutesPrefixHash>>  _hashes;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV6ROUTES_HH_
