//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmPthreadQueue.hh 8401 $
// @(#) $Id: DwmPthreadQueue.hh 8401 2016-04-17 06:44:31Z dwm $
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmPthreadQueue.hh
//!  \brief Dwm::Pthread::Queue class template definition
//---------------------------------------------------------------------------

#ifndef _DWMPTHREADQUEUE_HH_
#define _DWMPTHREADQUEUE_HH_

#include <deque>

#include "DwmSysLogger.hh"
#include "DwmConditionVariable.hh"
#include "DwmMutex.hh"

using std::deque;

namespace Dwm {

  namespace Pthread {

    //------------------------------------------------------------------------
    //!  This template provides inter-thread first-in first-out (FIFO) 
    //!  queueing.  A source thread may insert objects into the back of the 
    //!  queue using PushBack() while a sink thread pops objects from
    //!  the front of the queue using PopFront().  A sink thread may use
    //!  WaitForNotEmpty() to wait for entries to be added to the queue;
    //!  the calling thread will be blocked until the queue is non-empty.
    //!  A sink thread may also choose to use ConditionWait() and PopFront(),
    //!  which allows a source to wake up the sink using ConditionBroadcast()
    //!  or ConditionSignal() without pushing an entry into the queue.
    //------------------------------------------------------------------------
    template <typename _ValueType> 
    class Queue
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructor
      //----------------------------------------------------------------------
      Queue()
          : _queue(), _mutex(), _cv()
      {
        this->_maxLength = 0;
      }
    
      //----------------------------------------------------------------------
      //!  Destructor
      //----------------------------------------------------------------------
      ~Queue()
      {
        if (! this->Lock()) {
          SysLogger::Log(LOG_ERR,"Pthread::~Queue() failed to lock! {%s:%d}",
                         __FILE__,__LINE__);
        }
        this->_queue.clear();
        this->Unlock();
      }
     
      //----------------------------------------------------------------------
      //!  Returns the max length of the queue.  If 0, no maximum length will
      //!  be enforced.
      //----------------------------------------------------------------------
      uint32_t MaxLength() const
      {
        return(this->_maxLength);
      }
     
      //----------------------------------------------------------------------
      //!  Sets and returns the max length of the queue.  If 0, no maximum 
      //!  length will be enforced.
      //----------------------------------------------------------------------
      uint32_t MaxLength(uint32_t maxLength)
      {
        this->_maxLength = maxLength;
        return(this->_maxLength);
      }
    
