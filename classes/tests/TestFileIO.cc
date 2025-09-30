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
//!  \file TestFileIO.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <cassert>
#include <cstdlib>

#include "DwmIpv4Prefix.hh"
#include "DwmFileIO.hh"
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
static bool WriteTestBlob(FILE * f)
{
  bool  rc = false;

  if (f) {
    char  c = k_charVal;
    if (FileIO::Write(f,c) == sizeof(c)) {
      uint8_t  uc = k_ucharVal;
      if (FileIO::Write(f,uc) == sizeof(uc)) {
        int16_t sh = k_int16Val;
        if (FileIO::Write(f,sh) == sizeof(sh)) {
          uint16_t ush = k_uint16Val;
          if (FileIO::Write(f,ush) == sizeof(ush)) {
            int32_t  w = k_int32Val;
            if (FileIO::Write(f,w) == sizeof(w)) {
              uint32_t  uw = k_uint32Val;
              if (FileIO::Write(f,uw) == sizeof(uw)) {
                int64_t  d = k_int64Val;
                if (FileIO::Write(f,d) == sizeof(d)) {
                  uint64_t  ud = k_uint64Val;
                  if (FileIO::Write(f,ud) == sizeof(ud)) {
                    std::string  s = k_stringVal;
                    if (FileIO::Write(f,s) > 4) {
                      float  fl = k_floatVal;
                      if (FileIO::Write(f,fl) == sizeof(fl)) {
                        double  dfl = k_doubleVal;
                        if (FileIO::Write(f,dfl) == sizeof(dfl)) {
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
static bool WriteTestBlobs(FILE * f, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! WriteTestBlob(f)) {
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
static bool ReadTestBlob(FILE * f)
{
  bool  rc = false;
  if (f) {
    char  c;
    if (FileIO::Read(f,c) && (c == k_charVal)) {
      uint8_t  uc = 0;
      if (FileIO::Read(f,uc) && (uc == k_ucharVal)) {
        int16_t  sh = 0;
        if (FileIO::Read(f,sh) && (sh == k_int16Val)) {
          uint16_t  ush = 0;
          if (FileIO::Read(f,ush) && (ush == k_uint16Val)) {
            int32_t  w = 0;
            if (FileIO::Read(f,w) && (w == k_int32Val)) {
              uint32_t uw = 0;
              if (FileIO::Read(f,uw) && (uw == k_uint32Val)) {
                int64_t  d = 0;
                if (FileIO::Read(f,d) && (d == k_int64Val)) {
                  uint64_t  ud = 0;
                  if (FileIO::Read(f,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (FileIO::Read(f,s) && (s == k_stringVal)) {
                      float  fl;
                      if (FileIO::Read(f,fl) && (fl == k_floatVal)) {
                        double  dfl;
                        if (FileIO::Read(f,dfl) && (dfl == k_doubleVal)) {
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
static bool ReadTestBlobs(FILE * f, uint32_t numBlobs)
{
  assert(numBlobs > 0);
  bool  rc = true;
  for (uint32_t i = 0; i < numBlobs; ++i) {
    if (! ReadTestBlob(f)) {
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
static bool FileTest()
{
  bool  rc = false;
  FILE  *f = fopen("/tmp/DWMIOFileTest","w");
  if (f) {
    if (WriteTestBlobs(f,3)) {
      fclose(f);
      
      f = fopen("/tmp/DWMIOFileTest","r");
      if (f) {
        if (ReadTestBlobs(f,3)) {
          rc = true;
        }
        fclose(f);
      }
      else {
        cerr << "fopen(\"/tmp/DWMIOFileTest\",\"r\") failed: "
             << strerror(errno) << endl;
      }
    }
    else {
      fclose(f);
    }
    std::remove("/tmp/DWMIOFileTest");
  }
  else {
    cerr << "fopen(\"/tmp/DWMIOFileTest\",\"w\")" << " failed: "
         << strerror(errno) << endl;
  }

  UnitAssert(rc == true);
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool MapFileTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMMapFileTest");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  FILE  *f = fopen(fn.c_str(), "w");
  if (f) {
    FileIO::Write(f, m1);
    fclose(f);

    f = fopen(fn.c_str(), "r");
    if (f) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (FileIO::Read(f, m2)) {
        if (m1 == m2) {
          rc = true;
        }
      }
      fclose(f);
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
static bool VectorFileTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorFileTest");
  
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  FILE  *f = fopen(fn.c_str(), "w");
  if (f) {
    FileIO::Write(f, v1);
    fclose(f);

    f = fopen(fn.c_str(), "r");
    if (f) {
      vector<Ipv4Prefix>  v2;
      if (FileIO::Read(f, v2)) {
        if (v1 == v2) {
          rc = true;
        }
      }
      fclose(f);
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
static bool ArrayFileTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMArrayFileTest");
  
  array<Ipv4Prefix,2>
    a1({Ipv4Prefix("192.168.168/24"),
        Ipv4Prefix("10/8")});
  
  FILE  *f = fopen(fn.c_str(), "w");
  if (f) {
    FileIO::Write(f, a1);
    fclose(f);

    f = fopen(fn.c_str(), "r");
    if (f) {
      array<Ipv4Prefix,2>  a2;
      if (FileIO::Read(f, a2)) {
        if (a1 == a2) {
          rc = true;
        }
      }
      fclose(f);
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
static bool VarArgFileTest()
{
  bool  rc = false;
  
  std::string  s("Hello");
  uint16_t     u = 0xf00f;
  bool         b = true;
  int32_t      i = -2020;
  pair<string,bool>  p("Goodbye", false);

  string  fn("/tmp/DWMVarArgFileTest");
  FILE  *f = fopen(fn.c_str(), "w");
  if (f) {
    UnitAssert(FileIO::WriteV(f, s, u, b, i, p));
    fclose(f);
    f = fopen(fn.c_str(), "r");
    if (f) {
      std::string        s2;
      uint16_t           u2;
      bool               b2;
      int32_t            i2;
      pair<string,bool>  p2;
      if (UnitAssert(FileIO::ReadV(f, s2, u2, b2, i2, p2))) {
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
      fclose(f);
    }
    std::remove(fn.c_str());
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool VarArgFileTestFail()
{
  bool  rc = false;
  std::string  s("HeLlO");

  string  fn("/tmp/DWMVarArgFileTest");
  FILE  *f = fopen(fn.c_str(), "wb");
  if (UnitAssert(f)) {
    if (UnitAssert(FileIO::WriteV(f, s))) {
      fclose(f);
      f = fopen(fn.c_str(), "rb");
      if (UnitAssert(f)) {
        std::string        s2;
        uint16_t           u2;
        if (UnitAssert(! FileIO::ReadV(f, s2, u2))) {
          //  We expect ReadV() to fail because we only wrote a string, so the
          //  stream should end before we can read u2.  But we should have
          //  successfully read s2.
          rc = UnitAssert(s == s2);
        }
        fclose(f);
      }
    }
    else {
      fclose(f);
    }
    std::remove(fn.c_str());
  }
  return rc;
}

#if defined(DWM_CAN_USE_REFLECTION)
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool ReflectionFileTest()
{
  bool  rc = false;

  typedef struct {
    int                a;
    int                b;
    string             c;
    struct timeval     tv;
    std::vector<int>   vi;
    std::map<int,int>  mi;
  } ReflTestStruct;

  UnitAssert((fileio_detail::IsWritable<ReflTestStruct>));
  
  ReflTestStruct  rts1{9,42,"ReflectionStreamTest",{42,0xCCCC},{6,7,8},
                       {{1,2},{3,4}}};
  string  fn("/tmp/DWMReflectionFileTest");
  FILE  *f = fopen(fn.c_str(), "wb");
  if (UnitAssert(f)) {
    if (UnitAssert(FileIO::Write(f, rts1))) {
      fclose(f);
      f = fopen(fn.c_str(), "rb");
      if (UnitAssert(f)) {
        ReflTestStruct  rts2;
        if (UnitAssert(FileIO::Read(f, rts2))) {
          if (UnitAssert(rts1.a == rts2.a)
              && UnitAssert(rts1.b == rts2.b)
              && UnitAssert(rts1.c == rts2.c)
              && UnitAssert(rts1.tv.tv_sec == rts2.tv.tv_sec)
              && UnitAssert(rts1.tv.tv_usec == rts2.tv.tv_usec)
              && UnitAssert(rts1.vi == rts2.vi)
              && UnitAssert(rts1.mi == rts2.mi)) {
            rc = true;
          }
        }
        fclose(f);
      }
    }
    else {
      fclose(f);
    }
    std::remove(fn.c_str());
  }
  return rc;
}
#endif  //  defined(DWM_CAN_USE_REFLECTION)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  SysLogger::Open("TestFileIO", LOG_PERROR, LOG_USER);
  SysLogger::MinimumPriority(LOG_INFO);

  FileTest();
  MapFileTest();
  VectorFileTest();
  ArrayFileTest();
  UnitAssert(VarArgFileTest());
  UnitAssert(VarArgFileTestFail());
#if defined(DWM_CAN_USE_REFLECTION)
  ReflectionFileTest();
#endif
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
