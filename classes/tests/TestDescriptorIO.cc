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
#if defined(DWM_CAN_USE_REFLECTION)
#endif
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
