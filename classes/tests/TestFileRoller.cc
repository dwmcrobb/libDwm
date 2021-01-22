//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestFileRoller.cc 8389 $
// @(#) $Id: TestFileRoller.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2016
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
//!  \file TestFileRoller.cc
//!  \brief Unit tests for Dwm::FileRoller class
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>  // for open(), write(), et. al.
}

#include <cassert>
#include <cstdio>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmFileRoller.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestFileRoller.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool CreateLogFile(const std::string & fileBase)
{
  bool  rc = false;
  int  fd = open(fileBase.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 0644);
  assert(fd >= 0);
  if (fd >= 0) {
    assert(write(fd, "test log file\n", 14) == 14);
    close(fd);
    rc = true;
  }
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBzipFileRoller()
{
  string  fileBase("TestFileRoller.log");
  
  CreateLogFile(fileBase);
  size_t      numFiles = 5;
  FileRoller  fileRoller(".", fileBase, 0, numFiles, 
                         FileRoller::e_compressionBzip2);
  for (size_t i = 0; i < numFiles; ++i) {
    fileRoller.RollFiles();
  }
  for (size_t i = 0; i < numFiles; ++i) {
    ostringstream  filename;
    filename << fileBase << "." << i << ".bz2";
    UnitAssert(access(filename.str().c_str(), F_OK) == 0);
    std::remove(filename.str().c_str());
  }
  std::remove(fileBase.c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGzipFileRoller()
{
  string  fileBase("TestFileRoller.log");
  size_t  numFiles = 5;
  
  CreateLogFile(fileBase);
  FileRoller  fileRoller(".", fileBase, 0, numFiles,
                         FileRoller::e_compressionGzip);
  for (size_t i = 0; i < numFiles; ++i) {
    fileRoller.RollFiles();
  }
  for (size_t i = 0; i < numFiles; ++i) {
    ostringstream  filename;
    filename << fileBase << "." << i << ".gz";
    UnitAssert(access(filename.str().c_str(), F_OK) == 0);
    std::remove(filename.str().c_str());
  }
  std::remove(fileBase.c_str());
  return;
}
    
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestBzipFileRoller();
  TestGzipFileRoller();

  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  
  return(0);
}
