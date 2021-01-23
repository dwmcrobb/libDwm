//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007, 2016
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
//!  \file DwmPacer.hh
//!  \brief Dwm::Pacer class definition
//---------------------------------------------------------------------------

#ifndef _DWMPACER_HH_
#define _DWMPACER_HH_

#include "DwmTimeValue.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class permits throttling of functions to a max number of
  //!  repetitions per second.  Instead of bogusly calling one of the
  //!  sleep functions between each call (which will result in more 
  //!  throttling than desired), it will always sleep at least one clock 
  //!  tick when it sleeps.
  //--------------------------------------------------------------------------
  class Pacer
  {
  public:
    //------------------------------------------------------------------------
    //!  Construct with the given \c rate (reps per second).
    //------------------------------------------------------------------------
    Pacer(uint32_t rate);

    //------------------------------------------------------------------------
    //!  Returns the rate (reps per second).
    //------------------------------------------------------------------------
    uint32_t Rate() const;
    
    //------------------------------------------------------------------------
    //!  Sets the rate (reps per second).
    //------------------------------------------------------------------------
    uint32_t Rate(uint32_t rate);
    
    //------------------------------------------------------------------------
    //!  This should be called between each call you wish to throttle.
    //!  If the current time is less than or equal to the time in which
    //!  the next function call is supposed to be made, it will sleep
    //!  the difference.
    //------------------------------------------------------------------------
    void Pace();

  private:
    uint32_t   _rate;
    TimeValue  _nextTime;
    TimeValue  _interval;

    static TimeValue  _minSleep;
    void SetMinSleep();
  };
  
}  // namespace Dwm

#endif  // _DWMPACER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
