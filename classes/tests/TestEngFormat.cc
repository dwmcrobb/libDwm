//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestEngFormat.cc 8389 $
// @(#) $Id: TestEngFormat.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007
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
//!  \file TestEngFormat.cc
//!  \brief Unit tests for Dwm::EngFormat
//---------------------------------------------------------------------------

#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmEngFormat.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestEngFormat.cc 8389 $");

using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
struct ValueFormatted
{
  T            value;
  std::string  formatted;
};

static ValueFormatted<uint64_t>  uint64_values[] = {
  { 123456ULL,              "123.5K" },
  { 1234560ULL,             "1.235M" },
  { 12345600ULL,            "12.35M" },
  { 123456000ULL,           "123.5M" },
  { 1234560000ULL,          "1.235G" },
  { 12345600000ULL,         "12.35G" },
  { 123456000000ULL,        "123.5G" },
  { 1234560000000ULL,       "1.235T" },
  { 12345600000000ULL,      "12.35T" },
  { 123456000000000ULL,     "123.5T" },
  { 1234560000000000ULL,    "1.235P" },
  { 12345600000000000ULL,   "12.35P" },
  { 123456000000000000ULL,  "123.5P" },
  { 1234560000000000000ULL, "1.235E" },
  { 0, "" }
};

static ValueFormatted<uint32_t>  uint32_values[] = {
  { 123456UL,     "123.5K" },
  { 1234560UL,    "1.235M" },
  { 12345600UL,   "12.35M" },
  { 123456000UL,  "123.5M" },
  { 1234560000UL, "1.235G" },
  { 0, "" }
};

static ValueFormatted<uint16_t>  uint16_values[] = {
  { 123,   "  123 " },
  { 1230,  " 1.23K" },
  { 12300, " 12.3K" },
  { 0, "" }
};

static ValueFormatted<int64_t>  int64_values[] = {
  { -123456LL,              "-123.5K" },
  { -1234560LL,             "-1.235M" },
  { -12345600LL,            "-12.35M" },
  { -123456000LL,           "-123.5M" },
  { -1234560000LL,          "-1.235G" },
  { -12345600000LL,         "-12.35G" },
  { -123456000000LL,        "-123.5G" },
  { -1234560000000LL,       "-1.235T" },
  { -12345600000000LL,      "-12.35T" },
  { -123456000000000LL,     "-123.5T" },
  { -1234560000000000LL,    "-1.235P" },
  { -12345600000000000LL,   "-12.35P" },
  { -123456000000000000LL,  "-123.5P" },
  { -1234560000000000000LL, "-1.235E" },
  { 0, "" }
};

static ValueFormatted<int32_t>  int32_values[] = {
  { -123456,     "-123.5K" },
  { -1234560,    "-1.235M" },
  { -12345600,   "-12.35M" },
  { -123456000,  "-123.5M" },
  { -1234560000, "-1.235G" },
  { 0, "" }
};

static ValueFormatted<int16_t>  int16_values[] = {
  { -123,   " -123 " },
  { -1230,  "-1.23K" },
  { -12300, "-12.3K" },
  { 0, "" }
};

static ValueFormatted<float>  float_values[] = {
  { -123.456,       "-123.5 " },
  { -1234.56,       "-1.235K" },
  { -12345.6,       "-12.35K" },
  { -123456.0,      "-123.5K" },
  { -1234560.0,     "-1.235M" },
  { -12345600.0,    "-12.35M" },
  { -123456000.0,   "-123.5M" },    
  { -1234560000.0,  "-1.235G" },
  { -12345600000.0, "-12.35G" },
  { 0, "" }
};

static ValueFormatted<double>  double_values[] = {
  { -0.123456,     "-123.5m" },
  { -1.23456,      "-1.235 " },
  { -12.3456,      "-12.35 " },
  { -123.456,      "-123.5 " },
  { -1234.56,      "-1.235K" },
  { -12345.6,      "-12.35K" },
  { -123456,       "-123.5K" },
  { -1234560,      "-1.235M" },
  { -12345600,     "-12.35M" },
  { 0.123456,      "123.5m" },
  { 1.23456,       "1.235 " },
  { 12.3456,       "12.35 " },
  { 123.456,       "123.5 " },
  { 1234.56,       "1.235K"},
  { 12345.6,       "12.35K"},
  { 123456,        "123.5K"},
  { 1.23456e+06,   "1.235M"},
  { 1.23456e+07,   "12.35M"},
  { 1.23456e+08,   "123.5M"},
  { 1.23456e+09,   "1.235G"},
  { 1.23456e+10,   "12.35G"},
  { 1.23456e+11,   "123.5G"},
  { 1.23456e+12,   "1.235T"},
  { 1.23456e+13,   "12.35T"},
  { 1.23456e+14,   "123.5T"},
  { 1.23456e+15,   "1.235P"},
  { 1.23456e+16,   "12.35P"},
  { 1.23456e+17,   "123.5P"},
  { 1.23456e+18,   "1.235E"},
  { 1.23456e+19,   "12.35E"},
  { 1.23456e+20,   "123.5E"},
  { 1.23456e+21,   "1.235Z"},
  { 1.23456e+22,   "12.35Z"},
  { 1.23456e+23,   "123.5Z"},
  { 1.23456e+24,   "1.235Y"},
  { 1.23456e+25,   "12.35Y"},
  { 1.23456e+26,   "123.5Y"},
  { 1.23456e+27,   " 1235Y"},
  { 1.23456e+28,   "1.235e+04Y"},
  { 1.23456e+29,   "1.235e+05Y"},
  { 1.23456e+30,   "1.235e+06Y"},
  { 1.23456e+31,   "1.235e+07Y"},
  { 1.23456e+32,   "1.235e+08Y"},
  { 0, "" }
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
void TestEngFormat(const ValueFormatted<T> *values)
{
  for (uint8_t i = 0; ! values[i].formatted.empty(); ++i) {
    std::ostringstream  os;
    os << EngFormat(values[i].value);
    UnitAssert(os.str() == values[i].formatted);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestEngFormat(uint64_values);
  TestEngFormat(uint32_values);
  TestEngFormat(uint16_values);
  TestEngFormat(int64_values);
  TestEngFormat(int32_values);
  TestEngFormat(int16_values);
  TestEngFormat(float_values);
  TestEngFormat(double_values);
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  return(0);
}

  
