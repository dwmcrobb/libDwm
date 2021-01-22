//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmPthreadReadWriteLock.cc 11130 $
// @(#) $Id: DwmPthreadReadWriteLock.cc 11130 2020-09-07 23:26:49Z dwm $
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
//!  \file DwmPthreadReadWriteLock.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Pthread::ReadWriteLock class implementation
//---------------------------------------------------------------------------

#include <cassert>
#include <string>

#include "DwmPthreadReadWriteLock.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmPthreadReadWriteLock.cc 11130 $");

namespace Dwm {

  namespace Pthread {
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ReadWriteLock::
    ReadWriteLock(const pthread_rwlockattr_t *attr)
    {
      this->_attr = attr;
      int  rc = pthread_rwlock_init(&this->_lock,attr);
      assert(rc == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ReadWriteLock::
    ReadWriteLock(const ReadWriteLock & rwlock)
    {
      int  rc = pthread_rwlock_init(&this->_lock,rwlock._attr);
      assert(rc == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ReadWriteLock::~ReadWriteLock()
    {
      int  rc = pthread_rwlock_destroy(&this->_lock);
      assert(rc == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ReadWriteLock::ReadLock()
    {
      bool  rc = false;
      if (pthread_rwlock_rdlock(&this->_lock) == 0)
        rc = true;
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int ReadWriteLock::TryReadLock()
    {
      return(pthread_rwlock_tryrdlock(&this->_lock));
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ReadWriteLock::WriteLock()
    {
      bool  rc = false;
      if (pthread_rwlock_wrlock(&this->_lock) == 0)
        rc = true;
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int ReadWriteLock::TryWriteLock()
    {
      return(pthread_rwlock_trywrlock(&this->_lock));
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool ReadWriteLock::Unlock()
    {
      bool  rc = false;
      if (pthread_rwlock_unlock(&this->_lock) == 0)
        rc = true;
      return(rc);
    }
    

  }  // namespace Pthread
    
}  // namespace Dwm
