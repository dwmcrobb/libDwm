//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestValueRange.cc 8389 $
// @(#) $Id: TestValueRange.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestValueRange.cc
//!  \brief Unit tests for Dwm::ValueRange
//---------------------------------------------------------------------------

#include <cstdlib>
#include <string>

#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"
#include "DwmValueRange.hh"

using namespace std;
using Dwm::ValueRange;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestValueRange.cc 8389 $");

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ValueRange<string>  stringRange("b", "d");
  UnitAssert(stringRange.Contains("c"));
  
  ValueRange<string>  stringRange2("a", "e");
  UnitAssert(stringRange2.Contains(stringRange));
  UnitAssert(stringRange2.Overlaps(stringRange));

  ValueRange<string>  stringRange3("f", "g");
  UnitAssert(! stringRange2.Overlaps(stringRange3));

  ValueRange<string>  stringRange4("dwm", "rms");
  UnitAssert(stringRange4.Overlaps(stringRange2));
  UnitAssert(stringRange4.Contains(stringRange3));
  
  ValueRange<int>  intRange(5, 15);
  ValueRange<int>  intRange2(0, 20);
  UnitAssert(intRange2.Contains(intRange));
  UnitAssert(! intRange.Contains(intRange2));
  UnitAssert(intRange2.Overlaps(intRange));
  UnitAssert(intRange.Overlaps(intRange2));
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
