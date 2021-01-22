//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4TcpPacket.cc 9018 $
// @(#) $Id: TestIpv4TcpPacket.cc 9018 2017-04-11 20:05:43Z dwm $
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
//!  \file TestTcpHeader.cc
//!  \brief Unit tests for Dwm::Ipv4TcpPacket
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <fcntl.h>
}

#include <fstream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4TcpPacket.hh"
#include "DwmPcap.hh"
#include "DwmSocket.hh"
#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4TcpPacket.cc 9018 $");

using namespace std;
using namespace Dwm;

static Socket  g_socket;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRawSend(Ipv4TcpPacket & tcpPacket)
{
  if (g_socket) {
    tcpPacket.IpHeader().SourceAddress(Ipv4Address("10.10.10.10"));
    tcpPacket.IpHeader().DestinationAddress(Ipv4Address("127.0.0.1"));
    tcpPacket.TcpHeader().SetChecksum(tcpPacket.IpHeader(), 
                                      tcpPacket.Payload());
    UnitAssert(tcpPacket.SendTo(g_socket));
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestChecksum(Ipv4TcpPacket & tcpPacket)
{
  uint16_t  cksum = tcpPacket.TcpHeader().Checksum();
  tcpPacket.TcpHeader().SetChecksum(tcpPacket.IpHeader(), 
                                    tcpPacket.Payload());
  UnitAssert(tcpPacket.TcpHeader().Checksum() == cksum);
  Ipv4TcpHeader  tcpHeader2 = tcpPacket.TcpHeader();
  UnitAssert(tcpHeader2 == tcpPacket.TcpHeader());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStreamIO(const Ipv4TcpPacket & tcpPacket)
{
  string  filename("TestIpv4TcpPacket.tmp");
  ofstream  os(filename.c_str());
  assert(os);
  UnitAssert(tcpPacket.Write(os));
  os.close();

  Ipv4TcpPacket  tcpPacket2;
  ifstream  is(filename.c_str());
  assert(is);
  UnitAssert(tcpPacket2.Read(is));
  is.close();
  unlink(filename.c_str());

  UnitAssert(tcpPacket.TcpHeader() == tcpPacket2.TcpHeader());
  UnitAssert(tcpPacket.Payload() == tcpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDescriptorIO(const Ipv4TcpPacket & tcpPacket)
{
  string  filename("TestIpv4TcpPacket.tmp");
  int  fd = open(filename.c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0644);
  assert(fd >= 0);
  UnitAssert(tcpPacket.Write(fd) > 0);
  close(fd);

  Ipv4TcpPacket  tcpPacket2;
  fd = open(filename.c_str(), O_RDONLY);
  assert(fd >= 0);
  UnitAssert(tcpPacket2.Read(fd) > 0);
  close(fd);
  unlink(filename.c_str());

  UnitAssert(tcpPacket.TcpHeader() == tcpPacket2.TcpHeader());
  UnitAssert(tcpPacket.Payload() == tcpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestFileIO(const Ipv4TcpPacket & tcpPacket)
{
  string  filename("TestIpv4TcpPacket.tmp");
  FILE  *f = fopen(filename.c_str(), "wb");
  assert(f);
  UnitAssert(tcpPacket.Write(f));
  fclose(f);
  f = fopen(filename.c_str(), "rb");
  assert(f);
  Ipv4TcpPacket  tcpPacket2;
  UnitAssert(tcpPacket2.Read(f));
  fclose(f);
  unlink(filename.c_str());
  UnitAssert(tcpPacket.TcpHeader() == tcpPacket2.TcpHeader());
  UnitAssert(tcpPacket.Payload() == tcpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGZIO(const Ipv4TcpPacket & tcpPacket)
{
  string  filename("TestIpv4TcpPacket.tmp.gz");
  gzFile  gzf = gzopen(filename.c_str(), "w");
  assert(gzf);
  UnitAssert(tcpPacket.Write(gzf));
  gzclose(gzf);
  gzf = gzopen(filename.c_str(), "r");
  assert(gzf);
  Ipv4TcpPacket  tcpPacket2;
  UnitAssert(tcpPacket2.Read(gzf));
  gzclose(gzf);
  unlink(filename.c_str());
  UnitAssert(tcpPacket.TcpHeader() == tcpPacket2.TcpHeader());
  UnitAssert(tcpPacket.Payload() == tcpPacket2.Payload());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestBZ2IO(const Ipv4TcpPacket & tcpPacket)
{
  string  filename("TestIpv4TcpPacket.tmp.bz2");
  BZFILE  *bzf = BZ2_bzopen(filename.c_str(), "w");
  assert(bzf);
  UnitAssert(tcpPacket.BZWrite(bzf));
  BZ2_bzclose(bzf);
  bzf = BZ2_bzopen(filename.c_str(), "r");
  assert(bzf);
  Ipv4TcpPacket  tcpPacket2;
  UnitAssert(tcpPacket2.BZRead(bzf));
  BZ2_bzclose(bzf);
  unlink(filename.c_str());
  UnitAssert(tcpPacket.TcpHeader() == tcpPacket2.TcpHeader());
  UnitAssert(tcpPacket.Payload() == tcpPacket2.Payload());
  return;
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO(const Ipv4TcpPacket & tcpPacket)
{
  TestStreamIO(tcpPacket);
  TestDescriptorIO(tcpPacket);
  TestFileIO(tcpPacket);
  TestGZIO(tcpPacket);
  TestBZ2IO(tcpPacket);
  
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
  
  uint8_t  *endptr = (uint8_t *)(pkt + pchdr->caplen);
  uint8_t  *ptr = (uint8_t *)(pkt + pcap->DataLinkOffset());
  Ipv4TcpPacket   tcpPacket(ptr);

  if (tcpPacket.IpHeader().Protocol() != 6)
    return;
  if ((tcpPacket.IpHeader().FragmentOffset() & IP_DF) == 0)
    return;
  if ((endptr - ptr) < tcpPacket.IpHeader().TotalLength())
    return;
  TestChecksum(tcpPacket);
  TestIO(tcpPacket);
  TestRawSend(tcpPacket);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (geteuid() == 0) {
    SysLogger::Open("TestIpv4TcpPacket", LOG_PERROR, LOG_USER);
    SysLogger::ShowPriorities(true);
    SysLogger::ShowFileLocation(true);
    SysLogger::MinimumPriority(LOG_INFO);
    
    UnitAssert(g_socket.Open(PF_INET, SOCK_RAW, IPPROTO_RAW));
    int  hincl = 1;
    UnitAssert(g_socket.Setsockopt(IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl)));
    
    Pcap  pcap;
    pcap.OpenOffline("./tcp.pcap");
    pcap.SetFilter("tcp", true);
    while (pcap.Loop(-1, pcapcb, (uint8_t *)&pcap))
      ;
    pcap.Close();
    g_socket.Close();
    
    if (Dwm::Assertions::Total().Failed() > 0) {
      Dwm::Assertions::Print(std::cerr, true);
      exit(1);
    }
    else
      std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
    
    exit(0);
  }
  else {
    std::cout << "must run as root\n";
    exit(0);
  }
}
