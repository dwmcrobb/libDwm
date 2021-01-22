//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestUnorderedMultisetIO.cc 11087 $
// @(#) $Id: TestUnorderedMultisetIO.cc 11087 2020-09-07 09:58:37Z dwm $
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
//!  \file TestUnorderedMultisetIO.cc
//!  \author Daniel W. McRobb
//!  \brief unordered_multiset I/O unit tests
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
}

#include <cstdio>
#include <fstream>
#include <string>

#include "DwmPortability.hh"

#include <unordered_set>
using std::unordered_multiset;

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmStreamIO.hh"
#include "DwmOperators.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestUnorderedMultisetIO.cc 11087 $");

using namespace std;
using namespace Dwm;

#define STOREFILE "/tmp/DwmTestUnorderedMultisetIO.store"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestStreamIO(const unordered_multiset<T> & set1)
{
  ofstream  os(STOREFILE);
  UnitAssert(os);
  UnitAssert(StreamIO::Write(os, set1));
  os.close();
  ifstream  is(STOREFILE);
  UnitAssert(is);
  unordered_multiset<T>  set2;
  UnitAssert(StreamIO::Read(is, set2));
  is.close();
  UnitAssert(set2 == set1);
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestDescriptorIO(const unordered_multiset<T> & set1)
{
  int  fd = open(STOREFILE, O_WRONLY|O_CREAT, 0666);
  if (UnitAssert(fd >= 0)) {
    UnitAssert(DescriptorIO::Write(fd, set1));
    close(fd);
    fd = open(STOREFILE,O_RDONLY);
    if (UnitAssert(fd >= 0)) {
      unordered_multiset<T>  set2;
      UnitAssert(DescriptorIO::Read(fd, set2));
      close(fd);
      UnitAssert(set2 == set1);
    }
  }
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestFileIO(const unordered_multiset<T> & set1)
{
  FILE  *f = fopen(STOREFILE, "w");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Write(f, set1));
    fclose(f);
    f = fopen(STOREFILE,"r");
    if (UnitAssert(f)) {
      unordered_multiset<T>  set2;
      UnitAssert(FileIO::Read(f, set2));
      fclose(f);
      UnitAssert(set2 == set1);
    }
  }
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestBZ2IO(const unordered_multiset<T> & set1)
{
  BZFILE  *bzf = BZ2_bzopen(STOREFILE, "wb");
  UnitAssert(bzf);
  UnitAssert(BZ2IO::BZWrite(bzf, set1));
  BZ2_bzclose(bzf);
  unordered_multiset<T>  set2;
  bzf = BZ2_bzopen(STOREFILE, "rb");
  UnitAssert(bzf);
  UnitAssert(BZ2IO::BZRead(bzf, set2));
  BZ2_bzclose(bzf);
  UnitAssert(set2 == set1);
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestGZIO(const unordered_multiset<T> & set1)
{
  gzFile  gzf = gzopen(STOREFILE, "wb");
  UnitAssert(gzf);
  UnitAssert(GZIO::Write(gzf, set1));
  gzclose(gzf);
  unordered_multiset<T>  set2;
  gzf = gzopen(STOREFILE, "rb");
  UnitAssert(gzf);
  UnitAssert(GZIO::Read(gzf, set2));
  gzclose(gzf);
  UnitAssert(set2 == set1);
  unlink(STOREFILE);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestAllIO(const unordered_multiset<T> & unorderedSet)
{
  TestStreamIO(unorderedSet);
  TestDescriptorIO(unorderedSet);
  TestFileIO(unorderedSet);
  TestBZ2IO(unorderedSet);
  TestGZIO(unorderedSet);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestUint32Set()
{
  unordered_multiset<uint32_t>  uint32Set;
  for (uint32_t i = 0; i < 250; ++i) {
    uint32Set.insert(i);
    uint32Set.insert(i);
  }
  UnitAssert(uint32Set.size() == 500);
  TestAllIO(uint32Set);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStringSet()
{
  unordered_multiset<string>  stringSet;

  stringSet.insert("alpha");     stringSet.insert("beta");
  stringSet.insert("gamma");     stringSet.insert("delta");
  stringSet.insert("epsilon");   stringSet.insert("zeta");
  stringSet.insert("eta");       stringSet.insert("theta");
  stringSet.insert("iota");      stringSet.insert("kappa");
  stringSet.insert("lambda");    stringSet.insert("mu");
  stringSet.insert("nu");        stringSet.insert("xi");
  stringSet.insert("omicron");   stringSet.insert("pi");
  stringSet.insert("rho");       stringSet.insert("sigma");
  stringSet.insert("tau");       stringSet.insert("upsilon");
  stringSet.insert("phi");       stringSet.insert("chi");
  stringSet.insert("psi");       stringSet.insert("omega");

  stringSet.insert("alpha");     stringSet.insert("beta");
  stringSet.insert("gamma");     stringSet.insert("delta");
  stringSet.insert("epsilon");   stringSet.insert("zeta");
  stringSet.insert("eta");       stringSet.insert("theta");
  stringSet.insert("iota");      stringSet.insert("kappa");
  stringSet.insert("lambda");    stringSet.insert("mu");
  stringSet.insert("nu");        stringSet.insert("xi");
  stringSet.insert("omicron");   stringSet.insert("pi");
  stringSet.insert("rho");       stringSet.insert("sigma");
  stringSet.insert("tau");       stringSet.insert("upsilon");
  stringSet.insert("phi");       stringSet.insert("chi");
  stringSet.insert("psi");       stringSet.insert("omega");

  UnitAssert(stringSet.size() == 48);
  TestAllIO(stringSet);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestUint32Set();
  TestStringSet();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }

  return(0);
}
