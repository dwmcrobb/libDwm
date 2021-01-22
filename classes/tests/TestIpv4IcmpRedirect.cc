//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpRedirect.cc 8389 $
// @(#) $Id: TestIpv4IcmpRedirect.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestIpv4IcmpRedirect.cc
//!  \brief Unit tests for Dwm::Ipv4IcmpRedirect
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
}

#include <cstdlib>

#include "DwmSvnTag.hh"
#include "DwmIpv4IcmpRedirect.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestIpv4IcmpRedirect.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Ipv4Address  ipAddr("192.168.168.1");
  
  Ipv4PacketHeader  ipHeader;
  ipHeader.HeaderLength(20);
  ipHeader.TypeOfService(0);
  ipHeader.TotalLength(28);
  ipHeader.Id(getpid() & 0xFFFF);
  ipHeader.TimeToLive(64);
  ipHeader.Protocol(17);
  ipHeader.SourceAddress(Ipv4Address("192.168.168.2"));
  ipHeader.DestinationAddress(Ipv4Address("192.168.172.10"));
  ipHeader.SetChecksum();

  Ipv4IcmpRedirect redirect(1, ipAddr, ipHeader, "redirect");
  UnitAssert(redirect.Type() == 5);
  UnitAssert(redirect.Code() == 1);
  UnitAssert(redirect.IpHeader() == ipHeader);
  UnitAssert(redirect.Data() == "redirect");
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
