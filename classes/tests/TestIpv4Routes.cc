//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4Routes.cc 8389 $
// @(#) $Id: TestIpv4Routes.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file TestIpv4Routes.hh
//!  \brief Unit tests for Dwm::Ipv4Routes class template
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <errno.h>
  #include <inttypes.h>
}

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#include "DwmIpv4Routes.hh"
#include "DwmOperators.hh"
#include "DwmOptArgs.hh"
#include "DwmSvnTag.hh"
#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4Routes.cc 8389 $");

using namespace std;
using namespace Dwm;

#define STOREFILE "/tmp/DWMTestIpv4Routes.store"

static bool  g_performanceTests = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestWithString()
{
  Ipv4Routes<string>  r;
  vector<Ipv4Address>  ipVec;
  ifstream is("./IPV4_prefixes.20210123");
  if (is) {
    char  buf[512];
    memset(buf,0,512);
    while (is.getline(buf,512,'\n')) {
      Ipv4Prefix  pfx(buf);
      r.Add(pfx, buf);
      ipVec.push_back(pfx.Network());
      memset(buf,0,512);
    }
    is.close();

    uint64_t                             rsize = r.Size();
    vector<Ipv4Address>::const_iterator  ipVecIter;
    pair<Ipv4Prefix, const string *>     match;
    const uint8_t                        numIterations = 10;
    uint32_t                             numFound = 0;
    if (g_performanceTests) {
      Dwm::TimeValue  startTime(true);
      for (uint8_t i = 0; i < numIterations; ++i) {
        for (ipVecIter = ipVec.begin(); 
             ipVecIter != ipVec.end(); ++ipVecIter) {
          numFound += r.FindLongest(*ipVecIter, match);
        }
      }
      Dwm::TimeValue endTime(true);

      UnitAssert(numFound == (rsize * numIterations));
    
      endTime -= startTime;
      uint64_t  usecs = (endTime.Secs() * 1000000ULL) + endTime.Usecs();
      uint64_t  lookupsPerSec = (rsize * 1000000ULL * 10) / usecs;
      cout << rsize << " prefixes, " << lookupsPerSec
           << " string pointer lookups/sec" << endl;
    }
    
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
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestBasicIO(Ipv4Routes<uint32_t> & r)
{
  ofstream os(STOREFILE);
  if (os) {
    UnitAssert(r.Write(os));
    os.close();
    
    Ipv4Routes<uint32_t>  r2;
    ifstream is(STOREFILE);
    if (is) {
      UnitAssert(r2.Read(is));
      is.close();
      UnitAssert(r2 == r);
    }
    else {
      cerr << "Failed to open ifstream for '" << STOREFILE << "': " 
           << strerror(errno) << endl;
    }
    std::remove(STOREFILE);
  }
  else {
    cerr << "Failed to open ofstream for '" << STOREFILE << "': " 
         << strerror(errno) << endl;
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO(const Ipv4Routes<uint32_t> & r)
{
  string  fileName(STOREFILE);
  fileName += ".gz";
  gzFile  gzf = gzopen(fileName.c_str(), "wb");
  if (gzf) {
    UnitAssert(r.Write(gzf));
    gzclose(gzf);
    
    Ipv4Routes<uint32_t>  r2;
    gzf = gzopen(fileName.c_str(), "rb");
    if (gzf) {
      UnitAssert(r2.Read(gzf));
      gzclose(gzf);
      UnitAssert(r2.Size() == r.Size());
    }
    else {
      cerr << "Failed to open gzFile '" << STOREFILE << "' for reading: " 
           << strerror(errno) << endl;
    }
    std::remove(fileName.c_str());
  }
  else {
    cerr << "Failed to open gzFile '" << STOREFILE << "' for writing: " 
         << strerror(errno) << endl;
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZIO(Ipv4Routes<uint32_t> & r)
{
  string  fileName(STOREFILE);
  fileName += ".bz2";
  BZFILE  *bzf = BZ2_bzopen(fileName.c_str(),"wb");
  if (bzf) {
    UnitAssert(r.BZWrite(bzf));
    BZ2_bzclose(bzf);
    
    Ipv4Routes<uint32_t>  r2;
    bzf = BZ2_bzopen(fileName.c_str(),"rb");
    if (bzf) {
      UnitAssert(r2.BZRead(bzf));
      BZ2_bzclose(bzf);
      UnitAssert(r2.Size() == r.Size());
    }
    else {
      cerr << "Failed to open BZFILE '" << fileName.c_str() 
           << "' for reading: " << strerror(errno) << endl;
    }
    std::remove(fileName.c_str());
  }
  else {
    cerr << "Failed to open BZFILE '" << fileName.c_str() << "' for writing: " 
         << strerror(errno) << endl;
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void CheckHashSizes(const vector<Ipv4Prefix> & pfxVec,
                           const Ipv4Routes<uint32_t> & r)
{
  vector<pair<uint8_t,uint32_t> >  hashSizes;
  
  r.HashSizes(hashSizes);
  vector<pair<uint8_t,uint32_t> >::const_iterator  hsiter;
  for (hsiter = hashSizes.begin(); hsiter != hashSizes.end(); ++hsiter) {
    uint32_t  count = 0;
    vector<Ipv4Prefix>::const_iterator  pfiter;
    for (pfiter = pfxVec.begin(); pfiter != pfxVec.end(); ++pfiter) {
      if (pfiter->MaskLength() == hsiter->first) {
        ++count;
      }
    }
    UnitAssert(hsiter->second == count);
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void CheckLookups(const vector<Ipv4Prefix> & pfxVec,
                         const Ipv4Routes<uint32_t> & r)
{
  pair<Ipv4Prefix,uint32_t>           match;
  vector<Ipv4Prefix>::const_iterator  iter;
  for (iter = pfxVec.begin(); iter != pfxVec.end(); ++iter) {
    UnitAssert(r.FindLongest(iter->Network(), match));
    UnitAssert(match.first.Contains(iter->Network()));
    UnitAssert(r.Find(*iter, match.second));
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void CheckDeletions(const vector<Ipv4Prefix> & pfxVec,
                           Ipv4Routes<uint32_t> & r)
{
  vector<Ipv4Prefix>::const_iterator   pfxVecIter = pfxVec.begin();
  for ( ; pfxVecIter != pfxVec.end(); ++pfxVecIter) {
    UnitAssert(r.Delete(*pfxVecIter));
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestInsertions(Ipv4Routes<uint32_t> & r, 
                           vector<Ipv4Prefix> & pfxVec)
{
  ifstream is("./IPV4_prefixes.20210123");
  if (UnitAssert(is)) {
    string  s;
    while (getline(is, s, '\n')) {
      Ipv4Prefix  pfx(s);
      UnitAssert(r.Add(pfx, inet_addr(s.c_str())));
      pfxVec.push_back(pfx);
    }
    is.close();
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestInsertionOperator(Ipv4Routes<uint32_t> & r,
                                  const vector<Ipv4Prefix> & pfxVec)
{
  vector<Ipv4Prefix>::const_iterator   iter;
  for (iter = pfxVec.begin(); iter != pfxVec.end(); ++iter) {
    r[*iter] = iter->Network().Raw();
    uint32_t  match = 0;
    UnitAssert(r.Find(*iter, match));
    UnitAssert(match == iter->Network().Raw());
  }
  for (iter = pfxVec.begin(); iter != pfxVec.end(); ++iter) {
    UnitAssert(r.Add(*iter, iter->Network().Raw()) == false);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestPerformance(Ipv4Routes<uint32_t> & r,
                            vector<Ipv4Prefix> & pfxVec)
{
  vector<Ipv4Prefix>::const_iterator   iter;
  pair<Ipv4Prefix, uint32_t>           match;
  
  Dwm::TimeValue  startTime(true);
  for (uint8_t i = 0; i < 10; ++i) {
    for (iter = pfxVec.begin(); iter != pfxVec.end(); ++iter) {
      r.FindLongest(iter->Network(), match);
    }
  }
  Dwm::TimeValue endTime(true);
  endTime -= startTime;

  uint64_t  rsize = r.Size();
  uint64_t  lookupsPerSec = 
    (rsize * 1000000ULL * 10) / 
    ((endTime.Secs() * 1000000ULL) + endTime.Usecs());
  cout << rsize << " prefixes, " << lookupsPerSec 
       << " uint32_t value lookups/sec" << endl;

  pair<Ipv4Prefix, const uint32_t *>  ptrMatch;
  startTime.SetNow();
  for (uint8_t i = 0; i < 10; ++i) {
    for (iter = pfxVec.begin(); iter != pfxVec.end(); ++iter) {
      r.FindLongest(iter->Network(), ptrMatch);
    }
  }
  endTime.SetNow();
  endTime -= startTime;
  lookupsPerSec = 
    (rsize * 1000000ULL * 10) / 
    ((endTime.Secs() * 1000000ULL) + endTime.Usecs());
  cout << rsize << " prefixes, " << lookupsPerSec 
       << " uint32_t pointer lookups/sec" << endl;

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestWithUint32()
{
  Ipv4Routes<uint32_t>  r;
  vector<Ipv4Prefix>    pfxVec;

  TestInsertions(r, pfxVec);

  if (! pfxVec.empty()) {
    if (g_performanceTests) {
      TestPerformance(r, pfxVec);
    }
    CheckLookups(pfxVec, r);
    CheckHashSizes(pfxVec, r);
    
    TestBasicIO(r);
    TestGZIO(r);
    TestBZIO(r);

    CheckDeletions(pfxVec, r);
    
    r.Clear();
    UnitAssert(r.Empty());

    TestInsertionOperator(r, pfxVec);
    r.Clear();
    UnitAssert(r.Empty());
  }

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
typedef struct {
  const char  *p;
  uint32_t     h;
} prefix_test_struct;

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
  
  Ipv4Routes<uint32_t>  r;
  
  const prefix_test_struct prefixes[] = {
    { "24/8",         inet_addr("24.0.0.0") },
    { "24.1/16",      inet_addr("24.1.0.0") },
    { "24.1.1/24",    inet_addr("24.1.1.0") },
    { "24.1.1.1/32",  inet_addr("24.1.1.1") },
    { "64.1/16",      inet_addr("64.1.0.0") },
    { "128.1.1/24",   inet_addr("128.1.1.0") },
    { "192.1.1.1/32", inet_addr("192.1.1.1") }
  };

  for (uint8_t i = 0; i < 7; ++i)
    r.Add(Ipv4Prefix(prefixes[i].p), prefixes[i].h);

  //  test lookups
  pair<Ipv4Prefix,uint32_t>  match;
  for (uint8_t i = 0; i < 7; ++i) {
    UnitAssert(r.FindLongest(Ipv4Address(prefixes[i].h), match));
    UnitAssert(match.first == Ipv4Prefix(prefixes[i].p));
    UnitAssert(match.second == prefixes[i].h);
  }
  vector<pair<Ipv4Prefix,uint32_t> >  matches;
  UnitAssert(r.Find(Ipv4Address("24.1.1.1"), matches));
  UnitAssert(matches.size() == 4);

  //  test entry deletion
  for (uint8_t i = 0; i < 7; ++i)
    UnitAssert(r.Delete(Ipv4Prefix(prefixes[i].p)));
  UnitAssert(! r.Delete(Ipv4Prefix(prefixes[0].p)));

  //  simple performance tests
  TestWithUint32();
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
