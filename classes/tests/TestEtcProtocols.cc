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
//!  @file TestEtcProtocols.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::EtcProtocols unit tests
//---------------------------------------------------------------------------

#include "DwmEtcProtocols.hh"
#include "DwmUnitAssert.hh"

//----------------------------------------------------------------------------
static void TestEntry(const Dwm::EtcProtocols & ep,
                      const std::string & name, int number,
                      const std::vector<std::string> & aliases)
{
  auto  ebyname = ep.GetEntry(name);
  if (UnitAssert(ebyname)) {
    auto  ebynum = ep.GetEntry(number);
    if (UnitAssert(ebynum)) {
      UnitAssert(*ebynum == *ebyname);
      for (const auto & a : aliases) {
        UnitAssert(std::find(ebynum->Aliases().begin(),
                             ebynum->Aliases().end(), a)
                   != ebynum->Aliases().end());
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::EtcProtocols  ep;
  UnitAssert(ep.NumEntries() > 0);

  TestEntry(ep, "ip",   0,   {"IP"});
  TestEntry(ep, "icmp", 1,   {"ICMP"});
  TestEntry(ep, "tcp",  6,   {"TCP"});
  TestEntry(ep, "udp",  17,  {"UDP"});
  TestEntry(ep, "vrrp", 112, {});
  
  if (Dwm::Assertions::Total().Failed()) {
    Dwm::Assertions::Print(std::cerr, true);
    return 1;
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed\n";
    return 0;
  }
}

