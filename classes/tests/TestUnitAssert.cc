//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestUnitAssert.cc 8389 $
// @(#) $Id: TestUnitAssert.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestUnitAssert.cc
//!  \brief Unit tests for UnitAssert
//---------------------------------------------------------------------------

#include <string>

#include "DwmOptArgs.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestUnitAssert.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static int Test1()
{
  int  i;
  for (i = 0; i < 8; ++i) {
    UnitAssert(false == false);
    UnitAssert(false == true);
  }
  return(i);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static int Test2()
{
  int  i;
  for (i = 0; i < 5; ++i) {
    UnitAssert(0 == 0);
    UnitAssert(0 == 1);
  }
  return(i);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  unsigned int  totalTests;

  OptArgs  optargs;
  optargs.AddOptArg("x", "xml", false, "false", "produce XML output");
  
  optargs.Parse(argc, argv);
  
  bool  xml = optargs.Get<bool>('x');
  
  totalTests = Test1();
  totalTests += Test2();
  
  if ((Assertions::Total().Passed() != totalTests) ||
      (Assertions::Total().Failed() != totalTests)) {
    if (xml) {
      Assertions::PrintXML(cout);
    }
    else {
      Assertions::Print(cerr);
    }
    return(1);
  }

  return(0);
}
