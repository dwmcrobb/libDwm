//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmConditionVariable.hh 8401 $
// @(#) $Id: DwmConditionVariable.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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

//---------------------------------------------------------------------------
//!  \file DwmConditionVariable.hh
//!  \brief Dwm::Pthread::ConditionVariable class definition
//---------------------------------------------------------------------------

#ifndef _DWMCONDITIONVARIABLE_HH_
#define _DWMCONDITIONVARIABLE_HH_

extern "C" {
#include <pthread.h>
}

#include "DwmMutex.hh"

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  This class just encapsulates a condition variable (from the
    //!  pthread library), with default attributes.
    //------------------------------------------------------------------------
    class ConditionVariable
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructor, initializes the condition variable.
      //----------------------------------------------------------------------
      ConditionVariable();

      //----------------------------------------------------------------------
      //!  Destructor, destroys the condition variable.
      //----------------------------------------------------------------------
      ~ConditionVariable();

      //----------------------------------------------------------------------
      //!  Unblocks all threads waiting for the condition variable.
      //!  Returns true on success.
      //----------------------------------------------------------------------
      bool Broadcast();

      //----------------------------------------------------------------------
      //!  Unblocks one thread waiting for the condition variable.
      //!  Returns true on success.
      //----------------------------------------------------------------------
      bool Signal();

      //----------------------------------------------------------------------
      //!  Atomically blocks the calling thread waiting on the condition 
      //!  variable, and unblocks the mutex specified by \c mutex.  The 
      //!  waiting thread unblocks only after another thread calls
      //!  Signal(), or Broadcast() on the same object, or if the system
      //!  time reaches the time specified in \c abstime, and the current
      //!  thread reacquires the lock on \c mutex.
      //----------------------------------------------------------------------
      bool TimedWait(Mutex & mutex, const struct timespec * abstime);

      //----------------------------------------------------------------------
      //!  Atomically blocks the current thread waiting on the condition
      //!  variable, and unblocks the mutex specified by \c mutex.  The
      //!  waiting thread unblocks only after another thread calls 
      //!  Signal() or Broadcast() on the same object, and the current
      //!  thread reacquires the lock on \c mutex.
      //----------------------------------------------------------------------
      bool Wait(Mutex & mutex);
     
    protected:
      pthread_cond_t  _cv;
    };
    
  }  // namespace Pthread

}  // namespace Dwm

#endif  // _DWMCONDITIONVARIABLE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
