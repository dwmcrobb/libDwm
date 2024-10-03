//===========================================================================
// @(#) $DwmPath:$
// @(#) $Id:$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file TestEnumIO.cc
//!  \author Daniel W. McRobb
//!  \brief enum I/O unit tests
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
}

#include <sstream>

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestVariantIO.cc 11087 $");

enum BasicEnum {
  e_basicEnum0   = 0,
  e_basicEnumVal = 42
};

enum BasicU8Enum : uint8_t {
  e_basicU8Enum0 = 0,
  e_basicU8EnumVal = 57
};

enum BasicS8Enum : int8_t {
  e_basicS8Enum0 = 0,
  e_basicS8EnumVal = -43
};

enum BasicU16Enum : uint16_t {
  e_basicU16Enum0 = 0,
  e_basicU16EnumVal = 6192
};

enum BasicS16Enum : int16_t {
  e_basicS16Enum0 = 0,
  e_basicS16EnumVal = -6193
};

enum BasicU32Enum : uint32_t {
  e_basicU32Enum0 = 0,
  e_basicU32EnumVal = 131131
};

enum BasicS32Enum : int32_t {
  e_basicS32Enum0 = 0,
  e_basicS32EnumVal = -132132
};

enum class UntypedClassEnum {
  e_zero = 0,
  e_val = 73
};

enum class U8ClassEnum : uint8_t {
  e_zero = 0,
  e_val = 41
};

enum class S8ClassEnum : int8_t {
  e_zero = 0,
  e_val = -45
};

enum class U16ClassEnum : uint16_t {
  e_zero = 0,
  e_val = 5150
};

enum class S16ClassEnum : int16_t {
  e_zero = 0,
  e_val = -5051
};

enum class U32ClassEnum : uint32_t {
  e_val = 192813
};