      //----------------------------------------------------------------------
      //!  Inserts \c value on the back of the queue.  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool PushBack(const _ValueType & value)
      {
        bool  rc = false;
        if (this->Lock()) {
          if ((! this->_maxLength) ||
              (this->_queue.size() < this->_maxLength)) {
            this->_queue.push_back(value);
            this->_cv.Broadcast();
            rc = true;
          }
          this->Unlock();
        }
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      template <typename InputIterator>
      uint32_t PushBack(InputIterator firstIter, InputIterator lastIter)
      {
        uint32_t  rc = 0;
        if (this->Lock()) {
          if ((! this->_maxLength) ||
              (this->_queue.size() < this->_maxLength)) {
            while ((firstIter != lastIter) &&
                   ((! this->_maxLength) ||
                    (this->_queue.size() < this->_maxLength))) {
              this->_queue.push_back(*firstIter);
              ++firstIter;
              ++rc;
            }
            if (rc)
              this->_cv.Broadcast();
          }
          this->Unlock();
        }
        return(rc);
      }

      //----------------------------------------------------------------------
      //!  Inserts \c value on the front of the queue.  Returns true on
      //!  success, false on failure.
      //----------------------------------------------------------------------
      bool PushFront(const _ValueType & value)
      {
        bool  rc = false;
        if (this->Lock()) {
          if ((! this->_maxLength) ||
              (this->_queue.size() < this->_maxLength)) {
            this->_queue.push_front(value);
            this->_cv.Broadcast();
            rc = true;
          }
          this->Unlock();
        }
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Unblocks at least one thread waiting on the condition variable.
      //----------------------------------------------------------------------
      bool ConditionSignal()
      {
        return(_cv.Signal());
      }
     
      //----------------------------------------------------------------------
      //!  Waits for the condition variable to be signalled or broadcasted.
      //----------------------------------------------------------------------
      bool ConditionWait()
      {
        bool  rc = false;
        if (this->Lock()) {
          rc = _cv.Wait(_mutex);
          this->Unlock();
        }
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Waits for the condition variable to be signalled or broadcasted
      //!  for timeToWait to pass.  Returns true if the condition variable
      //!  was signalled or broadcasted, else returns false.
      //----------------------------------------------------------------------
      bool ConditionTimedWait(const struct timespec & timeToWait)
      {
        bool rc = false;
        struct timeval   tv;
        struct timezone  tz;
        gettimeofday(&tv,&tz);
        struct timespec  tspc;
        tspc.tv_sec = tv.tv_sec + timeToWait.tv_sec + 
          ((tv.tv_usec * 1000) + timeToWait.tv_nsec) / 1000000000;
        tspc.tv_nsec = ((tv.tv_usec * 1000) + timeToWait.tv_nsec) % 1000000000;
        if (this->Lock()) {
          rc = _cv.TimedWait(this->_mutex,&tspc);
          this->Unlock();
        }
       
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Pops the entry from the front of the queue and stores it in 
      //!  \c value.  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool PopFront(_ValueType & value)
      {
        bool  rc = false;
        if (this->Lock()) {
          if (! this->_queue.empty()) {
            value = this->_queue.front();
            this->_queue.pop_front();
            rc = true;
          }
          this->Unlock();
        }
        return(rc);
      }

      //----------------------------------------------------------------------
      //!  Pops the entry from the back of the queue and stores it in 
      //!  \c value.  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool PopBack(_ValueType & value)
      {
        bool  rc = false;
        if (this->Lock()) {
          if (! this->_queue.empty()) {
            value = this->_queue.back();
            this->_queue.pop_back();
            rc = true;
          }
          this->Unlock();
        }
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Blocks the calling thread until the queue contains at least one
      //!  entry.  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool WaitForNotEmpty()
      {
        bool  rc = false;
        if (this->Lock()) {
          while (this->_queue.empty()) {
            if (_cv.Wait(this->_mutex)) {
              break;
            }
          }
          rc = true;
          this->Unlock();
        }
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Waits \c timeToWait for the queue to be non-empty.  Returns true
      //!  if the queue is non-empty, else returns false.
      //----------------------------------------------------------------------
      bool TimedWaitForNotEmpty(const struct timespec & timeToWait)
      {
        bool  rc = false;
        if (this->_queue.empty()) {
          if (this->ConditionTimedWait(timeToWait)) {
            if (! this->_queue.empty()) {
              rc = true;
            }
          }
        }
        else {
          rc = true;
        }
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Returns true if the queue is empty, else returns false.
      //----------------------------------------------------------------------
      bool Empty()
      {
        return(this->_queue.empty());
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void RandomShuffle()
      {
        if (this->Lock()) {
          random_shuffle(this->_queue.begin(), this->_queue.end());
          this->Unlock();
        }
        return;
      }
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint32_t Copy(std::deque<_ValueType> & c)
      {
        uint32_t  rc = 0;
        if (! c.empty())
          c.clear();
       
        if (this->Lock()) {
          typename std::deque<_ValueType>::iterator  iter = this->_queue.begin();
          for ( ; iter != this->_queue.end(); ++iter) {
            c.push_back(*iter);
            ++rc;
          }
          this->Unlock();
        }
        return(rc);
      }
          
    protected:
      uint32_t                _maxLength;
      std::deque<_ValueType>  _queue;
      Mutex                   _mutex;
      ConditionVariable       _cv;
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool Lock()
      {
        return(_mutex.Lock());
      }
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool Unlock()
      {
        return(_mutex.Unlock());
      }
     
    };


  }  // namespace Pthread
  
}  // namespace Dwm

#endif  // _DWMPTHREADQUEUE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
