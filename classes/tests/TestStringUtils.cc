//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestStringUtils.cc 8389 $
// @(#) $Id: TestStringUtils.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestStringUtils.cc
//!  \brief Unit tests for Dwm::StringUtils
//---------------------------------------------------------------------------

extern "C" {
#include <inttypes.h>
}

#include <cassert>
#include <iomanip>
#include <iostream>

#include "DwmStringUtils.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestStringUtils.cc 8389 $");

using namespace std;
using namespace Dwm;
using namespace Dwm::StringUtils;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
bool TestStringToUnsignedIntType()
{
  bool    rc = true;
  
  try {
    T  maxValue = numeric_limits<T>::max();
    T  minValue = numeric_limits<T>::min();
    T  zeroValue = 0;
    
    ostringstream  os;
    
    if (typeid(T) == typeid(char))
      os << (short)maxValue;
    else if (typeid(T) == typeid(unsigned char))
      os << (unsigned short)maxValue;
    else
      os << maxValue;
    UnitAssert(StringTo<T>(os.str()) == maxValue);

    os.str("");
    if (typeid(T) == typeid(char))
      os << (short)minValue;
    else if (typeid(T) == typeid(unsigned char))
      os << (unsigned short)minValue;
    else
      os << minValue;
    UnitAssert(StringTo<T>(os.str()) == minValue);

    os << showbase;
    
    os.str("");
    if (typeid(T) == typeid(char))
      os << hex << (short)maxValue;
    else if (typeid(T) == typeid(unsigned char))
      os << hex << (unsigned short)maxValue;
    else
      os << hex << maxValue;
    UnitAssert(StringTo<T>(os.str()) == maxValue);

    os.str("");
    if (typeid(T) == typeid(char))
      os << hex << (short)minValue;
    else if (typeid(T) == typeid(unsigned char))
      os << hex << (unsigned short)minValue;
    else
      os << hex << minValue;
    UnitAssert(StringTo<T>(os.str()) == minValue);

    os.str("");
    if (typeid(T) == typeid(char))
      os << oct << (short)maxValue;
    else if (typeid(T) == typeid(unsigned char))
      os << oct << (unsigned short)maxValue;
    else
      os << oct << maxValue;
    UnitAssert(StringTo<T>(os.str()) == maxValue);

    os.str("");
    if (typeid(T) == typeid(char))
      os << oct << (short)minValue;
    else if (typeid(T) == typeid(unsigned char))
      os << oct << (unsigned short)minValue;
    else
      os << oct << minValue;
    UnitAssert(StringTo<T>(os.str()) == minValue);
    
    T  t = StringTo<T>("0");
    UnitAssert(t == zeroValue);
  }
  catch (BadStringConversion & bsc) {
    cerr << "BadStringConversion exception: " << bsc.what() << endl;
    rc = false;
  }
  
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
bool TestUnsignedIntTypeToString()
{
  bool    rc = true;
  
  try {
    T  maxValue = numeric_limits<T>::max();
    T  minValue = numeric_limits<T>::min();
    ostringstream  os;
    if (typeid(T) == typeid(char))
      os << (short)maxValue;
    else if (typeid(T) == typeid(unsigned char))
      os << (unsigned short)maxValue;
    else
      os << maxValue;
    UnitAssert(ToString<T>(maxValue) == os.str());

    os.str("");
    if (typeid(T) == typeid(char))
      os << (short)minValue;
    else if (typeid(T) == typeid(unsigned char))
      os << (unsigned short)minValue;
    else
      os << minValue;
    UnitAssert(ToString<T>(minValue) == os.str());

    os.str("");
    if (typeid(T) == typeid(char))
      os << hex << (short)maxValue;
    else if (typeid(T) == typeid(unsigned char))
      os << hex << (unsigned short)maxValue;
    else
      os << hex << maxValue;
    UnitAssert(ToString<T>(maxValue) == os.str());

    os.str("");
    os << hex << minValue;
    UnitAssert(ToString<T>(minValue) == os.str());
    os.str("");
    os << oct << maxValue;
    UnitAssert(ToString<T>(maxValue) == os.str());
    os.str("");
    os << oct << minValue;
    UnitAssert(ToString<T>(minValue) == os.str());
  }
  catch (BadStringConversion & bsc) {
    cerr << "BadStringConversion exception: " << bsc.what() << endl;
    rc = false;
  }
  
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
bool TestStringToIntType()
{
  bool    rc = true;
  
  try {
    T  maxValue = numeric_limits<T>::max();
    T  minValue = numeric_limits<T>::min();
    ostringstream  os;
    
    if (typeid(T) == typeid(char) || typeid(T) == typeid(signed char))
      os << (short)maxValue;
    else if (typeid(T) == typeid(unsigned char))
      os << (unsigned short)maxValue;
    else
      os << maxValue;
    UnitAssert(StringTo<T>(os.str()) == maxValue);

    os.str("");
    if (typeid(T) == typeid(char) || typeid(T) == typeid(signed char))
      os << (short)minValue;
    else if (typeid(T) == typeid(unsigned char))
      os << (unsigned short)minValue;
    else
      os << minValue;
    UnitAssert(StringTo<T>(os.str()) == minValue);

    UnitAssert(StringTo<T>("0") == (T)0);
  }
  catch (BadStringConversion & bsc) {
    cerr << "BadStringConversion exception: " << bsc.what() << endl;
    rc = false;
  }
  
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
bool TestIntTypeToString()
{
  bool    rc = true;
  
  try {
    T  maxValue = numeric_limits<T>::max();
    T  minValue = numeric_limits<T>::min();
    ostringstream  os;
    
    os << maxValue;
    UnitAssert(ToString<T>(maxValue) == os.str());
    os.str("");
    os << minValue;
    UnitAssert(ToString<T>(minValue) == os.str());
  }
  catch (BadStringConversion & bsc) {
    cerr << "BadStringConversion exception: " << bsc.what() << endl;
    rc = false;
  }
  
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
bool TestStringToFloatType()
{
  bool  rc = true;
  try {
    T  maxValue = numeric_limits<T>::max();
    T  minValue = numeric_limits<T>::min();
    ostringstream  os;
    os << setprecision(500);

    os << maxValue;
    UnitAssert(StringTo<T>(os.str()) == maxValue);
    os.str("");
    os << minValue;
    UnitAssert(StringTo<T>(os.str()) == minValue);
    UnitAssert(StringTo<T>("0.0") == 0.0);
  }
  catch (BadStringConversion & bsc) {
    cerr << "BadStringConversion exception: " << bsc.what() << endl;
    rc = false;
  }
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestStringToBool()
{
  bool  rc = true;
  try {
    UnitAssert(StringTo<bool>("1") == true);
    UnitAssert(StringTo<bool>("0") == false);
    UnitAssert(StringTo<bool>("true") == true);
    UnitAssert(StringTo<bool>("false") == false);
  }
  catch (BadStringConversion & bsc) {
    cerr << "BadStringConversion exception: " << bsc.what() << endl;
    rc = false;
  }
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestIntTypes()
{
  bool  rc = true;

  //  test conversion from string to int type
  rc &= TestStringToIntType<long long>();
  rc &= TestStringToIntType<long>();
  rc &= TestStringToIntType<int>();
  rc &= TestStringToIntType<short>();
  rc &= TestStringToIntType<char>();
  rc &= TestStringToIntType<int64_t>();
  rc &= TestStringToIntType<int32_t>();
  rc &= TestStringToIntType<int16_t>();
  rc &= TestStringToIntType<int8_t>();

  //  test conversion from int type to string
  rc &= TestIntTypeToString<char>();
  rc &= TestIntTypeToString<short>();
  rc &= TestIntTypeToString<int>();
  rc &= TestIntTypeToString<long>();
  rc &= TestIntTypeToString<int8_t>();
  rc &= TestIntTypeToString<int16_t>();
  rc &= TestIntTypeToString<int32_t>();
  rc &= TestIntTypeToString<int64_t>();

  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestUnsignedIntTypes()
{
  bool  rc = true;

  //  Test conversions from string to integer type
  rc &= TestStringToUnsignedIntType<unsigned char>();
  rc &= TestStringToUnsignedIntType<unsigned short>();
  rc &= TestStringToUnsignedIntType<unsigned int>();
  rc &= TestStringToUnsignedIntType<unsigned long>();
  rc &= TestStringToUnsignedIntType<uint8_t>();
  rc &= TestStringToUnsignedIntType<uint16_t>();
  rc &= TestStringToUnsignedIntType<uint32_t>();
  rc &= TestStringToUnsignedIntType<uint64_t>();

  //  Test conversions from integer type to string
  rc &= TestIntTypeToString<unsigned char>();
  rc &= TestIntTypeToString<unsigned short>();
  rc &= TestIntTypeToString<unsigned int>();
  rc &= TestIntTypeToString<unsigned long>();
  rc &= TestIntTypeToString<uint8_t>();
  rc &= TestIntTypeToString<uint16_t>();
  rc &= TestIntTypeToString<uint32_t>();
  rc &= TestIntTypeToString<uint64_t>();

  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIntTypes();
  TestUnsignedIntTypes();
  TestStringToFloatType<float>();
  TestStringToFloatType<double>();
  TestStringToBool();
  
  UnitAssert(StringTo<string>("string foo") == "string foo");
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
