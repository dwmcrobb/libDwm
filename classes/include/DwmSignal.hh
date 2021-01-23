//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmSignal.hh
//!  \brief Dwm::Signal class definition
//---------------------------------------------------------------------------

#ifndef _DWMSIGNAL_HH_
#define _DWMSIGNAL_HH_

extern "C" {
  #include <signal.h>
}

#include <string>
#include <deque>
#include <map>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates a signal.
  //--------------------------------------------------------------------------
  class Signal
  {
  public:
    //------------------------------------------------------------------------
    //!  Construct from a signal number (e.g. SIGHUP).
    //------------------------------------------------------------------------
    Signal(int sigNum);

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    Signal(const Signal & signal);
    
    //------------------------------------------------------------------------
    //!  Block the signal.
    //------------------------------------------------------------------------
    bool Block();

    //------------------------------------------------------------------------
    //!  Unblock the signal.
    //------------------------------------------------------------------------
    bool Unblock();

    //------------------------------------------------------------------------
    //!  Returns true if the signal is currently blocked.
    //------------------------------------------------------------------------
    bool IsBlocked();

    //------------------------------------------------------------------------
    //!  Returns true if the signal is currently pending.
    //------------------------------------------------------------------------
    bool IsPending();

    //------------------------------------------------------------------------
    //!  Waits for the delivery of the signal to the calling thread.
    //!  See the sigwait() manpage.
    //------------------------------------------------------------------------
    void Wait();
    
    //------------------------------------------------------------------------
    //!  Pushes a signal onto the top of a stack of signal handlers for 
    //!  the signal.  The handler on the top of the stack is the one which 
    //!  will be called on reception of the signal.
    //------------------------------------------------------------------------
    int PushHandler(void (*handler)(int));

    //------------------------------------------------------------------------
    //!  Pops the top signal handler off of the stack of signal handlers.
    //------------------------------------------------------------------------
    int PopHandler();

    //------------------------------------------------------------------------
    //!  Returns a string representation of the signal ("e.g. "SIGHUP").
    //------------------------------------------------------------------------
    const std::string & Name() const;

  protected:
    int                        _sigNum;
    std::deque<void (*)(int)>  _handlers;

    Signal();

    static const std::map<int,std::string,std::less<int> >  _names;
  };

}  // namespace Dwm

#endif  // _DWMSIGNAL_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
