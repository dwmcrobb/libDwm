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
//!  \file TestEncodedUint32.cc
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
#include "DwmEncodedSigned.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

using EncodedS64 = EncodedSigned<int64_t>;

//  EncodedS64 values and the number of bytes we expect to write/read
//  for each of the values, including the size byte.
static const pair<EncodedS64,ssize_t>  k_sizedLengths[] = {
  {0,                       2}, {1,                      2},
  {-128,                    2}, {127,                    2},
  {-255,                    2}, {255,                    2},
  {-32768,                  3}, {32767,                  3},
  {-65535,                  3}, {65535,                  3},
  {-8388608,                4}, {8388607,                4},
  {-2147483648,             5}, {2147483647,             5},
  {-4294967295,             5}, {4294967295,             5},
  {-549755813888ll,         6}, {549755813887ll,         6},
  {-1099511627775ll,        6}, {1099511627775ll,        6},
  {-140737488355328ll,      7}, {140737488355327ll,      7},
  {-281474976710655ll,      7}, {281474976710656ll,      7},
  {-36028797018963968ll,    8}, {36028797018963967ll,    8},
  {-72057594037927935ll,    8}, {72057594037927936ll,    8},
  {-9223372036854775807ll,  9}, {9223372036854775807ll,  9},
  {-18446744073709551615ll, 9}, {18446744073709551615ll, 9}
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
    EncodedS64  sizedLength;
    if (! UnitAssert(sizedLength.Read(ss))) {
      break;
    }
    if (! UnitAssert(sizedLength == k_sizedLengths[i].first)) {
      cerr << sizedLength << " != " << k_sizedLengths[i].first
           << " (" << __FILE__ << ':' << __LINE__ << ")\n";
    }
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
  std::uniform_int_distribution<int64_t>
    distrib(-0x8000000000000000,0x7FFFFFFFFFFFFFFF);
  stringstream  ss;
  for (size_t i = 0; i < numIterations; ++i) {
    EncodedS64  slw = distrib(gen);
    if (UnitAssert(slw.Write(ss))) {
      EncodedS64  slr;
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
    EncodedS64  sizedLength;
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
  FILE *f = fopen("/tmp/EncodedS64TestFileIO","w");
  if (UnitAssert(f)) {
    for (size_t i = 0; i < k_numLengths; ++i) {
      if (! UnitAssert(k_sizedLengths[i].first.Write(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/EncodedS64TestFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
        if (! UnitAssert(sizedLength.Read(f))) {
          break;
        }
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      fclose(f);
    }
    std::remove("/tmp/EncodedS64TestFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRandomFileIO(size_t numIterations)
{
  FILE *f = fopen("/tmp/EncodedS64TestRandomFileIO","w");
  if (UnitAssert(f)) {
    std::random_device  rnd;
    std::mt19937  gen(rnd());
    std::uniform_int_distribution<int64_t>
      distrib(-0x8000000000000000ll,0x7FFFFFFFFFFFFFFFll);
    std::vector<EncodedS64>  slvec;
    for (size_t i = 0; i < numIterations; ++i) {
      EncodedS64  slw = distrib(gen);
      slvec.push_back(slw);
    }
    for (const auto & sl : slvec) {
      if (! UnitAssert(sl.Write(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/EncodedS64TestRandomFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < slvec.size(); ++i) {
        EncodedS64  slr;
        if (! UnitAssert(slr.Read(f))) {
          break;
        }
        UnitAssert(slr == slvec[i]);
      }
      fclose(f);
    }
    std::remove("/tmp/EncodedS64TestRandomFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNFileIO()
{
  FILE *f = fopen("/tmp/EncodedS64TestNFileIO","w");
  if (UnitAssert(f)) {
    for (size_t i = 0; i < k_numLengths; ++i) {
      if (! UnitAssert(k_sizedLengths[i].first.NWrite(f))) {
        break;
      }
    }
    fclose(f);
    f = fopen("/tmp/EncodedS64TestNFileIO","r");
    if (UnitAssert(f)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
        if (! UnitAssert(sizedLength.NRead(f))) {
          break;
        }
        UnitAssert(sizedLength == k_sizedLengths[i].first);
      }
      fclose(f);
    }
    std::remove("/tmp/EncodedS64TestNFileIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO()
{
  //  EncodedS64 values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  int fd = open("/tmp/EncodedS64TestDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    uint64_t  totalBytesWritten = 0, totalBytesRead = 0;
    for (size_t i = 0; i < k_numLengths; ++i) {
      uint64_t  bytesWritten = k_sizedLengths[i].first.Write(fd);
      if (! UnitAssert(bytesWritten == k_sizedLengths[i].second)) {
        // did not write expected number of bytes
        std::cerr << bytesWritten << " != " << k_sizedLengths[i].second
                  << " for " << k_sizedLengths[i].first << '\n';
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
    
    fd = open("/tmp/EncodedS64TestDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRandomDescriptorIO(size_t numIterations)
{
  //  EncodedS64 values and the number of bytes we expect to write/read
  //  for each of the values, including the size byte.
  int fd = open("/tmp/EncodedS64TestDescriptorIO",
                O_WRONLY|O_CREAT|O_TRUNC,0644);
  if (UnitAssert(0 <= fd)) {
    std::random_device  rnd;
    std::mt19937  gen(rnd());
    std::uniform_int_distribution<int64_t>
      distrib(-0x8000000000000000,0x7FFFFFFFFFFFFFFFll);
    std::vector<EncodedS64>  slvec;
    for (size_t i = 0; i < numIterations; ++i) {
      EncodedS64  slw = distrib(gen);
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
    fd = open("/tmp/EncodedS64TestDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < slvec.size(); ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNDescriptorIO()
{
  int fd = open("/tmp/EncodedS64TestNDescriptorIO",
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
    
    fd = open("/tmp/EncodedS64TestNDescriptorIO",O_RDONLY);
    if (UnitAssert(0 <= fd)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestNDescriptorIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO()
{
  BZFILE  *bzf = BZ2_bzopen("/tmp/EncodedS64TestBZ2IO", "wb");
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
    
    bzf = BZ2_bzopen("/tmp/EncodedS64TestBZ2IO", "rb");
    if (UnitAssert(bzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestBZ2IO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNBZ2IO()
{
  BZFILE  *bzf = BZ2_bzopen("/tmp/EncodedS64TestNBZ2IO", "wb");
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

    bzf = BZ2_bzopen("/tmp/EncodedS64TestNBZ2IO", "rb");
    if (UnitAssert(bzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestNBZ2IO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO()
{
  gzFile  gzf = gzopen("/tmp/EncodedS64TestGZIO", "wb");
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

    gzf = gzopen("/tmp/EncodedS64TestGZIO", "rb");
    if (UnitAssert(gzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestGZIO");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestNGZIO()
{
  gzFile  gzf = gzopen("/tmp/EncodedS64TestNGZIO", "wb");
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
    
    gzf = gzopen("/tmp/EncodedS64TestNGZIO", "rb");
    if (UnitAssert(gzf)) {
      for (size_t i = 0; i < k_numLengths; ++i) {
        EncodedS64  sizedLength;
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
    std::remove("/tmp/EncodedS64TestNGZIO");
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
#if 0
  TestGZIO();
  TestNGZIO();
#endif
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
