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
void TestWithString()
{
  Ipv4PrefixPatricia<string>  r;
  vector<Ipv4Prefix>   pfxVec;
  vector<Ipv4Address>  ipVec;
  ifstream is(g_myDir + "/IPV4_prefixes.20210123");
  if (is) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv4Prefix  pfx(buf);
      r.Add(pfx, buf);
      ipVec.push_back(pfx.Network());
      pfxVec.push_back(pfx);
      memset(buf,0,512);
    }
    is.close();

    uint64_t                             rsize = r.Size();
    vector<Ipv4Address>::const_iterator  ipVecIter;
    pair<Ipv4Prefix, const string *>     match;
    const uint8_t                        numIterations = 10;
    uint64_t                             numFound = 0;

    Dwm::TimeValue  startTime(true);
    for (uint8_t i = 0; i < numIterations; ++i) {
      for (ipVecIter = ipVec.begin(); 
           ipVecIter != ipVec.end(); ++ipVecIter) {
        numFound += r.LongestMatch(*ipVecIter).has_value() ? 1 : 0;
      }
    }
    UnitAssert(numFound == ipVec.size() * numIterations);

    // std::cout << r;
    
    if (g_performanceTests) {
      Dwm::TimeValue endTime(true);
      endTime -= startTime;
      uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
      uint64_t  lookupsPerSec = (ipVec.size() * 1000000ULL * numIterations) / usecs;
      cout << ipVec.size() << " prefixes, " << lookupsPerSec
           << " string lookups/sec" << endl;
    }

    // std::cerr << "r.Size(): " << r.Size() << '\n';
    for (auto pfxVecIter = pfxVec.begin(); pfxVecIter != pfxVec.end();
         ++pfxVecIter) {
      UnitAssert(r.Remove(*pfxVecIter));
    }
    UnitAssert(0 == r.Size());
    
    // std::cerr << "r.Size(): " << r.Size() << '\n';
    
#if 0
    //  check sorting by value into a vector
    vector<pair<Ipv4Prefix,string> >  sortedVec;
    r.SortByValue(sortedVec);
    vector<pair<Ipv4Prefix,string> >::const_iterator  soIter = 
      sortedVec.begin();
    vector<pair<Ipv4Prefix,string> >::const_iterator  soIter2 =
      soIter;
    ++soIter2;
    for ( ; soIter2 != sortedVec.end(); ++soIter, ++soIter2) {
      UnitAssert(soIter->second > soIter2->second);
    }
    
    numFound = 0;
    for (ipVecIter = ipVec.begin(); ipVecIter != ipVec.end(); ++ipVecIter) {
      numFound += UnitAssert(r.FindLongest(*ipVecIter, match));
      UnitAssert(Ipv4Prefix(*match.second).Network() == *ipVecIter);
      UnitAssert(r.Delete(match.first));
    }
    UnitAssert(numFound == ipVec.size());
    UnitAssert(r.Empty());
#endif
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
  for (const auto & [pfx, val] : ctrie) {
    (void)pfx;
    (void)val;
    ++count;
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
  auto match = trie.LongestMatch(Ipv4Prefix("1.2.3.4/24"));
  UnitAssert(match.has_value());
  UnitAssert(match->second == "modified");

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
    auto match = trie.LongestMatch(Ipv4Prefix("10.1.5.5/24"));
    UnitAssert(match.has_value());
    UnitAssert(match->second == "modified");
  }
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

  //  simple performance tests
  TestWithString();

  TestIterators();
  TestFind();
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
