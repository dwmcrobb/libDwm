//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4PacketHeader.cc 8389 $
// @(#) $Id: TestIpv4PacketHeader.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestIpv4PacketHeader.cc
//!  \brief Unit tests for Dwm::Ipv4PacketHeader
//---------------------------------------------------------------------------

#include <iomanip>
#include <iostream>

#include "DwmSvnTag.hh"
#include "DwmIpv4PacketHeader.hh"
#include "DwmOptArgs.hh"
#include "DwmPcap.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4PacketHeader.cc 8389 $");

using Dwm::Ipv4Address;
using Dwm::Ipv4PacketHeader;
using Dwm::OptArgs;
using Dwm::Pcap;

static bool g_verbose = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestChecksum(Ipv4PacketHeader & ipHeader)
{
  uint16_t  cksum = ipHeader.Checksum();
  ipHeader.SetChecksum();
  UnitAssert(ipHeader.Checksum() == cksum);
  
  if (g_verbose)
    ipHeader.PrintXML(std::cout,"");
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void pcapcb(uint8_t *user, const struct pcap_pkthdr *pchdr, 
                   const uint8_t *pkt)
{
  Pcap  *pcap = (Pcap *)user;
  if (pchdr->caplen < (sizeof(struct ip) + pcap->DataLinkOffset()))
    return;
  
  const  uint8_t  *ptr = pkt + pcap->DataLinkOffset();
  Ipv4PacketHeader  ipHeader((struct ip *)ptr);
  TestChecksum(ipHeader);
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  OptArgs  optargs;
  
  optargs.AddOptArg("v", "verbose", false, "false",
                    "causes packet headers to be printed");
  optargs.Parse(argc, argv);
  g_verbose = optargs.Get<bool>('v');

  Pcap  pcap;
  pcap.OpenOffline("./ip.pcap");
  pcap.SetFilter("ip", true);
  
  while (pcap.Loop(-1, pcapcb, (uint8_t *)&pcap))
    ;
  
  //  TestChecksum();
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
