//===========================================================================
// @(#) $DwmPath:$
// @(#) $Id:$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016, 2025
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
//!  \file TestStreamIO.cc
//!  \brief Unit tests for Dwm::StreamIO
//---------------------------------------------------------------------------
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DwmIpv4Prefix.hh"
#include "DwmStreamIO.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const char      k_charVal   = 'a';
static const uint8_t   k_ucharVal  = 0xF0;
static const int16_t   k_int16Val  = 32222;
static const uint16_t  k_uint16Val = 33333;
static const int32_t   k_int32Val  = 0x7FFFFFFF;
static const uint32_t  k_uint32Val = 0xFBFBFBFB;
static const int64_t   k_int64Val  = 4294967296LL * 15;
static const uint64_t  k_uint64Val = 4294967296LL * 65535;
static const string    k_stringVal = "TestStreamIO";
static const float     k_floatVal  = 123456789.987654321;
static const double    k_doubleVal = 987654321.123456789;
static const timeval   k_timeVal   = { 42, 0xCCCC };

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlob(ostream & os)
{
  bool  rc = false;

  if (os) {
    char  c = k_charVal;
    if (StreamIO::Write(os,c)) {
      uint8_t  uc = k_ucharVal;
      if (StreamIO::Write(os,uc)) {
        int16_t sh = k_int16Val;
        if (StreamIO::Write(os,sh)) {
          uint16_t ush = k_uint16Val;
          if (StreamIO::Write(os,ush)) {
            int32_t  w = k_int32Val;
            if (StreamIO::Write(os,w)) {
              uint32_t  uw = k_uint32Val;
              if (StreamIO::Write(os,uw)) {
                int64_t  d = k_int64Val;
                if (StreamIO::Write(os,d)) {
                  uint64_t  ud = k_uint64Val;
                  if (StreamIO::Write(os,ud)) {
                    std::string  s = k_stringVal;
                    if (StreamIO::Write(os,s)) {
                      float  f = k_floatVal;
                      if (StreamIO::Write(os,f)) {
                        double df = k_doubleVal;
                        if (StreamIO::Write(os,df)) {
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

  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlobs(ostream & os, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! WriteTestBlob(os)) {
      rc = false;
      break;
    }
  }

  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReadTestBlob(istream & is)
{
  bool  rc = false;
  if (is) {
    char  c;
    if (StreamIO::Read(is,c) && (c == k_charVal)) {
      uint8_t  uc;
      if (StreamIO::Read(is,uc) && (uc == k_ucharVal)) {
        int16_t  sh;
        if (StreamIO::Read(is,sh) && (sh == k_int16Val)) {
          uint16_t  ush;
          if (StreamIO::Read(is,ush) && (ush == k_uint16Val)) {
            int32_t  w;
            if (StreamIO::Read(is,w) && (w == k_int32Val)) {
              uint32_t uw;
              if (StreamIO::Read(is,uw) && (uw == k_uint32Val)) {
                int64_t  d;
                if (StreamIO::Read(is,d) && (d == k_int64Val)) {
                  uint64_t  ud;
                  if (StreamIO::Read(is,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (StreamIO::Read(is,s) && (s == k_stringVal)) {
                      float  f;
                      if (StreamIO::Read(is,f) && (f == k_floatVal)) {
                        double  df;
                        if (StreamIO::Read(is,df) && (df == k_doubleVal)) {
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

  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReadTestBlobs(istream & is, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! ReadTestBlob(is)) {
      rc = false;
      break;
    }
  }
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool StreamTest()
{
  bool  rc = false;
  
  ofstream  os("/tmp/DwmIOStreamTest");
  if (WriteTestBlobs(os,3)) {
    os.close();

    ifstream  is("/tmp/DwmIOStreamTest");
    if (is) {
      if (ReadTestBlobs(is,3)) {
        rc = true;
      }
      is.close();
    }
  }
  std::remove("/tmp/DwmIOStreamTest");
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MapStreamTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMMapStreamTest");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, m1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (StreamIO::Read(is, m2)) {
        if (m1 == m2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove("/tmp/DWMMapStreamTest");
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "': " << strerror(errno)
         << endl;
  }
  
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MapStreamTestSimple()
{
  bool  rc = false;

  string  fn("/tmp/DWMMapStreamTestSimple");
  
  map<int,string>  m1;
  m1[1] = "192.168.168/24";
  m1[2] = "10/8";
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, m1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      map<int,string>  m2;
      if (StreamIO::Read(is, m2)) {
        if (m1 == m2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "': " << strerror(errno)
         << endl;
  }
  
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VectorStreamTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorStreamTest");
  
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, v1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      vector<Ipv4Prefix>  v2;
      if (StreamIO::Read(is, v2)) {
        if (v1 == v2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ArrayStreamTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMArrayStreamTest");
  
  array<Ipv4Prefix,2>
    a1({Ipv4Prefix("192.168.168/24"),
        Ipv4Prefix("10/8")});
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, a1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      array<Ipv4Prefix,2>  a2;
      if (StreamIO::Read(is, a2)) {
        if (a1 == a2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool DequeStreamTest()
{
  bool  rc = false;

  string fn("/tmp/DWMDequeStreamTest");
  deque<Ipv4Prefix>  d1;
  d1.push_back(Ipv4Prefix("192.168.168/24"));
  d1.push_back(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, d1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      deque<Ipv4Prefix>  d2;
      if (StreamIO::Read(is, d2)) {
        if (d1 == d2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }

  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ListStreamTest()
{
  bool  rc = false;

  string fn("/tmp/DWMListStreamTest");
  list<Ipv4Prefix>  l1;
  l1.push_back(Ipv4Prefix("192.168.168/24"));
  l1.push_back(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, l1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      list<Ipv4Prefix>  l2;
      if (StreamIO::Read(is, l2)) {
        if (l1 == l2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool SetStreamTest()
{
  bool  rc = false;

  string fn("/tmp/DWMSetStreamTest");
  set<Ipv4Prefix>  s1;
  s1.insert(Ipv4Prefix("192.168.168/24"));
  s1.insert(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    StreamIO::Write(os, s1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      set<Ipv4Prefix>  s2;
      if (StreamIO::Read(is, s2)) {
        if (s1 == s2) {
          rc = true;
        }
      }
      is.close();
    }
    else {
      cerr << "Failed to open '" << fn.c_str() << "' for reading: "
           << strerror(errno) << endl;
    }
    std::remove(fn.c_str());
  }
  else {
    cerr << "Failed to open '" << fn.c_str() << "' for writing: "
         << strerror(errno) << endl;
  }
  
  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VarArgStreamTest()
{
  bool  rc = false;
  
  std::string  s("Hello");
  uint16_t     u = 0xf00f;
  bool         b = true;
  int32_t      i = -2020;
  pair<string,bool>  p("Goodbye", false);

  ostringstream  oss;
  if (UnitAssert(StreamIO::WriteV(oss, s, u, b, i, p))) {
    istringstream      iss(oss.str());
    std::string        s2;
    uint16_t           u2;
    bool               b2;
    int32_t            i2;
    pair<string,bool>  p2;
    if (UnitAssert(StreamIO::ReadV(iss, s2, u2, b2, i2, p2))) {
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
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VarArgStreamTestFail()
{
  bool  rc = false;
  std::string  s("HeLlO");
  ostringstream  oss;
  if (UnitAssert(StreamIO::WriteV(oss, s))) {
    istringstream      iss(oss.str());
    std::string        s2;
    uint16_t           u2;
    if (UnitAssert(! StreamIO::ReadV(iss, s2, u2))) {
      //  We expect ReadV() to fail because we only wrote a string, so the
      //  stream should end before we can read u2.  But we should have
      //  successfully read s2.
      rc = UnitAssert(s == s2);
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool BoundedArrayStreamTest()
{
  bool  rc = false;
  
  int  ia1[5] = { 1,2,3,4,5 };
  static_assert(std::is_bounded_array_v<decltype(ia1)>);
  stringstream  ss;
  if (UnitAssert(StreamIO::Write(ss, ia1))) {
    int  ia2[5];
    if (UnitAssert(StreamIO::Read(ss, ia2))) {
      rc = true;
      for (size_t i = 0; i < 5; ++i) {
        if (! UnitAssert(ia1[i] == ia2[i])) {
          rc = false;
          goto done;
        }
      }
    }
  }

  if (rc) {
    int  ib1[3][3] = { { 1,2,3 }, { 4,5,6 }, { 7,8,9 } };
    static_assert(std::is_bounded_array_v<decltype(ib1)>);
    if (UnitAssert(StreamIO::Write(ss, ib1))) {
      int  ib2[3][3];
      if (UnitAssert(StreamIO::Read(ss, ib2))) {
        rc = true;
        for (size_t i = 0; i < 3; ++i) {
          for (size_t j = 0; j < 3; ++j) {
            if (! UnitAssert(ib1[i][j] == ib2[i][j])) {
              rc = false;
              break;
            }
          }
          if (! rc) {
            break;
          }
        }
      }
    }
  }

  if (rc) {
    int  ic1[][3] = { {2,3,4}, {6,7,8}, {10,11,12} };
    static_assert(std::is_bounded_array_v<decltype(ic1)>);
    rc = false;
    if (UnitAssert(StreamIO::Write(ss, ic1))) {
      int  ic2[][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
      if (UnitAssert(StreamIO::Read(ss, ic2))) {
        rc = true;
        for (size_t i = 0; i < 3; ++i) {
          for (size_t j = 0; j < 3; ++j) {
            if (! UnitAssert(ic1[i][j] == ic2[i][j])) {
              rc = false;
              break;
            }
          }
          if (! rc) {
            break;
          }
        }
      }
    }
  }
  
done:
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestStreamUniquePtr()
{
  bool  rc = false;
  std::unique_ptr<std::string>  sp1 = make_unique<std::string>("hello");
  stringstream  ss;
  if (UnitAssert(StreamIO::Write(ss, sp1))) {
    std::unique_ptr<std::string>  sp2;
    if (UnitAssert(StreamIO::Read(ss, sp2))) {
      rc = (*sp2 == *sp1);
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestStreamOptional()
{
  bool  rc = false;
  std::optional<std::string>  so1 = "hello";
  stringstream  ss;
  if (UnitAssert(StreamIO::Write(ss, so1))) {
    std::optional<std::string>  so2;
    if (UnitAssert(StreamIO::Read(ss, so2))) {
      if (UnitAssert(so2 == so1)) {
        rc = true;
      }
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestTupleWritable()
{
  UnitAssert((IsStreamWritable<std::tuple<int,std::string,int,std::pair<int,std::string>>>));
  UnitAssert((IsStreamWritable<std::tuple<int,string,int>>));
  UnitAssert((! IsStreamWritable<std::tuple<int,const string>>));
  UnitAssert((! IsStreamReadable<std::tuple<int,const string>>));
  UnitAssert((! IsStreamWritable<std::tuple<const string>>));
  return;
}

#if defined(DWM_CAN_USE_REFLECTION)
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReflectionSkipTest()
{
  bool  rc = false;
  
  struct Skip1_t {
    [[=Dwm::skip_io]] int  i;
    int                    j;
  };

  struct Skip2_t {
    string                 s;
    [[=Dwm::skip_io]] int  k;
  };

  struct Skip1_2_t {
    Skip1_t  sk1;
    Skip2_t  sk2;
  };
  
  if (UnitAssert(IsStreamWritable<Skip1_2_t>)) {
    const Skip1_2_t  sk1_2_1 = { {42, 99}, {"hello", 0xCCCC} };
    Skip1_2_t        sk1_2_2 = { {77, 0},  {"goodbye", 55 } };
    stringstream  ss;
    if (UnitAssert(StreamIO::Write(ss, sk1_2_1))) {
      if (UnitAssert(StreamIO::Read(ss, sk1_2_2))) {
        if (UnitAssert(sk1_2_2.sk1.j == sk1_2_1.sk1.j)) {
          if (UnitAssert(sk1_2_2.sk1.i == 77)) {
            if (UnitAssert(sk1_2_2.sk2.s == sk1_2_1.sk2.s)) {
              if (UnitAssert(sk1_2_2.sk2.k == 55)) {
                rc = true;
              }
            }
          }
        }
      }
    }
  }
  typedef struct {
    [[=Dwm::skip_io]] Skip1_t  sk1;
    Skip2_t                    sk2;
  } Skip3_t;

  if (UnitAssert(IsStreamWritable<Skip3_t>)) {
    const Skip3_t  sk3_1 = { {99, 42}, {"goodbye", 0xAAAA} };
    Skip3_t        sk3_2 = { {33, 88}, {"???",     0x1010} };
    stringstream  ss;
    if (UnitAssert(StreamIO::Write(ss, sk3_1))) {
      if (UnitAssert(StreamIO::Read(ss, sk3_2))) {
        UnitAssert(sk3_2.sk1.i == 33);
        UnitAssert(sk3_2.sk1.j == 88);
        UnitAssert(sk3_2.sk2.s == sk3_1.sk2.s);
        UnitAssert(sk3_2.sk2.k == 0x1010);
      }
    }
  }
        
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MembersWritableTest()
{
  bool  rc = true;

  //  Not writable: contains a pointer
  typedef struct {
    int  *ip;
  } UnwritableStruct1;
  rc &= UnitAssert(! IsStreamWritable<UnwritableStruct1>);

  //  writable: contains a std::mutex, which is skipped
  typedef struct {
    std::mutex  mtx;
  } UnwritableStruct2;
  rc &= UnitAssert(IsStreamWritable<UnwritableStruct2>);

  //  Not readable (contains a const member), hence not writable
  typedef struct {
    const int i;
  } UnwritableStruct3;
  rc &= UnitAssert(! IsStreamWritable<UnwritableStruct3>);

  typedef struct {
    UnwritableStruct1  us1;
    UnwritableStruct2  us2;
  } UnwritableStruct1_2;
  rc &= UnitAssert(! IsStreamWritable<UnwritableStruct1_2>);

  typedef struct {
    int     a;
    int     b;
    string  c;
  } WritableStruct1;
  rc &= UnitAssert(IsStreamWritable<WritableStruct1>);

  typedef struct {
    string  a;
    int     b;
    string  c;
  } WritableStruct2;
  rc &= UnitAssert(IsStreamWritable<WritableStruct2>);

  typedef struct {
    WritableStruct1  s1;
    WritableStruct2  s2;
  } WritableStruct1_2;
  rc &= UnitAssert(IsStreamWritable<WritableStruct1_2>);

  WritableStruct1_2  ws1 = { { 42, 0xCCCC, "hello"}, { "hi", 99, "goodbye" } };
  stringstream  ss;
  rc &= UnitAssert(StreamIO::Write(ss, ws1));
  WritableStruct1_2  ws2;
  rc &= UnitAssert(StreamIO::Read(ss, ws2));
  rc &= UnitAssert((ws1.s1.a == ws2.s1.a)
                   && (ws1.s1.b == ws2.s1.b)
                   && (ws1.s1.c == ws2.s1.c)
                   && (ws1.s2.a == ws2.s2.a)
                   && (ws1.s2.b == ws2.s2.b)
                   && (ws1.s2.c == ws2.s2.c));

  UnwritableStruct1  uws1_1, uws1_2;
  rc &= UnitAssert(! StreamIO::Write(ss, uws1_1));
  rc &= UnitAssert(! StreamIO::Read(ss, uws1_2));
  UnwritableStruct2  uws2_1, uws2_2;
  rc &= UnitAssert(! StreamIO::Write(ss, uws2_1));
  rc &= UnitAssert(! StreamIO::Read(ss, uws2_2));
  UnwritableStruct3  uws3_1 = {42}, uws3_2 = {0};
  rc &= UnitAssert(! StreamIO::Write(ss, uws3_1));
  rc &= UnitAssert(! StreamIO::Read(ss, uws3_2));
  
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReflectionStreamTest()
{
  bool  rc = false;

  typedef struct {
    int     a;
    int     b;
    string  c;
    // std::mutex  d;
    struct timeval  tv;
    std::vector<int>  vi;
    std::map<int,int>  mi;
    
  } ReflTestStruct;

  UnitAssert((iostream_detail::Writable<ReflTestStruct>()));
  
  ReflTestStruct  rts1{9,42,"ReflectionStreamTest",{42,0xCCCC},{6,7,8},
                       {{1,2},{3,4}}};
  stringstream  ss;
  if (UnitAssert(StreamIO::Write(ss, rts1))) {
    ReflTestStruct  rts2;
    if (UnitAssert(StreamIO::Read(ss, rts2))) {
      if (UnitAssert(rts1.a == rts2.a)
          && UnitAssert(rts1.b == rts2.b)
          && UnitAssert(rts1.c == rts2.c)
          && UnitAssert(rts1.tv.tv_sec == rts2.tv.tv_sec)
          && UnitAssert(rts1.tv.tv_usec == rts2.tv.tv_usec)
          && UnitAssert(rts1.vi == rts2.vi)) {
        rc = true;
      }
    }
  }

  struct DenyOneMember {
    [[=deny_io]] int  a;
  };

  DenyOneMember  dom;
  rc &= UnitAssert((! iostream_detail::Writable<DenyOneMember>()));
      
  return rc;
}

#endif  //  defined(DWM_CAN_USE_REFLECTION)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  SysLogger::Open("TestIO", LOG_PERROR, LOG_USER);
  SysLogger::MinimumPriority(LOG_INFO);

  StreamTest();
  MapStreamTestSimple();
  MapStreamTest();
  VectorStreamTest();
  ArrayStreamTest();
  DequeStreamTest();
  ListStreamTest();
  SetStreamTest();
  VarArgStreamTest();
  UnitAssert(VarArgStreamTestFail());
  BoundedArrayStreamTest();
  TestStreamUniquePtr();
  TestStreamOptional();
  TestTupleWritable();
#if defined(DWM_CAN_USE_REFLECTION)
  ReflectionSkipTest();
  ReflectionStreamTest();
  MembersWritableTest();
#endif
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
