//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIOSet.cc 11087 $
// @(#) $Id: TestIOSet.cc 11087 2020-09-07 09:58:37Z dwm $
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
//!  \file TestIOSet.cc
//!  \author Daniel W. McRobb
//!  \brief Unit tests for Dwm::IO with sets
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <stdlib.h>
  #include <unistd.h>
}

#include <cstdio>
#include <fstream>

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIOSet.cc 11087 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  set<std::string>  set1;
  
  set1.insert("1");
  set1.insert("2");
  set1.insert("3");
  
  ofstream  os("/tmp/DwmTestIOSet.store");
  if (UnitAssert(os)) {
    UnitAssert(StreamIO::Write(os, set1));
    os.close();
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOSet.store' for writing!" << endl;
    exit(1);
  }
  
  set<std::string>  set2;
  ifstream  is("/tmp/DwmTestIOSet.store");
  if (UnitAssert(is)) {
    UnitAssert(StreamIO::Read(is, set2));
    is.close();
    std::remove("/tmp/DwmTestIOSet.store");
    UnitAssert(set2 == set1);
    UnitAssert(set1.size() == 3);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOSet.store' for reading!" << endl;
    exit(1);
  }

  set<std::string>  set3 = set1;
  int  fd = open("/tmp/DwmTestIOSet.store",O_WRONLY|O_CREAT,0666);
  if (UnitAssert(fd >= 0)) {
    UnitAssert (DescriptorIO::Write(fd, set3));
    close(fd);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOSet.store' for writing!" << endl;
    exit(1);
  }
  
  fd = open("/tmp/DwmTestIOSet.store",O_RDONLY);
  if (UnitAssert(fd >= 0)) {
    UnitAssert(DescriptorIO::Read(fd, set2));
    close(fd);
    UnitAssert (set2 == set3);
    UnitAssert(set2.size() == 3);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOSet.store' for reading!" << endl;
    exit(1);
  }
    
  std::remove("/tmp/DwmTestIOSet.store");

  FILE  *f = fopen("/tmp/DwmTestIOSet.store","w");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Write(f, set1));
    fclose(f);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOSet.store' for writing!" << endl;
    exit(1);
  }
  
  set3.clear();
  f = fopen("/tmp/DwmTestIOSet.store","r");
  if (UnitAssert(f)) {
    UnitAssert(FileIO::Read(f, set3));
    fclose(f);
    UnitAssert (set2 == set3);
    UnitAssert(set3.size() == 3);
  }
  else {
    cerr << "Failed to open '/tmp/DwmTestIOSet.store' for reading!" << endl;
    exit(1);
  }

  std::remove("/tmp/DwmTestIOSet.store");
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
