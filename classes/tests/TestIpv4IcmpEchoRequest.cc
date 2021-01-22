//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpEchoRequest.cc 8389 $
// @(#) $Id: TestIpv4IcmpEchoRequest.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
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
//!  \file TestIpv4IcmpEchoRequest.cc
//!  \brief Unit tests for Dwm::Ipv4IcmpEchoRequest class
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <fcntl.h>
}

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4IcmpEchoRequest.hh"
#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4UdpHeader.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpEchoRequest.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO(const Ipv4IcmpEchoRequest & echoRequest)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpEchoRequest." << getpid();
  
  ofstream  os(filename.str().c_str());
  assert(os);
  UnitAssert(echoRequest.Write(os));
  os.close();
  
  ifstream  is(filename.str().c_str());
  assert(is);
  Ipv4IcmpEchoRequest  echoRequest2;
  UnitAssert(echoRequest2.Read(is, echoRequest.StreamedLength()));
  is.close();
  UnitAssert(echoRequest2 == echoRequest);
  
  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(const Ipv4IcmpEchoRequest & echoRequest)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpEchoRequest." << getpid();
  
  int  fd = open(filename.str().c_str(), O_WRONLY|O_CREAT, 0644);
  assert(fd >= 0);
  UnitAssert(echoRequest.Write(fd));
  close(fd);
  
  fd = open(filename.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  Ipv4IcmpEchoRequest  echoRequest2;
  UnitAssert(echoRequest2.Read(fd, echoRequest.StreamedLength()));
  close(fd);
  UnitAssert(echoRequest2 == echoRequest);
  
  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(const Ipv4IcmpEchoRequest & echoRequest)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpEchoRequest." << getpid();
  
  FILE  *f = fopen(filename.str().c_str(), "wb");
  assert(f);
  UnitAssert(echoRequest.Write(f));
  fclose(f);
  
  f = fopen(filename.str().c_str(), "r");
  assert(f);
  Ipv4IcmpEchoRequest  echoRequest2;
  UnitAssert(echoRequest2.Read(f, echoRequest.StreamedLength()));
  fclose(f);
  UnitAssert(echoRequest2 == echoRequest);
  
  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSetData()
{
  Ipv4IcmpEchoRequest  echoRequest;
  
  echoRequest.Identifier(getpid() & 0xFFFF);
  echoRequest.SequenceNumber(0xABAB);

  Ipv4PacketHeader  ipHeader;
  ipHeader.HeaderLength(20);
  ipHeader.TypeOfService(0);
  ipHeader.TotalLength(28);
  ipHeader.Id(getpid() & 0xFFFF);
  ipHeader.TimeToLive(64);
  ipHeader.Protocol(17);
  ipHeader.SourceAddress(Ipv4Address("192.168.168.2"));
  ipHeader.DestinationAddress(Ipv4Address("192.168.168.1"));
  ipHeader.SetChecksum();

  Ipv4UdpHeader  udpHeader;
  udpHeader.SourcePort(60111);
  udpHeader.DestinationPort(80);
  udpHeader.UdpLength(70);
  udpHeader.SetUdpChecksum(ipHeader, "");

  UnitAssert(echoRequest.SetData(udpHeader));

  Ipv4UdpHeader  udpHeader2;
  UnitAssert(echoRequest.GetData(udpHeader2));
  UnitAssert(udpHeader2 == udpHeader);

  UnitAssert(echoRequest.SetData(ipHeader));
  Ipv4PacketHeader  ipHeader2;
  UnitAssert(echoRequest.GetData(ipHeader2));
  UnitAssert(ipHeader2 == ipHeader);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRealloc()
{
  Ipv4IcmpEchoRequest  echoRequest;
  
  echoRequest.Identifier(getpid() & 0xFFFF);
  echoRequest.SequenceNumber(0xABAB);
  echoRequest.Data("test");
  
  UnitAssert(echoRequest.Type() == 8);
  UnitAssert(echoRequest.Code() == 0);
  UnitAssert(echoRequest.Identifier() == (getpid() & 0xFFFF));
  UnitAssert(echoRequest.SequenceNumber() == 0xABAB);
  UnitAssert(echoRequest.Data() == "test");
  
  echoRequest.Identifier(getpid() & 0xAAAA);
  echoRequest.SequenceNumber(0xFFFF);
  echoRequest.Data("test number 2");

  UnitAssert(echoRequest.Type() == 8);
  UnitAssert(echoRequest.Code() == 0);
  UnitAssert(echoRequest.Identifier() == (getpid() & 0xAAAA));
  UnitAssert(echoRequest.SequenceNumber() == 0xFFFF);
  UnitAssert(echoRequest.Data() == "test number 2");

  echoRequest.Identifier(getpid() & 0xBBBB);
  echoRequest.SequenceNumber(1);
  echoRequest.Data("test 3");

  UnitAssert(echoRequest.Type() == 8);
  UnitAssert(echoRequest.Code() == 0);
  UnitAssert(echoRequest.Identifier() == (getpid() & 0xBBBB));
  UnitAssert(echoRequest.SequenceNumber() == 1);
  UnitAssert(echoRequest.Data() == "test 3");
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Ipv4IcmpEchoRequest  echoRequest(getpid() & 0xFFFF, 0xAAAA,
                                   "test echo request");
  UnitAssert(echoRequest.Type() == 8);
  UnitAssert(echoRequest.Code() == 0);
  UnitAssert(echoRequest.Identifier() == (getpid() & 0xFFFF));
  UnitAssert(echoRequest.SequenceNumber() == 0xAAAA);
  UnitAssert(echoRequest.Data() == "test echo request");

  TestStreamIO(echoRequest);
  TestDescriptorIO(echoRequest);
  TestFileIO(echoRequest);
  TestSetData();
  TestRealloc();
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
