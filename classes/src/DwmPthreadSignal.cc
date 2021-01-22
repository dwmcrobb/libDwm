//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmPthreadSignal.cc 11131 $
// @(#) $Id: DwmPthreadSignal.cc 11131 2020-09-07 23:27:40Z dwm $
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmPthreadSignal.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Pthread::Signal class implementation
//---------------------------------------------------------------------------

#include <cstdlib>

#include "DwmPthreadSignal.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmPthreadSignal.cc 11131 $");

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Signal::Signal()
        : Dwm::Signal(), _mutex()
    {
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Signal::Signal(int sigNum)
        : _mutex()
    {
      if (! this->Lock())
        abort();
      this->_sigNum = sigNum;
      if (! this->Unlock())
        abort();
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Signal::Lock()
    {
      return(this->_mutex.Lock());
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Signal::Unlock()
    {
      return(this->_mutex.Unlock());
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Signal::BlockInThread()
    {
      bool  rc = false;
      
      sigset_t  blockSet;
      sigemptyset(&blockSet);
      sigaddset(&blockSet,this->_sigNum);
      if (pthread_sigmask(SIG_BLOCK,&blockSet,NULL) == 0)
        rc = true;
      
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Signal::UnblockInThread()
    {
      bool  rc = false;
      
      sigset_t  blockSet;
      sigemptyset(&blockSet);
      sigaddset(&blockSet,this->_sigNum);
      if (pthread_sigmask(SIG_UNBLOCK,&blockSet,NULL) == 0)
        rc = true;
      
      return(rc);
    }


  }  // namespace Pthread

}  // namespace Dwm


