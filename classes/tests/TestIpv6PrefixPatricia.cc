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
//!  @file TestIpv6PrefixPatricia.cc
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>
#include <iostream>

#include "DwmIpv6PrefixPatricia.hh"
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
  Ipv6PrefixPatricia<string>  r;
  vector<Ipv6Prefix>   pfxVec;
  ifstream is(g_myDir + "/IPV6_prefixes.20260709");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv6Prefix  pfx(buf);
      r[pfx] = buf;
      pfxVec.push_back(pfx);
      memset(buf,0,512);
    }
    is.close();
    UnitAssert(pfxVec.size() == r.size());
    for (const auto & pfx : pfxVec) {
      auto  it = r.find_longest(pfx);
      if (UnitAssert(it != r.end())) {
        UnitAssert(it->first == pfx);
        UnitAssert(it->second == pfx.ToString());
      }
      Ipv6PrefixPatricia<string>::const_iterator cit = r.find_longest(pfx);
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
  std::vector<std::pair<const Dwm::Ipv6Prefix,std::string>>  matches;
  Ipv6PrefixPatricia<string>  trie;
  trie.insert({Ipv6Prefix("0a0a::/16"), "0a0a::/16"});
  trie.insert({Ipv6Prefix("c0c0:a8a8::/32"), "c0c0:a8a8::/32"});
  trie.insert({Ipv6Prefix("acac:1000::/24"), "acac:1000::/24"});
  trie.insert({Ipv6Prefix("0a0a:0101::/32"), "0a0a:0101::/32"});
  trie.insert({Ipv6Prefix("0a0a:0101:0101::/48"), "0a0a:0101:0101::/48"});

  UnitAssert(trie.find_matches(Ipv6Prefix("0a0a:0101:0101:0101::/64"), matches));
  if (UnitAssert(matches.size() == 3)) {
    UnitAssert(std::find_if(matches.begin(), matches.end(),
                            [] (const auto & p) 
                            { return p.first == Ipv6Prefix("0a0a::/16"); })
               != matches.end());
    UnitAssert(std::find_if(matches.begin(), matches.end(),
                            [] (const auto & p) 
                            { return p.first == Ipv6Prefix("0a0a:0101::/32"); })
               != matches.end());
    UnitAssert(std::find_if(matches.begin(), matches.end(),
                            [] (const auto & p) 
                            { return p.first == Ipv6Prefix("0a0a:0101:0101::/48"); })
               != matches.end());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFindLongestPerformance()
{
  Ipv6PrefixPatricia<string>  r;
  vector<Ipv6Prefix>   pfxVec;
  ifstream is(g_myDir + "/IPV6_prefixes.20260709");
  if (UnitAssert(is)) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv6Prefix  pfx(buf);
      r.insert({pfx, buf});
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
  Ipv6PrefixPatricia<string>  trie;

  // Empty trie: begin == end
  UnitAssert(trie.begin() == trie.end());
  UnitAssert(trie.cbegin() == trie.cend());

  // Insert some prefixes
  trie.insert({Ipv6Prefix("a0a::/16"), "a0a::/16"});
  trie.insert({Ipv6Prefix("c0c0:a8a8::/32"), "c0c0:a8a8::/32"});
  trie.insert({Ipv6Prefix("acac:1000::/24"), "acac:1000::/24"});
  trie.insert({Ipv6Prefix("a0a:101::/32"), "a0a:101::/32"});
  trie.insert({Ipv6Prefix("a0a:101:101::/48"), "a0a:101:101::/48"});

  // Collect all entries via iterator
  vector<Ipv6Prefix>  prefixes;
  vector<string>      values;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    prefixes.push_back(it->first);
    values.push_back(it->second);
  }

  // Should have 5 entries
  UnitAssert(trie.size() == 5);
  UnitAssert(prefixes.size() == 5);

  // Verify traversal produces sorted output
  for (size_t i = 1; i < prefixes.size(); ++i) {
    UnitAssert(prefixes[i - 1] < prefixes[i]);
  }

  // Verify values match keys
  for (size_t i = 0; i < prefixes.size(); ++i) {
    if (! UnitAssert(values[i] == prefixes[i].ToString())) {
      std::cerr << "values[" << i << "]: " << values[i]
                << " prefixes[" << i << "].ToString(): "
                << prefixes[i].ToString() << '\n';
    }
  }

  // Test const_iterator
  const Ipv6PrefixPatricia<string> & ctrie = trie;
  vector<Ipv6Prefix>  cprefixes;
  for (auto it = ctrie.cbegin(); it != ctrie.cend(); ++it) {
    cprefixes.push_back(it->first);
  }
  UnitAssert(cprefixes.size() == 5);
  UnitAssert(cprefixes == prefixes);

  // Test const_iterator construction from iterator
  Ipv6PrefixPatricia<string>::const_iterator  cit = trie.begin();
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
  trie.insert({Ipv6Prefix("101:202:303::/48"), "original"});
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    if (it->first.ToString() == "101:202:303::/48") {
      it->second = "modified";
      break;
    }
  }
  auto matchit = trie.find_longest(Ipv6Prefix("101:202:303::/48"));
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
  vector<Ipv6Prefix>  toRemove;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    if (it->first.MaskLength() == 48) {
      toRemove.push_back(it->first);
    }
  }
  for (const auto & pfx : toRemove) {
    UnitAssert(trie.Remove(pfx));
  }

  // After removing /48 entries, should have fewer
  count = 0;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    (void)it;
    ++count;
  }
  UnitAssert(count == 4);  // 5 - 1 (0a0a:0101:0101::/48 removed)
}

