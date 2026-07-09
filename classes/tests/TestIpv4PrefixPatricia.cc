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
//!  @file TestIpv4PrefixPatricia.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>
#include <iostream>

#include "DwmIpv4PrefixPatricia.hh"
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
static void TestFindLongest()
{
  Ipv4PrefixPatricia<string>  r;
  vector<Ipv4Prefix>   pfxVec;
  ifstream is(g_myDir + "/IPV4_prefixes.20210123");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv4Prefix  pfx(buf);
      r.Add(pfx, buf);
      pfxVec.push_back(pfx);
      memset(buf,0,512);
    }
    is.close();
    for (const auto & pfx : pfxVec) {
      auto  it = r.find_longest(pfx);
      if (UnitAssert(it != r.end())) {
        UnitAssert(it->first == pfx);
        UnitAssert(it->second == pfx.ToString());
      }
      Ipv4PrefixPatricia<string>::const_iterator cit = r.find_longest(pfx);
      if (UnitAssert(cit != r.end())) {
        UnitAssert(cit->first == pfx);
        UnitAssert(cit->second == pfx.ToString());
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFindMatches()
{
  std::vector<std::pair<const Dwm::Ipv4Prefix,std::string>>  matches;
  Ipv4PrefixPatricia<string>  trie;
  trie.Add(Ipv4Prefix("10.0.0.0/8"),     "10.0.0.0/8");
  trie.Add(Ipv4Prefix("192.168.0.0/16"), "192.168.0.0/16");
  trie.Add(Ipv4Prefix("172.16.0.0/12"),  "172.16.0.0/12");
  trie.Add(Ipv4Prefix("10.1.0.0/16"),    "10.1.0.0/16");
  trie.Add(Ipv4Prefix("10.1.1.0/24"),    "10.1.1.0/24");
  UnitAssert(trie.find_matches(Ipv4Prefix("10.1.1.1"), matches));
  if (UnitAssert(matches.size() == 3)) {
    UnitAssert(std::find_if(matches.begin(), matches.end(),
                            [] (const auto & p) 
                            { return p.first == Ipv4Prefix("10.0.0.0/8"); })
               != matches.end());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFindLongestPerformance()
{
  Ipv4PrefixPatricia<string>  r;
  vector<Ipv4Prefix>   pfxVec;
  ifstream is(g_myDir + "/IPV4_prefixes.20210123");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv4Prefix  pfx(buf);
      r.Add(pfx, buf);
      pfxVec.push_back(pfx);
      memset(buf,0,512);
    }
    is.close();

    size_t  count = 0;
    Dwm::TimeValue  startTime(true);
    for (int i = 0; i < 5; ++i) {
      for (const auto & pfx : pfxVec) {
        auto  it = r.find_longest(pfx);
        count += (it != r.end() ? 1 : 0);
      }
    }
    Dwm::TimeValue  endTime(true);
    endTime -= startTime;
    uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
    uint64_t  lookupsPerSec = (pfxVec.size() * 1000000ULL * 5) / usecs;
    cout << pfxVec.size() << " prefixes, " << lookupsPerSec
         << " string lookups/sec (iterator)" << endl;
    UnitAssert((5 * pfxVec.size()) == count);
  }
  return;
}

//----------------------------------------------------------------------------
//!  Test iterator and const_iterator
//----------------------------------------------------------------------------
void TestIterators()
{
  Ipv4PrefixPatricia<string>  trie;

  // Empty trie: begin == end
  UnitAssert(trie.begin() == trie.end());
  UnitAssert(trie.cbegin() == trie.cend());

  // Insert some prefixes in non-sorted order
  trie.Add(Ipv4Prefix("10.0.0.0/8"),     "10.0.0.0/8");
  trie.Add(Ipv4Prefix("192.168.0.0/16"), "192.168.0.0/16");
  trie.Add(Ipv4Prefix("172.16.0.0/12"),  "172.16.0.0/12");
  trie.Add(Ipv4Prefix("10.1.0.0/16"),    "10.1.0.0/16");
  trie.Add(Ipv4Prefix("10.1.1.0/24"),    "10.1.1.0/24");

  // Collect all entries via iterator
  vector<Ipv4Prefix>  prefixes;
  vector<string>      values;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    prefixes.push_back(it->first);
    values.push_back(it->second);
  }

  // Should have 5 entries
  UnitAssert(prefixes.size() == 5);

  // Verify traversal produces sorted output
  for (size_t i = 1; i < prefixes.size(); ++i) {
    UnitAssert(prefixes[i - 1] < prefixes[i]);
  }

  // Verify values match keys
  for (size_t i = 0; i < prefixes.size(); ++i) {
    UnitAssert(values[i] == prefixes[i].ToString());
  }

  // Test const_iterator
  const Ipv4PrefixPatricia<string> & ctrie = trie;
  vector<Ipv4Prefix>  cprefixes;
  for (auto it = ctrie.cbegin(); it != ctrie.cend(); ++it) {
    cprefixes.push_back(it->first);
  }
  UnitAssert(cprefixes.size() == 5);
  UnitAssert(cprefixes == prefixes);

  // Test const_iterator construction from iterator
  Ipv4PrefixPatricia<string>::const_iterator  cit = trie.begin();
  UnitAssert(cit != trie.cend());
  UnitAssert(cit->first == prefixes[0]);

  // Test range-for with const
  size_t count = 0;
  auto  pit = cprefixes.begin();
  for (const auto & [pfx, val] : ctrie) {
    count += UnitAssert(pfx == *pit);
    ++pit;
  }
  UnitAssert(count == 5);

  // Test value modification through iterator
  trie.Add(Ipv4Prefix("1.2.3.0/24"), "original");
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    if (it->first.ToString() == "1.2.3.0/24") {
      it->second = "modified";
      break;
    }
  }
  auto matchit = trie.find_longest(Ipv4Prefix("1.2.3.4/24"));
  if (UnitAssert(matchit != trie.end())) {
    UnitAssert(matchit->second == "modified");
  }

  // Test post-increment
  auto it = trie.begin();
  auto it2 = it++;
  // After post-increment, it2 should be at first element, it at second
  UnitAssert(it2->first < it->first);
  // Advance it again and verify ordering
  ++it;
  UnitAssert(it2->first < it->first);

  // Test removal during iteration (iterate, collect, then remove)
  vector<Ipv4Prefix>  toRemove;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    if (it->first.MaskLength() == 24) {
      toRemove.push_back(it->first);
    }
  }
  for (const auto & pfx : toRemove) {
    UnitAssert(trie.Remove(pfx));
  }

  // After removing /24 entries, should have fewer
  count = 0;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    (void)it;
    ++count;
  }
  UnitAssert(count == 4);  // 5 - 1 (/24 removed, 10.1.1.0/24 was /24)
}

