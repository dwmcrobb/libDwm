//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestVariantIO.cc 11087 $
// @(#) $Id: TestVariantIO.cc 11087 2020-09-07 09:58:37Z dwm $
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
//!  \file TestVariantIO.cc
//!  \author Daniel W. McRobb
//!  \brief std::variant I/O unit tests
//---------------------------------------------------------------------------

#include <sstream>

#include "DwmStreamIO.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestVariantIO.cc 11087 $");

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  stringstream  ss;
  variant<monostate,int,string>  v1;
  UnitAssert(StreamIO::Write(ss, v1));
  v1 = "hello there";
  UnitAssert(StreamIO::Write(ss, v1));
  v1 = 42;
  UnitAssert(StreamIO::Write(ss, v1));

  variant<monostate,int,string>  v2;
  UnitAssert(StreamIO::Read(ss, v2));
  UnitAssert(0 == v2.index());

  UnitAssert(StreamIO::Read(ss, v2));
  UnitAssert(2 == v2.index());
  UnitAssert(string("hello there") == get<2>(v2));

  UnitAssert(StreamIO::Read(ss, v2));
  UnitAssert(1 == v2.index());
  UnitAssert(get<1>(v2) == 42);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestStreamIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    exit(0);
  }
}
