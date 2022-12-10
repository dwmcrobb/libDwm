//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmIO.cc 8389 $
// @(#) $Id: TestDwmIO.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestDwmIO.cc
//!  \brief Unit tests for Dwm::IO
//---------------------------------------------------------------------------

//  This program is just a simple test application for functions in Dwm::IO.

extern "C" {
  #include <fcntl.h>
  #include <errno.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstdlib>
#include <fstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmIO.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmIO.cc 8389 $");

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

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool WriteTestBlob(ostream & os)
{
  bool  rc = false;

  if (os) {
    char  c = k_charVal;
    if (IO::Write(os,c)) {
      uint8_t  uc = k_ucharVal;
      if (IO::Write(os,uc)) {
        int16_t sh = k_int16Val;
        if (IO::Write(os,sh)) {
          uint16_t ush = k_uint16Val;
          if (IO::Write(os,ush)) {
            int32_t  w = k_int32Val;
            if (IO::Write(os,w)) {
              uint32_t  uw = k_uint32Val;
              if (IO::Write(os,uw)) {
                int64_t  d = k_int64Val;
                if (IO::Write(os,d)) {
                  uint64_t  ud = k_uint64Val;
                  if (IO::Write(os,ud)) {
                    std::string  s = k_stringVal;
                    if (IO::Write(os,s)) {
                      float  f = k_floatVal;
                      if (IO::Write(os,f)) {
                        double df = k_doubleVal;
                        if (IO::Write(os,df)) {
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
static bool WriteTestBlob(int fd)
{
  bool  rc = false;

  if (fd >= 0) {
    char  c = k_charVal;
    if (IO::Write(fd,c) == sizeof(c)) {
      uint8_t  uc = k_ucharVal;
      if (IO::Write(fd,uc) == sizeof(uc)) {
        int16_t sh = k_int16Val;
        if (IO::Write(fd,sh) == sizeof(sh)) {
          uint16_t ush = k_uint16Val;
          if (IO::Write(fd,ush) == sizeof(ush)) {
            int32_t  w = k_int32Val;
            if (IO::Write(fd,w) == sizeof(w)) {
              uint32_t  uw = k_uint32Val;
              if (IO::Write(fd,uw) == sizeof(uw)) {
                int64_t  d = k_int64Val;
                if (IO::Write(fd,d) == sizeof(d)) {
                  uint64_t  ud = k_uint64Val;
                  if (IO::Write(fd,ud) == sizeof(ud)) {
                    std::string  s = k_stringVal;
                    if (IO::Write(fd,s) > 4) {
                      float  f = k_floatVal;
                      if (IO::Write(fd,f) == sizeof(f)) {
                        double  df = k_doubleVal;
                        if (IO::Write(fd,df) == sizeof(df)) {
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
static bool WriteTestBlob(FILE * f)
{
  bool  rc = false;

  if (f) {
    char  c = k_charVal;
    if (IO::Write(f,c) == sizeof(c)) {
      uint8_t  uc = k_ucharVal;
      if (IO::Write(f,uc) == sizeof(uc)) {
        int16_t sh = k_int16Val;
        if (IO::Write(f,sh) == sizeof(sh)) {
          uint16_t ush = k_uint16Val;
          if (IO::Write(f,ush) == sizeof(ush)) {
            int32_t  w = k_int32Val;
            if (IO::Write(f,w) == sizeof(w)) {
              uint32_t  uw = k_uint32Val;
              if (IO::Write(f,uw) == sizeof(uw)) {
                int64_t  d = k_int64Val;
                if (IO::Write(f,d) == sizeof(d)) {
                  uint64_t  ud = k_uint64Val;
                  if (IO::Write(f,ud) == sizeof(ud)) {
                    std::string  s = k_stringVal;
                    if (IO::Write(f,s) > 4) {
                      float  fl = k_floatVal;
                      if (IO::Write(f,fl) == sizeof(fl)) {
                        double  dfl = k_doubleVal;
                        if (IO::Write(f,dfl) == sizeof(dfl)) {
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
static bool ReadTestBlob(istream & is)
{
  bool  rc = false;
  if (is) {
    char  c;
    if (IO::Read(is,c) && (c == k_charVal)) {
      uint8_t  uc;
      if (IO::Read(is,uc) && (uc == k_ucharVal)) {
        int16_t  sh;
        if (IO::Read(is,sh) && (sh == k_int16Val)) {
          uint16_t  ush;
          if (IO::Read(is,ush) && (ush == k_uint16Val)) {
            int32_t  w;
            if (IO::Read(is,w) && (w == k_int32Val)) {
              uint32_t uw;
              if (IO::Read(is,uw) && (uw == k_uint32Val)) {
                int64_t  d;
                if (IO::Read(is,d) && (d == k_int64Val)) {
                  uint64_t  ud;
                  if (IO::Read(is,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (IO::Read(is,s) && (s == k_stringVal)) {
                      float  f;
                      if (IO::Read(is,f) && (f == k_floatVal)) {
                        double  df;
                        if (IO::Read(is,df) && (df == k_doubleVal)) {
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
static bool ReadTestBlob(int fd)
{
  bool  rc = false;
  if (fd >= 0) {
    char  c;
    if (IO::Read(fd,c) && (c == k_charVal)) {
      uint8_t  uc = 0;
      if (IO::Read(fd,uc) && (uc == k_ucharVal)) {
        int16_t  sh = 0;
        if (IO::Read(fd,sh) && (sh == k_int16Val)) {
          uint16_t  ush = 0;
          if (IO::Read(fd,ush) && (ush == k_uint16Val)) {
            int32_t  w = 0;
            if (IO::Read(fd,w) && (w == k_int32Val)) {
              uint32_t uw = 0;
              if (IO::Read(fd,uw) && (uw == k_uint32Val)) {
                int64_t  d = 0;
                if (IO::Read(fd,d) && (d == k_int64Val)) {
                  uint64_t  ud = 0;
                  if (IO::Read(fd,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (IO::Read(fd,s) && (s == k_stringVal)) {
                      float  f;
                      if (IO::Read(fd,f) && (f == k_floatVal)) {
                        double  df;
                        if (IO::Read(fd,df) && (df == k_doubleVal)) {
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
static bool ReadTestBlob(FILE * f)
{
  bool  rc = false;
  if (f) {
    char  c;
    if (IO::Read(f,c) && (c == k_charVal)) {
      uint8_t  uc = 0;
      if (IO::Read(f,uc) && (uc == k_ucharVal)) {
        int16_t  sh = 0;
        if (IO::Read(f,sh) && (sh == k_int16Val)) {
          uint16_t  ush = 0;
          if (IO::Read(f,ush) && (ush == k_uint16Val)) {
            int32_t  w = 0;
            if (IO::Read(f,w) && (w == k_int32Val)) {
              uint32_t uw = 0;
              if (IO::Read(f,uw) && (uw == k_uint32Val)) {
                int64_t  d = 0;
                if (IO::Read(f,d) && (d == k_int64Val)) {
                  uint64_t  ud = 0;
                  if (IO::Read(f,ud) && (ud == k_uint64Val)) {
                    string  s;
                    if (IO::Read(f,s) && (s == k_stringVal)) {
                      float  fl;
                      if (IO::Read(f,fl) && (fl == k_floatVal)) {
                        double  dfl;
                        if (IO::Read(f,dfl) && (dfl == k_doubleVal)) {
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
static bool MapStreamTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMMapStreamTest");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  ofstream  os(fn.c_str());
  if (os) {
    IO::Write(os, m1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (IO::Read(is, m2)) {
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
    IO::Write(os, m1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      map<int,string>  m2;
      if (IO::Read(is, m2)) {
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
static bool MapDescriptorTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMMapDescriptorTest");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  int  fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    IO::Write(fd, m1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (IO::Read(fd, m2) > 0) {
        if (m1 == m2) {
          rc = true;
        }
      }
      else {
        cerr << "IO::Read(fd, m2) failed in MapDescriptorTest" << endl;
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
static bool MapFileTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMMapFileTest");
  
  map<Ipv4Address,Ipv4Prefix>  m1;
  m1[Ipv4Address("192.168.168.1")] = Ipv4Prefix("192.168.168/24");
  m1[Ipv4Address("10.10.10.10")] = Ipv4Prefix("10/8");
  
  FILE  *f = fopen(fn.c_str(), "w");
  if (f) {
    IO::Write(f, m1);
    fclose(f);

    f = fopen(fn.c_str(), "r");
    if (f) {
      map<Ipv4Address,Ipv4Prefix>  m2;
      if (IO::Read(f, m2)) {
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
static bool VectorStreamTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorStreamTest");
  
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    IO::Write(os, v1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      vector<Ipv4Prefix>  v2;
      if (IO::Read(is, v2)) {
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
static bool VectorFileTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorFileTest");
  
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  FILE  *f = fopen(fn.c_str(), "w");
  if (f) {
    IO::Write(f, v1);
    fclose(f);

    f = fopen(fn.c_str(), "r");
    if (f) {
      vector<Ipv4Prefix>  v2;
      if (IO::Read(f, v2)) {
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
static bool VectorDescriptorTest()
{
  bool  rc = false;
  string  fn("/tmp/DWMVectorDescriptorTest");
  vector<Ipv4Prefix>  v1;
  v1.push_back(Ipv4Prefix("192.168.168/24"));
  v1.push_back(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    IO::Write(fd, v1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      vector<Ipv4Prefix>  v2;
      if (IO::Read(fd, v2) > 0) {
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
    IO::Write(fd, a1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      array<Ipv4Prefix,2>  a2;
      if (IO::Read(fd, a2) > 0) {
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
static bool DequeStreamTest()
{
  bool  rc = false;

  string fn("/tmp/DWMDequeStreamTest");
  deque<Ipv4Prefix>  d1;
  d1.push_back(Ipv4Prefix("192.168.168/24"));
  d1.push_back(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    IO::Write(os, d1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      deque<Ipv4Prefix>  d2;
      if (IO::Read(is, d2)) {
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
static bool DequeDescriptorTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeDescriptorTest");
  deque<Ipv4Prefix>  d1;
  d1.push_back(Ipv4Prefix("192.168.168/24"));
  d1.push_back(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    IO::Write(fd, d1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      deque<Ipv4Prefix>  d2;
      if (IO::Read(fd, d2) > 0) {
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
static bool ListStreamTest()
{
  bool  rc = false;

  string fn("/tmp/DWMListStreamTest");
  list<Ipv4Prefix>  l1;
  l1.push_back(Ipv4Prefix("192.168.168/24"));
  l1.push_back(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    IO::Write(os, l1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      list<Ipv4Prefix>  l2;
      if (IO::Read(is, l2)) {
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
static bool ListDescriptorTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeDescriptorTest");
  list<Ipv4Prefix>  l1;
  l1.push_back(Ipv4Prefix("192.168.168/24"));
  l1.push_back(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    IO::Write(fd, l1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      list<Ipv4Prefix>  l2;
      if (IO::Read(fd, l2) > 0) {
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
static bool SetStreamTest()
{
  bool  rc = false;

  string fn("/tmp/DWMSetStreamTest");
  set<Ipv4Prefix>  s1;
  s1.insert(Ipv4Prefix("192.168.168/24"));
  s1.insert(Ipv4Prefix("10/8"));
  
  ofstream  os(fn.c_str());
  if (os) {
    IO::Write(os, s1);
    os.close();

    ifstream  is(fn.c_str());
    if (is) {
      set<Ipv4Prefix>  s2;
      if (IO::Read(is, s2)) {
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
static bool SetDescriptorTest()
{
  bool  rc = false;

  string  fn("/tmp/DWMDequeDescriptorTest");
  set<Ipv4Prefix>  s1;
  s1.insert(Ipv4Prefix("192.168.168/24"));
  s1.insert(Ipv4Prefix("10/8"));
  
  int fd = open(fn.c_str(), O_WRONLY|O_CREAT, 0644);
  if (fd >= 0) {
    IO::Write(fd, s1);
    close(fd);

    fd = open(fn.c_str(), O_RDONLY);
    if (fd >= 0) {
      set<Ipv4Prefix>  s2;
      if (IO::Read(fd, s2) > 0) {
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
int main(int argc, char *argv[])
{
  if (! StreamTest())
    goto testFailed;
  if (! DescriptorTest())
    goto testFailed;
  if (! FileTest())
    goto testFailed;
  if (! MapStreamTestSimple())
    goto testFailed;
  if (! MapStreamTest())
    goto testFailed;
  if (! MapDescriptorTest())
    goto testFailed;
  if (! MapFileTest())
    goto testFailed;
  if (! VectorStreamTest())
    goto testFailed;
  if (! VectorFileTest())
    goto testFailed;
  if (! VectorDescriptorTest())
    goto testFailed;
  if (! ArrayDescriptorTest())
    goto testFailed;
  if (! DequeStreamTest())
    goto testFailed;
  if (! DequeDescriptorTest())
    goto testFailed;
  if (! ListStreamTest())
    goto testFailed;
  if (! ListDescriptorTest())
    goto testFailed;
  if (! SetStreamTest())
    goto testFailed;
  if (! SetDescriptorTest())
    goto testFailed;

  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;
  
  exit(0);

 testFailed:

  Assertions::Print(cerr, true);
  exit(1);
}