//----------------------------------------------------------------------------
//!  Test find() — exact-match lookup returning an iterator
//----------------------------------------------------------------------------
void TestFind()
{
  Ipv4PrefixPatricia<string>  trie;

  // Empty trie: find returns end
  UnitAssert(trie.find(Ipv4Prefix("10.0.0.0/8")) == trie.end());

  // Insert prefixes
  trie.Add(Ipv4Prefix("10.0.0.0/8"),     "10.0.0.0/8");
  trie.Add(Ipv4Prefix("192.168.0.0/16"), "192.168.0.0/16");
  trie.Add(Ipv4Prefix("172.16.0.0/12"),  "172.16.0.0/12");
  trie.Add(Ipv4Prefix("10.1.0.0/16"),    "10.1.0.0/16");
  trie.Add(Ipv4Prefix("10.1.1.0/24"),    "10.1.1.0/24");

  // find existing prefixes
  {
    auto it = trie.find(Ipv4Prefix("10.0.0.0/8"));
    UnitAssert(it != trie.end());
    UnitAssert(it->first == Ipv4Prefix("10.0.0.0/8"));
    UnitAssert(it->second == "10.0.0.0/8");
  }
  {
    auto it = trie.find(Ipv4Prefix("10.1.1.0/24"));
    UnitAssert(it != trie.end());
    UnitAssert(it->first == Ipv4Prefix("10.1.1.0/24"));
    UnitAssert(it->second == "10.1.1.0/24");
  }
  {
    auto it = trie.find(Ipv4Prefix("192.168.0.0/16"));
    UnitAssert(it != trie.end());
    UnitAssert(it->first == Ipv4Prefix("192.168.0.0/16"));
  }

  // find non-existing prefix
  UnitAssert(trie.find(Ipv4Prefix("8.8.8.0/24")) == trie.end());

  // find a prefix that is contained by a stored prefix (no exact match)
  // 10.1.1.128/25 is inside 10.1.1.0/24 but not stored
  UnitAssert(trie.find(Ipv4Prefix("10.1.1.128/25")) == trie.end());

  // find a shorter prefix that is not stored
  // 10.1.0.0/16 is stored but 10.1.0.0/17 is not
  UnitAssert(trie.find(Ipv4Prefix("10.1.0.0/17")) == trie.end());

  // Use iterator to modify value
  {
    auto it = trie.find(Ipv4Prefix("10.1.0.0/16"));
    UnitAssert(it != trie.end());
    it->second = "modified";
    auto matchit = trie.find_longest(Ipv4Prefix("10.1.5.5/24"));
    if (UnitAssert(matchit != trie.end())) {
      UnitAssert(matchit->second == "modified");
    }
  }

  // Test const find on a const reference
  const Ipv4PrefixPatricia<string> & ctrie = trie;
  {
    auto it = ctrie.find(Ipv4Prefix("10.0.0.0/8"));
    UnitAssert(it != ctrie.end());
    UnitAssert(it->first == Ipv4Prefix("10.0.0.0/8"));
    UnitAssert(it->second == "10.0.0.0/8");
  }
  {
    auto it = ctrie.find(Ipv4Prefix("10.1.1.0/24"));
    UnitAssert(it != ctrie.end());
    UnitAssert(it->first == Ipv4Prefix("10.1.1.0/24"));
  }
  // Non-existing prefix on const trie
  UnitAssert(ctrie.find(Ipv4Prefix("8.8.8.0/24")) == ctrie.end());
}

