//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2025
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
//!  \file DwmEndianness.hh
//!  \author Daniel W. McRobb
//!  \brief Endian conversions: host to/fron big endian
//---------------------------------------------------------------------------

#ifndef _DWMENDIANNESS_HH_
#define _DWMENDIANNESS_HH_

#include "DwmPortability.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsEndianSensitiveInteger =
    std::is_integral_v<T>
    and ((sizeof(T) == 2) or (sizeof(T) == 4) or (sizeof(T) == 8));
  
  //--------------------------------------------------------------------------
  //!  Returns the host endian conversion of the big endian @c t.
  //--------------------------------------------------------------------------
  template <typename T>
  requires IsEndianSensitiveInteger<T>
  [[nodiscard]] inline auto BE2Host(T t) -> T
  {
    if constexpr (sizeof(t) == 2)      { return be16toh(t); }
    else if constexpr (sizeof(t) == 4) { return be32toh(t); }
    else if constexpr (sizeof(t) == 8) { return be64toh(t); }
  }
  
  //--------------------------------------------------------------------------
  //!  Returns the big endian conversion of the host endian @c t.
  //--------------------------------------------------------------------------
  template <typename T>
  requires IsEndianSensitiveInteger<T>
  [[nodiscard]] inline auto Host2BE(T t) -> T
  {
    if constexpr (sizeof(t) == 2)      { return htobe16(t); }
    else if constexpr (sizeof(t) == 4) { return htobe32(t); }
    else if constexpr (sizeof(t) == 8) { return htobe64(t); }
  }

  //--------------------------------------------------------------------------
  //!  Converts all @c args from big endian to host endian, in place.
  //--------------------------------------------------------------------------
  template <typename ...Args>
  requires (IsEndianSensitiveInteger<Args> and ...)
  void BE2Host(Args & ...args)
  {
    auto  be2h = [&] (auto & f) -> void { f = BE2Host(f); };
    return ( be2h(args), ...);
  }

  //--------------------------------------------------------------------------
  //!  Converts all @c args from host endian to big endian, in place.
  //--------------------------------------------------------------------------
  template <typename ...Args>
  requires (IsEndianSensitiveInteger<Args> and ...)
  void Host2BE(Args & ...args)
  {
    auto  h2be = [&] (auto & f) -> void { f = Host2BE(f); };
    return ( h2be(args), ...);
  }
  
}  // namespace Dwm

#endif  // _DWMENDIANNESS_HH_
