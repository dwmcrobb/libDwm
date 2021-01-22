//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIOList.cc 11087 $
// @(#) $Id: TestIOList.cc 11087 2020-09-07 09:58:37Z dwm $
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
//!  \file TestIOList.cc
//!  \brief Unit tests for I/O with lists
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <stdlib.h>
  #include <unistd.h>
}

#include <cstdio>
#include <fstream>
#include <list>

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIOList.cc 11087 $");

using namespace std;
using namespace Dwm;
using std::list;

#define STOREFILE "/tmp/DwmTestIOList.store"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  list<std::string>  list1;
  
  list1.push_back("1");
  list1.push_back("2");
  list1.push_back("3");
  
  ofstream  os(STOREFILE);
  if (os) {
    if (! StreamIO::Write(os, list1)) {
      cerr << "Failed to write list1!" << endl;
      exit(1);
    }
    os.close();
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for writing!" << endl;
    exit(1);
  }
  
  list<std::string>  list2;
  ifstream  is(STOREFILE);
  if (is) {
    if (! StreamIO::Read(is, list2)) {
      cerr << "Failed to read list2!" << endl;
      exit(1);
    }
    is.close();
    std::remove(STOREFILE);
    
    UnitAssert(list2 == list1);
    UnitAssert(list1.size() == 3);
    
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for reading!" << endl;
    exit(1);
  }

  list<std::string>  list3 = list1;
  int  fd = open(STOREFILE,O_WRONLY|O_CREAT,0666);
  if (fd >= 0) {
    if (! DescriptorIO::Write(fd, list3)) {
      cerr << "Failed to write list3 to descriptor!" << endl;
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
    if (! DescriptorIO::Read(fd, list2)) {
      cerr << "Failed to read list2 from descriptor!" << endl;
      exit(1);
    }
    close(fd);
    UnitAssert (list2 == list3);
    UnitAssert(list2.size() == 3);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for reading!" << endl;
    exit(1);
  }
    
  std::remove(STOREFILE);

  FILE  *f = fopen(STOREFILE,"w");
  if (f) {
    if (! FileIO::Write(f, list1)) {
      cerr << "Failed to write list1 to FILE!" << endl;
      exit(1);
    }
    fclose(f);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for writing!" << endl;
    exit(1);
  }
  
  list3.clear();
  f = fopen(STOREFILE,"r");
  if (f) {
    if (! FileIO::Read(f, list3)) {
      cerr << "Failed to read list3 to FILE!" << endl;
      exit(1);
    }
    fclose(f);
    UnitAssert (list2 == list3);
    UnitAssert(list3.size() == 3);
  }
  else {
    cerr << "Failed to open '" << STOREFILE << "' for reading!" << endl;
    exit(1);
  }
  
  std::remove(STOREFILE);
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }

  return(0);
}
