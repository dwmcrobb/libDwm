//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2025
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
//!  \file TestDescriptorIO.cc
//!  \author Daniel W. McRobb
//!  \brief Unit tests for Dwm::DescriptorIO
//---------------------------------------------------------------------------

#include "DwmDescriptorIO.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmTypeName.hh"
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
static const string    k_stringVal = "TestDwmIO";
static const float     k_floatVal  = 123456789.987654321;
static const double    k_doubleVal = 987654321.123456789;
static const timeval   k_timeVal   = { 42, 0xCCCC };

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlob(int fd)
{
  bool  rc = false;

  if (fd >= 0) {
    char  c = k_charVal;
    if (DescriptorIO::Write(fd,c) == sizeof(c)) {
      uint8_t  uc = k_ucharVal;
      if (DescriptorIO::Write(fd,uc) == sizeof(uc)) {
        int16_t sh = k_int16Val;
        if (DescriptorIO::Write(fd,sh) == sizeof(sh)) {
          uint16_t ush = k_uint16Val;
          if (DescriptorIO::Write(fd,ush) == sizeof(ush)) {
            int32_t  w = k_int32Val;
            if (DescriptorIO::Write(fd,w) == sizeof(w)) {
              uint32_t  uw = k_uint32Val;
              if (DescriptorIO::Write(fd,uw) == sizeof(uw)) {
                int64_t  d = k_int64Val;
                if (DescriptorIO::Write(fd,d) == sizeof(d)) {
                  uint64_t  ud = k_uint64Val;
                  if (DescriptorIO::Write(fd,ud) == sizeof(ud)) {
                    std::string  s = k_stringVal;
                    if (DescriptorIO::Write(fd,s) > 4) {
                      float  f = k_floatVal;
                      if (DescriptorIO::Write(fd,f) == sizeof(f)) {
                        double  df = k_doubleVal;
                        if (DescriptorIO::Write(fd,df) == sizeof(df)) {
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
static bool WriteTestBlobs(int fd, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! WriteTestBlob(fd)) {
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
static bool ReadTestBlob(int fd)
{
  bool  rc = false;
  if (fd >= 0) {
    char  c;
    if (DescriptorIO::Read(fd,c) && (c == k_charVal)) {
      uint8_t  uc = 0;
      if (DescriptorIO::Read(fd,uc) && (uc == k_ucharVal)) {
        int16_t  sh = 0;
        if (DescriptorIO::Read(fd,sh) && (sh == k_int16Val)) {
          uint16_t  ush = 0;
          if (DescriptorIO::Read(fd,ush) && (ush == k_uint16Val)) {
            int32_t  w = 0;
            if (DescriptorIO::Read(fd,w) && (w == k_int32Val)) {
              uint32_t uw = 0;
              if (DescriptorIO::Read(fd,uw) && (uw == k_uint32Val)) {
                int64_t  d = 0;
                if (DescriptorIO::Read(fd,d) && (d == k_int64Val)) {
                  uint64_t  ud = 0;
                  if (DescriptorIO::Read(fd,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (DescriptorIO::Read(fd,s) && (s == k_stringVal)) {
                      float  f;
                      if (DescriptorIO::Read(fd,f) && (f == k_floatVal)) {
                        double  df;
                        if (DescriptorIO::Read(fd,df) && (df == k_doubleVal)) {
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
static bool ReadTestBlobs(int fd, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! ReadTestBlob(fd)) {
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
static bool DescriptorTest()
{
  bool  rc = false;
  int  fd = open("/tmp/DWMIODescriptorTest",O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (fd >= 0) {
    if (WriteTestBlobs(fd,3)) {
      close(fd);
      
      fd = open("/tmp/DWMIODescriptorTest",O_RDONLY);
      if (fd >= 0) {
        if (ReadTestBlobs(fd,3)) {
          rc = true;
        }
        close(fd);
      }
      else {
        cerr << "open(\"/tmp/DWMIODescriptorTest\",O_RDONLY) failed: "
             << strerror(errno) << endl;
      }
    }
    else {
      close(fd);
    }
    std::remove("/tmp/DWMIODescriptorTest");
  }
  else {
    cerr << "open(\"/tmp/DWMIODescriptorTest\",O_WRONLY|O_CREAT|O_TRUNC,0644)"
         << " failed: " << strerror(errno) << endl;
  }

  UnitAssert(rc == true);
  return(rc);
}
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MapDescriptorTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMMapDescriptorTest");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  int  fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    DescriptorIO::Write(fd, m1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (DescriptorIO::Read(fd, m2) > 0) {
        if (m1 == m2) {
          rc = true;
        }
      }
      else {
        cerr << "DescriptorIO::Read(fd, m2) failed in MapDescriptorTest" << endl;
      }
      close(fd);
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
static bool VectorDescriptorTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorDescriptorTest");
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    DescriptorIO::Write(fd, v1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      vector<Ipv4Prefix>  v2;
      if (DescriptorIO::Read(fd, v2) > 0) {
        if (v1 == v2) {
          rc = true;
        }
      }
      close(fd);
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
static bool ArrayDescriptorTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMArrayDescriptorTest");
  array<Ipv4Prefix,2>
    a1({Ipv4Prefix("192.168.168/24"),
        Ipv4Prefix("10/8")});
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    DescriptorIO::Write(fd, a1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      array<Ipv4Prefix,2>  a2;
      if (DescriptorIO::Read(fd, a2) > 0) {
        if (a1 == a2) {
          rc = true;
        }
      }
      close(fd);
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
static bool DequeDescriptorTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeDescriptorTest");
  deque<Ipv4Prefix>  d1;
  d1.push_back(Ipv4Prefix("192.168.168/24"));
  d1.push_back(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    DescriptorIO::Write(fd, d1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      deque<Ipv4Prefix>  d2;
      if (DescriptorIO::Read(fd, d2) > 0) {
        if (d1 == d2) {
          rc = true;
        }
      }
      close(fd);
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
static bool ListDescriptorTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeDescriptorTest");
  list<Ipv4Prefix>  l1;
  l1.push_back(Ipv4Prefix("192.168.168/24"));
  l1.push_back(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    DescriptorIO::Write(fd, l1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      list<Ipv4Prefix>  l2;
      if (DescriptorIO::Read(fd, l2) > 0) {
        if (l1 == l2) {
          rc = true;
        }
      }
      close(fd);
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
static bool SetDescriptorTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeDescriptorTest");
  set<Ipv4Prefix>  s1;
  s1.insert(Ipv4Prefix("192.168.168/24"));
  s1.insert(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    DescriptorIO::Write(fd, s1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      set<Ipv4Prefix>  s2;
      if (DescriptorIO::Read(fd, s2) > 0) {
        if (s1 == s2) {
          rc = true;
        }
      }
      close(fd);
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
static bool VarArgDescriptorTest()
{
  bool  rc = false;
  
  std::string  s("Hello");
  uint16_t     u = 0xf00f;
  bool         b = true;
  int32_t      i = -2020;
  pair<string,bool>  p("Goodbye", false);

  string  fn("/tmp/DWMVarArgDescriptorTest");
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (UnitAssert(fd >= 0)) {
    if (UnitAssert(DescriptorIO::WriteV(fd, s, u, b, i, p)) > 0) {
      close(fd);
      fd = open(fn.c_str(), O_RDONLY);
      if (UnitAssert(fd >= 0)) {
        std::string        s2;
        uint16_t           u2;
        bool               b2;
        int32_t            i2;
        pair<string,bool>  p2;
        if (UnitAssert(DescriptorIO::ReadV(fd, s2, u2, b2, i2, p2))) {
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
        close(fd);
      }
    }
    else {
      close(fd);
    }
    std::remove(fn.c_str());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VarArgDescriptorTestFail()
{
  bool  rc = false;
  std::string   s("HeLlO");

  string  fn("/tmp/DWMVarArgDescriptorTestFail");
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (UnitAssert(fd >= 0)) {
    if (UnitAssert(DescriptorIO::WriteV(fd, s))) {
      close(fd);
      fd = open(fn.c_str(), O_RDONLY);
      if (UnitAssert(fd >= 0)) {
        std::string        s2;
        uint16_t           u2;
        if (UnitAssert(DescriptorIO::ReadV(fd, s2, u2) < 0)) {
          //  We expect ReadV() to fail because we only wrote a string, so the
          //  stream should end before we can read u2.  But we should have
          //  successfully read s2.
          rc = UnitAssert(s == s2);
        }
        close(fd);
      }
    }
    else {
      close(fd);
    }
    std::remove(fn.c_str());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool UnionTest()
{
  string  fn("/tmp/DWMUnionTest." + std::to_string(getpid()));
  
  bool  rc = false;
  typedef union {
    uint32_t   i;
    char       s[16];
  } MyUnion;
  MyUnion  u1 = { .s = "hello" };
  MyUnion  u3 = { .i = 0xCAFEF00D };
  
  int  fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (UnitAssert(fd >= 0)) {
    if (UnitAssert((DescriptorIO::Write(fd, u1) > 0)
                   && (DescriptorIO::Write(fd, u3) > 0))) {
      close(fd);
      fd = open(fn.c_str(), O_RDONLY);
      if (UnitAssert(fd >= 0)) {
        MyUnion  u2, u4;
        if (UnitAssert(DescriptorIO::Read(fd, u2))) {
          if (UnitAssert(DescriptorIO::Read(fd, u4))) {
            rc = UnitAssert(string("hello") == string(u2.s));
            rc &= UnitAssert(u2.i == u1.i);
            rc = UnitAssert(string(u3.s) == string(u4.s));
            rc &= UnitAssert(u3.i == u4.i);
          }
        }
        close(fd);
      }
    }
    else {
      close(fd);
    }
    std::remove(fn.c_str());
  }
  return rc;
}

#if defined(DWM_CAN_USE_REFLECTION)
#if 0
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIOVecSize()
{
  struct SK0 {
    [[=Dwm::skip_io]]  int i;
    [[=Dwm::skip_io]]  int j;
  };
  UnitAssert(0 == io_detail::IOVecSize<SK0>());
  UnitAssert(! io_detail::IsIOVCapable<SK0>());

  struct SK1 {
    int                    i;
    [[=Dwm::skip_io]]  int j;
  };
  UnitAssert(1 == io_detail::IOVecSize<SK1>());
  UnitAssert(io_detail::IsIOVCapable<SK1>());

  struct SK2 {
    int     i;
    bool    b;
  };
  UnitAssert(2 == io_detail::IOVecSize<SK2>());
  UnitAssert(io_detail::IsIOVCapable<SK2>());
}
#endif

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGatherScatterN()
{
  string  fn("/tmp/TestGatherScatterN." + std::to_string(getpid()));
  struct S1 {
    bool      b;
    uint16_t  us;
    int       i;
    uint32_t  j[4][2];
  };
  const S1  s1 { true, 0xF00F, 42,
                 { { 0xCAFEF00D, 0xFF00FF00 },
                   { 0xDEADBEEF, 0xFFFF0000 },
                   { 0x0000FFFF, 0xBADC0FFE },
                   { 0xFADEDEAD, 0x00FF00FF } } };

  //  consteval {
    constexpr auto  aggInfo = DescriptorIO::TestAggregate<S1>();
    static_assert(Concepts::is_std_tuple<typename[:aggInfo:]>);
    static_assert(std::tuple_size_v<typename[:aggInfo:]> == 4);
    [:aggInfo:]  agg;
    // DescriptorIO::FillAggregate(agg, s1);
    cerr << "std::get<0>(agg): " << std::get<0>(agg) << '\n'
         << "std::get<1>(agg): " << std::get<1>(agg) << '\n'
         << "std::get<2>(agg): " << std::get<1>(agg) << '\n';
    static_assert(std::is_same_v<std::tuple_element_t<3, decltype(agg)>, uint32_t[4][2]>);

    //  }
  
  int  fd = open(fn.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (UnitAssert(0 <= fd)) {
    if (UnitAssert(0 < DescriptorIO::GatherNWrite(fd, s1))) {
      close(fd);
      fd = open(fn.c_str(), O_RDONLY);
      if (UnitAssert(0 <= fd)) {
        S1  s1_2;
        if (UnitAssert(0 < DescriptorIO::ScatterNRead(fd, s1_2))) {
          UnitAssert(s1.b == s1_2.b);
          UnitAssert(s1.us == s1_2.us);
          UnitAssert(s1.i == s1_2.i);
          for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 2; ++j) {
              UnitAssert(s1.j[i][j] == s1_2.j[i][j]);
            }
          }
        }
        close(fd);
      }
    }
    else {
      close(fd);
    }
    std::remove(fn.c_str());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGatherScatter()
{
  string  fn("/tmp/TestGatherScatter." + std::to_string(getpid()));
  struct S1 {
    bool      b;
    uint16_t  us;
    int       i;
    uint32_t  j[4][2];
  };
  const S1  s1 { true, 0xF00F, 42,
                 { { 0xCAFEF00D, 0xFF00FF00 },
                   { 0xDEADBEEF, 0xFFFF0000 },
                   { 0x0000FFFF, 0xBADC0FFE },
                   { 0xFADEDEAD, 0x00FF00FF } } };

  int fd = open(fn.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (UnitAssert(0 <= fd)) {
    if (UnitAssert(0 < DescriptorIO::GatherWrite(fd, s1))) {
      close(fd);
      fd = open(fn.c_str(), O_RDONLY);
      if (UnitAssert(0 <= fd)) {
        S1  s1_2;
        if (UnitAssert(0 < DescriptorIO::ScatterRead(fd, s1_2))) {
          UnitAssert(s1.b == s1_2.b);
          UnitAssert(s1.us == s1_2.us);
          UnitAssert(s1.i == s1_2.i);
          for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 2; ++j) {
              UnitAssert(s1.j[i][j] == s1_2.j[i][j]);
            }
          }
        }
        close(fd);
      }
    }
    else {
      close(fd);
    }
    std::remove(fn.c_str());
  }
  return;
}

#endif

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  SysLogger::Open("TestDescriptorIO", LOG_PERROR, LOG_USER);
  SysLogger::MinimumPriority(LOG_INFO);
  
  DescriptorTest();
  MapDescriptorTest();
  VectorDescriptorTest();
  ArrayDescriptorTest();
  DequeDescriptorTest();
  ListDescriptorTest();
  SetDescriptorTest();
  VarArgDescriptorTest();
  VarArgDescriptorTestFail();
  UnionTest();
#if defined(DWM_CAN_USE_REFLECTION)
  //  TestIOVecSize();
  TestGatherScatterN();
  TestGatherScatter();
#endif
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
