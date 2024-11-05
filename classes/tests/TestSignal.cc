//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmSignal.cc 8389 $
// @(#) $Id: TestDwmSignal.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestDwmSignal.cc
//!  \brief Unit tests for Dwm::Signal
//---------------------------------------------------------------------------

extern "C" {
  #include <inttypes.h>
  #include <unistd.h>
}

#include <iostream>
#include <string>

#include "DwmSignal.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmSignal.cc 8389 $");

static uint32_t  g_handler1Calls = 0;
static uint32_t  g_handler2Calls = 0;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Handler1(int sigNum)
{
  ++g_handler1Calls;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Handler2(int sigNum)
{
  ++g_handler2Calls;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Signal  sigHup(SIGHUP);
  sigHup.PushHandler(Handler1);
  sigHup.PushHandler(Handler2);
  
  kill(getpid(), SIGHUP);
  sleep(1);
  kill(getpid(), SIGHUP);
  sleep(1);
  
  sigHup.PopHandler();
  kill(getpid(), SIGHUP);
  sleep(1);
  kill(getpid(), SIGHUP);
  
  UnitAssert(g_handler1Calls == 2);
  UnitAssert(g_handler2Calls == 2);

  UnitAssert(sigHup.Block());
  UnitAssert(sigHup.IsBlocked());
  kill(getpid(), SIGHUP);
  UnitAssert(sigHup.IsPending());
  UnitAssert(sigHup.Unblock());
  UnitAssert(g_handler1Calls == 3);
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
