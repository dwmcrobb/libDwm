//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDataLog.cc 8389 $
// @(#) $Id: TestDwmDataLog.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008, 2016
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
//!  \file TestDwmDataLog.cc
//!  \brief Unit tests for Dwm::DataLog class
//---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

#include "DwmDataLog.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"
#include "DwmSysLogger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDataLog.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestWithStrings()
{
  static const string  entries[] = {
    "one", "two",   "three", "four", "five", 
    "six", "seven", "eight", "nine", "ten",
    ""
  };

  ostringstream  filename;
  filename << "/tmp/TestDwmDataLog.string." << getpid();
  
  DataLog<string>  dataLog(filename.str());
  UnitAssert(dataLog.Open(ios_base::out|ios_base::trunc));
  for (int i = 0; ! entries[i].empty(); ++i) {
    UnitAssert(dataLog.Record(entries[i]));
  }
  UnitAssert(dataLog.Close());
  
  UnitAssert(dataLog.Open(ios_base::in));
  UnitAssert(dataLog.Rewind());
  for (int i = 0; ! entries[i].empty(); ++i) {
    DataLog<string>::Entry  entry;
    UnitAssert(dataLog.GetNext(entry));
    // cout << entry << endl;
    UnitAssert(entry.Value() == entries[i]);
  }
  UnitAssert(dataLog.Close());

  std::remove(filename.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestWithIntegers()
{
  static const int  entries[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  
  ostringstream  filename;
  filename << "/tmp/TestDwmDataLog.int." << getpid();
  
  DataLog<int>  dataLog(filename.str());
  UnitAssert(dataLog.Open(ios_base::out|ios_base::trunc));
  for (int i = 0; i < 10; ++i) {
    UnitAssert(dataLog.Record(entries[i]));
  }
  UnitAssert(dataLog.Close());
  
  UnitAssert(dataLog.Open(ios_base::in));
  UnitAssert(dataLog.Rewind());
  for (int i = 0; i < 10; ++i) {
    DataLog<int>::Entry  entry;
    UnitAssert(dataLog.GetNext(entry));
    // cout << entry << endl;
    UnitAssert(entry.Value() == entries[i]);
  }
  UnitAssert(dataLog.Close());

  std::remove(filename.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // SysLogger::Open("TestDwmDataLog", LOG_PERROR, LOG_USER);
  SysLogger::ShowPriorities(true);
  SysLogger::ShowFileLocation(true);

  TestWithStrings();
  TestWithIntegers();

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }

  exit(0);
}

  
