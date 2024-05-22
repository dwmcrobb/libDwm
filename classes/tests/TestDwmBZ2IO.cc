//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmBZ2IO.cc 11071 $
// @(#) $Id: TestDwmBZ2IO.cc 11071 2020-09-05 15:20:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016, 2024
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
//!  \file TestDwmBZ2IO.cc
//!  \brief Unit tests for Dwm::BZ2IO
//---------------------------------------------------------------------------

//  This program is just a simple test application for functions in Dwm::BZ2IO.

extern "C" {
  #include <assert.h>
  #include <fcntl.h>
  #include <errno.h>
}

#include <cstdio>
#include <fstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmBZ2IO.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmBZ2IO.cc 11071 $");

static const char      k_charVal   = 'a';
static const uint8_t   k_ucharVal  = 0xF0;
static const bool      k_boolVal   = true;
static const int16_t   k_int16Val  = 32222;
static const uint16_t  k_uint16Val = 33333;
static const int32_t   k_int32Val  = 0x7FFFFFFF;
static const uint32_t  k_uint32Val = 0xFBFBFBFB;
static const int64_t   k_int64Val  = 4294967296LL * 15;
static const uint64_t  k_uint64Val = 4294967296LL * 65535;
static const string    k_stringVal = "TestDwmBZ2IO";
static const float     k_floatVal  = 123456789.987654321;
static const double    k_doubleVal = 987654321.123456789;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlob(BZFILE *bzf)
{
  bool  rc = false;

  if (bzf) {
    char  c = k_charVal;
    if (BZ2IO::BZWrite(bzf,c) == sizeof(c)) {
      uint8_t  uc = k_ucharVal;
      if (BZ2IO::BZWrite(bzf,uc) == sizeof(uc)) {
        bool  b = k_boolVal;
        if (BZ2IO::BZWrite(bzf,b) == sizeof(b)) {
          int16_t sh = k_int16Val;
          if (BZ2IO::BZWrite(bzf,sh) == sizeof(sh)) {
            uint16_t ush = k_uint16Val;
            if (BZ2IO::BZWrite(bzf,ush) == sizeof(ush)) {
              int32_t  w = k_int32Val;
              if (BZ2IO::BZWrite(bzf,w) == sizeof(w)) {
                uint32_t  uw = k_uint32Val;
                if (BZ2IO::BZWrite(bzf,uw) == sizeof(uw)) {
                  int64_t  d = k_int64Val;
                  if (BZ2IO::BZWrite(bzf,d) == sizeof(d)) {
                    uint64_t  ud = k_uint64Val;
                    if (BZ2IO::BZWrite(bzf,ud) == sizeof(ud)) {
                      std::string  s = k_stringVal;
                      if (BZ2IO::BZWrite(bzf,s) > 4) {
                        float  f = k_floatVal;
                        if (BZ2IO::BZWrite(bzf,f) == sizeof(f)) {
                          double  df = k_doubleVal;
                          if (BZ2IO::BZWrite(bzf,df) == sizeof(df)) {
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
  }
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlobs(BZFILE *bzf, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! WriteTestBlob(bzf)) {
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
static bool ReadTestBlob(BZFILE *bzf)
{
  bool  rc = false;
  if (bzf) {
    char  c;
    if (BZ2IO::BZRead(bzf,c) && (c == k_charVal)) {
      uint8_t  uc = 0;
      if (BZ2IO::BZRead(bzf,uc) && (uc == k_ucharVal)) {
        bool  b = false;
        if (BZ2IO::BZRead(bzf,b) && (b == k_boolVal)) {
          int16_t  sh = 0;
          if (BZ2IO::BZRead(bzf,sh) && (sh == k_int16Val)) {
            uint16_t  ush = 0;
            if (BZ2IO::BZRead(bzf,ush) && (ush == k_uint16Val)) {
              int32_t  w = 0;
              if (BZ2IO::BZRead(bzf,w) && (w == k_int32Val)) {
                uint32_t uw = 0;
                if (BZ2IO::BZRead(bzf,uw) && (uw == k_uint32Val)) {
                  int64_t  d = 0;
                  if (BZ2IO::BZRead(bzf,d) && (d == k_int64Val)) {
                    uint64_t  ud = 0;
                    if (BZ2IO::BZRead(bzf,ud) && (ud == k_uint64Val)) {
                      string  s;
                      if (BZ2IO::BZRead(bzf,s) && (s == k_stringVal)) {
                        float  f;
                        if (BZ2IO::BZRead(bzf,f) && (f == k_floatVal)) {
                          double  df;
                          if (BZ2IO::BZRead(bzf,df) && (df == k_doubleVal)) {
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
  }
  
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReadTestBlobs(BZFILE *bzf, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! ReadTestBlob(bzf)) {
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
static bool BZ2IOTest()
{
  bool  rc = false;
  BZFILE  *bzf = BZ2_bzopen("/tmp/DWMBZ2IOTest.bz2","wb");
  if (bzf) {
    if (WriteTestBlobs(bzf,3)) {
      BZ2_bzclose(bzf);
      
      bzf = BZ2_bzopen("/tmp/DWMBZ2IOTest.bz2","rb");
      if (bzf) {
        if (ReadTestBlobs(bzf,3)) {
          rc = true;
        }
        BZ2_bzclose(bzf);
      }
      else {
        cerr << "gzopen(\"/tmp/DWMBZ2IOTest.bz2\",\"rb\") failed: "
             << strerror(errno) << endl;
      }
    }
    else {
      BZ2_bzclose(bzf);
    }
    std::remove("/tmp/DWMBZ2IOTest.bz2");
  }
  else {
    cerr << "gzopen(\"/tmp/DWMBZ2IOTest.bz2\",\"wb\")"
         << " failed: " << strerror(errno) << endl;
  }
  
  UnitAssert(rc);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MapBZ2IOTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMMapBZ2IOTest.bz2");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  BZFILE  *bzf = BZ2_bzopen(fn.c_str(), "wb");
  if (bzf) {
    BZ2IO::BZWrite(bzf, m1);
    BZ2_bzclose(bzf);

    bzf = BZ2_bzopen(fn.c_str(), "rb");
    if (bzf) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (BZ2IO::BZRead(bzf, m2) > 0) {
        if (m1 == m2) {
          rc = true;
        }
      }
      else {
        cerr << "BZ2IO::BZRead(bzf, m2) failed in MapBZ2IOTest" << endl;
      }
      BZ2_bzclose(bzf);
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
static bool VectorBZ2IOTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorBZ2IOTest.bz2");
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  BZFILE  *bzf = BZ2_bzopen(fn.c_str(), "wb");
  if (bzf) {
    BZ2IO::BZWrite(bzf, v1);
    BZ2_bzclose(bzf);

    bzf = BZ2_bzopen(fn.c_str(), "rb");
    if (bzf) {
      vector<Ipv4Prefix>  v2;
      if (BZ2IO::BZRead(bzf, v2) > 0) {
        if (v1 == v2) {
          rc = true;
        }
      }
      BZ2_bzclose(bzf);
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
static bool DequeBZ2IOTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeBZ2IOTest.bz2");
  deque<Ipv4Prefix>  d1;
  d1.push_back(Ipv4Prefix("192.168.168/24"));
  d1.push_back(Ipv4Prefix("10/8"));
  
  BZFILE  *bzf = BZ2_bzopen(fn.c_str(), "wb");
  if (bzf) {
    BZ2IO::BZWrite(bzf, d1);
    BZ2_bzclose(bzf);

    bzf = BZ2_bzopen(fn.c_str(), "rb");
    if (bzf)  {
      deque<Ipv4Prefix>  d2;
      if (BZ2IO::BZRead(bzf, d2) > 0) {
        if (d1 == d2) {
          rc = true;
        }
      }
      BZ2_bzclose(bzf);
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
static bool ListBZ2IOTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeBZ2IOTest.bz2");
  list<Ipv4Prefix>  l1;
  l1.push_back(Ipv4Prefix("192.168.168/24"));
  l1.push_back(Ipv4Prefix("10/8"));
  
  BZFILE  *bzf = BZ2_bzopen(fn.c_str(), "wb");
  if (bzf)  {
    BZ2IO::BZWrite(bzf, l1);
    BZ2_bzclose(bzf);

    bzf = BZ2_bzopen(fn.c_str(), "rb");
    if (bzf)  {
      list<Ipv4Prefix>  l2;
      if (BZ2IO::BZRead(bzf, l2) > 0) {
        if (l1 == l2) {
          rc = true;
        }
      }
      BZ2_bzclose(bzf);
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
static bool SetBZ2IOTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeBZ2IOTest.bz2");
  set<Ipv4Prefix>  s1;
  s1.insert(Ipv4Prefix("192.168.168/24"));
  s1.insert(Ipv4Prefix("10/8"));
  
  BZFILE  *bzf = BZ2_bzopen(fn.c_str(), "wb");
  if (bzf)  {
    BZ2IO::BZWrite(bzf, s1);
    BZ2_bzclose(bzf);

    bzf = BZ2_bzopen(fn.c_str(), "rb");
    if (bzf)  {
      set<Ipv4Prefix>  s2;
      if (BZ2IO::BZRead(bzf, s2) > 0) {
        if (s1 == s2) {
          rc = true;
        }
      }
      BZ2_bzclose(bzf);
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
void TestNullBZ2File()
{
  // make sure we fail with null BZFILE
  BZFILE  *bzf = 0;
  string   bzfoo("bz2foo");
  UnitAssert(BZ2IO::BZWrite(bzf, bzfoo) < 0);
  int  bzint = 2345;
  UnitAssert(BZ2IO::BZWrite(bzf, bzint) < 0);
  vector<int>  vi = { 1, 2, 3, 4, 20000, 100000 };
  UnitAssert(BZ2IO::BZWrite(bzf, vi) < 0);
  map<int,string>  m = { 
    { 1, "one" },
    { 2, "two" },
    { 3, "three" }
  };
  UnitAssert(BZ2IO::BZWrite(bzf, m) < 0);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void VariantBZ2IOTest()
{
  string   fn("./VariantBZ2IOTest.bz2");
  BZFILE  *bzf = BZ2_bzopen(fn.c_str(), "wb");
  if (UnitAssert(bzf)) {
    std::variant<std::monostate,int,std::string>  v1;
    if (UnitAssert(BZ2IO::BZWrite(bzf, v1) >= 0)) {
      v1 = 42;
      if (UnitAssert(BZ2IO::BZWrite(bzf, v1) >= 0)) {
        v1 = "hello";
        UnitAssert(BZ2IO::BZWrite(bzf, v1));
      }
    }
    BZ2_bzclose(bzf);
  }

  bzf = BZ2_bzopen(fn.c_str(), "rb");
  if (UnitAssert(bzf)) {
    std::variant<std::monostate,int,std::string>  v2;
    if (UnitAssert(BZ2IO::BZRead(bzf, v2) >= 0)) {
      if (UnitAssert(v2.index() == 0)) {
        if (UnitAssert(BZ2IO::BZRead(bzf, v2) >= 0)) {
          UnitAssert(v2.index() == 1);
          UnitAssert(42 == std::get<1>(v2));
          if (UnitAssert(BZ2IO::BZRead(bzf, v2) >= 0)) {
            UnitAssert(v2.index() == 2);
            UnitAssert(string("hello") == std::get<2>(v2));
          }
        }
      }
    }
    BZ2_bzclose(bzf);
  }
  std::remove(fn.c_str());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  
  BZ2IOTest();
  MapBZ2IOTest();
  VectorBZ2IOTest();
  DequeBZ2IOTest();
  ListBZ2IOTest();
  SetBZ2IOTest();
  TestNullBZ2File();
  VariantBZ2IOTest();
  
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
