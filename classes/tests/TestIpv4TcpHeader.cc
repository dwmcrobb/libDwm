//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4TcpHeader.cc 8389 $
// @(#) $Id: TestIpv4TcpHeader.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file TestTcpHeader.cc
//!  \brief Unit tests for Dwm::Ipv4TcpHeader
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
}

#include <cstdlib>
#include <fstream>  
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4TcpHeader.hh"
#include "DwmPcap.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4TcpHeader.cc 8389 $");

using namespace std;

using Dwm::Ipv4Address;
using Dwm::Ipv4PacketHeader;
using Dwm::Ipv4TcpHeader;
using Dwm::Pcap;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestChecksum(Ipv4PacketHeader & ipHeader,
                         Ipv4TcpHeader & tcpHeader, 
                         const std::string & payload)
{
  uint16_t  cksum = tcpHeader.Checksum();
  tcpHeader.SetChecksum(ipHeader, payload);
  UnitAssert(tcpHeader.Checksum() == cksum);
  Ipv4TcpHeader tcpHeader2 = tcpHeader;
  UnitAssert(tcpHeader2 == tcpHeader);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO(const Ipv4TcpHeader & tcpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4TcpHeader." << getpid();

  ofstream  os(filename.str().c_str());
  assert(os);
  UnitAssert(tcpHeader.Write(os));
  os.close();
  
  ifstream  is(filename.str().c_str());
  assert(is);
  Ipv4TcpHeader  tcpHeader2;
  UnitAssert(tcpHeader2.Read(is));
  is.close();
  UnitAssert(tcpHeader2 == tcpHeader);

  unlink(filename.str().c_str());

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(const Ipv4TcpHeader & tcpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4TcpHeader." << getpid();

  FILE *f = fopen(filename.str().c_str(), "w");
  assert(f);
  UnitAssert(tcpHeader.Write(f));
  fclose(f);
  
  f = fopen(filename.str().c_str(), "r");
  assert(f);
  Ipv4TcpHeader  tcpHeader2;
  UnitAssert(tcpHeader2.Read(f));
  fclose(f);
  UnitAssert(tcpHeader2 == tcpHeader);
  
  unlink(filename.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(const Ipv4TcpHeader & tcpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4TcpHeader." << getpid();

  int  fd = open(filename.str().c_str(), O_WRONLY|O_TRUNC|O_CREAT, 0644);
  assert(fd >= 0);
  UnitAssert(tcpHeader.Write(fd));
  close(fd);
  
  fd = open(filename.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  Ipv4TcpHeader  tcpHeader2;
  UnitAssert(tcpHeader2.Read(fd));
  close(fd);
  UnitAssert(tcpHeader2 == tcpHeader);
  
  unlink(filename.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO(const Ipv4TcpHeader & tcpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4TcpHeader." << getpid();
  
  gzFile  gzf = gzopen(filename.str().c_str(), "wb");
  assert(gzf);
  UnitAssert(tcpHeader.Write(gzf));
  gzclose(gzf);
  
  gzf = gzopen(filename.str().c_str(), "rb");
  assert(gzf);
  Ipv4TcpHeader  tcpHeader2;
  UnitAssert(tcpHeader2.Read(gzf));
  gzclose(gzf);
  UnitAssert(tcpHeader2 == tcpHeader);
  
  unlink(filename.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO(const Ipv4TcpHeader & tcpHeader)
{
  ostringstream  filename;
  filename << "TestIpv4TcpHeader." << getpid();
  
  BZFILE  *bzf = BZ2_bzopen(filename.str().c_str(),"wb");
  assert(bzf);
  UnitAssert(tcpHeader.BZWrite(bzf));
  BZ2_bzclose(bzf);
  
  bzf = BZ2_bzopen(filename.str().c_str(), "rb");
  assert(bzf);
  Ipv4TcpHeader  tcpHeader2;
  UnitAssert(tcpHeader2.BZRead(bzf));
  BZ2_bzclose(bzf);
  UnitAssert(tcpHeader2 == tcpHeader);
  
  unlink(filename.str().c_str());
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO(const Ipv4TcpHeader & tcpHeader)
{
  TestStreamIO(tcpHeader);
  TestFileIO(tcpHeader);
  TestDescriptorIO(tcpHeader);
  TestGZIO(tcpHeader);
  TestBZ2IO(tcpHeader);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void pcapcb(uint8_t *user, const struct pcap_pkthdr *pchdr, 
                   const uint8_t *pkt)
{
  Pcap  *pcap = (Pcap *)user;
  if (pchdr->caplen < 
      (sizeof(struct ip) + sizeof(struct tcphdr) + pcap->DataLinkOffset()))
    return;
  
  const uint8_t  *endptr = pkt + pchdr->caplen;
  const uint8_t  *ptr = pkt + pcap->DataLinkOffset();
  Ipv4PacketHeader  ipHeader((struct ip *)ptr);
  if (ipHeader.Protocol() != 6)
    return;
  if (endptr > ptr + ipHeader.TotalLength())
    endptr = ptr + ipHeader.TotalLength();
  
  ptr += ipHeader.HeaderLength();
  Ipv4TcpHeader  tcpHeader((struct tcphdr *)ptr);
  ptr += tcpHeader.DataOffset();
  
  TestIO(tcpHeader);
  
  if (endptr > ptr) {
    std::string  payload((const char *)ptr, endptr - ptr);
    TestChecksum(ipHeader, tcpHeader, payload);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Pcap  pcap;
  pcap.OpenOffline("./tcp.pcap");
  pcap.SetFilter("tcp", true);
  while (pcap.Loop(-1, pcapcb, (uint8_t *)&pcap))
    ;
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
