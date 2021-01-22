//===========================================================================
// @(#) $Name$
// @(#) $Id: DwmConditionVariable.cc 8386 2016-04-17 03:01:19Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

#include "DwmConditionVariable.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmConditionVariable.cc 8386 $");

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ConditionVariable::ConditionVariable()
        : _cv()
    {
      pthread_cond_init(&_cv,0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ConditionVariable::~ConditionVariable()
    {
      pthread_cond_destroy(&_cv);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ConditionVariable::Broadcast()
    {
      return(pthread_cond_broadcast(&_cv) == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ConditionVariable::Signal()
    {
      return(pthread_cond_signal(&_cv) == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ConditionVariable::TimedWait(Mutex & mutex, 
                                      const struct timespec * abstime)
    {
      return(pthread_cond_timedwait(&_cv,&(mutex._mutex->Mutex()),abstime) == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ConditionVariable::Wait(Mutex & mutex)
    {
      return(pthread_cond_wait(&_cv,&(mutex._mutex->Mutex())) == 0);
    }
    
  }  // namespace Pthread

}  // namespace Dwm
