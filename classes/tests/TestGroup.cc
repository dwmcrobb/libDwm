//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestGroup.cc 11086 $
// @(#) $Id: TestGroup.cc 11086 2020-09-06 06:17:20Z dwm $
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
//!  \file TestGroup.cc
//!  \brief Unit tests for Group class
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <algorithm>

#include "DwmGroup.hh"
#include "DwmPassword.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestGroup.cc 11086 $");

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#ifndef __linux__
  Dwm::Group  wheel("wheel");
  UnitAssert(0 == wheel.Id());
  Dwm::Password  rootpwd("root");
  auto  gmit = std::find(wheel.Members().begin(), wheel.Members().end(),
                         rootpwd.UserName());
  UnitAssert(gmit != wheel.Members().end());
#else
  Dwm::Group  adm("adm");
  UnitAssert(0 != adm.Id());
  Dwm::Password  syslogpwd("syslog");
  auto  gmit = std::find(adm.Members().begin(), adm.Members().end(),
                         syslogpwd.UserName());
  if (gmit == adm.Members().end()) {
    Dwm::Password  pipwd("pi");
    gmit = std::find(adm.Members().begin(), adm.Members().end(),
                     pipwd.UserName());
  }
  UnitAssert(gmit != adm.Members().end());
#endif  
  
  struct group  *grp = getgrgid(getgid());
  Dwm::Group     setGroup(getgid());
  setGroup.Set(*grp);
  UnitAssert(setGroup.Id() == grp->gr_gid);
  UnitAssert(setGroup.Name() == grp->gr_name);
  UnitAssert(setGroup.Password() == grp->gr_passwd);
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }
  exit(0);
}
