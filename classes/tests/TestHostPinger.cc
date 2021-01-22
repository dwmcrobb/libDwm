//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestHostPinger.cc 8503 $
// @(#) $Id: TestHostPinger.cc 8503 2016-05-09 01:06:56Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016
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
//!  \file TestHostPinger.cc
//!  \brief Unit tests for HostPinger class
//---------------------------------------------------------------------------

#include <cstdlib>

#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"
#include "DwmHostPinger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestHostPinger.cc 8503 $");

using namespace std;

static bool      g_debug = false;
static uint32_t  g_icmpResponses = 0;
static uint32_t  g_tcpResponses = 0;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
class IcmpReceiver
  : public Dwm::HostPinger::Receiver
{
public:
  void operator () (const Dwm::PingDestination & destination,
                    const Dwm::TimeValue & recvTime,
                    const Dwm::TimeValue & rtt)
  {
    if (rtt.Secs() != 0xFFFFFFFF) {
      ++g_icmpResponses;
      if (g_debug) {
        cerr << destination.Address() << " icmp "
             << (double)(rtt.Secs() * 1000.0) + (rtt.Usecs() / 1000.0)
             << " ms\n";
      }
    }
    return;
  }
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestIcmpPinger()
{
  IcmpReceiver    *receiver = new IcmpReceiver();
  Dwm::HostPinger  icmpPinger(getpid() & 0xFFFF, "lo0");
  icmpPinger.SetReceiver(receiver);
  UnitAssert(icmpPinger.AddDestination(Dwm::PingDestination("127.0.0.1")));
  UnitAssert(icmpPinger.PacketRate(10) == 10);
  if (geteuid() == 0) {
    UnitAssert(icmpPinger.Start());
    sleep(2);
    UnitAssert(icmpPinger.Stop());
    UnitAssert(g_icmpResponses > 10);
  }
  delete receiver;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
class TcpReceiver
  : public Dwm::HostPinger::Receiver
{
public:
  void operator ()(const Dwm::PingDestination & destination,
                   const Dwm::TimeValue & recvTime,
                   const Dwm::TimeValue & rtt)
  {
    if (rtt.Secs() != 0xFFFFFFFF) {
      ++g_tcpResponses;
      if (g_debug) {
        cerr << destination.Address() << " tcp "
             << (double)(rtt.Secs() * 1000.0) + (rtt.Usecs() / 1000.0)
             << " ms\n";
      }
    }
    return;
  }
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestTcpPinger()
{
  TcpReceiver      *receiver = new TcpReceiver();
  Dwm::HostPinger   tcpPinger(getpid() & 0xFFFF, "lo0");
  tcpPinger.SetReceiver(receiver);
  UnitAssert(tcpPinger.AddDestination(Dwm::PingDestination("127.0.0.1:22")));
  UnitAssert(tcpPinger.PacketRate(10) == 10);
  if (geteuid() == 0) {
    UnitAssert(tcpPinger.Start());
    sleep(2);
    UnitAssert(tcpPinger.Stop());
    UnitAssert(g_tcpResponses > 10);
  }
  delete receiver;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int  optChar;
  
  while ((optChar = getopt(argc, argv, "d")) != EOF) {
    switch (optChar) {
      case 'd':
        g_debug = true;
        break;
      default:
        break;
    }
  }
  
  TestIcmpPinger();
  TestTcpPinger();
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }
  exit(0);
}
