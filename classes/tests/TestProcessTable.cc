//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestProcessTable.cc 11087 $
// @(#) $Id: TestProcessTable.cc 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file TestProcessTable.cc
//!  \brief Unit tests for Dwm::ProcessTable
//---------------------------------------------------------------------------

extern "C" {
  #include <stdlib.h>
  #include <unistd.h>
}

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmGZIO.hh"
#include "DwmStreamIO.hh"
#include "DwmProcessTable.hh"
#include "DwmEngFormat.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestProcessTable.cc 11087 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestProcessTableIO(ProcessTable & processTable)
{
  ostringstream  filename;
  filename << "/tmp/TestProcessTableIO." << getpid();
  
  ofstream  os(filename.str().c_str());
  if (os) {
    UnitAssert(StreamIO::Write(os, processTable));
    os.close();
    ifstream  is(filename.str().c_str());
    if (is) {
      ProcessTable  processTable2;
      UnitAssert(StreamIO::Read(is, processTable2));
      UnitAssert(processTable == processTable2);
      is.close();
    }
    unlink(filename.str().c_str());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestProcessTableGZIO(ProcessTable & processTable)
{
  ostringstream  filename;
  filename << "/tmp/TestProcessTableGZIO." << getpid() << ".gz";
  
  gzFile  gzf = gzopen(filename.str().c_str(), "wb");
  UnitAssert(gzf);
  if (gzf) {
    UnitAssert(GZIO::Write(gzf, processTable) > 0);
    gzclose(gzf);
    gzFile  gzf = gzopen(filename.str().c_str(), "rb");
    UnitAssert(gzf);
    if (gzf) {
      ProcessTable  processTable2;
      UnitAssert(GZIO::Read(gzf, processTable2));
      UnitAssert(processTable == processTable2);
      gzclose(gzf);
    }
    unlink(filename.str().c_str());
  }
  return;
}
    
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ProcessTable  processTable;

  UnitAssert(GetProcessTable(processTable));
  
  TestProcessTableIO(processTable);
  TestProcessTableGZIO(processTable);
  
  //  find myself in the process table, and verify against standard
  //  system calls.  
  ProcessTable::const_iterator  i = processTable.find(getpid());
  if (UnitAssert(i != processTable.end())) {
    UnitAssert(i->second.RealUserId() == getuid());
    UnitAssert(i->second.EffectiveUserId() == geteuid());
    UnitAssert(i->second.RealGroupId() == getgid());
    UnitAssert(i->second.Id() == getpid());
    UnitAssert(i->second.ParentId() == getppid());
  }
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
