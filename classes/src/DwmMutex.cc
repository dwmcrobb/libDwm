//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmMutex.cc 11119 $
// @(#) $Id: DwmMutex.cc 11119 2020-09-07 23:12:50Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2000-2007
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmMutex.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Mutex class implementation.  DEPRECATED
//---------------------------------------------------------------------------

#include <cstring>
#include <cstdlib>
#include <iostream>

#include "DwmSvnTag.hh"
#include "DwmMutex.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmMutex.cc 11119 $");

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Mutex::Mutex()
        : _mutex(new Wrapper())
    {
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Mutex::Mutex(const Mutex & mutex)
        : _mutex(mutex._mutex)
    {
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Mutex & Mutex::operator = (const Mutex & mutex)
    {
      if (&mutex != this) {
        _mutex = mutex._mutex;
      }
      return(*this);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Mutex::~Mutex()
    {
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Mutex::Lock()
    {
      return(this->_mutex->Lock());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Mutex::TryLock()
    {
      return(this->_mutex->TryLock());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Mutex::Unlock()
    {
      return(this->_mutex->Unlock());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Mutex::Wrapper::Wrapper()
    {
      pthread_mutex_init(&this->_mutex, 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Mutex::Wrapper::~Wrapper()
    {
      pthread_mutex_destroy(&this->_mutex);
    }
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Mutex::Wrapper::Lock()
    {
      return(pthread_mutex_lock(&this->_mutex) == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Mutex::Wrapper::TryLock()
    {
      return(pthread_mutex_trylock(&this->_mutex) == 0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Mutex::Wrapper::Unlock()
    {
      return(pthread_mutex_unlock(&this->_mutex) == 0);
    }
    
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    pthread_mutex_t & Mutex::Wrapper::Mutex()
    {
      return(_mutex);
    }
    
    
  }  // namespace Pthread

}  // namespace Dwm
