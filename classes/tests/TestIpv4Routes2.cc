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
//!  \file TestIpv4Routes2.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <errno.h>
  #include <unistd.h>
}

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"
#include "DwmIpv4Routes2.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetEntries(vector<pair<Ipv4Prefix,Ipv4Address>> & entries)
{
  entries.clear();
  ifstream is("./IPV4_prefixes.20210123");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf, 0, 512);
    while (is.getline(buf, 512, '\n')) {
      Ipv4Prefix  pfx(buf);
      entries.push_back({pfx, pfx.Network()});
    }
    is.close();
  }
  return (! entries.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void SimpleTest()
{
  typedef struct {
    const char  *p;
    uint32_t     h;
  } prefix_test_struct;

  const prefix_test_struct prefixes[] = {
    { "24/8",         inet_addr("24.0.0.0") },
    { "24.1/16",      inet_addr("24.1.0.0") },
    { "24.1.1/24",    inet_addr("24.1.1.0") },
    { "24.1.1.1/32",  inet_addr("24.1.1.1") },
    { "64.1/16",      inet_addr("64.1.0.0") },
    { "128.1.1/24",   inet_addr("128.1.1.0") },
    { "192.1.1.1/32", inet_addr("192.1.1.1") }
  };

  Ipv4Routes2<uint32_t>  routes;
  for (uint8_t i = 0; i < 7; ++i) {
    routes[Ipv4Prefix(prefixes[i].p)] = prefixes[i].h;
  }
  
  auto  it = FindRoute(routes, Ipv4Address("24.2.2.1"));
  if (UnitAssert(it != routes.end())) {
    UnitAssert(it->first == Ipv4Prefix("24/8"));
  }
  for (uint8_t i = 0; i < 7; ++i) {
    auto  it = FindRoute(routes, Ipv4Address(prefixes[i].h));
    if (UnitAssert(it != routes.end())) {
      UnitAssert(it->first == Ipv4Prefix(prefixes[i].p));
      UnitAssert(it->second == prefixes[i].h);
    }
    else {
      std::cerr << "failed to find " << Ipv4Address(prefixes[i].h) << '\n';
    }
  }
  it = routes.lower_bound(Ipv4Address("24.255.255.255"));
  if (it != routes.end()) {
    cout << "lower_bound(24.255.255.255): " << it->first << '\n';
  }
  it = routes.upper_bound(Ipv4Address("24.255.255.255"));
  if (it != routes.end()) {
    cout << "upper_bound(24.255.255.255): " << it->first << '\n';
  }
  
  it = FindRoute(routes, Ipv4Address("24.1.255.255"));
  if (UnitAssert(it != routes.end())) {
    UnitAssert(it->first == Ipv4Prefix("24.1/16"));
  }
  it = FindRoute(routes, Ipv4Address("24.1.1.255"));
  if (UnitAssert(it != routes.end())) {
    UnitAssert(it->first == Ipv4Prefix("24.1.1/24"));
  }
  it = FindRoute(routes, Ipv4Address("65.55.45.35"));
  UnitAssert(it == routes.end());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestPerformance()
{
  vector<pair<Ipv4Prefix,Ipv4Address>>  entries;
  if (UnitAssert(GetEntries(entries))) {
    Ipv4Routes2<Ipv4Address>  routes;
    for (const auto & entry : entries) {
      routes[entry.first] = entry.second;
    }
    if (UnitAssert(routes.size() == entries.size())) {
      Dwm::TimeValue  startTime(true);
      uint64_t  found = 0;
      Dwm::Ipv4Address  a("235.235.235.235");
      // Dwm::Ipv4Address  a("1.0.160.241");
#if 1
      for (const auto & entry : entries) {
        found += (FindRoute(routes, entry.second) != routes.end());
        // found += (FindRoute(routes, a) != routes.end());
      }
#endif
      found += (FindRoute(routes, a) != routes.end());
      Dwm::TimeValue  endTime(true);
      endTime -= startTime;
      uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
      uint64_t  lookupsPerSec = (found * 1000000ULL * 10) / usecs;
      cout << routes.size() << " prefixes, " << lookupsPerSec
           << " lookups/sec\n";
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIpv4Routes2()
{
  vector<pair<Ipv4Prefix,Ipv4Address>>  entries;
  if (UnitAssert(GetEntries(entries))) {
    Ipv4Routes2<Ipv4Address>  routes;
    for (const auto & entry : entries) {
      routes[entry.first] = entry.second;
    }
    if (UnitAssert(routes.size() == entries.size())) {
      for (const auto & entry : entries) {
        Ipv4Routes2<Ipv4Address>::const_iterator  it =
          FindRoute(routes, entry.second);
        UnitAssert(it != routes.end());
        UnitAssert(it->first.Contains(entry.second));
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool  testPerformance = false;
  int   optChar;
  while ((optChar = getopt(argc, argv, "p")) != -1) {
    switch (optChar) {
      case 'p':
        testPerformance = true;
        break;
      default:
        break;
    }
  }
  SimpleTest();
  // TestIpv4Routes2();
  if (testPerformance) {
    TestPerformance();
  }
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return 1;
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  return 0;
}
