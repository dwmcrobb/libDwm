//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestMacAddress.cc 8389 $
// @(#) $Id: TestMacAddress.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file TestMacAddress.cc
//!  \brief Unit tests for Dwm::MacAddress
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>   //  for open()
  #include <unistd.h>  //  for unlink(), getpid()
}

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmMacAddress.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestMacAddress.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestStreamIO()
{
  ostringstream  os;
  os << "TestMacAddress." << getpid();
  
  MacAddress  mac1("00:04:23:b0:ab:9e");
  ofstream    ofs(os.str().c_str());
  assert(ofs);
  UnitAssert(mac1.Write(ofs));
  ofs.close();
  
  MacAddress  mac2;
  ifstream    ifs(os.str().c_str());
  assert(ifs);
  UnitAssert(mac2.Read(ifs));
  ifs.close();
  UnitAssert(mac2 == mac1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestDescriptorIO()
{
  ostringstream  os;
  os << "TestMacAddress." << getpid();
  
  MacAddress  mac1("00:04:23:b0:ab:9e");
  int  fd = open(os.str().c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0644);
  assert(fd >= 0);
  UnitAssert(mac1.Write(fd));
  close(fd);
  
  MacAddress  mac2;
  fd = open(os.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  UnitAssert(mac2.Read(fd));
  close(fd);
  UnitAssert(mac2 == mac1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestFileIO()
{
  ostringstream  os;
  os << "TestMacAddress." << getpid();
  
  MacAddress  mac1("00:04:23:b0:ab:9e");
  FILE  *f = fopen(os.str().c_str(), "wb");
  assert(f);
  UnitAssert(mac1.Write(f));
  fclose(f);
  
  MacAddress  mac2;
  f = fopen(os.str().c_str(), "rb");
  assert(f);
  UnitAssert(mac2.Read(f));
  fclose(f);
  UnitAssert(mac2 == mac1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestGZIO()
{
  ostringstream  os;
  os << "TestMacAddress." << getpid() << ".gz";
  
  MacAddress  mac1("00:04:23:b0:ab:9e");
  gzFile  gzf = gzopen(os.str().c_str(), "w");
  assert(gzf);
  UnitAssert(mac1.Write(gzf));
  gzclose(gzf);
  
  MacAddress  mac2;
  gzf = gzopen(os.str().c_str(), "r");
  assert(gzf);
  UnitAssert(mac2.Read(gzf));
  gzclose(gzf);
  UnitAssert(mac2 == mac1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestBZ2IO()
{
  ostringstream  os;
  os << "TestMacAddress." << getpid() << ".bz2";
  
  MacAddress  mac1("00:04:23:b0:ab:9e");
  BZFILE  *bzf = BZ2_bzopen(os.str().c_str(), "w");
  assert(bzf);
  UnitAssert(mac1.BZWrite(bzf));
  BZ2_bzclose(bzf);
  
  MacAddress  mac2;
  bzf = BZ2_bzopen(os.str().c_str(), "r");
  assert(bzf);
  UnitAssert(mac2.BZRead(bzf));
  BZ2_bzclose(bzf);
  UnitAssert(mac2 == mac1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  TestStreamIO();
  TestDescriptorIO();
  TestFileIO();
  TestGZIO();
  TestBZ2IO();
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestPrint()
{
  string  s("00:04:23:b0:ab:9e");
  MacAddress  mac1(s);
  ostringstream  os;
  os << mac1;
  UnitAssert(os.str() == s);

  istringstream  is(s);
  MacAddress  mac2;
  is >> mac2;
  UnitAssert(mac1 == mac2);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIO();
  TestPrint();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);
}
