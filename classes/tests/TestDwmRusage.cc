//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmRusage.cc 9014 $
// @(#) $Id: TestDwmRusage.cc 9014 2017-04-11 09:53:02Z dwm $
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
//!  \file TestDwmRusage.cc
//!  \brief Unit tests for Dwm::Rusage class
//---------------------------------------------------------------------------

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmOptArgs.hh"
#include "DwmPassword.hh"
#include "DwmRusage.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmRusage.cc 9014 $");

using namespace std;
using Dwm::Assertions;
using Dwm::OptArgs;
using Dwm::Password;
using Dwm::Rusage;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRusageIO()
{
  Rusage  rusage;
  rusage.Get(RUSAGE_SELF);
  
  ostringstream  filename;
  filename << "/tmp/TestDwmRusageIO." << getpid();
  ofstream  os(filename.str().c_str());
  UnitAssert(os);
  if (os) {
    UnitAssert(rusage.Write(os));
    os.close();
    Rusage  rusage2;
    ifstream  is(filename.str().c_str());
    UnitAssert(is);
    if (is) {
      UnitAssert(rusage2.Read(is));
      is.close();
      UnitAssert(rusage2 == rusage);
    }
    std::remove(filename.str().c_str());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRusageGZIO()
{
  Rusage  rusage;
  rusage.Get(RUSAGE_SELF);
  
  ostringstream  filename;
  filename << "/tmp/TestDwmRusageGZIO." << getpid();
  gzFile  gzf = gzopen(filename.str().c_str(), "wb");
  UnitAssert(gzf);
  
  if (gzf) {
    UnitAssert(rusage.Write(gzf));
    gzclose(gzf);
    Rusage  rusage2;
    gzf = gzopen(filename.str().c_str(), "rb");
    UnitAssert(gzf);
    if (gzf) {
      UnitAssert(rusage2.Read(gzf));
      gzclose(gzf);
      UnitAssert(rusage2 == rusage);
    }
    std::remove(filename.str().c_str());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool  show = false;

  OptArgs  optargs;
  optargs.AddOptArg("s", "show", false, "false", "show rusage results");
  optargs.Parse(argc, argv);
  show = optargs.Get<bool>('s');
  
  for (int i = 0; i < 100000; ++i) {
    getpid();
  }

  Rusage  rusage;
  
  rusage.Get(RUSAGE_SELF);

  if (show)
    cout << rusage;
  
  for (int i = 0; i < 100000; ++i) {
    getpid();
  }

  for (int i = 0; i < 1000; ++i) {
    Password  passwd(getuid());
  }
  sleep(2);

  Rusage  rusage2;
  rusage2.Get(RUSAGE_SELF);
  if (show)
    cout << rusage2;
  
  UnitAssert((double)rusage2.UserTime() > 0.0);
  UnitAssert(rusage2.UserTime() >= rusage.UserTime());
  UnitAssert((double)rusage2.SystemTime() > 0.0);
  UnitAssert(rusage2.SystemTime() >= rusage.SystemTime());
  UnitAssert(rusage2.PageReclaims() >= rusage.PageReclaims());
#if ((! defined(__APPLE__)) && (! defined(__linux__)))
  UnitAssert(rusage2.MaxResidentSetSize() > 0);
  UnitAssert(rusage2.IntegralSharedTextMemorySize() > 0);
  UnitAssert(rusage2.IntegralUnsharedDataSize() > 0);
#endif
  UnitAssert(rusage2.MaxResidentSetSize() >= rusage.MaxResidentSetSize());

  UnitAssert(rusage2.IntegralSharedTextMemorySize() >= 
             rusage.IntegralSharedTextMemorySize());
  UnitAssert(rusage2.IntegralUnsharedDataSize() >=
             rusage.IntegralUnsharedDataSize());

  TestRusageIO();
  TestRusageGZIO();
  
  if (Assertions::Total().Failed() > 0) {
    Assertions::Print(std::cerr, true);
    return(1);
  }
  else
    std::cout << Assertions::Total() << " passed" << std::endl;
  
  return(0);
}
