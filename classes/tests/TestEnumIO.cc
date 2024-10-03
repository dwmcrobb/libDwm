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

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
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
  e_zero = 0,
  e_val = 192813
};

enum class S32ClassEnum : int32_t {
  e_zero = 0,
  e_val = -813192
};

static const BasicEnum         g_be1    = e_basicEnumVal;
static const BasicU8Enum       g_bu8e1  = e_basicU8EnumVal;
static const BasicU16Enum      g_bu16e1 = e_basicU16EnumVal;
static const BasicU32Enum      g_bu32e1 = e_basicU32EnumVal;
static const BasicS8Enum       g_bs8e1  = e_basicS8EnumVal;
static const BasicS16Enum      g_bs16e1 = e_basicS16EnumVal;
static const BasicS32Enum      g_bs32e1 = e_basicS32EnumVal;
static const UntypedClassEnum  g_utce1  = UntypedClassEnum::e_val;
static const U8ClassEnum       g_u8ce1  = U8ClassEnum::e_val;
static const U16ClassEnum      g_u16ce1 = U16ClassEnum::e_val;
static const U32ClassEnum      g_u32ce1 = U32ClassEnum::e_val;
static const S8ClassEnum       g_s8ce1  = S8ClassEnum::e_val;
static const S16ClassEnum      g_s16ce1 = S16ClassEnum::e_val;
static const S32ClassEnum      g_s32ce1 = S32ClassEnum::e_val;