//----------------------------------------------------------------------------
//!  Test find() — exact-match lookup returning an iterator
//----------------------------------------------------------------------------
void TestFind()
{
  Ipv6PrefixPatricia<string>  trie;

  // Empty trie: find returns end
  UnitAssert(trie.find(Ipv6Prefix("a0a::/16")) == trie.end());

  // Insert prefixes
  trie.insert({Ipv6Prefix("a0a::/16"), "a0a::/16"});
  trie.insert({Ipv6Prefix("c0c0:a8a8::/32"), "c0c0:a8a8::/32"});
  trie.insert({Ipv6Prefix("acac:1000::/24"), "acac:1000::/24"});
  trie.insert({Ipv6Prefix("a0a:101::/32"), "a0a:101::/32"});
  trie.insert({Ipv6Prefix("a0a:101:101::/48"), "a0a:101:101::/48"});

  // find existing prefixes
  {
    auto it = trie.find(Ipv6Prefix("a0a::/16"));
    UnitAssert(it != trie.end());
    UnitAssert(it->first == Ipv6Prefix("a0a::/16"));
    UnitAssert(it->second == "a0a::/16");
  }
  {
    auto it = trie.find(Ipv6Prefix("a0a:101:101::/48"));
    UnitAssert(it != trie.end());
    UnitAssert(it->first == Ipv6Prefix("a0a:101:101::/48"));
    UnitAssert(it->second == "a0a:101:101::/48");
  }
  {
    auto it = trie.find(Ipv6Prefix("c0c0:a8a8::/32"));
    UnitAssert(it != trie.end());
    UnitAssert(it->first == Ipv6Prefix("c0c0:a8a8::/32"));
  }

  // find non-existing prefix
  UnitAssert(trie.find(Ipv6Prefix("0808:0808::/48")) == trie.end());

  // find a prefix that is contained by a stored prefix (no exact match)
  UnitAssert(trie.find(Ipv6Prefix("a0a:101:101:8080/50")) == trie.end());

  // find a shorter prefix that is not stored
  UnitAssert(trie.find(Ipv6Prefix("a0a:101::/34")) == trie.end());

  // Use iterator to modify value
  {
    auto it = trie.find(Ipv6Prefix("a0a:101::/32"));
    UnitAssert(it != trie.end());
    it->second = "modified";
    auto matchit = trie.find_longest(Ipv6Prefix("a0a:101:505:505::/48"));
    if (UnitAssert(matchit != trie.end())) {
      UnitAssert(matchit->second == "modified");
    }
  }

  // Test const find on a const reference
  const Ipv6PrefixPatricia<string> & ctrie = trie;
  {
    auto it = ctrie.find(Ipv6Prefix("a0a::/16"));
    UnitAssert(it != ctrie.end());
    UnitAssert(it->first == Ipv6Prefix("a0a::/16"));
    UnitAssert(it->second == "a0a::/16");
  }
  {
    auto it = ctrie.find(Ipv6Prefix("a0a:101:101::/48"));
    UnitAssert(it != ctrie.end());
    UnitAssert(it->first == Ipv6Prefix("a0a:101:101::/48"));
  }
  // Non-existing prefix on const trie
  UnitAssert(ctrie.find(Ipv6Prefix("808:808::/48")) == ctrie.end());
}

//----------------------------------------------------------------------------
static void TestErase()
{
  Ipv6PrefixPatricia<string>  trie;

  // Empty trie: begin == end
  UnitAssert(trie.begin() == trie.end());
  UnitAssert(trie.cbegin() == trie.cend());

  // Insert some prefixes in non-sorted order
  trie.insert({Ipv6Prefix("a0a::/16"), "a0a::/16"});
  trie.insert({Ipv6Prefix("c0c0:a8a8::/32"), "c0c0:a8a8::/32"});
  trie.insert({Ipv6Prefix("acac:1000::/24"), "acac:1000::/24"});
  trie.insert({Ipv6Prefix("a0a:101::/32"), "a0a:101::/32"});
  trie.insert({Ipv6Prefix("a0a:101:101::/48"), "a0a:101:101::/48"});
  UnitAssert(trie.size() == 5);

  auto  it = trie.begin();
  auto  sz = trie.size();
  do {
    it = trie.erase(it);
    UnitAssert(--sz == trie.size());
  } while (it != trie.end());
  UnitAssert(trie.size() == 0);

  return;
}

