//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv6Prefix.cc 11305 $
// @(#) $Id: TestIpv6Prefix.cc 11305 2020-11-25 21:27:11Z dwm $
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
//!  \file TestIpv6Prefix.cc
//!  \brief Unit tests for Dwm::Ipv6Prefix
//---------------------------------------------------------------------------

extern "C" {
  #include <assert.h>
  #include <fcntl.h>
  #include <unistd.h>
}

static const char storeFile[] = "/tmp/Ipv6Prefix.store";

#include <fstream>
#include <tuple>
#include <vector>

#include "DwmIpv6Prefix.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv6Prefix.cc 11305 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestNetmask()
{
  Ipv6Prefix  prefix1("4444:8002:5:2::2/64");
  Ipv6Prefix  prefix2("4444:8001:5:2::2/30");

  UnitAssert(prefix1.MaskLength() == 64);
  UnitAssert(prefix2.MaskLength() == 30);

  Ipv6Address  addr1("ffff:ffff:ffff:ffff::");
  UnitAssert(prefix1.Netmask() == addr1);

  Ipv6Address  addr2("ffff:fffc::");
  UnitAssert(prefix2.Netmask() == addr2);

  Ipv6Address  host("4444:8001:5:2::2");
  UnitAssert(prefix2.Contains(host));
  UnitAssert(! prefix1.Contains(host));
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestOperators()
{
  Ipv6Prefix  prefix1("4444:8002:5:2::2/64");
  Ipv6Prefix  prefix2("4444:8001:5:2::2/30");
  Ipv6Prefix  prefix3 = prefix1;
  
  UnitAssert(prefix1 > prefix2);
  UnitAssert(prefix2 < prefix1);
  UnitAssert(prefix1 == prefix3);

  Ipv6Address  host("4444:8001:5:2::2");
  Ipv6Address  netmask("FFFF:FFFF::");
  Ipv6Address  host2 = host & netmask;
  
  UnitAssert(host2 == Ipv6Address("4444:8001::"));
  
  Ipv6Address host3 = string("4444:8001:5:2::2");
  UnitAssert(host3 == host);
  
  host &= netmask;
  UnitAssert(host == Ipv6Address("4444:8001::"));

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIO()
{
  Ipv6Prefix  prefix1("4444:8002:5:2::2/64");
  Ipv6Prefix  prefix2("4444:8001:5:2::2/30");

  Ipv6Prefix  prefix3, prefix4;
  
  //  test iostream read/write
  ofstream  os(storeFile);
  prefix1.Write(os);
  prefix2.Write(os);
  os.close();
  ifstream  is(storeFile);
  prefix3.Read(is);
  prefix4.Read(is);
  is.close();
  unlink(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);

  //  test file desriptor read/write
  int  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  UnitAssert(fd >= 0);
  prefix1.Write(fd);
  prefix2.Write(fd);
  close(fd);
  fd = open(storeFile, O_RDONLY);
  UnitAssert(fd >= 0);
  prefix3.Read(fd);
  prefix4.Read(fd);
  close(fd);
  unlink(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);

  //  test write to ostream, read from descriptor
  os.open(storeFile);
  prefix1.Write(os);
  prefix2.Write(os);
  os.close();
  fd = open(storeFile, O_RDONLY);
  UnitAssert(fd >= 0);
  prefix3.Read(fd);
  prefix4.Read(fd);
  close(fd);
  unlink(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);
  
  //  test write to descriptor, read from ostream
  fd = open(storeFile, O_WRONLY|O_CREAT, 0666);
  UnitAssert(fd >= 0);
  prefix1.Write(fd);
  prefix2.Write(fd);
  close(fd);
  is.open(storeFile);
  prefix3.Read(is);
  prefix4.Read(is);
  is.close();
  unlink(storeFile);
  UnitAssert(prefix3 == prefix1);
  UnitAssert(prefix4 == prefix2);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStringConstructor()
{
  const vector<tuple<string,string,uint8_t>>  goodPrefixes = {
    { "4444:8002:5:2::2/64",  "ffff:ffff:ffff:ffff::", 64 },
    { "4444:8001:5:2::2/30",  "ffff:fffc::", 30 },
    { "4444:8001:5:2::3",     "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128 },
    { "4444:8001:5:2::4/128", "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128 },
    { "4444:8001:5:2::5/0",   "0::", 0 }
  };

  for (const auto & t : goodPrefixes) {
    Ipv6Prefix  pfx(get<0>(t));
    UnitAssert(pfx.MaskLength() == get<2>(t));
    UnitAssert(pfx.Netmask() == Ipv6Address(get<1>(t)));
  }

  const vector<tuple<string,in6_addr,string,uint8_t>>  badPrefixes = {
    { "4444:8002:5:2::2/129", in6addr_any,
      "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128 },
    { "4444:8001:5:2::2/a",   in6addr_any,
      "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128 },
    { "4444:80G1:5:2::3",     in6addr_any,
      "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128 },
    { "4444:8001:5:2::Q/128", in6addr_any,
      "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128 }
  };
  for (const auto & t : badPrefixes) {
    Ipv6Prefix  pfx(get<0>(t));
    UnitAssert(pfx.MaskLength() == get<3>(t));
    UnitAssert(pfx.Netmask() == Ipv6Address(get<2>(t)));
    UnitAssert(pfx.Network() == Ipv6Address(in6addr_any));
  }
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestNetmask();
  TestOperators();
  TestIO();
  TestStringConstructor();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
