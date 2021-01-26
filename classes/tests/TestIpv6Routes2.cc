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
//!  \file TestIpv6RoutesList.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <algorithm>
#include <cstring>
#include <execution>
#include <fstream>
#include <iostream>

#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"
#include "DwmIpv6Routes2.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetEntries(vector<pair<Ipv6Prefix,Ipv6Address>> & entries)
{
  entries.clear();
  ifstream is("./IPV6_prefixes.20210122");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf, 0, 512);
    while (is.getline(buf, 512, '\n')) {
      Ipv6Prefix  pfx(buf);
      entries.push_back({pfx, pfx.Network()});
    }
    is.close();
  }
  return (! entries.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIpv6Routes2()
{
  vector<pair<Ipv6Prefix,Ipv6Address>>  entries;
  if (UnitAssert(GetEntries(entries))) {
    Ipv6Routes2<Ipv6Address>  routes;
    for (const auto & entry : entries) {
      routes[entry.first] = entry.second;
    }
    if (UnitAssert(routes.size() == entries.size())) {
      Dwm::TimeValue  startTime(true);
      for (const auto & entry : entries) {
        // UnitAssert(routes.find(entry.first) != routes.end());
        auto  it = FindRoute(routes, entry.second);
#if 0
        UnitAssert(it != routes.end());
        UnitAssert(it->first.Contains(entry.second));
#endif
      }
      Dwm::TimeValue  endTime(true);
      endTime -= startTime;
      uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
      uint64_t  lookupsPerSec =
        (routes.size() * 1000000ULL * 10) / usecs;
      cout << routes.size() << " prefixes, " << lookupsPerSec
           << " lookups/sec\n";
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIpv6Routes2();

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return 1;
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  return 0;
}

  
