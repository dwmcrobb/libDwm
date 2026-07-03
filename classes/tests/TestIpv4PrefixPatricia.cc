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
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
