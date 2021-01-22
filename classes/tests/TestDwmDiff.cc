//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDiff.cc 10284 $
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
//!  \file TestDwmDiff.cc
//!  \brief Unit tests for Dwm::Diff function templates
//---------------------------------------------------------------------------

#include <deque>
#include <set>
#include <vector>

#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"
#include "DwmDiff.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDiff.cc 10284 $");

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestMapDiff()
{
  map<int,int>  m1 = { {0,0}, {1,1}, {2,2}, {3,3} };
  map<int,int>  m2 = { {1,1}, {2,2}, {4,4} };
  map<int,int>  m1d, m2d;
  map<int,int>  exp1d = { {0,0}, {3,3} };
  map<int,int>  exp2d = { {4,4} };
  
  Dwm::Diff(m1, m2, m1d, m2d);

  UnitAssert(m1d == exp1d);
  UnitAssert(m2d == exp2d);

  m1d.clear();
  m2d.clear();

  Dwm::Diff(m1, m2, m1d, m2d,
            [] (const auto & m1e, const auto & m2e)
            { return (m1e.first < m2e.first); });
  UnitAssert(m1d == exp1d);
  UnitAssert(m2d == exp2d);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestVecDiff()
{
  vector<int>  v1 = { 0, 1, 2, 3 };
  vector<int>  v2 = { 1, 2, 4 };
  vector<int>  v1d, v2d;
  vector<int>  exp1d = { 0, 3 };
  vector<int>  exp2d = { 4 };
  
  Dwm::Diff(v1, v2, v1d, v2d);

  UnitAssert(v1d == exp1d);
  UnitAssert(v2d == exp2d);

  v1d.clear();
  v2d.clear();

  Dwm::Diff(v1, v2, v1d, v2d,
            [] (const auto & v1e, const auto & v2e)
            { return (v1e < v2e); });
  UnitAssert(v1d == exp1d);
  UnitAssert(v2d == exp2d);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDequeDiff()
{
  deque<int>  v1 = { 0, 1, 2, 3 };
  deque<int>  v2 = { 1, 2, 4 };
  deque<int>  v1d, v2d;
  deque<int>  exp1d = { 0, 3 };
  deque<int>  exp2d = { 4 };
  
  Dwm::Diff(v1, v2, v1d, v2d);

  UnitAssert(v1d == exp1d);
  UnitAssert(v2d == exp2d);

  v1d.clear();
  v2d.clear();

  Dwm::Diff(v1, v2, v1d, v2d,
            [] (const auto & v1e, const auto & v2e)
            { return (v1e < v2e); });
  UnitAssert(v1d == exp1d);
  UnitAssert(v2d == exp2d);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSetDiff()
{
  set<int>  s1 = { 0, 1, 2, 3 };
  set<int>  s2 = { 1, 2, 4 };
  set<int>  s1d, s2d;
  set<int>  exp1d = { 0, 3 };
  set<int>  exp2d = { 4 };
  
  Dwm::Diff(s1, s2, s1d, s2d);

  UnitAssert(s1d == exp1d);
  UnitAssert(s2d == exp2d);

  s1d.clear();
  s2d.clear();

  Dwm::Diff(s1, s2, s1d, s2d,
            [] (const auto & s1e, const auto & s2e)
            { return (s1e < s2e); });
  UnitAssert(s1d == exp1d);
  UnitAssert(s2d == exp2d);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStringDiff()
{
  string  s1("0123");
  string  s2("124");
  string  s1d, s2d;
  string  exp1d = "03";
  string  exp2d = "4";
  
  Dwm::Diff(s1, s2, s1d, s2d);

  UnitAssert(s1d == exp1d);
  UnitAssert(s2d == exp2d);

  s1d.clear();
  s2d.clear();

  Dwm::Diff(s1, s2, s1d, s2d,
            [] (const auto & s1e, const auto & s2e)
            { return (s1e < s2e); });
  UnitAssert(s1d == exp1d);
  UnitAssert(s2d == exp2d);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestMapDiff();
  TestVecDiff();
  TestDequeDiff();
  TestSetDiff();
  TestStringDiff();
  
  if (Dwm::Assertions::Total().Failed()) {
    Dwm::Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Dwm::Assertions::Total() << " passed" << endl;
  }
  exit(0);
}
