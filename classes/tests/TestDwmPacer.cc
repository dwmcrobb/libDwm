//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmPacer.cc 8389 $
// @(#) $Id: TestDwmPacer.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestDwmPacer.cc
//!  \brief Unit tests for Dwm::Pacer
//---------------------------------------------------------------------------

#include <iostream>

#include "DwmSvnTag.hh"
#include "DwmPacer.hh"
#include "DwmOptArgs.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmPacer.cc 8389 $");

using namespace std;
using Dwm::OptArgs;
using Dwm::Pacer;
using Dwm::TimeValue;

static bool g_verbose = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool Accurate(const TimeValue & elapsed, const Pacer & pacer, 
                     uint32_t reps)
{
  double  expectedTime = reps;
  expectedTime /= pacer.Rate();

  //  allow 2% error
  pair<double,double>  okRange(expectedTime * 0.98, expectedTime * 1.02);
  
  if (g_verbose) {
    double  diff = (expectedTime > (double)elapsed) ?
      expectedTime - (double)elapsed : (double)elapsed - expectedTime;
    cerr << "accuracy: "
         << 100.0 - (diff * 100.0) << endl;
  }
  if ((double)elapsed >= okRange.first && (double)elapsed <= okRange.second)
    return(true);
  
  return(false);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestPacer(Pacer & pacer, uint32_t reps)
{
  TimeValue  start(true);
  for (uint32_t i = 0; i < reps; ++i) {
    pacer.Pace();
  }
  TimeValue  elapsed(true);
  elapsed -= start;
  UnitAssert(Accurate(elapsed, pacer, reps));
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  OptArgs  optargs;
  optargs.AddOptArg("v", "verbose", false, "false", 
                    "causes accuracy to be printed");
  optargs.Parse(argc, argv);
  
  g_verbose = optargs.Get<bool>('v');

  Pacer  pacer(50);

  TestPacer(pacer, 100);

  pacer.Rate(1000);
  TestPacer(pacer, 5000);

  usleep(500000);

  TestPacer(pacer, 5000);

  pacer.Rate(2000);
  TestPacer(pacer, 4000);

  usleep(200000);

  TestPacer(pacer, 4000);

  pacer.Rate(20000);
  TestPacer(pacer, 40000);

  sleep(1);

  TestPacer(pacer, 40000);

  pacer.Rate(100000);
  TestPacer(pacer, 100000);
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

 exit(0);
}

      
