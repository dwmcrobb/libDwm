//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestStringConverter.cc 8389 $
// @(#) $Id: TestStringConverter.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestStringConverter.cc
//!  \brief Unit tests for Dwm::StringConverter
//---------------------------------------------------------------------------

#include <cassert>
#include <sstream>
#include <string>

#include "DwmStringConverter.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestStringConverter.cc 8389 $");

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestBool(const Dwm::StringConverter & sc)
{
  bool  rc = true;

  bool  val;
  rc &= sc("false", val);
  UnitAssert(val == false);
  rc &= sc("true", val);
  UnitAssert(val == true);
  rc &= sc("0", val);
  UnitAssert(val == false);
  rc &= sc("1", val);
  UnitAssert(val == true);

  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static bool TestSignedIntegral(const Dwm::StringConverter & sc)
{
  bool  rc = true;

  assert(numeric_limits<T>::is_signed);
  
  T  maxValue = numeric_limits<T>::max();
  T  minValue = numeric_limits<T>::min();
  ostringstream  osMax, osMin;
  osMax << (long long)maxValue;
  osMin << (long long)minValue;

  T  target;
  UnitAssert(sc(osMax.str(), target));
  UnitAssert(target == maxValue);
  UnitAssert(sc(osMin.str(), target));
  UnitAssert(target == minValue);

  return(rc);
}

  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static bool TestUnsignedIntegral(const Dwm::StringConverter & sc)
{
  bool  rc = true;

  assert(! numeric_limits<T>::is_signed);
  
  T  maxValue = numeric_limits<T>::max();
  T  minValue = numeric_limits<T>::min();
  
  ios_base::fmtflags flags[3] = {
    ios_base::dec, ios_base::hex, ios_base::oct
  };

  for (int i = 0; i < 3; ++i) {
    ostringstream  osMax, osMin;
    osMax.setf(flags[i], ios_base::basefield);
    osMin.setf(flags[i], ios_base::basefield);
    osMax << showbase << (unsigned long long)maxValue;
    osMin << showbase << (unsigned long long)minValue;
    T  target;
    UnitAssert(sc(osMax.str(), target));
    UnitAssert(target == maxValue);
    UnitAssert(sc(osMin.str(), target));
    UnitAssert(target == minValue);
  }
  
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static bool TestIntegral(const Dwm::StringConverter & sc)
{
  if (numeric_limits<T>::is_signed)
    return(TestSignedIntegral<T>(sc));
  return(TestUnsignedIntegral<T>(sc));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::StringConverter  sc;

  TestBool(sc);
  TestIntegral<long long>(sc);
  TestIntegral<unsigned long long>(sc);
  TestIntegral<long>(sc);
  TestIntegral<unsigned long>(sc);
  TestIntegral<int>(sc);
  TestIntegral<unsigned int>(sc);
  TestIntegral<short>(sc);
  TestIntegral<unsigned short>(sc);
  TestIntegral<char>(sc);
  TestIntegral<signed char>(sc);
  TestIntegral<unsigned char>(sc);

  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  return(0);
}