//----------------------------------------------------------------------------
static void TestErase()
{
  Ipv4PrefixPatricia<string>  trie;

  // Empty trie: begin == end
  UnitAssert(trie.begin() == trie.end());
  UnitAssert(trie.cbegin() == trie.cend());

  // Insert some prefixes in non-sorted order
  trie.Add(Ipv4Prefix("10.0.0.0/8"),     "10.0.0.0/8");
  trie.Add(Ipv4Prefix("192.168.0.0/16"), "192.168.0.0/16");
  trie.Add(Ipv4Prefix("172.16.0.0/12"),  "172.16.0.0/12");
  trie.Add(Ipv4Prefix("10.1.0.0/16"),    "10.1.0.0/16");
  trie.Add(Ipv4Prefix("10.1.1.0/24"),    "10.1.1.0/24");

  auto it = trie.begin();
  do {
    it = trie.erase(it);
  } while (it != trie.end());
  UnitAssert(trie.Size() == 0);
  std::cerr << "trie.Size(): " << trie.Size() << '\n';
  
  return;
}

//----------------------------------------------------------------------------
//!  Test reverse iterators
//----------------------------------------------------------------------------
void TestReverseIterators()
{
  Ipv4PrefixPatricia<string>  trie;

  // Empty trie: rbegin == rend
  UnitAssert(trie.rbegin() == trie.rend());
  UnitAssert(trie.crbegin() == trie.crend());

  // Insert some prefixes
  trie.Add(Ipv4Prefix("10.0.0.0/8"),     "10.0.0.0/8");
  trie.Add(Ipv4Prefix("192.168.0.0/16"), "192.168.0.0/16");
  trie.Add(Ipv4Prefix("172.16.0.0/12"),  "172.16.0.0/12");
  trie.Add(Ipv4Prefix("10.1.0.0/16"),    "10.1.0.0/16");
  trie.Add(Ipv4Prefix("10.1.1.0/24"),    "10.1.1.0/24");

  // Collect sorted prefixes (forward)
  vector<Ipv4Prefix>  prefixes;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    prefixes.push_back(it->first);
  }

  // Test reverse traversal
  vector<Ipv4Prefix>  revPrefixes;
  for (auto rit = trie.rbegin(); rit != trie.rend(); ++rit) {
    revPrefixes.push_back(rit->first);
  }

  UnitAssert(revPrefixes.size() == 5);
  for (size_t i = 0; i < prefixes.size(); ++i) {
    UnitAssert(revPrefixes[i] == prefixes[prefixes.size() - 1 - i]);
  }

  // Test const_reverse_iterator
  const Ipv4PrefixPatricia<string> & ctrie = trie;
  vector<Ipv4Prefix>  crevPrefixes;
  for (auto rit = ctrie.crbegin(); rit != ctrie.crend(); ++rit) {
    crevPrefixes.push_back(rit->first);
  }
  UnitAssert(crevPrefixes.size() == 5);
  UnitAssert(crevPrefixes == revPrefixes);

  // Test rbegin() and rend() on const trie
  auto rbeginConst = ctrie.rbegin();
  auto rendConst = ctrie.rend();
  UnitAssert(rbeginConst != rendConst);
  UnitAssert(rbeginConst->first == prefixes.back());
  --rendConst;
  UnitAssert(rendConst->first == prefixes.front());
}

