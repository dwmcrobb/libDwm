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
//!  \file TestIpv4PrefixMap.hh
//!  \author Daniel W. McRobb
//!  \brief unit tests for Dwm::Ipv4PrefixMap
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "DwmIpv4PrefixMap.hh"
#include "DwmTimeValue64.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static bool  g_testPerformance = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetEntries(vector<Ipv4Prefix> & entries)
{
  entries.clear();
  ifstream is("./IPV4_prefixes.20210123");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf, 0, 512);
    while (is.getline(buf, 512, '\n')) {
      Ipv4Prefix  pfx(buf);
      entries.push_back(pfx);
    }
    is.close();
  }
  return (! entries.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFind(const vector<Ipv4Prefix> & entries)
{
  Ipv4PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  uint32_t  val;
  i = 0;
  uint64_t  found = 0;
  for (const auto & entry : entries) {
    found += pfxMap.Find(entry, val);
    UnitAssert(val == i);
    ++i;
  }
  UnitAssert(found == entries.size());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBulkFindPerformance(const vector<Ipv4Prefix> & entries)
{
  Ipv4PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  Dwm::TimeValue64  startTime(true);
  auto  ulck = pfxMap.UniqueLock();
  for (const auto & entry : entries) {
    pfxMap.Add(ulck, entry, i);
    ++i;
  }
  Dwm::TimeValue64  endTime(true);
  endTime -= startTime;
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  insertsPerSec = (i * 1000000ULL) / usecs;
  cout << i << " prefixes, " << insertsPerSec
       << " prefix inserts/sec (bulk lock)\n";

  uint32_t  value;
  uint64_t  found = 0;
  startTime.SetNow();
  for (const auto & entry : entries) {
    found += pfxMap.Find(ulck, entry, value);
  }
  endTime.SetNow();
  endTime -= startTime;
  UnitAssert(found == entries.size());
  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL) / usecs;
  cout << found << " prefixes, " << lookupsPerSec
       << " prefix lookups/sec (bulk lock)\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFindPerformance(const vector<Ipv4Prefix> & entries)
{
  Ipv4PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  Dwm::TimeValue64  startTime(true);
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  Dwm::TimeValue64  endTime(true);
  endTime -= startTime;
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  insertsPerSec = (i * 1000000ULL) / usecs;
  cout << i << " prefixes, " << insertsPerSec
       << " prefix inserts/sec\n";

  uint32_t  val;
  uint64_t  found = 0;
  startTime.SetNow();
  for (const auto & entry : entries) {
    found += pfxMap.Find(entry, val);
  }
  endTime.SetNow();
  endTime -= startTime;
  UnitAssert(found == entries.size());
  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL) / usecs;
  cout << found << " prefixes, " << lookupsPerSec
       << " prefix lookups/sec\n";

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLongestMatch(const vector<Ipv4Prefix> & entries)
{
  Ipv4PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  pair<Ipv4Prefix,uint32_t>  val;
  uint64_t  found = 0;
  for (const auto & entry : entries) {
    found += pfxMap.FindLongest(entry.Network(), val);
    UnitAssert(entry.Contains(val.first.Network()));
  }
  UnitAssert(found == entries.size());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLongestMatchPerformance(const vector<Ipv4Prefix> & entries)
{
  Ipv4PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    pfxMap.Add(entry, i);
    ++i;
  }
  pfxMap.Rehash(0);

  pair<Ipv4Prefix,uint32_t>  val;
  uint64_t  found = 0;
  Dwm::TimeValue64  startTime(true);
  for (const auto & entry : entries) {
    found += pfxMap.FindLongest(entry.Network(), val);
  }
  Dwm::TimeValue64  endTime(true);
  endTime -= startTime;
  UnitAssert(found == entries.size());
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL) / usecs;
  cout << found << " addresses, " << lookupsPerSec
       << " longest match lookups/sec\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void
TestBulkLongestMatchPerformance(const vector<Ipv4Prefix> & entries)
{
  Ipv4PrefixMap<uint32_t>  pfxMap;
  uint32_t  i = 0;
  auto  ulck = pfxMap.UniqueLock();
  for (const auto & entry : entries) {
    pfxMap.Add(ulck, entry, i);
    ++i;
  }

  pair<Ipv4Prefix,uint32_t>  val;
  uint64_t  found = 0;
  Dwm::TimeValue64  startTime(true);
  for (const auto & entry : entries) {
    found += pfxMap.FindLongest(ulck, entry.Network(), val);
  }
  Dwm::TimeValue64  endTime(true);
  endTime -= startTime;
  UnitAssert(found == entries.size());
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL) / usecs;
  cout << found << " addresses, " << lookupsPerSec
       << " longest match lookups/sec (bulk lock)\n";
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

  cout.imbue(std::locale(""));

  vector<Ipv4Prefix>  entries;
  if (UnitAssert(GetEntries(entries))) {
    TestFind(entries);
    TestLongestMatch(entries);
    if (g_testPerformance) {
      TestFindPerformance(entries);
      TestLongestMatchPerformance(entries);
      TestBulkFindPerformance(entries);
      TestBulkLongestMatchPerformance(entries);
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
