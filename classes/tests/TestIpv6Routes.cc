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
//!  @file TestIpv6Routes.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>
#include <utility>

#include "DwmIpv6Routes.hh"
#include "DwmOptArgs.hh"
#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static bool    g_performanceTests = false;
static string  g_myDir;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void SetMyDir(const char *argv0)
{
  namespace  fs = std::filesystem;
  
  g_myDir = fs::path(argv0).parent_path();
  if (fs::path(g_myDir).filename() == ".libs") {
    g_myDir = fs::path(g_myDir).parent_path();
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool Load(vector<pair<Ipv6Prefix,string>> & vec, Ipv6Routes<string> & routes)
{
  vec.clear();
  routes.clear();

  ifstream  is(g_myDir + "/IPV6_prefixes.20260709");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv6Prefix  pfx(buf);
      routes[pfx] = buf;
      vec.push_back({pfx,buf});
      memset(buf,0,512);
    }
    is.close();
    UnitAssert(! routes.empty());
    UnitAssert(vec.size() == routes.size());
    for (const auto & pfx : vec) {
    }
  }
  return (! routes.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestLongestMatch(const vector<pair<Ipv6Prefix,string>> & vec,
                      const Ipv6Routes<string> & routes)
{
  pair<Ipv6Prefix,string>  match;
  
  for (const auto & v : vec) {
    auto  addr = v.first.Network();
    if (UnitAssert(routes.FindLongestMatch(addr, match))) {
      UnitAssert(match.first.Contains(addr));
      UnitAssert(match.first.Contains(v.first.Network()));
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestLongestMatchPerformance(const vector<pair<Ipv6Prefix,string>> & vec,
                                 const Ipv6Routes<string> & routes)
{
  vector<Ipv6Address>      addrs;
  pair<Ipv6Prefix,string>  match;
  size_t                   rc = 0;
  for (const auto & v : vec) {
    addrs.push_back(v.first.Network());
  }
  Dwm::TimeValue           startTime(true);
  for (int i = 0; i < 5; ++i) {
    for (const auto & a : addrs) {
      rc += routes.FindLongestMatch(a, match);
    }
  }
  Dwm::TimeValue  endTime(true);
  endTime -= startTime;
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (5 * vec.size() * 1000000ULL) / usecs;
  cout << vec.size() << " prefixes, " << lookupsPerSec
       << " string lookups/sec (iterator)" << endl;
  
  UnitAssert((routes.size() * 5) == rc);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestFromPrefixFile()
{
  vector<pair<Ipv6Prefix,string>>  vec;
  Ipv6Routes<string>  routes;
  if (UnitAssert(Load(vec, routes))) {
    if (g_performanceTests) {
      TestLongestMatchPerformance(vec, routes);    
    }
    else {
      TestLongestMatch(vec, routes);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  OptArgs  optargs;
  optargs.AddOptArg("p", "performance", false, "false", 
                    "run performance tests");
  optargs.Parse(argc, argv);
  g_performanceTests = optargs.Get<bool>('p');
  SetMyDir(argv[0]);

  Ipv6Routes<string>  routes;
  routes[Ipv6Prefix("fd60:3019:f4a:6aaf::/64")] = "fd60:3019:f4a:6aaf::/64";
  Ipv6Routes<string>::value_type  match;
  if (UnitAssert(routes.FindLongestMatch(Ipv6Address("fd60:3019:f4a:6aaf::39"),
                                         match))) {
    UnitAssert(match.first == Ipv6Prefix("fd60:3019:f4a:6aaf::/64"));
    UnitAssert(match.second == "fd60:3019:f4a:6aaf::/64");
  }

  TestFromPrefixFile();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
