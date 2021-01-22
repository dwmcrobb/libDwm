//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestLLCAT.cc 8389 $
// @(#) $Id: TestLLCAT.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestLLCAT.cc
//!  \brief Unit tests for Dwm::LLCAT
//---------------------------------------------------------------------------

#include <cassert>
#include <cstdlib>

#include "DwmLLCAT.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestLLCAT.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  LLCATSegment<uint32_t>  segment(4);

  segment.StopBit(true);
  UnitAssert(segment.StopBit());
  segment.StopBit(false);
  UnitAssert(! segment.StopBit());

  segment.ValidBit(true);
  UnitAssert(segment.ValidBit());
  segment.ValidBit(false);
  UnitAssert(! segment.ValidBit());

  segment.FullPrefixRoute(true);
  UnitAssert(segment.FullPrefixRoute());
  segment.FullPrefixRoute(false);
  UnitAssert(! segment.FullPrefixRoute());

  for (uint8_t i = 0; i < 8; ++i) {
    segment.LifeChildrenCounter(i);
    UnitAssert(segment.LifeChildrenCounter() == i);
  }
  segment.LifeChildrenCounter(0);
  UnitAssert(segment.LifeChildrenCounter() == 0);
  
  for (uint8_t i = 0; i < 8; ++i) {
    segment.ValidSubPrefixPattern(i);
    UnitAssert(segment.ValidSubPrefixPattern() == i);
  }
  segment.ValidSubPrefixPattern(0);
  UnitAssert(segment.ValidSubPrefixPattern() == 0);
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);  
}
