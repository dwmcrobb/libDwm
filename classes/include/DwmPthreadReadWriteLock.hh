//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmPthreadReadWriteLock.hh 8401 $
// @(#) $Id: DwmPthreadReadWriteLock.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007
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
//!  \file DwmPthreadReadWriteLock.hh
//!  \brief Dwm::Pthread::ReadWriteLock class definition
//---------------------------------------------------------------------------

#ifndef _PTHREADREADWRITELOCK_HH_
#define _PTHREADREADWRITELOCK_HH_

extern "C" {
  #include <errno.h>
  #include <pthread.h>
}

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  This class is just a simple wrapper around pthread_rwlock* functions
    //!  from the pthread library.
    //------------------------------------------------------------------------
    class ReadWriteLock
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructor
      //----------------------------------------------------------------------
      ReadWriteLock(const pthread_rwlockattr_t *attr = 0);
     
      ReadWriteLock(const ReadWriteLock & rwlock);
     
      //----------------------------------------------------------------------
      //!  Destructor
      //----------------------------------------------------------------------
      ~ReadWriteLock();
     
      //----------------------------------------------------------------------
      //!  Acquires a read lock.  If the lock can't be acquired immediately,
      //!  blocks until the lock can be acquired.  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool ReadLock();
     
      //----------------------------------------------------------------------
      //!  Acquires a read lock.  Returns EBUSY if the lock can't be acquired
      //!  immediately (due to a write lock).  Returns 0 on success.  Other
      //!  return values indicate an error (see the pthread_rwlock_tryrdlock()
      //!  manpage).
      //----------------------------------------------------------------------
      int TryReadLock();
     
      //----------------------------------------------------------------------
      //!  Acquires a write lock.  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool WriteLock();
     
      //----------------------------------------------------------------------
      //!  Acquires a write lock.  Returns EBUSY if the lock can't be acquired
      //!  immediately (due to other lock(s)).  Returns 0 on success.  Other
      //!  return values indicate an error (see the pthread_rwlock_trywrlock()
      //!  manpage).
      //----------------------------------------------------------------------
      int TryWriteLock();
     
      //----------------------------------------------------------------------
      //!  Unlocks.  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Unlock();
     
    private:
      pthread_rwlock_t             _lock;
      const pthread_rwlockattr_t  *_attr;
    };
    
  }  // namespace Pthread

}  // namespace Dwm

#endif  // _PTHREADREADWRITELOCK_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
