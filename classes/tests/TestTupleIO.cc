//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestTupleIO.cc 11087 $
// @(#) $Id: TestTupleIO.cc 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016, 2020, 2024
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
//!  \file TestTupleIO.cc
//!  \author Daniel W. McRobb
//!  \brief tuple I/O tests
//---------------------------------------------------------------------------

extern "C" {
#include <fcntl.h>
}

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "DwmBZ2IO.hh"
#include "DwmGZIO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmIOUtils.hh"
#include "DwmPortability.hh"
#include "DwmSvnTag.hh"
#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestTupleIO.cc 11087 $");

using namespace std;
using namespace Dwm;

#define TUPLE_TEST_FILE    "/tmp/TupleIOTest"
#define TUPLE_TEST_GZFILE  "/tmp/TupleIOTest.gz"
#define TUPLE_TEST_BZ2FILE "/tmp/TupleIOTest.gz"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename TupleType>
static void TestTupleStreamIO(const TupleType & t)
{
  ofstream  os(TUPLE_TEST_FILE);
  UnitAssert(StreamIO::Write(os, t));
  os.close();
  TupleType  u;
  ifstream  is(TUPLE_TEST_FILE);
  UnitAssert(StreamIO::Read(is, u));
  is.close();
  UnitAssert(u == t);
  std::remove(TUPLE_TEST_FILE);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename TupleType>
static void TestTupleDescriptorIO(const TupleType & t)
{
  int  fd = open(TUPLE_TEST_FILE, O_WRONLY|O_CREAT, 0666);
  if (UnitAssert(fd >= 0)) {
    UnitAssert(DescriptorIO::Write(fd, t) == IOUtils::StreamedLength(t));
    close(fd);
    fd = open(TUPLE_TEST_FILE, O_RDONLY);
    if (UnitAssert(fd >= 0)) {
      TupleType  u;
      UnitAssert(DescriptorIO::Read(fd, u));
      close(fd);
      UnitAssert(u == t);
    }
  }
  std::remove(TUPLE_TEST_FILE);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename TupleType>
static void TestTupleFileIO(const TupleType & t)
{
  FILE  *f = fopen(TUPLE_TEST_FILE, "wb");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Write(f, t));
    fclose(f);
    f = fopen(TUPLE_TEST_FILE, "rb");
    if (UnitAssert(f)) {
      TupleType  u;
      UnitAssert(FileIO::Read(f, u));
      fclose(f);
      UnitAssert(u == t);
    }
  }
  std::remove(TUPLE_TEST_FILE);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename TupleType>
static void TestTupleBZ2IO(const TupleType & t)
{
  BZFILE  *bzf = BZ2_bzopen(TUPLE_TEST_BZ2FILE, "wb");
  UnitAssert(bzf);
  UnitAssert(BZ2IO::BZWrite(bzf, t));
  BZ2_bzclose(bzf);
  bzf = BZ2_bzopen(TUPLE_TEST_BZ2FILE, "rb");
  if (UnitAssert(bzf)) {
    TupleType  u;
    UnitAssert(BZ2IO::BZRead(bzf, u));
    BZ2_bzclose(bzf);
    UnitAssert(u == t);
  }
  std::remove(TUPLE_TEST_BZ2FILE);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename TupleType>
static void TestTupleGZIO(const TupleType & t)
{
  gzFile  gzf = gzopen(TUPLE_TEST_GZFILE, "wb");
  UnitAssert(gzf);
  UnitAssert(GZIO::Write(gzf, t));
  gzclose(gzf);
  gzf = gzopen(TUPLE_TEST_GZFILE, "rb");
  if (UnitAssert(gzf)) {
    TupleType  u;
    UnitAssert(GZIO::Read(gzf, u));
    gzclose(gzf);
    UnitAssert(u == t);
  }
  std::remove(TUPLE_TEST_GZFILE);
  return;
}  
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename TupleType>
static void TestTupleIO(const TupleType & t)
{
  TestTupleStreamIO(t);
  TestTupleDescriptorIO(t);
  TestTupleFileIO(t);
  TestTupleBZ2IO(t);
  TestTupleGZIO(t);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSingleTuples()
{
  tuple<int,string>
    t2(0, "dwm");
  UnitAssert(IOUtils::StreamedLength(t2) == 15);
  TestTupleIO(t2);

  tuple<int,string,string>
    t3(0, "dwm", "mwd");
  UnitAssert(IOUtils::StreamedLength(t3) == 26);
  TestTupleIO(t3);

  tuple<int,string,int,string>
    t4(0, "dwm", 2, "wmd");
  UnitAssert(IOUtils::StreamedLength(t4) == 30);
  TestTupleIO(t4);

  tuple<int,string,int,string,int>
    t5(1, "alpha", 2, "beta", 3);
  UnitAssert(IOUtils::StreamedLength(t5) == 37);
  TestTupleIO(t5);

  tuple<int,string,int,string,int,string>
    t6(1, "alpha", 2, "beta", 3, "gamma");
  UnitAssert(IOUtils::StreamedLength(t6) == 50);
  TestTupleIO(t6);

  tuple<int,string,int,string,int,string,int>
    t7(1, "alpha", 2, "beta", 3, "gamma", 4);
  UnitAssert(IOUtils::StreamedLength(t7) == 54);
  TestTupleIO(t7);

  tuple<int,string,int,string,int,string,int,string>
    t8(1, "alpha", 2, "beta", 3, "gamma", 4, "delta");
  UnitAssert(IOUtils::StreamedLength(t8) == 67);
  TestTupleIO(t8);

  tuple<int,string,int,string,int,string,int,string,int>
    t9(1, "alpha", 2, "beta", 3, "gamma", 4, "delta", 5);
  UnitAssert(IOUtils::StreamedLength(t9) == 71);
  TestTupleIO(t9);

  tuple<int,string,int,string,int,string,int,string,int,string>
    t10(1, "alpha", 2, "beta", 3, "gamma", 4, "delta", 5, "epsilon");
  UnitAssert(IOUtils::StreamedLength(t10) == 86);
  
  TestTupleIO(t10);

  tuple<int,string,tuple<int,string>>  t11(1,"alpha",{2,"beta"});
  UnitAssert(IOUtils::StreamedLength(t11) == 33);
  TestTupleIO(t11);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestMap()
{
  typedef tuple<uint64_t,uint32_t,uint16_t,uint16_t,uint8_t>  key;
  typedef pair<uint64_t,uint64_t>  value;
  
  map<key,value>  m;
  
  srandom(getpid());
  for (uint32_t i = 0; i < 1000; ++i) {
    key  k(random(), random(), random() % 65535,
           random() % 65535, random() % 255);
    value  v(i, i * 512);
    m[k] = v;
  }
  UnitAssert(m.size() == 1000);
  
  TestTupleIO(m);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestVector()
{
  typedef tuple<uint64_t,uint32_t,uint16_t,uint8_t,string>  entry;
  vector<entry>  v;
  
  srandom(getpid());
  ostringstream  os;
  for (uint32_t i = 0; i < 1000; ++i) {
    os << i;
    entry  e(random(), random(), random() % 65535, random() % 255, os.str());
    v.push_back(e);
    os.str("");
  }
  
  TestTupleIO(v);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestSingleTuples();
  TestMap();
  TestVector();

  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;

  exit(0);
  
testFailed:
  
  Assertions::Print(cerr, true);
  exit(1);
}

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