//----------------------------------------------------------------------------
//!  Test bidirectional iterators
//----------------------------------------------------------------------------
void TestBidirectionalIterators()
{
  Ipv4PrefixPatricia<string>  trie;

  // Insert some prefixes
  trie.Add(Ipv4Prefix("10.0.0.0/8"),     "10.0.0.0/8");
  trie.Add(Ipv4Prefix("192.168.0.0/16"), "192.168.0.0/16");
  trie.Add(Ipv4Prefix("172.16.0.0/12"),  "172.16.0.0/12");
  trie.Add(Ipv4Prefix("10.1.0.0/16"),    "10.1.0.0/16");
  trie.Add(Ipv4Prefix("10.1.1.0/24"),    "10.1.1.0/24");

  // Collect sorted prefixes
  vector<Ipv4Prefix>  prefixes;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    prefixes.push_back(it->first);
  }

  // Test forward traversal
  auto it = trie.begin();
  for (size_t i = 0; i < prefixes.size(); ++i) {
    UnitAssert(it->first == prefixes[i]);
    ++it;
  }
  UnitAssert(it == trie.end());

  it = trie.find(prefixes[2]);
  ++it;
  UnitAssert(it->first == prefixes[3]);
  --it;
  UnitAssert(it->first == prefixes[2]);
  --it;
  UnitAssert(it->first == prefixes[1]);
  
  // Test backward traversal
  it = trie.begin();
  auto rit = trie.end();
  while (rit != it) {
    --rit;
    UnitAssert(rit->first == prefixes.back());
    prefixes.pop_back();
  }
  UnitAssert(prefixes.empty());
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

  TestIterators();
  TestBidirectionalIterators();
  TestReverseIterators();
  TestFind();
  TestFindLongest();
  TestFindMatches();
  if (g_performanceTests) {
    TestFindLongestPerformance();
  }
  TestErase();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
