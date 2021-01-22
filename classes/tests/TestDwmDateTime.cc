//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDateTime.cc 9013 $
// @(#) $Id: TestDwmDateTime.cc 9013 2017-04-11 09:27:40Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007
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
//!  \file TestDwmDateTime.cc
//!  \brief Unit tests for Dwm::DateTime
//---------------------------------------------------------------------------

#include <cstdlib>
#include <iostream>

#include "DwmDateTime.hh"
#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDateTime.cc 9013 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  setenv("TZ","CUT0",1);
  
  TimeValue  tv(0, 0);
  DateTime  dateTime(tv);

  UnitAssert(dateTime.Formatted("%a %b %e %H:%M:%S %Y %Z") == 
             "Thu Jan  1 00:00:00 1970 CUT");

  time_t     now = time((time_t *)0);
  TimeValue  nowTv(now, 0);
  DateTime   dateTimeNow(nowTv);
  string     ct = ctime(&now);
  UnitAssert(ct.find_first_of(dateTimeNow.Formatted("%a %b %e %H:%M:%S %Y")) == 0);

  DateTime  dt1;
  UnitAssert(dt1.Parse(dateTimeNow.Formatted("%a %b %e %H:%M:%S %Y %Z"),
                       "%a %b %e %H:%M:%S %Y %Z"));
  UnitAssert(dt1 == dateTimeNow);

  setenv("TZ","EST5EDT",1);
  tzset();
  
  UnitAssert(dt1.Parse("Sun Dec  3 18:33:16 2006 -0500",
                       "%a %b %e %H:%M:%S %Y %z"));
  DateTime  dt2;
  UnitAssert(dt2.Parse("Sun Dec  3 23:33:16 2006 +0000",
                       "%a %b %e %H:%M:%S %Y %z"));

  UnitAssert(dt1.GetTimeValue64() == dt2.GetTimeValue64());
  UnitAssert(dt1 == dt2);
  UnitAssert(dt1.Formatted("%a %b %e %H:%M:%S %Y %z") ==
             dt2.Formatted("%a %b %e %H:%M:%S %Y %z"));

  UnitAssert(dt1.Parse("Sun Dec  3 18:33:16 2006 -0500",
                       "%a %b %e %H:%M:%S %Y %z"));
  UnitAssert(dt2.Parse("Sun Dec  3 23:37:16 2006 +0000",
                       "%a %b %e %H:%M:%S %Y %z"));
  UnitAssert(dt1 < dt2);
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  
  return(0);
}
