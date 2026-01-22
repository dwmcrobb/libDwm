//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmFormatters.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMFORMATTERS_HH_
#define _DWMFORMATTERS_HH_

extern "C" {
  #include <sys/un.h>
}

#include "DwmIpv4Address.hh"
#include "DwmIpv6Address.hh"

#if __has_include(<format>)

namespace std {

  //--------------------------------------------------------------------------
  //!  sockaddr_in
  //--------------------------------------------------------------------------
  template <>
  struct formatter<sockaddr_in> 
  {
    constexpr auto parse(format_parse_context & ctx) 
    { return ctx.begin(); }
    
    auto format(const sockaddr_in & addr, format_context & ctx) const
    {
      return format_to(ctx.out(), "{}:{}",
                       Dwm::Ipv4Address(addr.sin_addr.s_addr),
                       ntohs(addr.sin_port));
    }
  };

  //--------------------------------------------------------------------------
  //!  sockaddr_un
  //--------------------------------------------------------------------------
  template <>
  struct formatter<sockaddr_un> 
  {
    constexpr auto parse(format_parse_context & ctx) 
    { return ctx.begin(); }
    
    auto format(const sockaddr_un & addr, format_context & ctx) const
    {
      return format_to(ctx.out(), "{}", addr.sun_path);
    }
  };

  //--------------------------------------------------------------------------
  //!  in_addr
  //--------------------------------------------------------------------------
  template <>
  struct formatter<in_addr> 
  {
    constexpr auto parse(format_parse_context & ctx) 
    { return ctx.begin(); }
    
    auto format(const in_addr & addr, format_context & ctx) const
    {
      return format_to(ctx.out(), "{}", Dwm::Ipv4Address(addr.s_addr));
    }
  };

  //--------------------------------------------------------------------------
  //!  in6_addr
  //--------------------------------------------------------------------------
  template <>
  struct formatter<in6_addr> 
  {
    constexpr auto parse(format_parse_context & ctx) 
    { return ctx.begin(); }
    
    auto format(const in6_addr & addr, format_context & ctx) const
    {
      return format_to(ctx.out(), "{}", Dwm::Ipv6Address(addr));
    }
  };

}  // namespace std

#else

#  if __has_include(<fmt/format.h>)

//----------------------------------------------------------------------------
//!  sockaddr_in
//----------------------------------------------------------------------------
template <>
struct fmt::formatter<sockaddr_in> 
{
  constexpr auto parse(fmt::format_parse_context & ctx) 
  { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const sockaddr_in & addr, FormatContext & ctx) 
  {
    return fmt::format_to(ctx.out(), "{}:{}",
                          Dwm::Ipv4Address(addr.sin_addr.s_addr),
                          ntohs(addr.sin_port));
  }
};

//----------------------------------------------------------------------------
//!  sockaddr_un
//----------------------------------------------------------------------------
template <>
struct fmt::formatter<sockaddr_un> 
{
  constexpr auto parse(fmt::format_parse_context & ctx) 
  { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const sockaddr_un & addr, FormatContext & ctx) 
  { return fmt::format_to(ctx.out(), "{}", addr.sun_path); }
};

//----------------------------------------------------------------------------
//!  in_addr
//----------------------------------------------------------------------------
template <>
struct fmt::formatter<in_addr> 
{
  constexpr auto parse(fmt::format_parse_context & ctx) 
  { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const in_addr & addr, FormatContext & ctx) 
  { return fmt::format_to(ctx.out(), "{}", Dwm::Ipv4Address(addr.s_addr)); }
};

//----------------------------------------------------------------------------
//!  in6_addr
//----------------------------------------------------------------------------
template <>
struct fmt::formatter<in6_addr> 
{
  constexpr auto parse(fmt::format_parse_context & ctx) 
  { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const in6_addr & addr, FormatContext & ctx) 
  { return fmt::format_to(ctx.out(), "{}", Dwm::Ipv6Address(addr)); }
};

#  endif  // __has_include(<fmt/format.h>)
  
#endif  // __has_include(<format>)

#endif  // _DWMFORMATTERS_HH_
