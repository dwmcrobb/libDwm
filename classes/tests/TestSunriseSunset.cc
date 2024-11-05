//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmSunriseSunset.cc 8652 $
// @(#) $Id: TestDwmSunriseSunset.cc 8652 2016-06-12 04:22:22Z dwm $
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
//!  \file TestDwmSunriseSunset.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <iostream>

#include "DwmDateTime.hh"
#include "DwmSvnTag.hh"
#include "DwmSunriseSunset.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmSunriseSunset.cc 8652 $");

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  double  latitude = 42.7351;
  double  longitude = 83.4193;

  Dwm::TimeValue64  now(true);
  Dwm::TimeValue64  sunrise = Dwm::Sunrise(latitude, longitude, now.Secs());
  Dwm::TimeValue64  sunset = Dwm::Sunset(latitude, longitude, now.Secs());

  UnitAssert(sunset > sunrise);
  
  Dwm::DateTime  riseDT(sunrise);
  Dwm::DateTime  setDT(sunset);

  Dwm::TimeValue64  dayTime = riseDT.GetTimeValue64();
  UnitAssert(Dwm::IsDaytime(latitude, longitude, dayTime.Secs() + 1));
  UnitAssert(! Dwm::IsDaytime(latitude, longitude, dayTime.Secs() - 1));
  
  Dwm::TimeValue64  nightTime = setDT.GetTimeValue64();
  UnitAssert(! Dwm::IsDaytime(latitude, longitude, nightTime.Secs() + 1));
  UnitAssert(Dwm::IsDaytime(latitude, longitude, nightTime.Secs() - 1));
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }
  
  return 0;
}
