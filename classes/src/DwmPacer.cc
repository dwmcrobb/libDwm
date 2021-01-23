//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007, 2020
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
//!  \file DwmPacer.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Pacer class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/param.h>
  #include <sys/types.h>
#if (defined __APPLE__)
  #include <sys/sysctl.h>
#endif
  #include <sys/time.h>
}

#include <iostream>

#include "DwmPacer.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  static _minSleep
  //--------------------------------------------------------------------------
  TimeValue  Pacer::_minSleep(0, 0);
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Pacer::Pacer(uint32_t rate)
    : _nextTime(true)
  {
    if ((! _minSleep.Secs()) && (! _minSleep.Usecs()))
      SetMinSleep();
    Rate(rate);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Pacer::Rate() const
  {
    return(_rate);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Pacer::Rate(uint32_t rate)
  {
    _rate = rate;
    
    if (_rate > 1)
      _interval.Set(0, 1000000UL/_rate);
    else
      _interval.Set(1, 0);

    _nextTime.SetNow();
    _nextTime += _interval;

    return(_rate);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Pacer::Pace()
  {
    TimeValue  now(true);

    //  The following is to cover the case where Pace() isn't called
    //  for a long while.  We don't want to let the caller race to
    //  the current time just because our _nextTime is way behind the
    //  the current time.
    TimeValue  raceTime = _minSleep;
    raceTime *= 10;
    if (now > (_nextTime + raceTime)) {
      _nextTime = now;
      return;
    }

    if (now < _nextTime) {
      //  We're going too fast.
      TimeValue  sleepTime = _nextTime - now;
      if (sleepTime >= _minSleep) {
        usleep(sleepTime.Secs() * 1000000UL + sleepTime.Usecs());
      }
    }
    _nextTime += _interval;

    return;
  }

#if (defined __APPLE__)

  //--------------------------------------------------------------------------
  //!  Set the static _minSleep based on the system's clock tick.
  //!  Sleeping less than this amount is a bad idea.
  //--------------------------------------------------------------------------
  void Pacer::SetMinSleep()
  {
    struct clockinfo  clockInfo;
    int mib[2];
    size_t len = sizeof(clockInfo);
    mib[0] = CTL_KERN;
    mib[1] = KERN_CLOCKRATE;
    if (sysctl(mib, 2, &clockInfo, &len, NULL, 0) == 0) {
      if (clockInfo.tick > 10000)
        _minSleep.Set(0, clockInfo.tick);
      else
        _minSleep.Set(0, 10000);
    }
    else {
      // assume 10 milliseconds
      _minSleep.Set(0, 10000);
    }
    return;
  }

#else

  //--------------------------------------------------------------------------
  //!  Set the static _minSleep based on the system's clock tick.
  //!  Sleeping less than this amount is a bad idea.
  //--------------------------------------------------------------------------
  void Pacer::SetMinSleep()
  {
    struct timespec  ts;
	  
#ifndef __APPLE__
    if (clock_getres(CLOCK_REALTIME, &ts) == 0) {
      uint32_t  usecs = ts.tv_nsec/1000;
      if (usecs > 10000)
        _minSleep.Set(0, usecs);
      else
        _minSleep.Set(0, 10000);
    }
    else {
      // assume 10 milliseconds
      _minSleep.Set(0, 10000);
    }
#else
    _minSleep.Set(0, 10000);
#endif
    return;
  }

#endif

}  // namespace Dwm

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
