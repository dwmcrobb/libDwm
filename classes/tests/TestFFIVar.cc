//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2012, 2023
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
//!  \file TestFFIVar.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::FFIVar unit tests
//---------------------------------------------------------------------------

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <map>

#include "DwmUnitAssert.hh"
#include "DwmFFIVar.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
requires std::is_unsigned<T>::value
static void TestBitwiseOperators()
{
  //  shifts and bitwise AND
  const Dwm::FFIVar<T>  mx(std::numeric_limits<T>::max());
  UnitAssert(mx.IsFFI());
  Dwm::FFIVar<T>        a = mx;  // 0xFF [0xFF]...
  UnitAssert(a.IsFFI());
  a >>= 7;                       // 0x01 [0xFF]...
  UnitAssert(a.IsFFI());
  T  msk = (T)0x7F << ((sizeof(T) * 8) - 7);  // 0xFE [0x00]...
  //  msk and a should be bitwise complements of each other.
  UnitAssert((a & msk) == 0);
  UnitAssert(~a == msk);
  UnitAssert((std::numeric_limits<T>::max() >> 7) == a);
  
  a <<= 7;
  UnitAssert(a.IsFFI());
  UnitAssert((a & 0x7F) == 0);  // the previous shift should have zero'd 7 LSB
  UnitAssert((a & 0x80) == 0x80);  // but not the 8th LSB
  if constexpr (sizeof(T) > 1) {
    UnitAssert(Dwm::FFIVar<T>(a & msk).IsFFI());
    UnitAssert((a & msk) == msk);
  }
  else {
    UnitAssert(0x80 == a);
  }
  
  //  or
  a = 0;
  a |= 0x0A;
  UnitAssert(a.IsFFI());
  UnitAssert(0x0A == a);
  a |= 0x05;
  UnitAssert(a.IsFFI());
  UnitAssert(0x0F == a);

  //  complement
  Dwm::FFIVar<T>  ffi_compl(0);
  UnitAssert(mx == ~ffi_compl);
  ffi_compl = ~mx;
  UnitAssert(ffi_compl.IsFFI());
  UnitAssert(0 == ffi_compl);

  //  exclusive or
  T  t_xor = 0;
  for (size_t i = 0; i < sizeof(T) * 8; i += 2) {
    t_xor |= ((T)1 << i);
  }
  Dwm::FFIVar<T>  ffi_xor(mx);
  ffi_xor ^= t_xor;
  UnitAssert((ffi_xor & t_xor) == 0);
  UnitAssert((ffi_xor | t_xor) == mx);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
requires std::is_integral<T>::value
static void TestIncrementDecrementOperators()
{
  const Dwm::FFIVar<T>  mx(std::numeric_limits<T>::max());
  const Dwm::FFIVar<T>  mn(std::numeric_limits<T>::min());
  
  UnitAssert(mx.IsFFI());
  Dwm::FFIVar<T>        a = mx;
  UnitAssert(a.IsFFI());
  Dwm::FFIVar<T>        b = a--;
  UnitAssert(a.IsFFI());
  UnitAssert(b.IsFFI());
  UnitAssert(a < b);
  UnitAssert((a + 1) == b);
  --b;
  UnitAssert(b.IsFFI());
  UnitAssert(a == b);

  a = mn;
  UnitAssert(a.IsFFI());
  b = a++;
  UnitAssert(a.IsFFI());
  UnitAssert(b.IsFFI());
  UnitAssert(a > b);
  UnitAssert((b + 1) == a);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestOperators()
{
  Dwm::FFIVar<T>  a = std::numeric_limits<T>::max();
  UnitAssert(a.IsFFI());

  Dwm::FFIVar<T>  b = a;
  UnitAssert(b.IsFFI());
  UnitAssert(b == a);

  if constexpr (std::is_integral<T>::value) {
    TestIncrementDecrementOperators<T>();
    if constexpr (std::is_unsigned<T>::value) {
      TestBitwiseOperators<T>();
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestFloatingPoint()
{
  Dwm::FFIVar<T>  a = std::numeric_limits<T>::max();
  UnitAssert(a.IsFFI());
  a = 0;
  UnitAssert(a.IsFFI());
  a = std::numeric_limits<T>::min();
  UnitAssert(a.IsFFI());
  a = 1;
  UnitAssert(a.IsFFI());
  a *= 2;
  UnitAssert(a.IsFFI());
  UnitAssert((a * 2) == 4);
  a += 4;
  UnitAssert(a.IsFFI());
  UnitAssert(6 == a);
  a /= 2;
  UnitAssert(a.IsFFI());
  UnitAssert(3 == a);
  UnitAssert((a / 2) == 1.5);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
requires std::is_integral<T>::value || std::is_floating_point<T>::value
static void TestPointers()
{
  const T           a[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  Dwm::FFIVar<const T*>  ap = a;
  UnitAssert(ap.IsFFI());
  for (size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i) {
    UnitAssert(ap.IsFFI());
    UnitAssert(&a[i] == ap);
    UnitAssert(a[i] == *ap);
    ++ap;
  }
  ap--;
  for (ssize_t i = sizeof(a)/sizeof(a[0]) - 1; i >= 0; --i) {
    UnitAssert(ap.IsFFI());
    UnitAssert(&a[i] == ap);
    UnitAssert(a[i] == *ap);
    --ap;
  }

  //  Intentionally corrupt (interference)
  memset(&ap, 0xA5, sizeof(ap) / 2);
  UnitAssert(! ap.IsFFI());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
requires std::is_integral<T>::value || std::is_floating_point<T>::value
static void TestArraysOfScalars()
{
  Dwm::FFIVar<T>  a[10];
  for (size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i) {
    UnitAssert(a[i].IsFFI());
    a[i] = i;
    UnitAssert(a[i].IsFFI());
    UnitAssert(a[i] == i);
  }

  std::array<Dwm::FFIVar<T>,10>  aa;
  for (size_t i = 0; i < aa.size(); ++i) {
    UnitAssert(aa.at(i).IsFFI());
    aa[i] = i;
    UnitAssert(aa.at(i).IsFFI());
    UnitAssert(aa.at(i) == i);
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestMap()
{
  //  Note this is questionable in real use, since we aren't protecting the
  //  metadata in the std::map.  We do protect the data and the less-than
  //  and equality operators in FFIVar check for interference.
  
  std::map<Dwm::FFIVar<T>,Dwm::FFIVar<T>>  m;
  size_t  maxSize = std::numeric_limits<T>::max();
  maxSize = (maxSize > 256) ? 256 : maxSize;
  for (size_t i = 0; i < maxSize; ++i) {
    m[i] = i;
  }
  UnitAssert(m.size() == maxSize);
  for (const auto & me : m) {
    UnitAssert(me.first.IsFFI());
    UnitAssert(me.second.IsFFI());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestOperators<int8_t>();
  TestOperators<uint8_t>();
  TestOperators<int16_t>();
  TestOperators<uint16_t>();
  TestOperators<int32_t>();
  TestOperators<uint32_t>();
  TestOperators<int64_t>();
  TestOperators<uint64_t>();

  TestFloatingPoint<float>();
  TestFloatingPoint<double>();

  TestArraysOfScalars<int8_t>();
  TestArraysOfScalars<uint8_t>();
  TestArraysOfScalars<int16_t>();
  TestArraysOfScalars<uint16_t>();
  TestArraysOfScalars<int32_t>();
  TestArraysOfScalars<uint32_t>();
  TestArraysOfScalars<int64_t>();
  TestArraysOfScalars<uint64_t>();
  TestArraysOfScalars<float>();
  TestArraysOfScalars<double>();

  TestMap<int8_t>();
  TestMap<uint8_t>();
  TestMap<int16_t>();
  TestMap<uint16_t>();
  TestMap<int32_t>();
  TestMap<uint32_t>();
  
  TestPointers<int8_t>();
  TestPointers<uint8_t>();
  TestPointers<int16_t>();
  TestPointers<uint16_t>();
  TestPointers<int32_t>();
  TestPointers<uint32_t>();
  TestPointers<int64_t>();
  TestPointers<uint64_t>();
  
  Dwm::FFIVar<int>  ffi_i = 20;
  Dwm::FFIVar<float>  ffi_f = 5;
  UnitAssert(4 == (ffi_i / ffi_f));
  ffi_f = 140;
  UnitAssert(7 == (ffi_f / ffi_i));

  Dwm::FFIVar<uint32_t>  ffi_u32(0x00000000);
  UnitAssert(~ffi_u32 == 0xFFFFFFFF);

  //  Intentionally corrupt (interference)
  memset(&ffi_u32, 0, sizeof(ffi_u32));
  UnitAssert(! ffi_u32.IsFFI());

  if (Assertions::Total().Failed())
    Assertions::Print(cerr, true);
  else
    cout << Assertions::Total() << " passed" << endl;
  exit(0);

 testFailed:
  Assertions::Print(cerr, true);
  exit(1);
}
