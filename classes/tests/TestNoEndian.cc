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
//!  \file TestNoEndian.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
}

#include <sstream>

#include "DwmNoEndian.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  Dwm::NoEndian<uint16_t>  u16val = 0xFF00;
  Dwm::NoEndian<uint32_t>  u32val = 0xAABBCCDD;
  Dwm::NoEndian<uint64_t>  u64val = 0x0011223344556677;

  stringstream  ss;
  if (UnitAssert(StreamIO::Write(ss, u16val))) {
    if (UnitAssert(StreamIO::Write(ss, u32val))) {
      if (UnitAssert(StreamIO::Write(ss, u64val))) {
        Dwm::NoEndian<uint16_t>  u16val_2 = 0;
        Dwm::NoEndian<uint32_t>  u32val_2 = 0;
        Dwm::NoEndian<uint64_t>  u64val_2 = 0;

        if (UnitAssert(StreamIO::Read(ss, u16val_2))) {
          if (UnitAssert(StreamIO::Read(ss, u32val_2))) {
            if (UnitAssert(StreamIO::Read(ss, u64val_2))) {
              UnitAssert(u16val_2 == u16val);
              UnitAssert(u32val_2 == u32val);
              UnitAssert(u64val_2 == u64val);
            }
          }
        }
      }
    }
  }

  uint16_t  u16val_2 = 0;
  uint32_t  u32val_2 = 0;
  uint64_t  u64val_2 = 0;
  if (UnitAssert(StreamIO::Write(ss, u16val))) {
    if (UnitAssert(StreamIO::Write(ss, u32val))) {
      if (UnitAssert(StreamIO::Write(ss, u64val))) {
        if (UnitAssert(ss.read((caddr_t)&u16val_2, sizeof(u16val_2)))) {
          if (UnitAssert(ss.read((caddr_t)&u32val_2, sizeof(u32val_2)))) {
            if (UnitAssert(ss.read((caddr_t)&u64val_2, sizeof(u64val_2)))) {
              UnitAssert(u16val_2 == u16val);
              UnitAssert(u32val_2 == u32val);
              UnitAssert(u64val_2 == u64val);
            }
          }
        }
      }
    }
  }
  u16val = 0;
  u32val = 0;
  u64val = 0;
  if (UnitAssert(ss.write((caddr_t)&u16val_2, sizeof(u16val_2)))) {
    if (UnitAssert(ss.write((caddr_t)&u32val_2, sizeof(u32val_2)))) {
      if (UnitAssert(ss.write((caddr_t)&u64val_2, sizeof(u64val_2)))) {
        if (UnitAssert(StreamIO::Read(ss, u16val))) {
          if (UnitAssert(StreamIO::Read(ss, u32val))) {
            if (UnitAssert(StreamIO::Read(ss, u64val))) {
              UnitAssert(u16val_2 == u16val);
              UnitAssert(u32val_2 == u32val);
              UnitAssert(u64val_2 == u64val);
            }
          }
        }
      }
    }
  }
  return;
}