static BasicEnum         g_be2;
static BasicU8Enum       g_bu8e2;
static BasicU16Enum      g_bu16e2;
static BasicU32Enum      g_bu32e2;
static BasicS8Enum       g_bs8e2;
static BasicS16Enum      g_bs16e2;
static BasicS32Enum      g_bs32e2;
static UntypedClassEnum  g_utce2;
static U8ClassEnum       g_u8ce2;
static U16ClassEnum      g_u16ce2;
static U32ClassEnum      g_u32ce2;
static S8ClassEnum       g_s8ce2;
static S16ClassEnum      g_s16ce2;
static S32ClassEnum      g_s32ce2;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ResetEnums()
{
  g_be2    = e_basicEnum0;
  g_bu8e2  = e_basicU8Enum0;
  g_bu16e2 = e_basicU16Enum0;
  g_bu32e2 = e_basicU32Enum0;
  g_bs8e2  = e_basicS8Enum0;
  g_bs16e2 = e_basicS16Enum0;
  g_bs32e2 = e_basicS32Enum0;
  g_utce2  = UntypedClassEnum::e_zero;
  g_u8ce2  = U8ClassEnum::e_zero;
  g_u16ce2 = U16ClassEnum::e_zero;
  g_u32ce2 = U32ClassEnum::e_zero;
  g_s8ce2  = S8ClassEnum::e_zero;
  g_s16ce2 = S16ClassEnum::e_zero;
  g_s32ce2 = S32ClassEnum::e_zero;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void AssertEnumsMatch()
{
  UnitAssert(g_be2 == g_be1);
  UnitAssert(g_bu8e2 == g_bu8e1);
  UnitAssert(g_bu16e2 == g_bu16e1);
  UnitAssert(g_bu32e2 == g_bu32e1);
  UnitAssert(g_bs8e2 == g_bs8e1);
  UnitAssert(g_bs16e2 == g_bs16e1);
  UnitAssert(g_bs32e2 == g_bs32e1);
  UnitAssert(g_utce2 == g_utce1);
  UnitAssert(g_u8ce2 == g_u8ce1);
  UnitAssert(g_u16ce2 == g_u16ce1);
  UnitAssert(g_u32ce2 == g_u32ce1);
  UnitAssert(g_s8ce2 == g_s8ce1);
  UnitAssert(g_s16ce2 == g_s16ce1);
  UnitAssert(g_s32ce2 == g_s32ce1);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO()
{
  int fd = open("./TestEnumIO.tmp", O_WRONLY|O_CREAT, 0600);
  if (UnitAssert(0 <= fd)) {
    if (UnitAssert(DescriptorIO::WriteV(fd, g_be1, g_bu8e1, g_bu16e1,
                                        g_bu32e1, g_bs8e1, g_bs16e1, g_bs32e1,
                                        g_utce1, g_u8ce1, g_u16ce1, g_u32ce1,
                                        g_s8ce1, g_s16ce1, g_s32ce1))) {
      close(fd);

      fd = open("./TestEnumIO.tmp", O_RDONLY, 0600);
      if (UnitAssert(0 <= fd)) {
        ResetEnums();
        if (UnitAssert(DescriptorIO::ReadV(fd, g_be2, g_bu8e2, g_bu16e2,
                                           g_bu32e2, g_bs8e2, g_bs16e2,
                                           g_bs32e2, g_utce2, g_u8ce2,
                                           g_u16ce2, g_u32ce2, g_s8ce2,
                                           g_s16ce2, g_s32ce2))) {
          AssertEnumsMatch();          
        }
        close(fd);
      }
    }
    else {
      close(fd);
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
  FILE *f = fopen("./TestEnumIO.tmp", "w");
  if (UnitAssert(f)) {
    if (UnitAssert(FileIO::WriteV(f, g_be1, g_bu8e1, g_bu16e1, g_bu32e1,
                                  g_bs8e1, g_bs16e1, g_bs32e1, g_utce1,
                                  g_u8ce1, g_u16ce1, g_u32ce1, g_s8ce1,
                                  g_s16ce1, g_s32ce1))) {
      fclose(f);
      f = fopen("./TestEnumIO.tmp", "r");
      if (UnitAssert(f)) {
        ResetEnums();
        if (UnitAssert(FileIO::ReadV(f, g_be2, g_bu8e2, g_bu16e2, g_bu32e2,
                                     g_bs8e2, g_bs16e2, g_bs32e2, g_utce2,
                                     g_u8ce2, g_u16ce2, g_u32ce2, g_s8ce2,
                                     g_s16ce2, g_s32ce2))) {
          AssertEnumsMatch();
        }
        fclose(f);
      }
    }
    else {
      fclose(f);
    }
    std::remove("./TestEnumIO.tmp");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO()
{
  BZFILE  *bzf = BZ2_bzopen("./TestEnumIO.bz2", "wb");
  if (UnitAssert(bzf)) {
    if (UnitAssert(BZ2IO::BZWriteV(bzf, g_be1, g_bu8e1, g_bu16e1,
                                   g_bu32e1, g_bs8e1, g_bs16e1, g_bs32e1,
                                   g_utce1, g_u8ce1, g_u16ce1, g_u32ce1,
                                   g_s8ce1, g_s16ce1, g_s32ce1))) {
      BZ2_bzclose(bzf);

      bzf = BZ2_bzopen("./TestEnumIO.bz2", "rb");
      if (UnitAssert(bzf)) {
        ResetEnums();
        if (UnitAssert(BZ2IO::BZReadV(bzf, g_be2, g_bu8e2, g_bu16e2,
                                      g_bu32e2, g_bs8e2, g_bs16e2,
                                      g_bs32e2, g_utce2, g_u8ce2,
                                      g_u16ce2, g_u32ce2, g_s8ce2,
                                      g_s16ce2, g_s32ce2))) {
          AssertEnumsMatch();          
        }
        BZ2_bzclose(bzf);
      }
    }
    else {
      BZ2_bzclose(bzf);
    }
    std::remove("./TestEnumIO.bz2");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO()
{
  gzFile  gzf = gzopen("./TestEnumIO.gz", "wb");
  if (UnitAssert(gzf)) {
    if (UnitAssert(GZIO::WriteV(gzf, g_be1, g_bu8e1, g_bu16e1, g_bu32e1,
                                g_bs8e1, g_bs16e1, g_bs32e1, g_utce1,
                                g_u8ce1, g_u16ce1, g_u32ce1, g_s8ce1,
                                g_s16ce1, g_s32ce1))) {
      gzclose(gzf);
      gzf = gzopen("./TestEnumIO.gz", "rb");
      if (UnitAssert(gzf)) {
        ResetEnums();
        if (UnitAssert(GZIO::ReadV(gzf, g_be2, g_bu8e2, g_bu16e2, g_bu32e2,
                                   g_bs8e2, g_bs16e2, g_bs32e2, g_utce2,
                                   g_u8ce2, g_u16ce2, g_u32ce2, g_s8ce2,
                                   g_s16ce2, g_s32ce2))) {
          AssertEnumsMatch();
        }
        gzclose(gzf);
      }
    }
    else {
      gzclose(gzf);
    }
    std::remove("./TestEnumIO.gz");
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO()
{
  stringstream  ss;
  if (UnitAssert(StreamIO::WriteV(ss, g_be1, g_bu8e1, g_bu16e1, g_bu32e1,
                                  g_bs8e1, g_bs16e1, g_bs32e1, g_utce1,
                                  g_u8ce1, g_u16ce1, g_u32ce1, g_s8ce1,
                                  g_s16ce1, g_s32ce1))) {
    ResetEnums();
    if (UnitAssert(StreamIO::ReadV(ss, g_be2, g_bu8e2, g_bu16e2, g_bu32e2,
                                   g_bs8e2, g_bs16e2, g_bs32e2, g_utce2,
                                   g_u8ce2, g_u16ce2, g_u32ce2, g_s8ce2,
                                   g_s16ce2, g_s32ce2))) {
      AssertEnumsMatch();
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
  TestBZ2IO();
  TestGZIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    exit(0);
  }
}
