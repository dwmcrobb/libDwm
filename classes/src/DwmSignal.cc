//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2000-2007, 2020
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
//!  \file DwmSignal.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Signal class implementation
//---------------------------------------------------------------------------

#include <cassert>
#include <cstring>

#include "DwmSignal.hh"
#include "DwmSysLogger.hh"

namespace Dwm {

  const std::map<int,std::string>  Signal::_names = {
    { 0, "Unknown" },
    { SIGHUP, "SIGHUP" },
    { SIGINT, "SIGINT" },
    { SIGQUIT, "SIGQUIT" },
    { SIGILL, "SIGILL" },
#ifdef SIGTRAP
    { SIGTRAP, "SIGTRAP" },
#endif
    { SIGABRT, "SIGABRT" },
#ifdef SIGEMT
    { SIGEMT, "SIGEMT" },
#endif
    { SIGFPE, "SIGFPE" },
    { SIGKILL, "SIGKILL" },
#ifdef SIGBUS
    { SIGBUS, "SIGBUS" },
#endif
    { SIGSEGV, "SIGSEGV" },
#ifdef SIGSYS
    { SIGSYS, "SIGSYS" },
#endif
    { SIGPIPE, "SIGPIPE" },
    { SIGALRM, "SIGALRM" },
    { SIGTERM, "SIGTERM" },
#ifdef SIGURG
    { SIGURG, "SIGURG" },
#endif
    { SIGSTOP, "SIGSTOP" },
    { SIGTSTP, "SIGTSTP" },
    { SIGCONT, "SIGCONT" },
    { SIGCHLD, "SIGCHLD" },
    { SIGTTIN, "SIGTTIN" },
    { SIGTTOU, "SIGTTOU" },
#ifdef SIGIO
    { SIGIO, "SIGIO" },
#endif
#ifdef SIGXCPU
    { SIGXCPU, "SIGXCPU" },
#endif
#ifdef SIGXFSZ
    { SIGXFSZ, "SIGXFSZ" },
#endif
#ifdef SIGVTALRM
    { SIGVTALRM, "SIGVTALRM" },
#endif
#ifdef SIGPROF
    { SIGPROF, "SIGPROF" },
#endif
#ifdef SIGWINCH
    { SIGWINCH, "SIGWINCH" },
#endif
#ifdef SIGINFO
    { SIGINFO, "SIGINFO" },
#endif
    { SIGUSR1, "SIGUSR1" },
    { SIGUSR2, "SIGUSR2" }
  };
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Signal::Signal()
  : _sigNum(-1)
  { }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Signal::Signal(int sigNum)
    : _sigNum(sigNum)
  { }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Signal::Block()
  {
    bool      rc = false;
    sigset_t  blockSet;
    
    sigemptyset(&blockSet);
    sigaddset(&blockSet,this->_sigNum);

    if (sigprocmask(SIG_BLOCK,&blockSet,NULL) == 0)
      rc = true;
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Signal::Unblock()
  {
    bool      rc = false;
    sigset_t  blockSet;
    
    sigemptyset(&blockSet);
    sigaddset(&blockSet,this->_sigNum);

    if (sigprocmask(SIG_UNBLOCK,&blockSet,NULL) == 0) {
      rc = true;
    }
    else {
      Syslog(LOG_ERR, "sigprocmask(SIG_UNBLOCK,%p,NULL) failed", &blockSet);
    }
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Signal::IsBlocked()
  {
    bool      rc = false;
    sigset_t  blockSet;
    
    sigemptyset(&blockSet);
    if (sigprocmask(SIG_BLOCK,NULL,&blockSet) == 0) {
      if (sigismember(&blockSet,this->_sigNum)) {
        rc = true;
      }
    }
    else {
      Syslog(LOG_ERR,"sigprocmask(SIG_BLOCK,NULL,%p) failed", &blockSet);
    }
      
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Signal::IsPending()
  {
    bool      rc = false;
    sigset_t  pendingSet;
    sigemptyset(&pendingSet);

    if (sigpending(&pendingSet) == 0) {
      if (sigismember(&pendingSet,this->_sigNum)) {
        rc = true;
      }
    }
    else {
      Syslog(LOG_ERR,"sigpending(%p) failed", &pendingSet);
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Signal::Wait()
  {
    sigset_t  sigSet;
    
    if (sigemptyset(&sigSet) != 0)
      ;
    if (sigaddset(&sigSet,_sigNum) != 0)
      ;
    int  sigNum;
    if (sigwait(&sigSet,&sigNum) == 0) {
      assert(sigNum == _sigNum);
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Signal::PushHandler(void (*handler)(int))
  {
    int  rc = -1;
    
    struct sigaction  action;
    memset(&action,0,sizeof(action));
    action.sa_handler = handler;
    action.sa_flags   = 0;

    if (sigaction(this->_sigNum,&action,NULL) < 0) {
      Syslog(LOG_ERR, "sigaction(%d,%p,NULL) failed",
             this->_sigNum, &action);
    }
    else {
      this->_handlers.push_front(handler);
      rc = this->_handlers.size();
    }

    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Signal::PopHandler()
  {
    int  rc = -1;
    if (! this->_handlers.empty()) {
      this->_handlers.pop_front();
      if (this->_handlers.empty()) {
        struct sigaction  action;
        memset(&action,0,sizeof(action));
        action.sa_handler = SIG_DFL;
        if (sigaction(this->_sigNum,&action,NULL) == 0) {
          rc = 0;
        }
      }
      else {
        struct sigaction  action;
        memset(&action,0,sizeof(action));
        action.sa_handler = this->_handlers.front();
        action.sa_flags = 0;
        sigaction(this->_sigNum,&action,NULL);
      }
      rc = this->_handlers.size();
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Signal::Name() const
  {
    auto  iter = Signal::_names.find(this->_sigNum);
    if (iter != Signal::_names.end())
      return(iter->second);
    else
      return(Signal::_names.find(0)->second);
  }
    
}  // namespace Dwm
