//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4Prefix.cc 9284 $
// @(#) $Id: TestIpv4Prefix.cc 9284 2017-04-28 17:17:27Z dwm $
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
//!  \file TestIpv4Prefix.cc
//!  \brief Unit tests for Dwm::Ipv4Prefix class
//---------------------------------------------------------------------------

extern "C" {
  #include <assert.h>
  #include <fcntl.h>
  #include <unistd.h>
}

static const char storeFile[] = "/tmp/Ipv4Prefix.store";

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#include "DwmIpv4Prefix.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4Prefix.cc 9284 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestNetmask()
{
  Ipv4Prefix  prefix1("192.168.168/24");
  Ipv4Prefix  prefix2("128.16/16");
  Ipv4Prefix  prefix3("128.16.26/24");
  
  UnitAssert(prefix1.MaskLength() == 24);
  UnitAssert(prefix2.MaskLength() == 16);

  Ipv4Address  addr1("255.255.255.0");
  UnitAssert(prefix1.Netmask() == addr1);

  Ipv4Address  addr2("255.255.0.0");
  UnitAssert(prefix2.Netmask() == addr2);

  Ipv4Address  host("128.16.25.1");

  UnitAssert(prefix2.Contains(host));
  UnitAssert(! prefix1.Contains(host));
  UnitAssert(prefix2.Contains(prefix3));
  UnitAssert(! prefix1.Contains(prefix3));

  prefix1.Set(Ipv4Address("192.168.168.1"), Ipv4Address("255.255.255.255"));
  UnitAssert(prefix1.MaskLength() == 32);
  prefix1.Set(Ipv4Address("192.168.168.1"), Ipv4Address("255.255.0.0"));
  UnitAssert(prefix1.MaskLength() == 16);
  prefix1.Set(Ipv4Address("192.168.168.1"), Ipv4Address("254.0.0.0"));
  UnitAssert(prefix1.MaskLength() == 7);
  prefix1.Set(Ipv4Address("192.168.168.1"), Ipv4Address("0.0.0.0"));
  UnitAssert(prefix1.MaskLength() == 0);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestOperators()
{
  Ipv4Prefix  prefix1("192.168.168.0/24");
  Ipv4Prefix  prefix2("128.16.0.0/16");
  Ipv4Prefix  prefix3 = prefix1;
  
  UnitAssert(prefix1 > prefix2);
  UnitAssert(prefix2 < prefix1);
  UnitAssert(prefix1 == prefix3);

  Ipv4Address  host("192.168.168.1");
  Ipv4Address  netmask("255.255.255.0");
  Ipv4Address  host2 = host & netmask;
  
  UnitAssert(host2 == Ipv4Address("192.168.168.0"));
  
  Ipv4Address host3 = string("192.168.168.1");
  UnitAssert(host3 == host);
  
  host &= netmask;
  UnitAssert(host == Ipv4Address("192.168.168.0"));

  Ipv4Prefix  cpfx1("191.0.2.0/23");
  pair<bool,Ipv4Prefix>  combine = cpfx1.Combine(Ipv4Prefix("191.0.4.0/23"));
  UnitAssert(! combine.first);
  UnitAssert(combine.second == Ipv4Prefix("255.255.255.255/32"));
  combine = cpfx1.Combine(Ipv4Prefix("191.0.0.0/23"));
  UnitAssert(combine.first);
  UnitAssert(combine.second == Ipv4Prefix("191.0.0.0/22"));

  cpfx1 = Ipv4Prefix("191.0.254.0/24");
  combine = cpfx1.Combine(Ipv4Prefix("191.0.253.0/24"));
  UnitAssert(! combine.first);
  UnitAssert(combine.second == Ipv4Prefix("255.255.255.255/32"));
  combine = cpfx1.Combine(Ipv4Prefix("191.0.252.0/22"));
  UnitAssert(combine.first);
  UnitAssert(combine.second == Ipv4Prefix("191.0.252.0/22"));

  cpfx1 = Ipv4Prefix("25.24/16");
  combine = cpfx1.Combine(Ipv4Prefix("25.25.0/17"));
  UnitAssert(! combine.first);
  UnitAssert(combine.second == Ipv4Prefix("255.255.255.255/32"));
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestRanges()
{
  Ipv4Address  startAddr("192.172.226.0");
  Ipv4Address  endAddr("192.172.226.255");
  vector<Ipv4Prefix>  &&prefixes = Ipv4RangePrefixes(startAddr, endAddr);
  if (UnitAssert(prefixes.size() == 1)) {
    UnitAssert(prefixes.front()
               == Ipv4Prefix(Ipv4Address("192.172.226.0"), 24));
  }
  prefixes = Ipv4RangePrefixes(Ipv4Address("192.172.226.31"),
                               Ipv4Address("192.172.226.255"));
  if (UnitAssert(prefixes.size() == 4)) {
    UnitAssert(prefixes[0]
               == Ipv4Prefix(Ipv4Address("192.172.226.31"), 32));
    UnitAssert(prefixes[1]
               == Ipv4Prefix(Ipv4Address("192.172.226.32"), 27));
    UnitAssert(prefixes[2]
               == Ipv4Prefix(Ipv4Address("192.172.226.64"), 26));
    UnitAssert(prefixes[3]
               == Ipv4Prefix(Ipv4Address("192.172.226.128"), 25));
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIO()
{
  Ipv4Prefix  prefix1("192.168.168.0/24");
  Ipv4Prefix  prefix2("128.16.0.0/16");

  Ipv4Prefix  prefix3, prefix4;
  
  //  test iostream read/write
  ofstream  os(storeFile);
  prefix1.Write(os);
  prefix2.Write(os);
  os.close();
  ifstream  is(storeFile);
  prefix3.Read(is);
  prefix4.Read(is);
  is.close();
  std::remove(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);

  //  test file desriptor read/write
  int  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  assert(fd >= 0);
  prefix1.Write(fd);
  prefix2.Write(fd);
  close(fd);
  fd = open(storeFile, O_RDONLY);
  assert(fd >= 0);
  close(fd);
  std::remove(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);

  //  test write to ostream, read from descriptor
  os.open(storeFile);
  prefix1.Write(os);
  prefix2.Write(os);
  os.close();
  fd = open(storeFile, O_RDONLY);
  assert(fd >= 0);
  prefix3.Read(fd);
  prefix4.Read(fd);
  close(fd);
  std::remove(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);
  
  //  test write to descriptor, read from ostream
  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  assert(fd >= 0);
  prefix1.Write(fd);
  prefix2.Write(fd);
  close(fd);
  is.open(storeFile);
  prefix3.Read(is);
  prefix4.Read(is);
  is.close();
  std::remove(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestNetmask();
  TestOperators();
  TestRanges();
  TestIO();

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);
}
