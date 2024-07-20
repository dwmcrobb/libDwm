//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4Address.cc 8939 $
// @(#) $Id: TestIpv4Address.cc 8939 2017-03-31 04:02:30Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file TestIpv4Address.cc
//!  \brief Unit tests for Dwm::Ipv4Address class
//---------------------------------------------------------------------------

extern "C" {
  #include <assert.h>
  #include <fcntl.h>
  #include <unistd.h>
}

static const char storeFile[] = "/tmp/Ipv4Address.store";

#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "DwmIpv4Address.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIncrement()
{
  Ipv4Address  a1("255.255.255.254");
  Ipv4Address  a2 = a1++;
  UnitAssert(a2 == Ipv4Address("255.255.255.254"));
  UnitAssert(a1 == Ipv4Address("255.255.255.255"));
  a1++;
  UnitAssert(a1 == Ipv4Address("255.255.255.255"));
  ++a2;
  UnitAssert(a2 == Ipv4Address("255.255.255.255"));
  ++a2;
  UnitAssert(a2 == Ipv4Address("255.255.255.255"));
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestDecrement()
{
  Ipv4Address  a1("0.0.0.1");
  Ipv4Address  a2 = a1--;
  UnitAssert(a2 == Ipv4Address("0.0.0.1"));
  UnitAssert(a1 == Ipv4Address("0.0.0.0"));
  --a1;
  UnitAssert(a1 == Ipv4Address("0.0.0.0"));
  --a2;
  UnitAssert(a2 == Ipv4Address("0.0.0.0"));
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestMinusEqualPlusEqual()
{
  Ipv4Address  a1("0.0.0.255");
  a1 -= 254;
  UnitAssert(a1 == Ipv4Address("0.0.0.1"));
  a1 -= 2;
  UnitAssert(a1 == Ipv4Address("0.0.0.0"));
  a1 += 0xFFFFFF00;
  UnitAssert(a1 == Ipv4Address("255.255.255.0"));
  a1 += 255;
  UnitAssert(a1 == Ipv4Address("255.255.255.255"));
  a1 += 1;
  UnitAssert(a1 == Ipv4Address("255.255.255.255"));
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestOperators()
{
  TestIncrement();
  TestDecrement();
  TestMinusEqualPlusEqual();
  
  Ipv4Address  a1("0.0.0.1");
  Ipv4Address  a2("0.0.0.0");
  UnitAssert(a1 > a2);
  UnitAssert(a1 >= a2);
  UnitAssert(a2 < a1);
  UnitAssert(a2 <= a1);
  UnitAssert(a1 != a2);
  UnitAssert(! (a1 == a2));

  Ipv4Address  a3("192.168.168.201");
  Ipv4Address  a4("192.168.168.55");
  UnitAssert(a3 > a4);
  UnitAssert(a3 >= a4);
  UnitAssert(a4 < a3);
  UnitAssert(a4 <= a3);
  UnitAssert(a3 != a4);
  UnitAssert(! (a3 == a4));
  UnitAssert(a3 == Ipv4Address("192.168.168.201"));
  UnitAssert(a4 == Ipv4Address("192.168.168.55"));

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIOStreamConcept()
{
  Ipv4Address  addr1("192.168.168.1");
  Ipv4Address  addr2("128.16.5.7");
  Ipv4Address  addr3, addr4;

  ofstream  os(storeFile);
  if (UnitAssert(os)) {
    if (UnitAssert(StreamIO::Write(os, addr1))) {
      UnitAssert(StreamIO::Write(os, addr2));
    }
    os.close();
    ifstream  is(storeFile);
    if (UnitAssert(is)) {
      if (UnitAssert(StreamIO::Read(is, addr3))) {
        UnitAssert(StreamIO::Read(is, addr4));
        UnitAssert(addr1 == addr3);
        UnitAssert(addr2 == addr4);
      }
      is.close();
    }
    std::remove(storeFile);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestFileIOConcept()
{
  Ipv4Address  addr1("192.168.168.1");
  Ipv4Address  addr2("128.16.5.7");
  Ipv4Address  addr3, addr4;

  FILE  *f = fopen(storeFile, "wb");
  if (UnitAssert(f)) {
    if (UnitAssert(FileIO::Write(f, addr1))) {
      UnitAssert(FileIO::Write(f, addr2));
    }
    fclose(f);
    f = fopen(storeFile, "rb");
    if (UnitAssert(f)) {
      if (UnitAssert(FileIO::Read(f, addr3))) {
        if (UnitAssert(FileIO::Read(f, addr4))) {
          UnitAssert(addr1 == addr3);
          UnitAssert(addr2 == addr4);
        }
      }
      fclose(f);
    }
    std::remove(storeFile);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIO()
{
  Ipv4Address  addr1("192.168.168.1");
  Ipv4Address  addr2("128.16.5.7");

  Ipv4Address  addr3, addr4;
  
  //  test iostream read/write
  ofstream  os(storeFile);
  addr1.Write(os);
  addr2.Write(os);
  os.close();
  ifstream  is(storeFile);
  addr3.Read(is);
  addr4.Read(is);
  is.close();
  std::remove(storeFile);
  UnitAssert(addr3 == addr1);
  UnitAssert(addr4 == addr2);

  //  test file desriptor read/write
  int  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  assert(fd >= 0);
  addr1.Write(fd);
  addr2.Write(fd);
  close(fd);
  fd = open(storeFile, O_RDONLY);
  assert(fd >= 0);
  close(fd);
  std::remove(storeFile);
  UnitAssert(addr3 == addr1);
  UnitAssert(addr4 == addr2);

  //  test write to ostream, read from descriptor
  os.open(storeFile);
  addr1.Write(os);
  addr2.Write(os);
  os.close();
  fd = open(storeFile, O_RDONLY);
  assert(fd >= 0);
  addr3.Read(fd);
  addr4.Read(fd);
  close(fd);
  std::remove(storeFile);
  UnitAssert(addr3 == addr1);
  UnitAssert(addr4 == addr2);
  
  //  test write to descriptor, read from ostream
  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  assert(fd >= 0);
  addr1.Write(fd);
  addr2.Write(fd);
  close(fd);
  is.open(storeFile);
  addr3.Read(is);
  addr4.Read(is);
  is.close();
  std::remove(storeFile);
  UnitAssert(addr3 == addr1);
  UnitAssert(addr4 == addr2);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestOperators();
  TestIO();
  TestIOStreamConcept();
  TestFileIOConcept();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);
}
