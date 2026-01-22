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
//!  \file TestFormatters.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmIpv4Prefix.hh"
#include "DwmFormatters.hh"
#include "DwmUnitAssert.hh"

#if __has_include(<format>)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIpv4()
{
  Dwm::Ipv4Address  ip4_1("127.1.2.3");
  UnitAssert(std::format("{}", ip4_1) == "127.1.2.3");

  in_addr  in_1 = { .s_addr = ip4_1.Raw() };
  UnitAssert(std::format("{}", in_1) == "127.1.2.3");

  sockaddr_in  sockAddr_1;
  sockAddr_1.sin_addr.s_addr = ip4_1.Raw();
  sockAddr_1.sin_port = htons(4321);

  UnitAssert(std::format("{}", sockAddr_1) == "127.1.2.3:4321");

  Dwm::Ipv4Prefix  pfx_1("192.168.0.0/16");
  UnitAssert(std::format("{}", pfx_1) == "192.168.0.0/16");
  
  return;
}

#else

#  if __has_include(<fmt/format.h>)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIpv4()
{
  Dwm::Ipv4Address  ip4_1("127.1.2.3");
  UnitAssert(fmt::format("{}", ip4_1) == "127.1.2.3");

  in_addr  in_1 = { .s_addr = ip4_1.Raw() };
  UnitAssert(fmt::format("{}", in_1) == "127.1.2.3");

  sockaddr_in  sockAddr_1;
  sockAddr_1.sin_addr.s_addr = ip4_1.Raw();
  sockAddr_1.sin_port = htons(4321);

  UnitAssert(fmt::format("{}", sockAddr_1) == "127.1.2.3:4321");

  Dwm::Ipv4Prefix  pfx_1("192.168.0.0/16");
  UnitAssert(fmt::format("{}", pfx_1) == "192.168.0.0/16");
  
  return;
}

#  endif  // __has_include(<fmt/format.h>)

#endif  // __has_include(<format>)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIpv4();
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }
  
  exit(0);

}
