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
//!  \file TestIpv6AddrMap.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "DwmIpv6PrefixMap.hh"
#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

bool  g_testPerformance = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetEntries(vector<Ipv6Prefix> & entries)
{
  entries.clear();
  ifstream is("./IPV6_prefixes.20210122");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf, 0, 512);
    while (is.getline(buf, 512, '\n')) {
      Ipv6Prefix  pfx(buf);
      entries.push_back(pfx);
    }
    is.close();
  }
  return (! entries.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFind(const vector<Ipv6Prefix> & entries)
{
  Ipv6PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  uint32_t  val;
  uint64_t  found = 0;
  i = 0;
  for (const auto & entry : entries) {
    if (UnitAssert(pfxMap.Find(entry, val))) {
      UnitAssert(val == i);
      ++found;
    }
    ++i;
  }
  UnitAssert(found == entries.size());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLongestMatch(const vector<Ipv6Prefix> & entries)
{
  Ipv6PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  pair<Ipv6Prefix,uint32_t>  val;
  uint64_t  found = 0;
  for (const auto & entry : entries) {
    if (UnitAssert(pfxMap.FindLongest(entry.Network(), val))) {
      UnitAssert(entry.Contains(val.first.Network()));
      ++found;
    }
  }
  UnitAssert(found == entries.size());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLongestMatchPerformance(const vector<Ipv6Prefix> & entries)
{
  Ipv6PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  pair<Ipv6Prefix,uint32_t>  val;
  uint64_t  found = 0;
  Dwm::TimeValue  startTime(true);
  for (const auto & entry : entries) {
    found += pfxMap.FindLongest(entry.Network(), val);
  }
  Dwm::TimeValue  endTime(true);
  endTime -= startTime;
  UnitAssert(found == entries.size());
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL * 10) / usecs;
  cout << found << " addresses, " << lookupsPerSec
       << " longest match lookups/sec\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int  optChar;
  while ((optChar = getopt(argc, argv, "p")) != -1) {
    switch (optChar) {
      case 'p':
        g_testPerformance = true;
        break;
      default:
        break;
    }
  }
  
  vector<Ipv6Prefix>  entries;
  if (UnitAssert(GetEntries(entries))) {
    TestFind(entries);
    TestLongestMatch(entries);
    if (g_testPerformance) {
      TestLongestMatchPerformance(entries);
    }
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
