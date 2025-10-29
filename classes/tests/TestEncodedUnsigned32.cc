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
//!  \file TestEncodedUnsigned32.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
}

#include <numeric>
#include <random>
#include <sstream>

#include "DwmEndianness.hh"
#include "DwmEncodedUnsigned.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

using EncodedUint32 = EncodedUnsigned<uint32_t>;

//  EncodedUint32 values and the number of bytes we expect to write/read
//  for each of the values, including the size byte.
static const pair<EncodedUint32,ssize_t>  k_sizedLengths[] = {
  {0x0,               2}, {0xFF,               2},
  {0x1,               2}, {0x80,               2},
  {0x100,             3}, {0xFFFF,             3},
  {0x101,             3}, {0x8000,             3},
  {0x8001,            3}, {0xABCD,             3},
  {0x10000,           4}, {0xFFFFFF,           4},
  {0x10001,           4}, {0x800000,           4},
  {0x1000000,         5}, {0xFFFFFFFF,         5},
  {0x1000001,         5}, {0x80000000,         5}    
};
static const size_t  k_numLengths =
  sizeof(k_sizedLengths)/sizeof(k_sizedLengths[0]);

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  stringstream  ss;
  for (size_t i = 0; i < k_numLengths; ++i) {
    if (! UnitAssert(k_sizedLengths[i].first.Write(ss))) {
      break;
    }
  }
  for (size_t i = 0; i < k_numLengths; ++i) {
    EncodedUint32  sizedLength;
    if (! UnitAssert(sizedLength.Read(ss))) {
      break;
    }
    UnitAssert(sizedLength == k_sizedLengths[i].first);
  }

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRandomStreamIO(size_t numIterations)
{
  std::random_device  rnd;
  std::mt19937  gen(rnd());
  std::uniform_int_distribution<uint32_t>  distrib(0,0xFFFFFFFFull);
  stringstream  ss;
  for (size_t i = 0; i < numIterations; ++i) {
    EncodedUint32  slw = distrib(gen);
    if (UnitAssert(slw.Write(ss))) {
      EncodedUint32  slr;
      if (UnitAssert(slr.Read(ss))) {
        UnitAssert(slr == slw);
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNStreamIO()
{
  stringstream  ss;
  for (size_t i = 0; i < k_numLengths; ++i) {
    if (! UnitAssert(k_sizedLengths[i].first.NWrite(ss))) {
      break;
    }
  }
  for (size_t i = 0; i < k_numLengths; ++i) {
    EncodedUint32  sizedLength;
    if (! UnitAssert(sizedLength.NRead(ss))) {
      break;
    }
    UnitAssert(sizedLength == k_sizedLengths[i].first);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO()
{
  FILE *f = fopen("/tmp/EncodedUint32TestFileIO","w");
  if (UnitAssert(f)) {
    for (size_t i = 0; i < k_numLengths; ++i) {
      if (! UnitAssert(k_sizedLengths[i].first.Write(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/EncodedUint32TestFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        if (! UnitAssert(sizedLength.Read(f))) {
          break;
        }
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      fclose(f);
    }
    std::remove("/tmp/EncodedUint32TestFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRandomFileIO(size_t numIterations)
{
  FILE *f = fopen("/tmp/EncodedUint32TestRandomFileIO","w");
  if (UnitAssert(f)) {
    std::random_device  rnd;
    std::mt19937  gen(rnd());
    std::uniform_int_distribution<uint32_t>  distrib(0,0xFFFFFFFFull);
    std::vector<EncodedUint32>  slvec;
    for (size_t i = 0; i < numIterations; ++i) {
      EncodedUint32  slw = distrib(gen);
      slvec.push_back(slw);
    }
    for (const auto & sl : slvec) {
      if (! UnitAssert(sl.Write(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/EncodedUint32TestRandomFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < slvec.size(); ++i) {
        EncodedUint32  slr;
        if (! UnitAssert(slr.Read(f))) {
          break;
        }
        UnitAssert(slr == slvec[i]);
      }
      fclose(f);
    }
    std::remove("/tmp/EncodedUint32TestRandomFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNFileIO()
{
  FILE *f = fopen("/tmp/EncodedUint32TestNFileIO","w");
  if (UnitAssert(f)) {
    for (size_t i = 0; i < k_numLengths; ++i) {
      if (! UnitAssert(k_sizedLengths[i].first.NWrite(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/EncodedUint32TestNFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        if (! UnitAssert(sizedLength.NRead(f))) {
          break;
        }
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      fclose(f);
    }
    std::remove("/tmp/EncodedUint32TestNFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO()
{
  //  EncodedUint32 values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  int fd = open("/tmp/EncodedUint32TestDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (size_t i = 0; i < k_numLengths; ++i) {
      uint64_t  bytesWritten = k_sizedLengths[i].first.Write(fd);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        // did not write expected number of bytes
        break;
      }
      totalBytesWritten += bytesWritten;
    }
    ::close(fd);
    uint64_t  expectedTotalBytes =
      std::accumulate(k_sizedLengths, &k_sizedLengths[k_numLengths],
                      (uint64_t)0,
                      [] (auto && a, auto l) { return a + l.second; });
    UnitAssert(totalBytesWritten == expectedTotalBytes);
    
    fd = open("/tmp/EncodedUint32TestDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        uint64_t  bytesRead = sizedLength.Read(fd);
        if (! UnitAssert(bytesRead == k_sizedLengths[i].second)) {
          // did not read expected number of bytes
          break;
        }
        totalBytesRead += bytesRead;
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      ::close(fd);
      UnitAssert(totalBytesRead == expectedTotalBytes);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRandomDescriptorIO(size_t numIterations)
{
  //  EncodedUint32 values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  int fd = open("/tmp/EncodedUint32TestDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    std::random_device  rnd;
    std::mt19937  gen(rnd());
    std::uniform_int_distribution<uint64_t>  distrib(0,0xFFFFFFFFFFFFFFFFull);
    std::vector<EncodedUint32>  slvec;
    for (size_t i = 0; i < numIterations; ++i) {
      EncodedUint32  slw = distrib(gen);
      slvec.push_back(slw);
    }

    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (const auto & sl : slvec) {
      ssize_t  bytesWritten = sl.Write(fd);
      if (UnitAssert(bytesWritten > 0)) {
        totalBytesWritten += bytesWritten;
      }
      else {
        break;
      }
    }
    ::close(fd);
    fd = open("/tmp/EncodedUint32TestDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < slvec.size(); ++i) {
        EncodedUint32  sizedLength;
        ssize_t  bytesRead = sizedLength.Read(fd);
        if (! UnitAssert(bytesRead > 0)) {
          // did not read expected number of bytes
          break;
        }
        totalBytesRead += bytesRead;
        UnitAssert(sizedLength == slvec[i]);
      }
      ::close(fd);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNDescriptorIO()
{
  int fd = open("/tmp/EncodedUint32TestNDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (size_t i = 0; i < k_numLengths; ++i) {
      uint64_t  bytesWritten = k_sizedLengths[i].first.NWrite(fd);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        // did not write expected number of bytes
        break;
      }
      totalBytesWritten += bytesWritten;
    }
    ::close(fd);
    uint64_t  expectedTotalBytes =
      std::accumulate(k_sizedLengths, &k_sizedLengths[k_numLengths],
                      (uint64_t)0,
                      [] (auto && a, auto l) { return a + l.second; });
    UnitAssert(totalBytesWritten == expectedTotalBytes);
    
    fd = open("/tmp/EncodedUint32TestNDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        uint64_t  bytesRead = sizedLength.NRead(fd);
        if (! UnitAssert(bytesRead == k_sizedLengths[i].second)) {
          // did not read expected number of bytes
          break;
        }
        UnitAssert(sizedLength == k_sizedLengths[i].first);
        totalBytesRead += bytesRead;
      }
      ::close(fd);
      UnitAssert(totalBytesRead == expectedTotalBytes);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestNDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO()
{
  BZFILE  *bzf = BZ2_bzopen("/tmp/EncodedUint32TestBZ2IO", "wb");
  if (UnitAssert(bzf)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (size_t i = 0; i < k_numLengths; ++i) {
      uint64_t  bytesWritten = k_sizedLengths[i].first.BZWrite(bzf);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        //  did not write expected number of bytes
        break;
      }
      totalBytesWritten += bytesWritten;
    }
    BZ2_bzclose(bzf);
    uint64_t  expectedTotalBytes =
      std::accumulate(k_sizedLengths, &k_sizedLengths[k_numLengths],
                      (uint64_t)0,
                      [] (auto && a, auto l) { return a + l.second; });
    UnitAssert(totalBytesWritten == expectedTotalBytes);
    
    bzf = BZ2_bzopen("/tmp/EncodedUint32TestBZ2IO", "rb");
    if (UnitAssert(bzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        uint64_t  bytesRead = sizedLength.BZRead(bzf);
        if (! UnitAssert(bytesRead == k_sizedLengths[i].second)) {
          // did not read expected number of bytes
          break;
        }
        UnitAssert(sizedLength == k_sizedLengths[i].first);
        totalBytesRead += bytesRead;
      }
      BZ2_bzclose(bzf);
      UnitAssert(totalBytesRead == expectedTotalBytes);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestBZ2IO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNBZ2IO()
{
  BZFILE  *bzf = BZ2_bzopen("/tmp/EncodedUint32TestNBZ2IO", "wb");
  if (UnitAssert(bzf)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    
    for (size_t i = 0; i < k_numLengths; ++i) {
      int  bytesWritten = k_sizedLengths[i].first.NBZWrite(bzf);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        break;
      }
      totalBytesWritten += bytesWritten;
    }
    BZ2_bzclose(bzf);
    const uint64_t  expectedTotalBytes =
      std::accumulate(k_sizedLengths, &k_sizedLengths[k_numLengths],
                      (uint64_t)0,
                      [] (auto && a, auto l) { return a + l.second; });
    UnitAssert(expectedTotalBytes == totalBytesWritten);

    bzf = BZ2_bzopen("/tmp/EncodedUint32TestNBZ2IO", "rb");
    if (UnitAssert(bzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        int  bytesRead = sizedLength.NBZRead(bzf);
        if (! UnitAssert(bytesRead == k_sizedLengths[i].second)) {
          // did not read expected number of bytes
          break;
        }
        totalBytesRead += bytesRead;
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      BZ2_bzclose(bzf);
      UnitAssert(expectedTotalBytes == totalBytesRead);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestNBZ2IO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO()
{
  gzFile  gzf = gzopen("/tmp/EncodedUint32TestGZIO", "wb");
  if (UnitAssert(gzf)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (size_t i = 0; i < k_numLengths; ++i) {
      uint64_t  bytesWritten = k_sizedLengths[i].first.Write(gzf);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        //  did not write expected number of bytes
        break;
      }
      totalBytesWritten += bytesWritten;
    }
    gzclose(gzf);
    uint64_t  expectedTotalBytes =
      std::accumulate(k_sizedLengths, &k_sizedLengths[k_numLengths],
                      (uint64_t)0,
                      [] (auto && a, auto l) { return a + l.second; });
    UnitAssert(totalBytesWritten == expectedTotalBytes);

    gzf = gzopen("/tmp/EncodedUint32TestGZIO", "rb");
    if (UnitAssert(gzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        uint64_t  bytesRead = sizedLength.Read(gzf);
        if (! UnitAssert(bytesRead == k_sizedLengths[i].second)) {
          //  did not read expected number of bytes
          break;
        }
        totalBytesRead += bytesRead;
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      gzclose(gzf);
      UnitAssert(totalBytesRead == expectedTotalBytes);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestGZIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNGZIO()
{
  gzFile  gzf = gzopen("/tmp/EncodedUint32TestNGZIO", "wb");
  if (UnitAssert(gzf)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (size_t i = 0; i < k_numLengths; ++i) {
      uint64_t  bytesWritten = k_sizedLengths[i].first.NWrite(gzf);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        // did not write expected number of bytes
        break;
      }
      totalBytesWritten += bytesWritten;
    }
    gzclose(gzf);
    uint64_t  expectedTotalBytes =
      std::accumulate(k_sizedLengths, &k_sizedLengths[k_numLengths],
                      (uint64_t)0,
                      [] (auto && a, auto l) { return a + l.second; });
    UnitAssert(totalBytesWritten == expectedTotalBytes);
    
    gzf = gzopen("/tmp/EncodedUint32TestNGZIO", "rb");
    if (UnitAssert(gzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedUint32  sizedLength;
        uint64_t  bytesRead = sizedLength.NRead(gzf);
        if (! UnitAssert(bytesRead == k_sizedLengths[i].second)) {
          //  did not read expected number of bytes
          break;
        }
        totalBytesRead += bytesRead;
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      gzclose(gzf);
      UnitAssert(totalBytesRead == expectedTotalBytes);
      UnitAssert(totalBytesRead == totalBytesWritten);
    }
    std::remove("/tmp/EncodedUint32TestNGZIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestStreamIO();
  TestRandomStreamIO(512);
  TestNStreamIO();
  TestFileIO();
  TestRandomFileIO(512);
  TestNFileIO();
  TestDescriptorIO();
  TestRandomDescriptorIO(512);
  TestNDescriptorIO();
  TestBZ2IO();
  TestNBZ2IO();
  TestGZIO();
  TestNGZIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
