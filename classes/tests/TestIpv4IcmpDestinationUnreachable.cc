//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpDestinationUnreachable.cc 8389 $
// @(#) $Id: TestIpv4IcmpDestinationUnreachable.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestIpv4IcmpDestinationUnreachable.cc
//!  \brief Unit tests for Dwm::Ipv4IcmpDestinationUnreachable class
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <fcntl.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4IcmpDestinationUnreachable.hh"
#include "DwmIpv4UdpHeader.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpDestinationUnreachable.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO(const Ipv4IcmpDestinationUnreachable & unreach,
                         const Ipv4UdpHeader & udpHeader)
{
  stringstream  ss;
  UnitAssert(unreach.Write(ss));
  
  ss.seekg(0);
  Ipv4IcmpDestinationUnreachable  unreach2;
  UnitAssert(unreach2.Read(ss, unreach.StreamedLength()));
  
  UnitAssert(unreach2 == unreach);
  
  istringstream  is(unreach.Data());
  Ipv4UdpHeader  udpHeader2;
  UnitAssert(udpHeader2.Read(is));
  UnitAssert(udpHeader2 == udpHeader);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(const Ipv4IcmpDestinationUnreachable & unreach,
                             const Ipv4UdpHeader & udpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpDestinationUnreachable." << getpid();
  int  fd = open(filename.str().c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0644);
  assert(fd >= 0);
  UnitAssert(unreach.Write(fd) == unreach.StreamedLength());
  close(fd);
  
  fd = open(filename.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  Ipv4IcmpDestinationUnreachable  unreach2;
  UnitAssert(unreach2.Read(fd, unreach.StreamedLength()));
  close(fd);
  UnitAssert(unreach2 == unreach);

  istringstream  is(unreach.Data());
  Ipv4UdpHeader  udpHeader2;
  UnitAssert(udpHeader2.Read(is));
  UnitAssert(udpHeader2 == udpHeader);

  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(const Ipv4IcmpDestinationUnreachable & unreach,
                       const Ipv4UdpHeader & udpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpDestinationUnreachable." << getpid();
  FILE  *f = fopen(filename.str().c_str(), "wb");
  assert(f);
  UnitAssert(unreach.Write(f) > 0);
  fclose(f);
  
  f = fopen(filename.str().c_str(), "rb");
  assert(f);
  Ipv4IcmpDestinationUnreachable  unreach2;
  UnitAssert(unreach2.Read(f, unreach.StreamedLength()));
  fclose(f);
  UnitAssert(unreach2 == unreach);

  istringstream  is(unreach.Data());
  Ipv4UdpHeader  udpHeader2;
  UnitAssert(udpHeader2.Read(is));
  UnitAssert(udpHeader2 == udpHeader);

  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO(const Ipv4IcmpDestinationUnreachable & unreach,
                     const Ipv4UdpHeader & udpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpDestinationUnreachable." << getpid() << ".gz";
  gzFile gzf = gzopen(filename.str().c_str(), "w");
  assert(gzf);
  UnitAssert(unreach.Write(gzf) == unreach.StreamedLength());
  gzclose(gzf);
  
  gzf = gzopen(filename.str().c_str(), "r");
  assert(gzf);
  Ipv4IcmpDestinationUnreachable  unreach2;
  UnitAssert(unreach2.Read(gzf, unreach.StreamedLength()));
  gzclose(gzf);
  UnitAssert(unreach2 == unreach);

  istringstream  is(unreach.Data());
  Ipv4UdpHeader  udpHeader2;
  UnitAssert(udpHeader2.Read(is));
  UnitAssert(udpHeader2 == udpHeader);

  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO(const Ipv4IcmpDestinationUnreachable & unreach,
                      const Ipv4UdpHeader & udpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4IcmpDestinationUnreachable." << getpid() << ".bz2";
  BZFILE  *bzf = BZ2_bzopen(filename.str().c_str(), "w");
  assert(bzf);
  UnitAssert(unreach.BZWrite(bzf) == unreach.StreamedLength());
  BZ2_bzclose(bzf);
  
  bzf = BZ2_bzopen(filename.str().c_str(), "r");
  assert(bzf);
  Ipv4IcmpDestinationUnreachable  unreach2;
  UnitAssert(unreach2.BZRead(bzf, unreach.StreamedLength()));
  BZ2_bzclose(bzf);
  UnitAssert(unreach2 == unreach);

  istringstream  is(unreach.Data());
  Ipv4UdpHeader  udpHeader2;
  UnitAssert(udpHeader2.Read(is));
  UnitAssert(udpHeader2 == udpHeader);

  std::remove(filename.str().c_str());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRealloc(const Ipv4PacketHeader & ipHeader)
{
  Ipv4IcmpDestinationUnreachable  unreachable;
  unreachable.Type(3);
  unreachable.Code(Ipv4IcmpDestinationUnreachable::e_hostUnreachable);
  unreachable.NexthopMTU(1500);
  unreachable.IpHeader(ipHeader);
  
  UnitAssert(unreachable.Type() == 3);
  UnitAssert(unreachable.Code() ==
             Ipv4IcmpDestinationUnreachable::e_hostUnreachable);
  UnitAssert(unreachable.NexthopMTU() == 1500);
  UnitAssert(unreachable.IpHeader() == ipHeader);

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
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
  
  ostringstream  os;
  udpHeader.Write(os);
  
  Ipv4IcmpDestinationUnreachable
    unreachable(Ipv4IcmpDestinationUnreachable::e_hostUnreachable, 1500,
                ipHeader, os.str());
  UnitAssert(unreachable.Type() == 3);
  UnitAssert(unreachable.Code() == 
             Ipv4IcmpDestinationUnreachable::e_hostUnreachable);
  UnitAssert(unreachable.NexthopMTU() == 1500);
  UnitAssert(unreachable.IpHeader() == ipHeader);
  UnitAssert(unreachable.Data() == os.str());

  istringstream  is(os.str());
  Ipv4UdpHeader  udpHeader2;
  UnitAssert(udpHeader2.Read(is));
  UnitAssert(udpHeader2 == udpHeader);
  
  TestRealloc(ipHeader);
  
  TestStreamIO(unreachable, udpHeader);
  TestDescriptorIO(unreachable, udpHeader);
  TestFileIO(unreachable, udpHeader);
  TestGZIO(unreachable, udpHeader);
  TestBZ2IO(unreachable, udpHeader);

  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
