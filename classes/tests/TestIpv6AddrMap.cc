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

#include "DwmIpv6AddrMap.hh"
#include "DwmTimeValue64.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static bool  g_testPerformance = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetEntries(vector<Ipv6Address> & entries)
{
  entries.clear();
  ifstream is("./IPV6_addrs.20210122");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf, 0, 512);
    while (is.getline(buf, 512, '\n')) {
      Ipv6Address  addr(buf);
      entries.push_back(addr);
    }
    is.close();
  }
  return (! entries.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Test(const vector<Ipv6Address> & entries)
{
  Ipv6AddrMap<uint32_t>  addrMap;
  uint32_t  i = 0;
  for (const auto & entry : entries) {
    addrMap.Add(entry, i);
    ++i;
  }
  
  uint32_t  val;
  uint64_t  found = 0;
  i = 0;
  for (const auto & entry : entries) {
    found += addrMap.Find(entry, val);
    UnitAssert(val == i);
    ++i;
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBulkPerformance(const vector<Ipv6Address> & entries)
{
  Ipv6AddrMap<uint32_t>  addrMap;
  uint32_t  i = 0;
  Dwm::TimeValue64  startTime(true);
  auto  ulck(addrMap.UniqueLock());
  for (const auto & entry : entries) {
    addrMap.Add(ulck, entry, i);
    ++i;
  }
  Dwm::TimeValue64  endTime(true);
  endTime -= startTime;
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  insertsPerSec = (i * 1000000ULL) / usecs;
  cout << i << " addresses, " << insertsPerSec
       << " inserts/sec (bulk lock)\n";

  uint32_t  val;
  uint64_t  found = 0;
  startTime.SetNow();
  for (const auto & entry : entries) {
    found += addrMap.Find(ulck, entry, val);
  }
  endTime.SetNow();
  endTime -= startTime;
  UnitAssert(found == entries.size());
  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL) / usecs;
  cout << found << " addresses, " << lookupsPerSec
       << " lookups/sec (bulk lock)\n";

  uint64_t  removals = 0;
  startTime.SetNow();
  for (const auto & entry : entries) {
    removals += addrMap.Remove(ulck, entry);
  }
  endTime.SetNow();
  endTime -= startTime;
  UnitAssert(found == entries.size());
  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  removalsPerSec = (removals * 1000000ULL) / usecs;
  cout << removals << " addresses, " << removalsPerSec
       << " removals/sec (bulk lock)\n";
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestPerformance(const vector<Ipv6Address> & entries)
{
  Ipv6AddrMap<uint32_t>  addrMap;
  uint32_t  i = 0;
  Dwm::TimeValue64  startTime(true);
  for (const auto & entry : entries) {
    addrMap.Add(entry, i);
    ++i;
  }
  Dwm::TimeValue64  endTime(true);
  endTime -= startTime;
  uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  insertsPerSec = (i * 1000000ULL) / usecs;
  cout << i << " addresses, " << insertsPerSec
       << " inserts/sec\n";

  uint32_t  val;
  uint64_t  found = 0;
  startTime.SetNow();
  for (const auto & entry : entries) {
    found += addrMap.Find(entry, val);
  }
  endTime.SetNow();
  endTime -= startTime;
  UnitAssert(found == entries.size());
  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  lookupsPerSec = (found * 1000000ULL) / usecs;
  cout << found << " addresses, " << lookupsPerSec
       << " lookups/sec\n";

  uint64_t  removals = 0;
  startTime.SetNow();
  for (const auto & entry : entries) {
    removals += addrMap.Remove(entry);
  }
  endTime.SetNow();
  endTime -= startTime;
  UnitAssert(found == entries.size());
  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
  uint64_t  removalsPerSec = (removals * 1000000ULL) / usecs;
  cout << removals << " addresses, " << removalsPerSec
       << " removals/sec\n";
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
  
  vector<Ipv6Address>  entries;
  if (UnitAssert(GetEntries(entries))) {
    Test(entries);
    if (g_testPerformance) {
      TestPerformance(entries);
      TestBulkPerformance(entries);
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
