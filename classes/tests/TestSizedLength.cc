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
//!  \file TestSizedLength.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
}

#include <sstream>

#include "DwmSizedLength.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  static SizedLength  sizedLengths[8] = {
    0ull,           0xFFull,
    0x100ull,       0xFFFFull,
    0x10000ull,     0xFFFFFFFFull,
    0x100000000ull, 0xFFFFFFFFFFFFFFFFull
  };
  stringstream  ss;
  for (size_t i = 0; i < 8; ++i) {
    if (! UnitAssert(sizedLengths[i].Write(ss))) {
      break;
    }
  }
  for (size_t i = 0; i < 8; ++i) {
    SizedLength  sizedLength;
    if (! UnitAssert(sizedLength.Read(ss))) {
      break;
    }
    UnitAssert(sizedLength == sizedLengths[i]);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO()
{
  static SizedLength  sizedLengths[8] = {
    0, 255, 256, 65535, 65536, 4294967295, 4294967296, 0xFFFFFFFFFFFFFFFFull
  };
  FILE *f = fopen("/tmp/SizedLengthTestFileIO","w");
  if (UnitAssert(f)) {
    for (size_t i = 0; i < 8; ++i) {
      if (! UnitAssert(sizedLengths[i].Write(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/SizedLengthTestFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < 8; ++i) {
        SizedLength  sizedLength;
        if (! UnitAssert(sizedLength.Read(f))) {
          break;
        }
        UnitAssert(sizedLength == sizedLengths[i]);
      }
      fclose(f);
    }
    std::remove("/tmp/SizedLengthTestFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO()
{
  static pair<SizedLength,ssize_t>  sizedLengths[8] = {
    {0,2}, {255,2},
    {256,3}, {65535,3},
    {65536,5}, {4294967295,5},
    {4294967296,9}, {0xFFFFFFFFFFFFFFFFull,9}
  };
  int fd = open("/tmp/SizedLengthTestDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    for (size_t i = 0; i < 8; ++i) {
      if (! UnitAssert(sizedLengths[i].first.Write(fd)
                       == sizedLengths[i].second)) {
        break;
      }
    }
    close(fd);
    fd = open("/tmp/SizedLengthTestDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < 8; ++i) {
        SizedLength  sizedLength;
        if (! UnitAssert(sizedLength.Read(fd) == sizedLengths[i].second)) {
          break;
        }
        UnitAssert(sizedLength == sizedLengths[i].first);
      }
      close(fd);
    }
    std::remove("/tmp/SizedLengthTestDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO()
{
  static pair<SizedLength,int>  sizedLengths[8] = {
    {0,2}, {255,2}, {256,3}, {65535,3},
    {65536,5}, {4294967295,5},
    {4294967296,9}, {0xFFFFFFFFFFFFFFFFull,9}
  };
  BZFILE  *bzf = BZ2_bzopen("/tmp/SizedLengthTestBZ2IO", "wb");
  if (UnitAssert(bzf)) {
    for (size_t i = 0; i < 8; ++i) {
      if (! UnitAssert(sizedLengths[i].first.BZWrite(bzf)
                       == sizedLengths[i].second)) {
        break;
      }
    }
    BZ2_bzclose(bzf);
    bzf = BZ2_bzopen("/tmp/SizedLengthTestBZ2IO", "rb");
    if (UnitAssert(bzf)) {
      for (size_t i = 0; i < 8; ++i) {
        SizedLength  sizedLength;
        if (! UnitAssert(sizedLength.BZRead(bzf) == sizedLengths[i].second)) {
          break;
        }
        UnitAssert(sizedLength == sizedLengths[i].first);
      }
      BZ2_bzclose(bzf);
    }
    std::remove("/tmp/SizedLengthTestBZ2IO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO()
{
  static pair<SizedLength,int>  sizedLengths[8] = {
    {0,2}, {255,2},
    {256,3}, {65535,3},
    {65536,5}, {4294967295,5},
    {4294967296,9}, {0xFFFFFFFFFFFFFFFFull,9}
  };
  gzFile  gzf = gzopen("/tmp/SizedLengthTestGZIO", "wb");
  if (UnitAssert(gzf)) {
    for (size_t i = 0; i < 8; ++i) {
      if (! UnitAssert(sizedLengths[i].first.Write(gzf)
                       == sizedLengths[i].second)) {
        break;
      }
    }
    gzclose(gzf);
    gzf = gzopen("/tmp/SizedLengthTestGZIO", "rb");
    if (UnitAssert(gzf)) {
      for (size_t i = 0; i < 8; ++i) {
        SizedLength  sizedLength;
        if (! UnitAssert(sizedLength.Read(gzf)
                         == sizedLengths[i].second)) {
          break;
        }
        UnitAssert(sizedLength == sizedLengths[i].first);
      }
      gzclose(gzf);
    }
    std::remove("/tmp/SizedLengthTestGZIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestStreamIO();
  TestFileIO();
  TestDescriptorIO();
  TestBZ2IO();
  TestGZIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
