//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmMutex.hh 8401 $
// @(#) $Id: DwmMutex.hh 8401 2016-04-17 06:44:31Z dwm $
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
//!  \file DwmMutex.hh
//!  \brief Dwm::Pthread::Mutex class definition
//---------------------------------------------------------------------------

#ifndef _DWMMUTEX_HH_
#define _DWMMUTEX_HH_

#include "DwmPortability.hh"

extern "C" {
#include <pthread.h>
}

#ifdef HAVE_TR1
#include <tr1/memory>
#define STD_SHARED_PTR std::tr1::shared_ptr
#else
#include <memory>
#define STD_SHARED_PTR std::shared_ptr
#endif

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  This class just encapsulates a pthread_mutex_t (from the pthread
    //!  library).
    //------------------------------------------------------------------------
    class Mutex
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructor.  Initializes the mutex.
      //----------------------------------------------------------------------
      Mutex();

      //----------------------------------------------------------------------
      //!  Constructor.  Destroys the mutex.
      //----------------------------------------------------------------------
      ~Mutex();

      //----------------------------------------------------------------------
      //!  Copy constructor.
      //----------------------------------------------------------------------
      Mutex(const Mutex & mutex);
      
      //----------------------------------------------------------------------
      //!  operator =
      //----------------------------------------------------------------------
      Mutex & operator = (const Mutex & mutex);
      
      //----------------------------------------------------------------------
      //!  Lock the mutex.  This is a blocking call; it will not return 
      //!  until the mutex has been locked.  Returns true on success.
      //----------------------------------------------------------------------
      bool Lock();

      //----------------------------------------------------------------------
      //!  Try to lock the mutex, without blocking.  Returns true on 
      //!  success.
      //----------------------------------------------------------------------
      bool TryLock();

      //----------------------------------------------------------------------
      //!  Unlock the mutex.  Returns true on success.
      //----------------------------------------------------------------------
      bool Unlock();

      friend class ConditionVariable;
     
    protected:
      class Wrapper
      {
      public:
        Wrapper();
        ~Wrapper();
        bool Lock();
        bool TryLock();
        bool Unlock();
        pthread_mutex_t & Mutex();
        
      private:
        pthread_mutex_t  _mutex;
      };
      
      STD_SHARED_PTR<Wrapper>  _mutex;
    };
    
  }  // namespace Pthread

}  // namespace Dwm

#endif  // _DWMMUTEX_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
