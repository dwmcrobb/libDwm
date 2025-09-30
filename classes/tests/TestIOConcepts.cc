//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2025
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
//!  \file TestIOConcepts.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmASIOCapable.hh"
#include "DwmBZ2IOCapable.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmIOConcepts.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, template <typename> typename W>
static bool TestWritableTrue()
{
  return UnitAssert((Dwm::io_detail::Writable<T,W>()));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, template <typename> typename R>
static bool TestReadableTrue()
{
  return UnitAssert((Dwm::io_detail::Writable<T,R>()));
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <template <typename> typename W, template <typename> typename R,
          typename T>
static bool TestReadableWritableTrue()
{
  bool  rc = UnitAssert((TestWritableTrue<T,W>()));
  rc &= UnitAssert((TestReadableTrue<T,R>()));
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static bool TestReadableWritableTrue()
{
  bool  rc = UnitAssert((TestReadableWritableTrue<HasStreamWrite_t,HasStreamRead_t,T>()));
  rc &= UnitAssert((TestReadableWritableTrue<HasAsioWrite_t,HasAsioRead_t,T>()));
  rc &= UnitAssert((TestReadableWritableTrue<HasBZWrite_t,HasBZRead_t,T>()));
  rc &= UnitAssert((TestReadableWritableTrue<HasDescriptorWrite_t,HasDescriptorRead_t,T>()));
  rc &= UnitAssert((TestReadableWritableTrue<HasFileWrite_t,HasFileRead_t,T>()));
  rc &= UnitAssert((TestReadableWritableTrue<HasGZWrite_t,HasGZWrite_t,T>()));
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestDirectlySupported()
{
  typedef enum : uint8_t { a, b, c, d } EnumU8_t;
  typedef enum : uint16_t { aa, bb, cc, dd } EnumU16_t;
  typedef enum : uint32_t { aaa, bbb, ccc, ddd } EnumU32_t;
  typedef enum : uint32_t { aaaa, bbbb, cccc, dddd } EnumU64_t;
  
  bool  rc = UnitAssert(TestReadableWritableTrue<char>());
  rc &= UnitAssert(TestReadableWritableTrue<int8_t>());
  rc &= UnitAssert(TestReadableWritableTrue<uint8_t>());
  rc &= UnitAssert(TestReadableWritableTrue<int16_t>());
  rc &= UnitAssert(TestReadableWritableTrue<uint16_t>());
  rc &= UnitAssert(TestReadableWritableTrue<int32_t>());
  rc &= UnitAssert(TestReadableWritableTrue<uint32_t>());
  rc &= UnitAssert(TestReadableWritableTrue<int64_t>());
  rc &= UnitAssert(TestReadableWritableTrue<uint64_t>());
  rc &= UnitAssert(TestReadableWritableTrue<bool>());
  rc &= UnitAssert(TestReadableWritableTrue<float>());
  rc &= UnitAssert(TestReadableWritableTrue<double>());
  rc &= UnitAssert(TestReadableWritableTrue<std::string>());
  rc &= UnitAssert(TestReadableWritableTrue<EnumU8_t>());
  rc &= UnitAssert(TestReadableWritableTrue<EnumU16_t>());
  rc &= UnitAssert(TestReadableWritableTrue<EnumU32_t>());
  rc &= UnitAssert(TestReadableWritableTrue<EnumU64_t>());
  rc &= UnitAssert(TestReadableWritableTrue<std::vector<bool>>());
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestVectors()
{
  bool  rc = UnitAssert(TestReadableWritableTrue<std::vector<uint8_t>>());
  rc &= UnitAssert(TestReadableWritableTrue<std::vector<std::string>>());
  rc &= UnitAssert(TestReadableWritableTrue<std::vector<std::set<std::string>>>());
  rc &= UnitAssert((TestReadableWritableTrue<std::vector<std::map<std::string,int>>>()));
  rc &= UnitAssert((TestReadableWritableTrue<std::vector<std::map<int,std::string>>>()));
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestMaps()
{
  bool  rc = UnitAssert((TestReadableWritableTrue<std::map<std::string,std::vector<std::string>>>()));
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestTuples()
{
  bool  rc = UnitAssert((TestReadableWritableTrue<std::tuple<int,std::string,
                                                             std::vector<std::string>,
                                                             std::map<int,bool>>>));
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIOConcepts()
{
  UnitAssert(TestDirectlySupported());
  UnitAssert(TestVectors());
  UnitAssert(TestMaps());
  UnitAssert(TestTuples());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIOConcepts();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  cout << Assertions::Total() << " passed" << endl;
  exit(0);
}
