//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIODeque.cc 11087 $
// @(#) $Id: TestIODeque.cc 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file TestIODeque.cc
//!  \brief Unit tests for Dwm::IO and Dwm::GZIO with deques
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <stdlib.h>
  #include <unistd.h>
}

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmGZIO.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIODeque.cc 11087 $");

using namespace std;
using namespace Dwm;

#define STOREFILE "/tmp/DwmTestIODeque.store"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO()
{
  string  fileName(STOREFILE);
  fileName += ".gz";
  gzFile  gzf = gzopen(fileName.c_str(),"wb");
  if (gzf) {
    deque<uint32_t>  d1;
    for (uint32_t i = 0; i < 10000; ++i)
      d1.push_back(i);
    
    GZIO::Write(gzf, d1);
    gzclose(gzf);
    
    gzf = gzopen(fileName.c_str(),"rb");
    if (gzf) {
      deque<uint32_t>  d2;
      GZIO::Read(gzf, d2);
      gzclose(gzf);
      UnitAssert(d2 == d1);
    }
    else {
      cerr << "Failed to gzopen('" << fileName.c_str() << "') for reading: "
           << strerror(errno) << endl;
    }

    std::remove(fileName.c_str());
  }
  else {
    cerr << "Failed to gzopen('" << fileName.c_str() << "') for writing: "
         << strerror(errno) << endl;
  }

  return;
}
    
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  deque<string>  deque1;
  
  deque1.push_back("1");
  deque1.push_back("2");
  deque1.push_back("3");
  
  ofstream  os(STOREFILE);
  if (os) {
    if (! StreamIO::Write(os, deque1)) {
      cerr << "Failed to write deque1!" << endl;
      exit(1);
    }
    os.close();
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for writing!" << endl;
    exit(1);
  }
  
  deque<string>  deque2;
  ifstream  is(STOREFILE);
  if (is) {
    if (! StreamIO::Read(is, deque2)) {
      cerr << "Failed to read deque2!" << endl;
      exit(1);
    }
    is.close();
    std::remove(STOREFILE);
    
    UnitAssert(deque2 == deque1);
    UnitAssert(deque1.size() == 3);
    
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for reading!" << endl;
    exit(1);
  }

  deque<string>  deque3 = deque1;
  int  fd = open(STOREFILE,O_WRONLY|O_CREAT,0666);
  if (fd >= 0) {
    if (! DescriptorIO::Write(fd, deque3)) {
      cerr << "Failed to write deque3 to descriptor!" << endl;
      exit(1);
    }
    close(fd);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for writing!" << endl;
    exit(1);
  }
  
  fd = open(STOREFILE,O_RDONLY);
  if (fd >= 0) {
    if (! DescriptorIO::Read(fd, deque2)) {
      cerr << "Failed to read deque2 from descriptor!" << endl;
      exit(1);
    }
    close(fd);
    UnitAssert(deque2 == deque3);
    UnitAssert(deque2.size() == 3);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for reading!" << endl;
    exit(1);
  }
    
  std::remove(STOREFILE);

  FILE  *f = fopen(STOREFILE,"w");
  if (f) {
    if (! FileIO::Write(f, deque1)) {
      cerr << "Failed to write deque1 to FILE!" << endl;
      exit(1);
    }
    fclose(f);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for writing!" << endl;
    exit(1);
  }
  
  deque3.clear();
  f = fopen(STOREFILE,"r");
  if (f) {
    if (! FileIO::Read(f, deque3)) {
      cerr << "Failed to read deque3 to FILE!" << endl;
      exit(1);
    }
    fclose(f);
    UnitAssert (deque2 == deque3);
    UnitAssert(deque3.size() == 3);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for reading!" << endl;
    exit(1);
  }

  std::remove(STOREFILE);

  TestGZIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);
}