enum class S32ClassEnum : int32_t {
  e_val = -813192
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO()
{
  BasicEnum         be1    = e_basicEnumVal;
  BasicU8Enum       bu8e1  = e_basicU8EnumVal;
  BasicU16Enum      bu16e1 = e_basicU16EnumVal;
  BasicU32Enum      bu32e1 = e_basicU32EnumVal;
  BasicS8Enum       bs8e1  = e_basicS8EnumVal;
  BasicS16Enum      bs16e1 = e_basicS16EnumVal;
  BasicS32Enum      bs32e1 = e_basicS32EnumVal;
  UntypedClassEnum  utce1  = UntypedClassEnum::e_val;
  U8ClassEnum       u8ce1  = U8ClassEnum::e_val;
  U16ClassEnum      u16ce1 = U16ClassEnum::e_val;
  U32ClassEnum      u32ce1 = U32ClassEnum::e_val;
  S8ClassEnum       s8ce1  = S8ClassEnum::e_val;
  S16ClassEnum      s16ce1 = S16ClassEnum::e_val;
  S32ClassEnum      s32ce1 = S32ClassEnum::e_val;

  int fd = open("./TestEnumIO.tmp", O_WRONLY|O_CREAT, 0600);
  if (UnitAssert(0 <= fd)) {
    if (UnitAssert(DescriptorIO::WriteV(fd, be1, bu8e1, bu16e1, bu32e1, bs8e1,
                                        bs16e1, bs32e1, utce1, u8ce1, u16ce1,
                                        u32ce1, s8ce1, s16ce1, s32ce1))) {
      BasicEnum         be2;
      BasicU8Enum       bu8e2;
      BasicU16Enum      bu16e2;
      BasicU32Enum      bu32e2;
      BasicS8Enum       bs8e2;
      BasicS16Enum      bs16e2;
      BasicS32Enum      bs32e2;
      UntypedClassEnum  utce2;
      U8ClassEnum       u8ce2;
      U16ClassEnum      u16ce2;
      U32ClassEnum      u32ce2;
      S8ClassEnum       s8ce2;
      S16ClassEnum      s16ce2;
      S32ClassEnum      s32ce2;
      close(fd);

      fd = open("./TestEnumIO.tmp", O_RDONLY, 0600);
      if (UnitAssert(0 <= fd)) {
        if (UnitAssert(DescriptorIO::ReadV(fd, be2, bu8e2, bu16e2, bu32e2,
                                           bs8e2, bs16e2, bs32e2, utce2,
                                           u8ce2, u16ce2, u32ce2, s8ce2,
                                           s16ce2, s32ce2))) {
          UnitAssert(be2 == be1);
          UnitAssert(bu8e2 == bu8e1);
          UnitAssert(bu16e2 == bu16e1);
          UnitAssert(bu32e2 == bu32e1);
          UnitAssert(bs8e2 == bs8e1);
          UnitAssert(bs16e2 == bs16e1);
          UnitAssert(bs32e2 == bs32e1);
          UnitAssert(utce2 == utce1);
          UnitAssert(u8ce2 == u8ce1);
          UnitAssert(u16ce2 == u16ce1);
          UnitAssert(u32ce2 == u32ce1);
          UnitAssert(s8ce2 == s8ce1);
          UnitAssert(s16ce2 == s16ce1);
          UnitAssert(s32ce2 == s32ce1);
        }
        close(fd);
      }
    }
    std::remove("./TestEnumIO.tmp");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO()
{
  BasicEnum         be1    = e_basicEnumVal;
  BasicU8Enum       bu8e1  = e_basicU8EnumVal;
  BasicU16Enum      bu16e1 = e_basicU16EnumVal;
  BasicU32Enum      bu32e1 = e_basicU32EnumVal;
  BasicS8Enum       bs8e1  = e_basicS8EnumVal;
  BasicS16Enum      bs16e1 = e_basicS16EnumVal;
  BasicS32Enum      bs32e1 = e_basicS32EnumVal;
  UntypedClassEnum  utce1  = UntypedClassEnum::e_val;
  U8ClassEnum       u8ce1  = U8ClassEnum::e_val;
  U16ClassEnum      u16ce1 = U16ClassEnum::e_val;
  U32ClassEnum      u32ce1 = U32ClassEnum::e_val;
  S8ClassEnum       s8ce1  = S8ClassEnum::e_val;
  S16ClassEnum      s16ce1 = S16ClassEnum::e_val;
  S32ClassEnum      s32ce1 = S32ClassEnum::e_val;

  FILE *f = fopen("./TestEnumIO.tmp", "w");
  if (UnitAssert(f)) {
    if (UnitAssert(FileIO::WriteV(f, be1, bu8e1, bu16e1, bu32e1, bs8e1,
                                  bs16e1, bs32e1, utce1, u8ce1, u16ce1,
                                  u32ce1, s8ce1, s16ce1, s32ce1))) {
      BasicEnum         be2;
      BasicU8Enum       bu8e2;
      BasicU16Enum      bu16e2;
      BasicU32Enum      bu32e2;
      BasicS8Enum       bs8e2;
      BasicS16Enum      bs16e2;
      BasicS32Enum      bs32e2;
      UntypedClassEnum  utce2;
      U8ClassEnum       u8ce2;
      U16ClassEnum      u16ce2;
      U32ClassEnum      u32ce2;
      S8ClassEnum       s8ce2;
      S16ClassEnum      s16ce2;
      S32ClassEnum      s32ce2;
      fclose(f);

      f = fopen("./TestEnumIO.tmp", "r");
      if (UnitAssert(f)) {
        if (UnitAssert(FileIO::ReadV(f, be2, bu8e2, bu16e2, bu32e2,
                                     bs8e2, bs16e2, bs32e2, utce2,
                                     u8ce2, u16ce2, u32ce2, s8ce2,
                                     s16ce2, s32ce2))) {
          UnitAssert(be2 == be1);
          UnitAssert(bu8e2 == bu8e1);
          UnitAssert(bu16e2 == bu16e1);
          UnitAssert(bu32e2 == bu32e1);
          UnitAssert(bs8e2 == bs8e1);
          UnitAssert(bs16e2 == bs16e1);
          UnitAssert(bs32e2 == bs32e1);
          UnitAssert(utce2 == utce1);
          UnitAssert(u8ce2 == u8ce1);
          UnitAssert(u16ce2 == u16ce1);
          UnitAssert(u32ce2 == u32ce1);
          UnitAssert(s8ce2 == s8ce1);
          UnitAssert(s16ce2 == s16ce1);
          UnitAssert(s32ce2 == s32ce1);
        }
        fclose(f);
      }
    }
    std::remove("./TestEnumIO.tmp");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  BasicEnum         be1    = e_basicEnumVal;
  BasicU8Enum       bu8e1  = e_basicU8EnumVal;
  BasicU16Enum      bu16e1 = e_basicU16EnumVal;
  BasicU32Enum      bu32e1 = e_basicU32EnumVal;
  BasicS8Enum       bs8e1  = e_basicS8EnumVal;
  BasicS16Enum      bs16e1 = e_basicS16EnumVal;
  BasicS32Enum      bs32e1 = e_basicS32EnumVal;
  UntypedClassEnum  utce1  = UntypedClassEnum::e_val;
  U8ClassEnum       u8ce1  = U8ClassEnum::e_val;
  U16ClassEnum      u16ce1 = U16ClassEnum::e_val;
  U32ClassEnum      u32ce1 = U32ClassEnum::e_val;
  S8ClassEnum       s8ce1  = S8ClassEnum::e_val;
  S16ClassEnum      s16ce1 = S16ClassEnum::e_val;
  S32ClassEnum      s32ce1 = S32ClassEnum::e_val;

  stringstream  ss;
  if (UnitAssert(StreamIO::WriteV(ss, be1, bu8e1, bu16e1, bu32e1, bs8e1,
                                  bs16e1, bs32e1, utce1, u8ce1, u16ce1,
                                  u32ce1, s8ce1, s16ce1, s32ce1))) {
    BasicEnum         be2;
    BasicU8Enum       bu8e2;
    BasicU16Enum      bu16e2;
    BasicU32Enum      bu32e2;
    BasicS8Enum       bs8e2;
    BasicS16Enum      bs16e2;
    BasicS32Enum      bs32e2;
    UntypedClassEnum  utce2;
    U8ClassEnum       u8ce2;
    U16ClassEnum      u16ce2;
    U32ClassEnum      u32ce2;
    S8ClassEnum       s8ce2;
    S16ClassEnum      s16ce2;
    S32ClassEnum      s32ce2;
    
    if (UnitAssert(StreamIO::ReadV(ss, be2, bu8e2, bu16e2, bu32e2, bs8e2,
                                   bs16e2, bs32e2, utce2, u8ce2, u16ce2,
                                   u32ce2, s8ce2, s16ce2, s32ce2))) {
      UnitAssert(be2 == be1);
      UnitAssert(bu8e2 == bu8e1);
      UnitAssert(bu16e2 == bu16e1);
      UnitAssert(bu32e2 == bu32e1);
      UnitAssert(bs8e2 == bs8e1);
      UnitAssert(bs16e2 == bs16e1);
      UnitAssert(bs32e2 == bs32e1);
      UnitAssert(utce2 == utce1);
      UnitAssert(u8ce2 == u8ce1);
      UnitAssert(u16ce2 == u16ce1);
      UnitAssert(u32ce2 == u32ce1);
      UnitAssert(s8ce2 == s8ce1);
      UnitAssert(s16ce2 == s16ce1);
      UnitAssert(s32ce2 == s32ce1);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestStreamIO();
  TestDescriptorIO();
  TestFileIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    exit(0);
  }
}
