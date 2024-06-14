//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmGZIO.cc 9112 $
// @(#) $Id: TestDwmGZIO.cc 9112 2017-04-14 22:34:43Z dwm $
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
#include "DwmIOUtils.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmGZIO.cc 9112 $");

static const char      k_charVal   = 'a';
static const uint8_t   k_ucharVal  = 0xF0;
static const bool      k_boolVal   = true;
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
    rc = true;
    
    char         c = k_charVal;
    uint8_t      uc = k_ucharVal;
    bool         b = k_boolVal;
    int16_t      sh = k_int16Val;
    uint16_t     ush = k_uint16Val;
    int32_t      w = k_int32Val;
    uint32_t     uw = k_uint32Val;
    int64_t      d = k_int64Val;
    uint64_t     ud = k_uint64Val;
    std::string  s = k_stringVal;
    float        f = k_floatVal;
    double       df = k_doubleVal;
    rc &= UnitAssert(GZIO::Write(gzf,c) == sizeof(c));
    rc &= UnitAssert(GZIO::Write(gzf,uc) == sizeof(uc));
    rc &= UnitAssert(GZIO::Write(gzf,b) == sizeof(b));
    rc &= UnitAssert(GZIO::Write(gzf,sh) == sizeof(sh));
    rc &= UnitAssert(GZIO::Write(gzf,ush) == sizeof(ush));
    rc &= UnitAssert(GZIO::Write(gzf,w) == sizeof(w));
    rc &= UnitAssert(GZIO::Write(gzf,uw) == sizeof(uw));
    rc &= UnitAssert(GZIO::Write(gzf,d) == sizeof(d));
    rc &= UnitAssert(GZIO::Write(gzf,ud) == sizeof(ud));
    rc &= UnitAssert(GZIO::Write(gzf,s) > 4);
    rc &= UnitAssert(GZIO::Write(gzf,f) == sizeof(f));
    rc &= UnitAssert(GZIO::Write(gzf,df) == sizeof(df));
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
  
  char      c;
  uint8_t   uc = 0;
  bool      b = false;
  int16_t   sh = 0;
  uint16_t  ush = 0;
  int32_t   w = 0;
  uint32_t  uw = 0;
  int64_t   d = 0;
  uint64_t  ud = 0;
  string    s;
  float     f;
  double    df;
  if (gzf) {
    rc = true;
    rc &= UnitAssert((GZIO::Read(gzf,c) == sizeof(c)) && (c == k_charVal));
    rc &= UnitAssert((GZIO::Read(gzf,uc) == sizeof(uc)) && (uc == k_ucharVal));
    rc &= UnitAssert((GZIO::Read(gzf,b) == sizeof(b)) && (b == k_boolVal));
    rc &= UnitAssert((GZIO::Read(gzf,sh) == sizeof(sh)) && (sh == k_int16Val));
    rc &= UnitAssert((GZIO::Read(gzf,ush) == sizeof(ush))
                     && (ush == k_uint16Val));
    rc &= UnitAssert((GZIO::Read(gzf,w) == sizeof(w)) && (w == k_int32Val));
    rc &= UnitAssert((GZIO::Read(gzf,uw) == sizeof(uw))
                     && (uw == k_uint32Val));
    rc &= UnitAssert((GZIO::Read(gzf,d) == sizeof(d)) && (d == k_int64Val));
    rc &= UnitAssert((GZIO::Read(gzf,ud) == sizeof(ud))
                     && (ud == k_uint64Val));
    rc &= UnitAssert((GZIO::Read(gzf,s) > 0) && (s == k_stringVal));
    rc &= UnitAssert((GZIO::Read(gzf,f) == sizeof(f)) && (f == k_floatVal));
    rc &= UnitAssert((GZIO::Read(gzf,df) == sizeof(df))
                     && (df == k_doubleVal));
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
      gzFile  gzf2 = gzopen("/tmp/DWMGZIOTest.gz","rb");
      if (gzf2) {
        if (ReadTestBlobs(gzf2,3)) {
          rc = true;
        }
        gzclose(gzf2);
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
static bool TupleGZIOTest()
{
  bool  rc = false;
  string  fn("/tmp/DwmTupleGZIOTest.gz");
  tuple<string,int,string,int>  t1("alpha",1,"beta",2);
  gzFile gzf = gzopen(fn.c_str(), "wb");
  if (UnitAssert(gzf)) {
    GZIO::Write(gzf, t1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (UnitAssert(gzf)) {
      tuple<string,int,string,int>  t2;
      if (UnitAssert(GZIO::Read(gzf, t2) > 0)) {
        if (UnitAssert(t2 == t1)) {
          rc = true;
        }
      }
      gzclose(gzf);
    }
    std::remove(fn.c_str());
  }

  fn = "/dev/null";
  gzf = gzopen(fn.c_str(), "rb");
  if (UnitAssert(gzf)) {
    tuple<string,int,string,int>  t2;
    UnitAssert(GZIO::Read(gzf, t2) == -1);
    gzclose(gzf);
  }
  
  return rc;
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
static bool ArrayGZIOTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMArrayGZIOTest.gz");
  array<Ipv4Prefix,2>
    a1({Ipv4Prefix("192.168.168/24"),
        Ipv4Prefix("10/8")});
  
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (gzf) {
    GZIO::Write(gzf, a1);
    gzclose(gzf);

    gzf = gzopen(fn.c_str(), "rb");
    if (gzf) {
      array<Ipv4Prefix,2>  a2;
      if (GZIO::Read(gzf, a2) > 0) {
        if (a1 == a2) {
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
  tuple<string,int,string,int>  t("beta",2,"alpha",1);
  UnitAssert(GZIO::Write(gzf, t) < 0);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VarArgGZIOTest()
{
  bool  rc = false;
  
  std::string  s("Hello");
  uint16_t     u = 0xf00f;
  bool         b = true;
  int32_t      i = -2020;
  pair<string,bool>  p("Goodbye", false);

  string  fn("/tmp/DWMVarArgGZIOTest");
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (gzf)  {
    rc = UnitAssert(GZIO::WriteV(gzf, s, u, b, i, p)
                    == IOUtils::StreamedLengthV(s, u, b, i, p));
    gzclose(gzf);
    if (rc) {
      gzf = gzopen(fn.c_str(), "rb");
      if (gzf) {
        std::string        s2;
        uint16_t           u2;
        bool               b2;
        int32_t            i2;
        pair<string,bool>  p2;
        if (UnitAssert(GZIO::ReadV(gzf, s2, u2, b2, i2, p2)
                       == IOUtils::StreamedLengthV(s, u, b, i, p))) {
          if (UnitAssert(s == s2)) {
            if (UnitAssert(u == u2)) {
              if (UnitAssert(b == b2)) {
                if (UnitAssert(i == i2)) {
                  if (UnitAssert(p == p2)) {
                    rc = true;
                  }
                }
              }
            }
          }
        }
        gzclose(gzf);
      }
    }
    std::remove(fn.c_str());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VarArgGZIOTestFail()
{
  bool  rc = false;
  std::string  s("HeLlO");
  
  string  fn("/tmp/DWMVarArgGZIOTestFail");
  gzFile  gzf = gzopen(fn.c_str(), "wb");
  if (UnitAssert(gzf))  {
    if (UnitAssert(GZIO::WriteV(gzf, s))) {
      gzclose(gzf);
      gzf = gzopen(fn.c_str(), "r");
      if (UnitAssert(gzf)) {
        std::string        s2;
        uint16_t           u2;
        if (UnitAssert(GZIO::ReadV(gzf, s2, u2) < 0)) {
          //  We expect ReadV() to fail because we only wrote a string, so the
          //  stream should end before we can read u2.  But we should have
          //  successfully read s2.
          rc = UnitAssert(s == s2);
        }
        gzclose(gzf);
      }
    }
    else {
      gzclose(gzf);
    }
    std::remove(fn.c_str());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  GZIOTest();
  ArrayGZIOTest();
  TupleGZIOTest();
  MapGZIOTest();
  VectorGZIOTest();
  DequeGZIOTest();
  ListGZIOTest();
  SetGZIOTest();
  TestNullGzfile();
  VarArgGZIOTest();
  VarArgGZIOTestFail();
  
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
