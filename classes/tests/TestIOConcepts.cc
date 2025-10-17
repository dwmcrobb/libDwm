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
#include "DwmIOConcepts.hh"
#include "DwmIpv4Address.hh"
#include "DwmIpv4Routes.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmRusage.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, template <typename> typename W>
static void TestWritableTrue()
{
  UnitAssert(Dwm::io_detail::Writable<T,W>());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, template <typename> typename W>
static void TestWritableFalse()
{
  UnitAssert(! (Dwm::io_detail::Writable<T,W>()));
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, template <typename> typename R>
static void TestReadableTrue()
{
  UnitAssert(Dwm::io_detail::Writable<T,R>());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T, template <typename> typename R>
static void TestReadableFalse()
{
  UnitAssert(! Dwm::io_detail::Writable<T,R>());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <template <typename> typename W, template <typename> typename R,
          typename T>
static void TestReadableWritableTrue()
{
  TestWritableTrue<T,W>();
  TestReadableTrue<T,R>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <template <typename> typename W, template <typename> typename R,
          typename T>
static void TestReadableWritableFalse()
{
  TestWritableFalse<T,W>();
  TestReadableFalse<T,R>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestReadableWritableTrue()
{
  TestReadableWritableTrue<HasStreamWrite_t,HasStreamRead_t,T>();
  TestReadableWritableTrue<HasAsioWrite_t,HasAsioRead_t,T>();
  TestReadableWritableTrue<HasBZWrite_t,HasBZRead_t,T>();
  TestReadableWritableTrue<HasDescriptorWrite_t,HasDescriptorRead_t,T>();
  TestReadableWritableTrue<HasFileWrite_t,HasFileRead_t,T>();
  TestReadableWritableTrue<HasGZWrite_t,HasGZWrite_t,T>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <typename T>
static void TestReadableWritableFalse()
{
  TestReadableWritableFalse<HasStreamWrite_t,HasStreamRead_t,T>();
  TestReadableWritableFalse<HasAsioWrite_t,HasAsioRead_t,T>();
  TestReadableWritableFalse<HasBZWrite_t,HasBZRead_t,T>();
  TestReadableWritableFalse<HasDescriptorWrite_t,HasDescriptorRead_t,T>();
  TestReadableWritableFalse<HasFileWrite_t,HasFileRead_t,T>();
  TestReadableWritableFalse<HasGZWrite_t,HasGZWrite_t,T>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDirectlySupported()
{
  typedef enum : uint8_t { a, b, c, d } EnumU8_t;
  typedef enum : uint16_t { aa, bb, cc, dd } EnumU16_t;
  typedef enum : uint32_t { aaa, bbb, ccc, ddd } EnumU32_t;
  typedef enum : uint32_t { aaaa, bbbb, cccc, dddd } EnumU64_t;
  
  TestReadableWritableTrue<char>();
  TestReadableWritableTrue<int8_t>();
  TestReadableWritableTrue<uint8_t>();
  TestReadableWritableTrue<int16_t>();
  TestReadableWritableTrue<uint16_t>();
  TestReadableWritableTrue<int32_t>();
  TestReadableWritableTrue<uint32_t>();
  TestReadableWritableTrue<int64_t>();
  TestReadableWritableTrue<uint64_t>();
  TestReadableWritableTrue<bool>();
  TestReadableWritableTrue<float>();
  TestReadableWritableTrue<double>();
  TestReadableWritableTrue<std::string>();
  TestReadableWritableTrue<EnumU8_t>();
  TestReadableWritableTrue<EnumU16_t>();
  TestReadableWritableTrue<EnumU32_t>();
  TestReadableWritableTrue<EnumU64_t>();
  TestReadableWritableTrue<std::vector<bool>>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestVectors()
{
  TestReadableWritableTrue<std::vector<uint8_t>>();
  TestReadableWritableTrue<std::vector<std::string>>();
  TestReadableWritableTrue<std::vector<std::set<std::string>>>();
  TestReadableWritableTrue<std::vector<std::map<std::string,int>>>();
  TestReadableWritableTrue<std::vector<std::map<int,std::string>>>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestVectorsNotWritable()
{
  TestReadableWritableFalse<std::vector<int *>>();
  TestReadableWritableFalse<const std::vector<int>>();
  TestReadableWritableFalse<std::vector<std::map<int,int *>>>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestMaps()
{
  TestReadableWritableTrue<std::map<std::string,std::vector<std::string>>>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestTuples()
{
  TestReadableWritableTrue<std::tuple<int,std::string,
                                      std::vector<std::string>,
                                      std::map<int,bool>>>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestUniquePtr()
{
  TestReadableWritableTrue<std::unique_ptr<int>>();
  TestReadableWritableFalse<std::unique_ptr<int[]>>();
  UnitAssert(io_detail::IsUniquePtrToArray<std::unique_ptr<int[]>>);
  UnitAssert(! io_detail::IsUniquePtrToArray<std::unique_ptr<int>>);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestOptional()
{
  TestReadableWritableTrue<std::optional<std::string>>();
  TestReadableWritableTrue<std::unique_ptr<std::optional<int>>>();
  TestReadableWritableFalse<std::optional<const std::string>>();
  TestReadableWritableFalse<const std::optional<std::string>>();
  TestReadableWritableFalse<std::optional<std::unique_ptr<int[]>>>();
  TestReadableWritableFalse<std::unique_ptr<std::optional<int>[]>>();
  return;
}

#if defined(DWM_CAN_USE_REFLECTION)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSkipAnnotation()
{
  struct SK1 {
    int                    i;
    [[=Dwm::skip_io]]  int j;
  };
  UnitAssert(! io_detail::HasSkipAnnotation<^^SK1::i>);
  UnitAssert(io_detail::HasSkipAnnotation<^^SK1::j>);
  UnitAssert(! io_detail::Skip<decltype(SK1::i),^^SK1::i>());
  if (UnitAssert(io_detail::Skip<decltype(SK1::j),^^SK1::j>())) {
    UnitAssert(! io_detail::SkipReason<decltype(SK1::j),^^SK1::j>().empty());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDenyAnnotation()
{
  struct SK1 {
    int                    i;
    [[=Dwm::deny_io]]  int j;
  };
  UnitAssert(! io_detail::HasSkipAnnotation<^^SK1::i>);
  UnitAssert(! io_detail::HasSkipAnnotation<^^SK1::j>);
  UnitAssert(! io_detail::HasDenyAnnotation<^^SK1::i>);
  UnitAssert(io_detail::HasDenyAnnotation<^^SK1::j>);
  UnitAssert(! io_detail::Skip<decltype(SK1::i),^^SK1::i>());
  UnitAssert(! io_detail::Skip<decltype(SK1::j),^^SK1::j>());
  if (UnitAssert(io_detail::Deny<^^SK1::j>())) {
    UnitAssert(! io_detail::DenyReason<^^SK1::j>().empty());
  }
  return;
}

#endif

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestConstStreamedLength()
{
#if defined(DWM_CAN_USE_REFLECTION)
  struct SK1 {
    int    i;
    float  f;
    bool   b[10];
    char   c[4][4];
  };
  UnitAssert(io_detail::HasConstStreamedLength<SK1>());
  UnitAssert(io_detail::ConstStreamedLength<SK1>() == 34);
  
  struct SK2 {
    std::string  s;
    float        f;
    bool         b[10];
    char         c[4][4];
  };
  UnitAssert(! io_detail::HasConstStreamedLength<SK2>());

  struct SK3 {
    std::map<int,int>  m;
    int                i;
  };
  UnitAssert(! io_detail::HasConstStreamedLength<SK3>());
  
  struct SK4 {
    [[=Dwm::skip_io]] std::string  s;
    float        f;
    bool         b[10];
    char         c[4][4];
  };
  UnitAssert(io_detail::HasConstStreamedLength<SK4>());
  UnitAssert(io_detail::ConstStreamedLength<SK4>() == 30);

  struct SK5 {
    std::array<int,8>  a;
    int                i;
  };
  UnitAssert(io_detail::HasConstStreamedLength<SK5>());
  UnitAssert(io_detail::ConstStreamedLength<SK5>() == 36);

  struct SK6 {
    std::array<std::string,8>  a;
    int                        i;
  };
  UnitAssert(! io_detail::HasConstStreamedLength<SK6>());

  struct SK7 {
    std::array<std::array<uint32_t,4>,8>  aa;
    uint8_t                               i;
  };
  UnitAssert(io_detail::HasConstStreamedLength<SK7>());
  UnitAssert(io_detail::ConstStreamedLength<SK7>() == 129);

  struct SK8 {
    std::array<std::array<uint32_t,4>,8>  aa[2];
    uint8_t                               i;
  };
  UnitAssert(io_detail::HasConstStreamedLength<SK8>());
  UnitAssert(io_detail::ConstStreamedLength<SK8>() == 257);

  struct SK9 {
    bool  b;
    struct S {
      int                ia[2][2];
      [[=Dwm::skip_io]]  std::string  s;
    } s;
  };
  UnitAssert(io_detail::HasConstStreamedLength<SK9>());
  UnitAssert(io_detail::ConstStreamedLength<SK9>() == 17);

  struct SK10 {
    bool  b;
    struct S {
      int          ia[2][2];
      std::string  s;
    } s;
  };
  UnitAssert(! io_detail::HasConstStreamedLength<SK10>());

#endif  // defined(DWM_CAN_USE_REFLECTION)

  UnitAssert(io_detail::HasConstStreamedLength<Dwm::Rusage>());
  UnitAssert(io_detail::ConstStreamedLength<Dwm::Rusage>() == 96);

  UnitAssert(io_detail::HasConstStreamedLength<Dwm::Ipv4Address>());
  UnitAssert(io_detail::ConstStreamedLength<Dwm::Ipv4Address>() == 4);

  UnitAssert(! io_detail::HasConstStreamedLength<Dwm::Ipv4Routes<int>>());
  UnitAssert(HasStreamedLength<Dwm::Ipv4Routes<int>>);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestAtomics()
{
  TestReadableWritableTrue<std::atomic<char>>();
  TestReadableWritableTrue<std::atomic<uint64_t>>();
  TestReadableWritableFalse<std::atomic<char *>>();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIOConcepts()
{
  TestDirectlySupported();
  TestVectors();
  TestMaps();
  TestTuples();
  TestVectorsNotWritable();
  TestUniquePtr();
  TestOptional();
  TestAtomics();
  TestConstStreamedLength();
#if defined(DWM_CAN_USE_REFLECTION)
  TestSkipAnnotation();
  TestDenyAnnotation();
#endif  
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
