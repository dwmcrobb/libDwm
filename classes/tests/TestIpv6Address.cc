//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv6Address.cc 8389 $
// @(#) $Id: TestIpv6Address.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestIpv6Address.cc
//!  \brief Unit tests for Dwm::Ipv6Address
//---------------------------------------------------------------------------

extern "C" {
  #include <assert.h>
  #include <fcntl.h>
  #include <unistd.h>
}

static const char storeFile[] = "/tmp/Ipv6Address.store";

#include <fstream>

#include "DwmIpv6Prefix.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv6Address.cc 8389 $");

using namespace std;
using namespace Dwm;

int main(int argc, char *argv[])
{
  Ipv6Address  addr1("4444:8002:5:2::2");
  Ipv6Address  addr2("4444:8001:9500:172::1");

  //  test less-than and greater-than operators
  UnitAssert(addr1 > addr2);
  UnitAssert(addr2 < addr1);

  Ipv6Address  addr3, addr4;
  
  //  test iostream read/write
  ofstream  os(storeFile);
  addr1.Write(os);
  addr2.Write(os);
  os.close();
  ifstream  is(storeFile);
  addr3.Read(is);
  addr4.Read(is);
  is.close();
  unlink(storeFile);
  UnitAssert(addr3 == addr1);
  UnitAssert(addr4 == addr2);

  //  test file desriptor read/write
  int  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  UnitAssert(fd >= 0);
  addr1.Write(fd);
  addr2.Write(fd);
  close(fd);
  fd = open(storeFile, O_RDONLY);
  UnitAssert(fd >= 0);
  addr3.Read(fd);
  addr4.Read(fd);
  close(fd);
  unlink(storeFile);
  UnitAssert(addr3 == addr1);
  UnitAssert(addr4 == addr2);

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
