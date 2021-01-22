//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4UdpPacket.cc 9018 $
// @(#) $Id: TestIpv4UdpPacket.cc 9018 2017-04-11 20:05:43Z dwm $
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
//!  \file TestIpv4UdpPacket.cc
//!  \brief Unit tests for Dwm::Ipv4UdpPacket
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <fcntl.h>
#if defined(__FreeBSD__)
  #include <osreldate.h>
#endif
}

#include <fstream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4UdpPacket.hh"
#include "DwmPcap.hh"
#include "DwmSocket.hh"
#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4UdpPacket.cc 9018 $");

using namespace std;
using namespace Dwm;

static Socket  g_socket;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRawSend(Ipv4UdpPacket & udpPacket)
{
  if (g_socket) {
    udpPacket.IpHeader().SourceAddress(Ipv4Address("10.10.10.10"));
    udpPacket.IpHeader().DestinationAddress(Ipv4Address("127.0.0.1"));
    UnitAssert(udpPacket.SendTo(g_socket));
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestChecksum(Ipv4UdpPacket & udpPacket)
{
  Ipv4UdpHeader  udpHeader2 = udpPacket.UdpHeader();
  uint16_t  cksum = udpPacket.UdpHeader().UdpChecksum();
  udpPacket.UdpHeader().SetUdpChecksum(udpPacket.IpHeader(), 
                                       udpPacket.Payload());
  UnitAssert(udpPacket.UdpHeader().UdpChecksum() == cksum);
  UnitAssert(udpHeader2 == udpPacket.UdpHeader());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO(Ipv4UdpPacket & udpPacket)
{
  ofstream  os("./TestIpv4UdpPacket.tmp");
  assert(os);
  UnitAssert(udpPacket.Write(os));
  os.close();

  Ipv4UdpPacket  udpPacket2;
  ifstream  is("./TestIpv4UdpPacket.tmp");
  assert(is);
  UnitAssert(udpPacket2.Read(is));
  is.close();
  unlink("./TestIpv4UdpPacket.tmp");

  UnitAssert(udpPacket.UdpHeader() == udpPacket2.UdpHeader());
  UnitAssert(udpPacket.Payload() == udpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(Ipv4UdpPacket & udpPacket)
{
  int  fd = open("./TestIpv4UdpPacket.tmp", O_CREAT|O_TRUNC|O_WRONLY, 0644);
  assert(fd >= 0);
  UnitAssert(udpPacket.Write(fd));
  close(fd);

  Ipv4UdpPacket  udpPacket2;
  fd = open("./TestIpv4UdpPacket.tmp", O_RDONLY);
  assert(fd >= 0);
  UnitAssert(udpPacket2.Read(fd));
  close(fd);
  unlink("./TestIpv4UdpPacket.tmp");

  UnitAssert(udpPacket.UdpHeader() == udpPacket2.UdpHeader());
  UnitAssert(udpPacket.Payload() == udpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(Ipv4UdpPacket & udpPacket)
{
  FILE  *f = fopen("./TestIpv4UdpPacket.tmp", "wb");
  assert(f);
  UnitAssert(udpPacket.Write(f));
  fclose(f);

  Ipv4UdpPacket  udpPacket2;
  f = fopen("./TestIpv4UdpPacket.tmp", "rb");
  assert(f);
  UnitAssert(udpPacket2.Read(f));
  fclose(f);
  unlink("./TestIpv4UdpPacket.tmp");

  UnitAssert(udpPacket.UdpHeader() == udpPacket2.UdpHeader());
  UnitAssert(udpPacket.Payload() == udpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO(Ipv4UdpPacket & udpPacket)
{
  gzFile  gzf = gzopen("./TestIpv4UdpPacket.tmp.gz", "wb");
  assert(gzf);
  UnitAssert(udpPacket.Write(gzf));
  gzclose(gzf);

  Ipv4UdpPacket  udpPacket2;
  gzf = gzopen("./TestIpv4UdpPacket.tmp.gz", "rb");
  assert(gzf);
  UnitAssert(udpPacket2.Read(gzf));
  gzclose(gzf);
  unlink("./TestIpv4UdpPacket.tmp.gz");

  UnitAssert(udpPacket.UdpHeader() == udpPacket2.UdpHeader());
  UnitAssert(udpPacket.Payload() == udpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZIO(Ipv4UdpPacket & udpPacket)
{
  BZFILE *bzf = BZ2_bzopen("./TestIpv4UdpPacket.tmp.gz", "wb");
  assert(bzf);
  UnitAssert(udpPacket.BZWrite(bzf));
  BZ2_bzclose(bzf);

  Ipv4UdpPacket  udpPacket2;
  bzf = BZ2_bzopen("./TestIpv4UdpPacket.tmp.gz", "rb");
  assert(bzf);
  UnitAssert(udpPacket2.BZRead(bzf));
  BZ2_bzclose(bzf);
  unlink("./TestIpv4UdpPacket.tmp.gz");

  UnitAssert(udpPacket.UdpHeader() == udpPacket2.UdpHeader());
  UnitAssert(udpPacket.Payload() == udpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO(Ipv4UdpPacket & udpPacket)
{
  TestStreamIO(udpPacket);
  TestDescriptorIO(udpPacket);
  TestFileIO(udpPacket);
  TestGZIO(udpPacket);
  TestBZIO(udpPacket);
  
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
      (sizeof(struct ip) + sizeof(struct udphdr) + pcap->DataLinkOffset()))
    return;
  
  uint8_t  *endptr = (uint8_t *)(pkt + pchdr->caplen);
  uint8_t  *ptr = (uint8_t *)(pkt + pcap->DataLinkOffset());
  Ipv4UdpPacket   udpPacket(ptr);
  
  if (udpPacket.IpHeader().Protocol() != 17)
    return;
  if (endptr - ptr < udpPacket.IpHeader().TotalLength())
    return;
  TestChecksum(udpPacket);
  TestIO(udpPacket);
  TestRawSend(udpPacket);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (geteuid() == 0) {
    SysLogger::Open("TestUdpPacket", LOG_PERROR, LOG_USER);
    SysLogger::ShowPriorities(true);
    SysLogger::ShowFileLocation(true);
    SysLogger::MinimumPriority(LOG_INFO);
    
    UnitAssert(g_socket.Open(PF_INET, SOCK_RAW, IPPROTO_RAW));
    int  hincl = 1;
    g_socket.Setsockopt(IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl));
    
    Pcap  pcap;
    pcap.OpenOffline("./udp.pcap");
    pcap.SetFilter("udp", true);
    while (pcap.Loop(-1, pcapcb, (uint8_t *)&pcap))
      ;
    pcap.Close();
    g_socket.Close();
    
    if (Dwm::Assertions::Total().Failed() > 0) {
      Dwm::Assertions::Print(std::cerr, true);
      exit(1);
    }
    else {
      std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
    }
  }
  else {
    std::cout << "must run as root\n";
    exit(0);
  }
  
  exit(0);
}
