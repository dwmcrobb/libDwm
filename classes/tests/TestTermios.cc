//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmTermios.cc 8389 $
// @(#) $Id: TestDwmTermios.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestDwmTermios.cc
//!  \brief Unit tests for Dwm::Termios
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <cstdlib>
#include <iostream>
#include <string>

#include "DwmSvnTag.hh"
#include "DwmTermios.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmTermios.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestFlags(Dwm::TermioFlags & flags)
{
  bool  rc = true;

  const Dwm::TermioFlagInfo_t *knownFlags = flags.KnownFlags();
                      
  for (int i = 0; knownFlags[i].name; ++i) {
    flags.Set(knownFlags[i].name);
    if (! UnitAssert(flags.IsSet(knownFlags[i].name))) {
      rc = false;
    }
    flags.Unset(knownFlags[i].name);
    if (! UnitAssert(flags.IsSet(knownFlags[i].name) == false)) {
      rc = false;
    }

    flags.Set(knownFlags[i].longName);
    if (! UnitAssert(flags.IsSet(knownFlags[i].longName))) {
      rc = false;
    }
    flags.Unset(knownFlags[i].longName);
    if (! UnitAssert(flags.IsSet(knownFlags[i].longName) == false)) {
      rc = false;
    }
  }
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestAllFlags()
{
  bool  rc = true;
  Dwm::Termios  t;
  if (! UnitAssert(TestFlags(t.InputFlags()))) {
    rc = false;
  }
  if (! UnitAssert(TestFlags(t.OutputFlags()))) {
    rc = false;
  }
  if (! UnitAssert(TestFlags(t.ControlFlags()))) {
    rc = false;
  }
  if (! UnitAssert(TestFlags(t.LocalFlags()))) {
    rc = false;
  }
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestSpeeds()
{
  bool  rc = true;
  Dwm::Termios  t;

  t.InputSpeed(B9600);
  if (! UnitAssert(t.InputSpeed() == B9600)) {
    rc = false;
  }

  t.OutputSpeed(B9600);
  if (! UnitAssert(t.OutputSpeed() == B9600)) {
    rc = false;
  }
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestControlChars()
{
  bool  rc = true;
  Dwm::Termios  t;
  
  t.ControlChar("VEOF", 4);
  if (! UnitAssert(t.ControlChar("VEOF") == 4))
    rc = false;
  t.ControlChar("VEOL", 13);
  if (! UnitAssert(t.ControlChar("VEOL") == 13))
    rc = false;
  t.ControlChar("VERASE", '');
  if (! UnitAssert(t.ControlChar("VERASE") == ''))
    rc = false;
  t.ControlChar("VKILL", '');
  if (! UnitAssert(t.ControlChar("VKILL") == ''))
    rc = false;
  t.ControlChar("VINTR", '');
  if (! UnitAssert(t.ControlChar("VINTR") == ''))
    rc = false;
  t.ControlChar("VQUIT", '');
  if (! UnitAssert(t.ControlChar("VQUIT") == ''))
    rc = false;
  t.ControlChar("VSUSP", '');
  if (! UnitAssert(t.ControlChar("VSUSP") == ''))
    rc = false;
  t.ControlChar("VSTART", '');
  if (! UnitAssert(t.ControlChar("VSTART") == ''))
    rc = false;
  t.ControlChar("VSTOP", '');
  if (! UnitAssert(t.ControlChar("VSTOP") == ''))
    rc = false;
  t.ControlChar("VMIN", 1);
  if (! UnitAssert(t.ControlChar("VMIN") == 1))
    rc = false;
  t.ControlChar("VTIME", 0);
  if (! UnitAssert(t.ControlChar("VTIME") == 0))
    rc = false;

  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestGetSet()
{
  bool  rc = false;
  Dwm::Termios  t;

  if (t.Get(STDOUT_FILENO)) {
    if (UnitAssert(t.Set(STDOUT_FILENO, TCSANOW))) {
      Dwm::Termios  t2;
      if (UnitAssert(t2.Get(STDOUT_FILENO))) {
        rc = UnitAssert(t2 == t);
      }
    }
  }
  return(rc);
}

  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  UnitAssert(TestAllFlags());
  UnitAssert(TestSpeeds());
  UnitAssert(TestControlChars());
  TestGetSet();

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  exit(0);
}
