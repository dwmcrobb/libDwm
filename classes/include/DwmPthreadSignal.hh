//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmPthreadSignal.hh 8401 $
// @(#) $Id: DwmPthreadSignal.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2000-2007, 2016
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

//---------------------------------------------------------------------------
//!  \file DwmPthreadSignal.hh
//!  \brief Dwm::Pthread::Signal class definition
//---------------------------------------------------------------------------

#ifndef _DWMPTHREADSIGNAL_HH_
#define _DWMPTHREADSIGNAL_HH_

extern "C" {
  #include <pthread.h>
}

#include "DwmSignal.hh"
#include "DwmMutex.hh"

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  This class inherits from Dwm::Signal and adds locking and unlocking
    //!  for threaded programs as well as members to block and unblock a
    //!  in a particular thread (wrappers for pthread_sigmask()).  Note that
    //!  no members perform automatic locking; you must call Lock() and
    //!  Unlock() explicitly to lock and unlock the Signal.
    //------------------------------------------------------------------------
    class Signal
      : public Dwm::Signal
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructs from a signal number (e.g. SIGHUP).
      //----------------------------------------------------------------------
      Signal(int sigNum);
     
      //----------------------------------------------------------------------
      //!  Attempts to lock the Signal object.  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool Lock();
     
      //----------------------------------------------------------------------
      //!  Attempts to unlock the Signal object.  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool Unlock();
     
      //----------------------------------------------------------------------
      //!  Attempts to block the signal in the caller's thread.  This is
      //!  just a wrapper for pthread_sigmask().  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool BlockInThread();

      //----------------------------------------------------------------------
      //!  Attempts to unblock the signal in the caller's thread.  This is
      //!  just a wrapper for pthread_sigmask().  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool UnblockInThread();
     
    protected:
      Mutex  _mutex;
     
      Signal();
    };
    
    
  }  // namespace Pthread
  
}  // namespace Dwm


#endif  // _DWMPTHREADSIGNAL_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
