//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmGZIO.cc 9112 $
// @(#) $Id: TestDwmGZIO.cc 9112 2017-04-14 22:34:43Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016
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
//!  \file TestDwmGZIO.cc
//!  \brief Unit tests for Dwm::GZIO
//---------------------------------------------------------------------------

//  This program is just a simple test application for functions in Dwm::GZIO.

extern "C" {
  #include <assert.h>
  #include <fcntl.h>
  #include <errno.h>
}

#include <cstdio>
#include <fstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmGZIO.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmGZIO.cc 9112 $");

static const char      k_charVal   = 'a';
static const uint8_t   k_ucharVal  = 0xF0;
static const int16_t   k_int16Val  = 32222;
static const uint16_t  k_uint16Val = 33333;
static const int32_t   k_int32Val  = 0x7FFFFFFF;
static const uint32_t  k_uint32Val = 0xFBFBFBFB;
static const int64_t   k_int64Val  = 4294967296LL * 15;
static const uint64_t  k_uint64Val = 4294967296LL * 65535;
static const string    k_stringVal = "TestDwmGZIO";
static const float     k_floatVal  = 123456789.987654321;
static const double    k_doubleVal = 987654321.123456789;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlob(gzFile gzf)
{
  bool  rc = false;

  if (gzf) {
    char  c = k_charVal;
    if (GZIO::Write(gzf,c) == sizeof(c)) {
      uint8_t  uc = k_ucharVal;
      if (GZIO::Write(gzf,uc) == sizeof(uc)) {
        int16_t sh = k_int16Val;
        if (GZIO::Write(gzf,sh) == sizeof(sh)) {
          uint16_t ush = k_uint16Val;
          if (GZIO::Write(gzf,ush) == sizeof(ush)) {
            int32_t  w = k_int32Val;
            if (GZIO::Write(gzf,w) == sizeof(w)) {
              uint32_t  uw = k_uint32Val;
              if (GZIO::Write(gzf,uw) == sizeof(uw)) {
                int64_t  d = k_int64Val;
                if (GZIO::Write(gzf,d) == sizeof(d)) {
                  uint64_t  ud = k_uint64Val;
                  if (GZIO::Write(gzf,ud) == sizeof(ud)) {
                    std::string  s = k_stringVal;
                    if (GZIO::Write(gzf,s) > 4) {
                      float  f = k_floatVal;
                      if (GZIO::Write(gzf,f) == sizeof(f)) {
                        double  df = k_doubleVal;
                        if (GZIO::Write(gzf,df) == sizeof(df)) {
                          rc = true;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlobs(gzFile gzf, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! WriteTestBlob(gzf)) {
      rc = false;
      break;
    }
  }
  UnitAssert(rc);
  return(rc);
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReadTestBlob(gzFile gzf)
{
  bool  rc = false;
  if (gzf) {
    char  c;
    if (GZIO::Read(gzf,c) && (c == k_charVal)) {
      uint8_t  uc = 0;
      if (GZIO::Read(gzf,uc) && (uc == k_ucharVal)) {
        int16_t  sh = 0;
        if (GZIO::Read(gzf,sh) && (sh == k_int16Val)) {
          uint16_t  ush = 0;
          if (GZIO::Read(gzf,ush) && (ush == k_uint16Val)) {
            int32_t  w = 0;
            if (GZIO::Read(gzf,w) && (w == k_int32Val)) {
              uint32_t uw = 0;
              if (GZIO::Read(gzf,uw) && (uw == k_uint32Val)) {
                int64_t  d = 0;
                if (GZIO::Read(gzf,d) && (d == k_int64Val)) {
                  uint64_t  ud = 0;
                  if (GZIO::Read(gzf,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (GZIO::Read(gzf,s) && (s == k_stringVal)) {
                      float  f;
                      if (GZIO::Read(gzf,f) && (f == k_floatVal)) {
                        double  df;
                        if (GZIO::Read(gzf,df) && (df == k_doubleVal)) {
                          rc = true;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReadTestBlobs(gzFile gzf, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! ReadTestBlob(gzf)) {
      rc = false;
      break;
    }
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GZIOTest()
{
  bool  rc = false;
  gzFile gzf = gzopen("/tmp/DWMGZIOTest.gz","wb");
  if (gzf) {
    if (WriteTestBlobs(gzf,3)) {
      gzclose(gzf);
      
      gzf = gzopen("/tmp/DWMGZIOTest.gz","rb");
      if (gzf) {
        if (ReadTestBlobs(gzf,3)) {
          rc = true;
        }
        gzclose(gzf);
      }
      else {
        cerr << "gzopen(\"/tmp/DWMGZIOTest.gz\",\"rb\") failed: "
             << strerror(errno) << endl;
      }
    }
    else {
      gzclose(gzf);
    }
    std::remove("/tmp/DWMGZIOTest.gz");
  }
  else {
    cerr << "gzopen(\"/tmp/DWMGZIOTest.gz\",\"wb\")"
         << " failed: " << strerror(errno) << endl;
  }
  
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MapGZIOTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMMapGZIOTest.gz");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  gzFile gzf = gzopen(fn.c_str(), "wb");
  if (gzf) {
    GZIO::Write(gzf, m1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (gzf) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (GZIO::Read(gzf, m2) > 0) {
        if (m1 == m2) {
          rc = true;
        }
      }
      else {
        cerr << "GZIO::Read(gzf, m2) failed in MapGZIOTest" << endl;
      }
      gzclose(gzf);
    }
    else {
      cerr << "Failed to gzopen '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to gzopen '" << fn.c_str() << "': " << strerror(errno)
         << endl;
  }
  
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VectorGZIOTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorGZIOTest.gz");
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (gzf) {
    GZIO::Write(gzf, v1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (gzf) {
      vector<Ipv4Prefix>  v2;
      if (GZIO::Read(gzf, v2) > 0) {
        if (v1 == v2) {
          rc = true;
        }
      }
      gzclose(gzf);
    }
    else {
      cerr << "Failed to gzopen '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to gzopen '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool DequeGZIOTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeGZIOTest.gz");
  deque<Ipv4Prefix>  d1;
  d1.push_back(Ipv4Prefix("192.168.168/24"));
  d1.push_back(Ipv4Prefix("10/8"));
  
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (gzf) {
    GZIO::Write(gzf, d1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (gzf)  {
      deque<Ipv4Prefix>  d2;
      if (GZIO::Read(gzf, d2) > 0) {
        if (d1 == d2) {
          rc = true;
        }
      }
      gzclose(gzf);
    }
    else {
      cerr << "Failed to gzopen '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to gzopen '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ListGZIOTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeGZIOTest.gz");
  list<Ipv4Prefix>  l1;
  l1.push_back(Ipv4Prefix("192.168.168/24"));
  l1.push_back(Ipv4Prefix("10/8"));
  
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (gzf)  {
    GZIO::Write(gzf, l1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (gzf)  {
      list<Ipv4Prefix>  l2;
      if (GZIO::Read(gzf, l2) > 0) {
        if (l1 == l2) {
          rc = true;
        }
      }
      gzclose(gzf);
    }
    else {
      cerr << "Failed to gzopen '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to gzopen '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool SetGZIOTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeGZIOTest.gz");
  set<Ipv4Prefix>  s1;
  s1.insert(Ipv4Prefix("192.168.168/24"));
  s1.insert(Ipv4Prefix("10/8"));
  
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (gzf)  {
    GZIO::Write(gzf, s1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (gzf)  {
      set<Ipv4Prefix>  s2;
      if (GZIO::Read(gzf, s2) > 0) {
        if (s1 == s2) {
          rc = true;
        }
      }
      gzclose(gzf);
    }
    else {
      cerr << "Failed to gzopen '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to gzopen '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestNullGzfile()
{
  // make sure we fail with null gzFile
  gzFile  gzf = 0;
  string  gzfoo("gzfoo");
  UnitAssert(GZIO::Write(gzf, gzfoo) < 0);
  int  gzint = 2345;
  UnitAssert(GZIO::Write(gzf, gzint) < 0);
  vector<int>  vi = { 1, 2, 3, 4, 20000, 100000 };
  UnitAssert(GZIO::Write(gzf, vi) < 0);
  map<int,string>  m = { 
    { 1, "one" },
    { 2, "two" },
    { 3, "three" }
  };
  UnitAssert(GZIO::Write(gzf, m) < 0);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  
  GZIOTest();
  MapGZIOTest();
  VectorGZIOTest();
  DequeGZIOTest();
  ListGZIOTest();
  SetGZIOTest();
  TestNullGzfile();

  int  rc = 1;
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    rc = 0;
  }
  
  return rc;
}
