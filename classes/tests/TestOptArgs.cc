//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmOptArgs.cc 8389 $
// @(#) $Id: TestDwmOptArgs.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestDwmOptArgs.cc
//!  \brief Unit tests for Dwm::OptArgs
//---------------------------------------------------------------------------

#include <string>

#include "DwmOptArgs.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmOptArgs.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Test1(char *argv0)
{
  OptArgs  optargs;
  
  optargs.AddOptArg("b", "bool");
  optargs.AddOptArg("i:", "int", true);
  optargs.AddOptArg("s:", "string", true);

  string  argSet1[] = { argv0, "-b", "-i", "20", "-s", "dwm" };

  int nextArg = optargs.Parse(6, argSet1);

  UnitAssert(nextArg == 6);
  
  bool b = optargs.Get<bool>('b');
  int  i = optargs.Get<int>('i');
  string  s = optargs.Get<string>('s');
  
  UnitAssert(b);
  UnitAssert(i == 20);
  UnitAssert(s == "dwm");

  b = optargs.Get<bool>("bool");
  i = optargs.Get<int>("int");
  s = optargs.Get<string>("string");

  UnitAssert(b);
  UnitAssert(i == 20);
  UnitAssert(s == "dwm");
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestLong1(char *argv0)
{
  OptArgs  optargs;
  
  optargs.AddOptArg("b", "bool");
  optargs.AddOptArg("i:", "int", true);
  optargs.AddOptArg("s:", "string", true);

  string argSet1[] = {
    argv0, "--bool", "--int", "20", "--string", "dwm", "dwm" 
  };
  
  int nextArg = optargs.Parse(6, argSet1);

  UnitAssert(nextArg == 6);

  bool b = optargs.Get<bool>("bool");
  int  i = optargs.Get<int>("int");
  string  s = optargs.Get<string>("string");
  
  UnitAssert(b);
  UnitAssert(i == 20);
  UnitAssert(s == "dwm");

  b = optargs.Get<bool>('b');
  i = optargs.Get<int>('i');
  s = optargs.Get<string>('s');
  
  UnitAssert(b);
  UnitAssert(i == 20);
  UnitAssert(s == "dwm");
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Test2(char *argv0)
{
  OptArgs  optargs;
  
  optargs.AddOptArg("b", "bool");
  optargs.AddOptArg("i:", "int", true);
  optargs.AddOptArg("s:", "string");
  optargs.AddNormalArg("file", true);
  
  string  argSet1[] = { argv0, "-i", "20", "./TestDefaults" };

  int nextArg = optargs.Parse(4, argSet1);

  UnitAssert(nextArg == 3);

  bool b = optargs.Get<bool>('b');
  int i = optargs.Get<int>('i');
  string s = optargs.Get<string>('s');
  
  UnitAssert(! b);
  UnitAssert(i == 20);
  UnitAssert(s.empty());

  b = optargs.Get<bool>("bool");
  i = optargs.Get<int>("int");
  s = optargs.Get<string>("string");
  
  UnitAssert(! b);
  UnitAssert(i == 20);
  UnitAssert(s.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDefaults(char *argv0)
{
  OptArgs  optargs;
  
  optargs.AddOptArg("b",  "bool", false, "true", 
                    "a boolean value, true or false");
  optargs.AddOptArg("i:", "int", false, "20",
                    "an integer value, default is 20");
  optargs.AddOptArg("s:", "string", false, "dwm",
                    "a string value, default is dwm");

  string argSet1[] = { argv0 };
  
  int nextArg = optargs.Parse(1, argSet1);

  UnitAssert(nextArg == 1);

  bool b = optargs.Get<bool>('b');
  int i = optargs.Get<int>('i');
  string s = optargs.Get<string>('s');
  
  UnitAssert(b);
  UnitAssert(i == 20);
  UnitAssert(s == "dwm");

  b = optargs.Get<bool>("bool");
  i = optargs.Get<int>("int");
  s = optargs.Get<string>("string");
  
  UnitAssert(b);
  UnitAssert(i == 20);
  UnitAssert(s == "dwm");
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Test1(argv[0]);
  Test2(argv[0]);
  TestLong1(argv[0]);
  TestDefaults(argv[0]);
  
  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;
  
  exit(0);
}

