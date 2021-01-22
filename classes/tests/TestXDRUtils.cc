//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestXDRUtils.cc 10964 $
// @(#) $Id: TestXDRUtils.cc 10964 2020-08-24 07:08:29Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020
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
//!  \file TestXDRUtils.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"
#include "DwmXDRUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestXDRUtils.cc 10964 $");

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  using namespace std;
  using Dwm::XDRUtils;
  
  float  f1 = 23.7;
  std::array<char,4>  flbuf;
  UnitAssert(XDRUtils::Encode(f1, flbuf));
  float  f2;
  UnitAssert(XDRUtils::Decode(flbuf, f2));
  UnitAssert(f2 == f1);

  double  d1 = 197.48;
  std::array<char,8>  dblbuf;
  UnitAssert(XDRUtils::Encode(d1, dblbuf));
  double  d2;
  UnitAssert(XDRUtils::Decode(dblbuf, d2));
  UnitAssert(d2 == d1);

  if (Dwm::Assertions::Total().Failed()) {
    Dwm::Assertions::Print(cerr, true);
  }
  else {
    cout << Dwm::Assertions::Total() << " passed" << endl;
  }
}
