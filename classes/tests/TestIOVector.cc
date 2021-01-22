//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIOVector.cc 11087 $
// @(#) $Id: TestIOVector.cc 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016, 2020
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
//!  \file TestIOVector.cc
//!  \author Daniel W. McRobb
//!  \brief Unit tests for Dwm::IO with vectors
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <stdlib.h>
  #include <unistd.h>
}

#include <cstdio>
#include <fstream>
#include <vector>

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIOVector.cc 11087 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  vector<std::string>  vector1;
  
  vector1.push_back("1");
  vector1.push_back("2");
  vector1.push_back("3");
  
  ofstream  os("/tmp/DwmTestIOVector.store");
  if (UnitAssert(os)) {
    UnitAssert(StreamIO::Write(os, vector1));
    os.close();
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOVector.store' for writing!" << endl;
    exit(1);
  }
  
  vector<std::string>  vector2;
  ifstream  is("/tmp/DwmTestIOVector.store");
  if (UnitAssert(is)) {
    UnitAssert(StreamIO::Read(is, vector2));
    is.close();
    std::remove("/tmp/DwmTestIOVector.store");
    UnitAssert(vector2 == vector1);
    UnitAssert(vector1.size() == 3);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOVector.store' for reading!" << endl;
    exit(1);
  }

  vector<std::string>  vector3 = vector1;
  int  fd = open("/tmp/DwmTestIOVector.store",O_WRONLY|O_CREAT,0666);
  if (UnitAssert(fd >= 0)) {
    UnitAssert(DescriptorIO::Write(fd, vector3));
    close(fd);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOVector.store' for writing!" << endl;
    exit(1);
  }
  
  fd = open("/tmp/DwmTestIOVector.store",O_RDONLY);
  if (UnitAssert(fd >= 0)) {
    UnitAssert(DescriptorIO::Read(fd, vector2));
    close(fd);
    UnitAssert (vector2 == vector3);
    UnitAssert(vector2.size() == 3);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOVector.store' for reading!" << endl;
    exit(1);
  }
    
  std::remove("/tmp/DwmTestIOVector.store");

  FILE  *f = fopen("/tmp/DwmTestIOVector.store","w");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Write(f, vector1));
    fclose(f);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOVector.store' for writing!" << endl;
    exit(1);
  }
  
  vector3.clear();
  f = fopen("/tmp/DwmTestIOVector.store","r");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Read(f, vector3));
    fclose(f);
    UnitAssert (vector2 == vector3);
    UnitAssert(vector3.size() == 3);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOVector.store' for reading!" << endl;
    exit(1);
  }

  std::remove("/tmp/DwmTestIOVector.store");
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