#if 0
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO()
{
  static const SizedLength  sizedLengths[] = {
    0x0,               0xFF,                 // 1 byte
    0x100,             0xFFFF,               // 2 bytes
    0x10000,           0xFFFFFF,             // 3 bytes
    0x1000000,         0xFFFFFFFF,           // 4 bytes
    0x100000000,       0xFFFFFFFFFF,         // 5 bytes
    0x10000000000,     0xFFFFFFFFFFFF,       // 6 bytes
    0x1000000000000,   0xFFFFFFFFFFFFFF,     // 7 bytes
    0x100000000000000, 0xFFFFFFFFFFFFFFFF    // 8 bytes
  };
  static const size_t  numLengths =
    sizeof(sizedLengths)/sizeof(sizedLengths[0]);
  
  FILE *f = fopen("/tmp/SizedLengthTestFileIO","w");
  if (UnitAssert(f)) {
    for (size_t i = 0; i < numLengths; ++i) {
      if (! UnitAssert(sizedLengths[i].Write(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/SizedLengthTestFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < numLengths; ++i) {
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
  //  SizedLength values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  static const pair<SizedLength,ssize_t>  sizedLengths[] = {
    {0x0,               2}, {0xFF,              2},
    {0x100,             3}, {0xFFFF,            3},
    {0x10000,           4}, {0xFFFFFF,          4},
    {0x1000000,         5}, {0xFFFFFFFF,        5},
    {0x100000000,       6}, {0xFFFFFFFFFF,      6},
    {0x10000000000,     7}, {0xFFFFFFFFFFFF,    7},
    {0x1000000000000,   8}, {0xFFFFFFFFFFFFFF,  8},
    {0x100000000000000, 9}, {0xFFFFFFFFFFFFFFFF,9}
  };
  static const size_t  numLengths =
    sizeof(sizedLengths)/sizeof(sizedLengths[0]);
  
  int fd = open("/tmp/SizedLengthTestDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    for (size_t i = 0; i < numLengths; ++i) {
      if (! UnitAssert(sizedLengths[i].first.Write(fd)
                       == sizedLengths[i].second)) {
        break;
      }
    }
    close(fd);
    fd = open("/tmp/SizedLengthTestDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < numLengths; ++i) {
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
  //  SizedLength values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  static const pair<SizedLength,int>  sizedLengths[] = {
    {0x00,              2}, {0xFF,              2},
    {0x100,             3}, {0xFFFF,            3},
    {0x10000,           4}, {0xFFFFFF,          4},
    {0x1000000,         5}, {0xFFFFFFFF,        5},
    {0x100000000,       6}, {0xFFFFFFFFFF,      6},
    {0x10000000000,     7}, {0xFFFFFFFFFFFF,    7},
    {0x1000000000000,   8}, {0xFFFFFFFFFFFFFF,  8},
    {0x100000000000000, 9}, {0xFFFFFFFFFFFFFFFF,9}
  };
  static const size_t numLengths =
    sizeof(sizedLengths)/sizeof(sizedLengths[0]);
  
  BZFILE  *bzf = BZ2_bzopen("/tmp/SizedLengthTestBZ2IO", "wb");
  if (UnitAssert(bzf)) {
    for (size_t i = 0; i < numLengths; ++i) {
      if (! UnitAssert(sizedLengths[i].first.BZWrite(bzf)
                       == sizedLengths[i].second)) {
        break;
      }
    }
    BZ2_bzclose(bzf);
    bzf = BZ2_bzopen("/tmp/SizedLengthTestBZ2IO", "rb");
    if (UnitAssert(bzf)) {
      for (size_t i = 0; i < numLengths; ++i) {
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
  //  SizedLength values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  static const pair<SizedLength,int>  sizedLengths[] = {
    {0x0,               2}, {0xFF,               2}, // 1 byte  + 1 size byte
    {0x100,             3}, {0xFFFF,             3}, // 2 bytes + 1 size byte
    {0x10000,           4}, {0xFFFFFF,           4}, // 3 bytes + 1 size byte
    {0x1000000,         5}, {0xFFFFFFFF,         5}, // 4 bytes + 1 size byte
    {0x100000000,       6}, {0xFFFFFFFFFF,       6}, // 5 bytes + 1 size byte
    {0x10000000000,     7}, {0xFFFFFFFFFFFF,     7}, // 6 bytes + 1 size byte
    {0x1000000000000,   8}, {0xFFFFFFFFFFFFFF,   8}, // 7 bytes + 1 size byte
    {0x100000000000000, 9}, {0xFFFFFFFFFFFFFFFF, 9}  // 8 bytes + 1 size byte
  };
  static const size_t numLengths =
    sizeof(sizedLengths)/sizeof(sizedLengths[0]);
  
  gzFile  gzf = gzopen("/tmp/SizedLengthTestGZIO", "wb");
  if (UnitAssert(gzf)) {
    for (size_t i = 0; i < numLengths; ++i) {
      if (! UnitAssert(sizedLengths[i].first.Write(gzf)
                       == sizedLengths[i].second)) {
        break;
      }
    }
    gzclose(gzf);
    gzf = gzopen("/tmp/SizedLengthTestGZIO", "rb");
    if (UnitAssert(gzf)) {
      for (size_t i = 0; i < numLengths; ++i) {
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
#endif

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestStreamIO();
#if 0
  TestFileIO();
  TestDescriptorIO();
  TestBZ2IO();
  TestGZIO();
#endif
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
