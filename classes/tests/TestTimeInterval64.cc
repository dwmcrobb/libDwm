//===========================================================================
// @(#) $Name:$
// @(#) $Id: TestTimeInterval64.cc 9101 2017-04-14 20:37:59Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2017
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
//!  \file TestTimeInterval64.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
}

#include <fstream>

#include "DwmSvnTag.hh"
#include "DwmTimeInterval64.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestTimeInterval64.cc 9101 $");

using namespace std;
using namespace Dwm;

#define TEST_FILE "./Time_Interval_Test.out"

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestStreamIO()
{
  TimeInterval64  ti;
  ofstream  os(TEST_FILE);
  if (UnitAssert(ti.Write(os))) {
    os.close();
    TimeInterval64  ti2;
    ifstream  is(TEST_FILE);
    if (UnitAssert(ti2.Read(is))) {
      UnitAssert(ti2 == ti);
    }
    is.close();
    remove(TEST_FILE);
  }

  TimeInterval64  ti3;
  TimeValue64     yesterday(true);
  yesterday.Set(yesterday.Secs() - (24 * 60 * 60), 0);
  ti3.Start(yesterday);
  ti3.End(TimeValue64(true));
  UnitAssert(ti3.Contains(yesterday));
  os.open(TEST_FILE);
  if (UnitAssert(os)) {
    if (UnitAssert(ti3.Write(os))) {
      if (UnitAssert(ti.Write(os))) {
        os.close();
        TimeInterval64  ti4, ti5;
        ifstream  is(TEST_FILE);
        if (UnitAssert(ti4.Read(is))) {
          if (UnitAssert(ti5.Read(is))) {
            UnitAssert(ti4 == ti3);
            UnitAssert(ti4.Contains(yesterday));
            UnitAssert(ti5 == ti);
            UnitAssert(! ti5.Contains(yesterday));
          }
          is.close();
        }
      }
    }
    remove(TEST_FILE);
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestFileIO()
{
  TimeInterval64  ti;
  FILE  *f = fopen(TEST_FILE, "w");
  if (UnitAssert(ti.Write(f) == 1)) {
    fclose(f);
    TimeInterval64  ti2;
    f = fopen(TEST_FILE, "r");
    if (UnitAssert(ti2.Read(f))) {
      UnitAssert(ti2 == ti);
    }
    fclose(f);
    remove(TEST_FILE);
  }

  TimeInterval64  ti3;
  TimeValue64     yesterday(true);
  yesterday.Set(yesterday.Secs() - (24 * 60 * 60), 0);
  ti3.Start(yesterday);
  ti3.End(TimeValue64(true));
  UnitAssert(ti3.Contains(yesterday));
  f = fopen(TEST_FILE, "w");
  if (UnitAssert(f)) {
    if (UnitAssert(ti3.Write(f) == 1)) {
      if (UnitAssert(ti.Write(f) == 1)) {
        fclose(f);
        TimeInterval64  ti4, ti5;
        f = fopen(TEST_FILE, "r");
        if (UnitAssert(ti4.Read(f))) {
          if (UnitAssert(ti5.Read(f))) {
            UnitAssert(ti4 == ti3);
            UnitAssert(ti4.Contains(yesterday));
            UnitAssert(ti5 == ti);
            UnitAssert(! ti5.Contains(yesterday));
          }
          fclose(f);
        }
      }
    }
    remove(TEST_FILE);
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestDescriptorIO()
{
  TimeInterval64  ti;
  int  fd = open(TEST_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (UnitAssert(fd >= 0)) {
    if (UnitAssert(ti.Write(fd) == ti.StreamedLength())) {
      close(fd);
      TimeInterval64  ti2;
      fd = open(TEST_FILE, O_RDONLY);
      if (UnitAssert(fd >= 0)) {
        if (UnitAssert(ti2.Read(fd))) {
          UnitAssert(ti2 == ti);
        }
        close(fd);
      }
    }
    remove(TEST_FILE);
  }

  TimeInterval64  ti3;
  TimeValue64     yesterday(true);
  yesterday.Set(yesterday.Secs() - (24 * 60 * 60), 0);
  ti3.Start(yesterday);
  ti3.End(TimeValue64(true));
  UnitAssert(ti3.Contains(yesterday));
  fd = open(TEST_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (UnitAssert(fd >= 0)) {
    if (UnitAssert(ti3.Write(fd) == ti3.StreamedLength())) {
      if (UnitAssert(ti.Write(fd) == ti.StreamedLength())) {
        close(fd);
        TimeInterval64  ti4, ti5;
        fd = open(TEST_FILE, O_RDONLY);
        if (UnitAssert(ti4.Read(fd))) {
          if (UnitAssert(ti5.Read(fd))) {
            UnitAssert(ti4 == ti3);
            UnitAssert(ti4.Contains(yesterday));
            UnitAssert(ti5 == ti);
            UnitAssert(! ti5.Contains(yesterday));
          }
          close(fd);
        }
      }
    }
    remove(TEST_FILE);
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIO()
{
  TestStreamIO();
  TestFileIO();
  TestDescriptorIO();
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestIO();

  int  rc = 1;
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    rc = 0;
  }
  
  return rc;
}
