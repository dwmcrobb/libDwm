//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpEchoReply.cc 8389 $
// @(#) $Id: TestIpv4IcmpEchoReply.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestIpv4IcmpEchoReply.cc
//!  \brief Unit tests for Dwm::Ipv4IcmpEchoReply class
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
#include "DwmIpv4IcmpEchoReply.hh"
#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4UdpHeader.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpEchoReply.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO(const Ipv4IcmpEchoReply & echoReply)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpEchoReply." << getpid();
  
  ofstream  os(filename.str().c_str());
  assert(os);
  UnitAssert(echoReply.Write(os));
  os.close();
  
  ifstream  is(filename.str().c_str());
  assert(is);
  Ipv4IcmpEchoReply  echoReply2;
  UnitAssert(echoReply2.Read(is, echoReply.StreamedLength()));
  is.close();
  UnitAssert(echoReply2 == echoReply);
  
  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(const Ipv4IcmpEchoReply & echoReply)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpEchoReply." << getpid();
  
  int  fd = open(filename.str().c_str(), O_WRONLY|O_CREAT, 0644);
  assert(fd >= 0);
  UnitAssert(echoReply.Write(fd));
  close(fd);
  
  fd = open(filename.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  Ipv4IcmpEchoReply  echoReply2;
  UnitAssert(echoReply2.Read(fd, echoReply.StreamedLength()));
  close(fd);
  UnitAssert(echoReply2 == echoReply);
  
  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(const Ipv4IcmpEchoReply & echoReply)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpEchoReply." << getpid();
  
  FILE *f = fopen(filename.str().c_str(), "wb");
  assert(f);
  UnitAssert(echoReply.Write(f));
  fclose(f);
  
  f = fopen(filename.str().c_str(), "r");
  assert(f);
  Ipv4IcmpEchoReply  echoReply2;
  UnitAssert(echoReply2.Read(f, echoReply.StreamedLength()));
  fclose(f);
  UnitAssert(echoReply2 == echoReply);
  
  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestSetData()
{
  Ipv4IcmpEchoReply  echoReply;
  
  echoReply.Identifier(getpid() & 0xFFFF);
  echoReply.SequenceNumber(0xABAB);

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

  UnitAssert(echoReply.SetData(udpHeader));

  Ipv4UdpHeader  udpHeader2;
  UnitAssert(echoReply.GetData(udpHeader2));
  UnitAssert(udpHeader2 == udpHeader);

  UnitAssert(echoReply.SetData(ipHeader));
  Ipv4PacketHeader  ipHeader2;
  UnitAssert(echoReply.GetData(ipHeader2));
  UnitAssert(ipHeader2 == ipHeader);

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRealloc()
{
  Ipv4IcmpEchoReply  echoReply;
  
  echoReply.Identifier(getpid() & 0xFFFF);
  echoReply.SequenceNumber(0xABAB);
  echoReply.Data("test");
  
  UnitAssert(echoReply.Type() == 0);
  UnitAssert(echoReply.Code() == 0);
  UnitAssert(echoReply.Identifier() == (getpid() & 0xFFFF));
  UnitAssert(echoReply.SequenceNumber() == 0xABAB);
  UnitAssert(echoReply.Data() == "test");
  
  echoReply.Identifier(getpid() & 0xAAAA);
  echoReply.SequenceNumber(0xFFFF);
  echoReply.Data("test 2");

  UnitAssert(echoReply.Type() == 0);
  UnitAssert(echoReply.Code() == 0);
  UnitAssert(echoReply.Identifier() == (getpid() & 0xAAAA));
  UnitAssert(echoReply.SequenceNumber() == 0xFFFF);
  UnitAssert(echoReply.Data() == "test 2");

  echoReply.Identifier(getpid() & 0xBBBB);
  echoReply.SequenceNumber(1);
  echoReply.Data("test3");

  UnitAssert(echoReply.Type() == 0);
  UnitAssert(echoReply.Code() == 0);
  UnitAssert(echoReply.Identifier() == (getpid() & 0xBBBB));
  UnitAssert(echoReply.SequenceNumber() == 1);
  UnitAssert(echoReply.Data() == "test3");
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Ipv4IcmpEchoReply  echoReply(getpid() & 0xFFFF, 0xAAAA, "test echo reply");
  UnitAssert(echoReply.Type() == 0);
  UnitAssert(echoReply.Code() == 0);
  UnitAssert(echoReply.Identifier() == (getpid() & 0xFFFF));
  UnitAssert(echoReply.SequenceNumber() == 0xAAAA);
  UnitAssert(echoReply.Data() == "test echo reply");

  TestStreamIO(echoReply);
  TestDescriptorIO(echoReply);
  TestFileIO(echoReply);
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
