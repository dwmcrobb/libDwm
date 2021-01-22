//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestMplsLabel.cc 8389 $
// @(#) $Id: TestMplsLabel.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestMplsLabel.cc
//!  \brief Unit tests for Dwm::MplsLabel
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>   // for open()
  #include <unistd.h>  // for unlink(), getpid()
}

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmMplsLabel.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestMplsLabel.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void FillLabel(MplsLabel & label)
{
  label.Label(0xF0F0F);
  label.Exp(0x05);
  label.S(true);
  label.Ttl(170);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  MplsLabel  label1("986895:5:1:170");
  
  ofstream    ofs(os.str().c_str());
  assert(ofs);
  UnitAssert(label1.Write(ofs));
  ofs.close();
  
  MplsLabel  label2;
  ifstream    ifs(os.str().c_str());
  assert(ifs);
  UnitAssert(label2.Read(ifs));
  ifs.close();
  UnitAssert(label2 == label1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestDescriptorIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  MplsLabel  label1;
  FillLabel(label1);
  
  int  fd = open(os.str().c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0644);
  assert(fd >= 0);
  UnitAssert(label1.Write(fd));
  close(fd);
  
  MplsLabel  label2;
  fd = open(os.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  UnitAssert(label2.Read(fd));
  close(fd);
  UnitAssert(label2 == label1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestFileIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  MplsLabel  label1;
  FillLabel(label1);
  
  FILE *f = fopen(os.str().c_str(), "w");
  assert(f);
  UnitAssert(label1.Write(f));
  fclose(f);
  
  MplsLabel  label2;
  f = fopen(os.str().c_str(), "r");
  assert(f);
  UnitAssert(label2.Read(f));
  fclose(f);
  UnitAssert(label2 == label1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestGZIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  MplsLabel  label1;
  FillLabel(label1);
  
  gzFile gzf = gzopen(os.str().c_str(), "w");
  assert(gzf);
  UnitAssert(label1.Write(gzf));
  gzclose(gzf);
  
  MplsLabel  label2;
  gzf = gzopen(os.str().c_str(), "r");
  assert(gzf);
  UnitAssert(label2.Read(gzf));
  gzclose(gzf);
  UnitAssert(label2 == label1);
  
  unlink(os.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestBZ2IO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  MplsLabel  label1;
  FillLabel(label1);
  
  BZFILE  *bzf = BZ2_bzopen(os.str().c_str(), "w");
  assert(bzf);
  UnitAssert(label1.BZWrite(bzf));
  BZ2_bzclose(bzf);
  
  MplsLabel  label2;
  bzf = BZ2_bzopen(os.str().c_str(), "r");
  assert(bzf);
  UnitAssert(label2.BZRead(bzf));
  BZ2_bzclose(bzf);
  UnitAssert(label2 == label1);
  
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
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestPrint()
{
  string  s("986895:5:1:170");
  MplsLabel  label1(s);
  UnitAssert(label1.Label() == 986895);
  UnitAssert(label1.Exp() == 5);
  UnitAssert(label1.S());
  UnitAssert(label1.Ttl() == 170);
  
  ostringstream  os;
  os << label1;
  UnitAssert(os.str() == s);

  s = "65535:3:0:127";
  MplsLabel  label2(s);
  UnitAssert(label2.Label() == 65535);
  UnitAssert(label2.Exp() == 3);
  UnitAssert(! label2.S());
  UnitAssert(label2.Ttl() == 127);
  os.str("");
  os << label2;
  UnitAssert(os.str() == s);
  
  istringstream  is(s);
  MplsLabel  label3;
  is >> label3;
  UnitAssert(label3 == label2);
  
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
