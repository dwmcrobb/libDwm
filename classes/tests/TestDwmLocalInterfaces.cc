//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmLocalInterfaces.cc 9020 $
// @(#) $Id: TestDwmLocalInterfaces.cc 9020 2017-04-11 20:28:41Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file TestDwmLocalInterfaces.cc
//!  \brief Unit tests for Dwm::LocalInterfaces
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <iomanip>
#include <iostream>
#include <vector>

#include "DwmSvnTag.hh"
#include "DwmLocalInterfaces.hh"
#include "DwmOptArgs.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmLocalInterfaces.cc 9020 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool  show = false;

  OptArgs  optargs;
  optargs.AddOptArg("s", "show", false, "false", "show interfaces");
  optargs.Parse(argc, argv);
  show = optargs.Get<bool>('s');
  
  map<string,LocalInterface>  interfaces;
  
  UnitAssert(GetLocalInterfaces(interfaces));
  if (! interfaces.empty()) {
    map<string,LocalInterface>::iterator  iter;
    for (iter = interfaces.begin(); iter != interfaces.end(); ++iter) {
      if (iter->second.Name() == "lo0") {
        UnitAssert(iter->second.Addr() == Ipv4Address("127.0.0.1"));
        UnitAssert(iter->second.Mask() == Ipv4Address("255.0.0.0"));
#ifndef __linux__
        if (geteuid() == 0) {
          UnitAssert(iter->second.AddAlias(Ipv4Address("192.168.199.199"), 
                                           Ipv4Address("255.255.255.0")));
          UnitAssert(! iter->second.AddAlias(Ipv4Address("192.168.199.199"), 
                                             Ipv4Address("255.255.255.0")));
          UnitAssert(iter->second.AddAlias(Ipv4Address("192.168.200.200"), 
                                           Ipv4Address("255.255.255.0")));
          UnitAssert(iter->second.RemoveAlias(Ipv4Address("192.168.199.199"), 
                                              Ipv4Address("255.255.255.0")));
          UnitAssert(iter->second.RemoveAlias(Ipv4Address("192.168.200.200"), 
                                              Ipv4Address("255.255.255.0")));
        }
#endif
      }
      
      if (iter->second.IsUp() && iter->second.IsRunning()) {
        UnitAssert(iter->second.Mtu() > 0);
      }
      if (iter->second.Addr() != Ipv4Address("0.0.0.0")) {
        UnitAssert(iter->second.Mask() != Ipv4Address("0.0.0.0"));
      }
      if (show) {
        cout << iter->second;
      }
    }
  }

  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  
  return(0);
}
