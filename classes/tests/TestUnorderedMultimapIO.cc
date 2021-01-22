//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestUnorderedMultimapIO.cc 11087 $
// @(#) $Id: TestUnorderedMultimapIO.cc 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2020
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
//!  \file TestUnorderedMultimapIO.cc
//!  \author Daniel W. McRobb
//!  \brief unordered_multimap I/O unit tests
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
}

#include <cstdio>
#include <fstream>
#include <sstream>
#include <utility>

#include "DwmPortability.hh"

#include <unordered_map>
using std::unordered_multimap;

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmStreamIO.hh"
#include "DwmOperators.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestUnorderedMultimapIO.cc 11087 $");

using namespace std;
using namespace Dwm;

#define STOREFILE "/tmp/DwmTestUnorderedMultimapIO.store"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename K, typename V>
static void TestStreamIO(const unordered_multimap<K,V> & map1)
{
  ofstream  os(STOREFILE);
  if (UnitAssert(os)) {
    UnitAssert(StreamIO::Write(os, map1));
    os.close();
    unordered_multimap<K,V>  map2;
    ifstream  is(STOREFILE);
    if (UnitAssert(is)) {
      UnitAssert(StreamIO::Read(is, map2));
      is.close();
      UnitAssert(map2 == map1);
    }
  }
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename K, typename V>
static void TestDescriptorIO(const unordered_multimap<K,V> & map1)
{
  int  fd = open(STOREFILE,O_WRONLY|O_CREAT,0666);
  if (UnitAssert(fd >= 0)) {
    UnitAssert(DescriptorIO::Write(fd, map1));
    close(fd);
    fd = open(STOREFILE,O_RDONLY);
    if (UnitAssert(fd >= 0)) {
      unordered_multimap<K,V>  map2;
      UnitAssert(DescriptorIO::Read(fd, map2));
      close(fd);
      UnitAssert(map2 == map1);
    }
  }
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename K, typename V>
static void TestFileIO(const unordered_multimap<K,V> & map1)
{
  FILE  *f = fopen(STOREFILE, "w");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Write(f, map1));
    fclose(f);
    f = fopen(STOREFILE, "r");
    if (UnitAssert(f)) {
      unordered_multimap<K,V>  map2;
      UnitAssert(FileIO::Read(f, map2));
      fclose(f);
      UnitAssert (map2 == map1);
    }
  }
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename K, typename V>
static void TestBZ2IO(const unordered_multimap<K,V> & map1)
{
  BZFILE  *bzf = BZ2_bzopen(STOREFILE, "wb");
  UnitAssert(bzf);
  UnitAssert(BZ2IO::BZWrite(bzf, map1));
  BZ2_bzclose(bzf);
  
  unordered_multimap<K,V>  map2;
  bzf = BZ2_bzopen(STOREFILE, "rb");
  UnitAssert(bzf);
  UnitAssert(BZ2IO::BZRead(bzf, map2));
  BZ2_bzclose(bzf);
  UnitAssert(map2 == map1);

  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename K, typename V>
static void TestGZIO(const unordered_multimap<K,V> & map1)
{
  gzFile  gzf = gzopen(STOREFILE, "wb");
  UnitAssert(gzf);
  UnitAssert(GZIO::Write(gzf, map1));
  gzclose(gzf);
  
  unordered_multimap<K,V>  map2;
  gzf = gzopen(STOREFILE, "rb");
  UnitAssert(gzf);
  UnitAssert(GZIO::Read(gzf, map2));
  gzclose(gzf);
  UnitAssert(map2 == map1);

  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ostringstream  os;
  
  unordered_multimap<uint32_t,string>  map1;
  for (uint32_t i = 0; i < 250; ++i) {
    os << i;
    map1.insert(unordered_multimap<uint32_t,string>::value_type(i, os.str()));
    os << i;
    map1.insert(unordered_multimap<uint32_t,string>::value_type(i, os.str()));
    os.str("");
  }
  UnitAssert(map1.size() == 500);
  TestStreamIO(map1);
  TestDescriptorIO(map1);
  TestFileIO(map1);
  TestBZ2IO(map1);
  TestGZIO(map1);

  unordered_multimap<string,uint32_t>  map2;
  for (uint32_t i = 0; i < 250; ++i) {
    os << i;
    map2.insert(unordered_multimap<string,uint32_t>::value_type(os.str(), i));
    map2.insert(make_pair<string,uint32_t>(os.str(), i+1));
    os.str("");
  }
  UnitAssert(map2.size() == 500);
  TestStreamIO(map2);
  TestDescriptorIO(map2);
  TestFileIO(map2);
  TestBZ2IO(map2);
  TestGZIO(map2);
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }

  return(0);
}