//----------------------------------------------------------------------------
//!  Test insert()
//----------------------------------------------------------------------------
void TestInsert()
{
  Ipv6PrefixPatricia<string>  trie;

  // Test insert new element
  Ipv6Prefix  pfx1("a0a::/16");
  auto res1 = trie.insert({pfx1, "value1"});
  UnitAssert(res1.second == true);
  UnitAssert(res1.first->first == pfx1);
  UnitAssert(res1.first->second == "value1");
  UnitAssert(trie.size() == 1);

  // Test insert existing element (should fail)
  auto res2 = trie.insert({pfx1, "value2"});
  UnitAssert(res2.second == false);
  UnitAssert(res2.first->first == pfx1);
  UnitAssert(res2.first->second == "value1"); // Value should NOT be updated
  UnitAssert(trie.size() == 1);

  // Test insert another element
  Ipv6Prefix  pfx2("c0c0:a8a8::/32");
  auto res3 = trie.insert({pfx2, "value2"});
  UnitAssert(res3.second == true);
  UnitAssert(res3.first->first == pfx2);
  UnitAssert(trie.size() == 2);
}

//----------------------------------------------------------------------------
//!  Test operator[]
//----------------------------------------------------------------------------
void TestOperatorSquareBrackets()
{
  Ipv6PrefixPatricia<string>  trie;

  // Test insertion of new element via operator[]
  Ipv6Prefix  pfx1("a0a::/16");
  trie[pfx1] = "value1";
  UnitAssert(trie.size() == 1);
  UnitAssert(trie.find(pfx1)->second == "value1");

  // Test updating existing element via operator[]
  trie[pfx1] = "updated1";
  UnitAssert(trie.size() == 1);
  UnitAssert(trie.find(pfx1)->second == "updated1");

  // Test default construction via operator[]
  Ipv6Prefix  pfx2("c0c0:a8a8::/32");
  string  val2 = trie[pfx2]; // Should create pfx2 with default string ("")
  UnitAssert(trie.size() == 2);
  UnitAssert(val2 == "");
  UnitAssert(trie.find(pfx2)->second == "");

  // Test with more complex setup
  Ipv6Prefix  pfx3("acac:1000::/24");
  trie[pfx3] = "value3";
  UnitAssert(trie.size() == 3);

  // Verify all are present
  UnitAssert(trie[pfx1] == "updated1");
  UnitAssert(trie[pfx2] == "");
  UnitAssert(trie[pfx3] == "value3");

  // Test with another type (int)
  Ipv6PrefixPatricia<int>  trieInt;
  Ipv6Prefix  pfx4("101:101:101::/48");
  int val4 = trieInt[pfx4]; // default int (0)
  UnitAssert(val4 == 0);
  UnitAssert(trieInt.size() == 1);
  
  trieInt[pfx4] = 42;
  UnitAssert(trieInt[pfx4] == 42);
}

//----------------------------------------------------------------------------
//!  Test reverse iterators
//----------------------------------------------------------------------------
void TestReverseIterators()
{
  Ipv6PrefixPatricia<string>  trie;

  // Empty trie: rbegin == rend
  UnitAssert(trie.rbegin() == trie.rend());
  UnitAssert(trie.crbegin() == trie.crend());

  // Insert some prefixes
  trie.insert({Ipv6Prefix("a0a::/16"), "a0a::/16"});
  trie.insert({Ipv6Prefix("c0c0:a8a8::/32"), "c0c0:a8a8::/32"});
  trie.insert({Ipv6Prefix("acac:1000::/24"), "acac:1000::/24"});
  trie.insert({Ipv6Prefix("a0a:101::/32"), "a0a:101::/32"});
  trie.insert({Ipv6Prefix("a0a:101:101::/48"), "a0a:101:101::/48"});

  // Collect sorted prefixes (forward)
  vector<Ipv6Prefix>  prefixes;
  for (auto it = trie.begin(); it != trie.end(); ++it) {
    prefixes.push_back(it->first);
  }

  // Test reverse traversal
  vector<Ipv6Prefix>  revPrefixes;
  for (auto rit = trie.rbegin(); rit != trie.rend(); ++rit) {
    revPrefixes.push_back(rit->first);
  }

  UnitAssert(revPrefixes.size() == 5);
  for (size_t i = 0; i < prefixes.size(); ++i) {
    UnitAssert(revPrefixes[i] == prefixes[prefixes.size() - 1 - i]);
  }

  // Test const_reverse_iterator
  const Ipv6PrefixPatricia<string> & ctrie = trie;
  vector<Ipv6Prefix>  crevPrefixes;
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
  Ipv6PrefixPatricia<string>  trie;

  // Insert some prefixes
  trie.insert({Ipv6Prefix("a0a::/16"), "a0a::/16"});
  trie.insert({Ipv6Prefix("c0c0:a8a8::/32"), "c0c0:a8a8::/32"});
  trie.insert({Ipv6Prefix("acac:1000::/24"), "acac:1000::/24"});
  trie.insert({Ipv6Prefix("a0a:101::/32"), "a0a:101::/32"});
  trie.insert({Ipv6Prefix("a0a:101:101::/48"), "a0a:101:101::/48"});

  // Collect sorted prefixes
  vector<Ipv6Prefix>  prefixes;
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
  TestInsert();
  TestOperatorSquareBrackets();
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
