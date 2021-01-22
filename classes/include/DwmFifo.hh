//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmFifo.hh 8401 $
// @(#) $Id: DwmFifo.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004, 2016
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
//!  \file DwmFifo.hh
//!  \brief Dwm::Fifo class template definition
//---------------------------------------------------------------------------

#ifndef _DWMFIFO_HH_
#define _DWMFIFO_HH_

#include <deque>

#include "DwmMutex.hh"
#include "DwmConditionVariable.hh"

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class ValueType>
    class Fifo
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      Fifo(bool doLocking = true)
          : _deque(), _mutex(), _doLocking(doLocking)
      {}
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool PushBack(const ValueType & value)
      {
        bool  rc = false;
       
        if (_doLocking)
          if (! _mutex.Lock())
            return(rc);
       
        _deque.push_back(value);
        rc = true;
       
        if (_doLocking)
          _mutex.Unlock();
       
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool PopFront(ValueType & value)
      {
        bool  rc = false;
       
        if (_doLocking)
          if (! _mutex.Lock())
            return(rc);
       
        if (! _deque.empty()) {
          value = _deque.front();
          _deque.pop_front();
          rc = true;
        }
       
        if (_doLocking)
          _mutex.Unlock();
       
        return(rc);
      }
    
    protected:
      std::deque<ValueType>  _deque;
      Mutex               _mutex;
      bool                _doLocking;
    };
    
  }  // namespace Pthread
  
}  // namespace Dwm

#endif  // _DWMFIFO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
