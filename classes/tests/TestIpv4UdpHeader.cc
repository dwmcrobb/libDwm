//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4UdpHeader.cc 8389 $
// @(#) $Id: TestIpv4UdpHeader.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestUdpHeader.cc
//!  \brief Unit tests for Dwm::UdpHeader
//---------------------------------------------------------------------------

#include <cstdlib>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4UdpHeader.hh"
#include "DwmPcap.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4UdpHeader.cc 8389 $");

using Dwm::Ipv4Address;
using Dwm::Ipv4PacketHeader;
using Dwm::Ipv4UdpHeader;
using Dwm::Pcap;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestChecksum(Ipv4PacketHeader & ipHeader,
                         Ipv4UdpHeader & udpHeader, 
                         const std::string & payload)
{
  Ipv4UdpHeader udpHeader2 = udpHeader;
  
  uint16_t  cksum = udpHeader.UdpChecksum();
  udpHeader.SetUdpChecksum(ipHeader, payload);
  UnitAssert(udpHeader.UdpChecksum() == cksum);
  UnitAssert(udpHeader2 == udpHeader);
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
  
  const uint8_t  *endptr = pkt + pchdr->caplen;
  const uint8_t  *ptr = pkt + pcap->DataLinkOffset();
  Ipv4PacketHeader  ipHeader((struct ip *)ptr);
  if (ipHeader.Protocol() != 17)
    return;
  ptr += ipHeader.HeaderLength();
  Ipv4UdpHeader  udpHeader((struct udphdr *)ptr);
  ptr += sizeof(struct udphdr);
  if (endptr > ptr) {
    std::string  payload((const char *)ptr, endptr - ptr);
    TestChecksum(ipHeader, udpHeader, payload);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Pcap  pcap;
  pcap.OpenOffline("./udp.pcap");
  pcap.SetFilter("udp", true);
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
