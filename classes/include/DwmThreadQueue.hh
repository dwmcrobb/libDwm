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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmThreadQueue.hh
//!  \brief Dwm::Thread::Queue class template definition.
//---------------------------------------------------------------------------

#ifndef _DWMTHREADQUEUE_HH_
#define _DWMTHREADQUEUE_HH_

#include <deque>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <type_traits>

namespace Dwm {

  namespace Thread {

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
          : _maxLength(0), _queue(), _mutex(), _lock(_mutex), _cv()
      {
        _lock.unlock();
      }
    
      //----------------------------------------------------------------------
      //!  Destructor
      //----------------------------------------------------------------------
      ~Queue()
      {
        std::unique_lock<std::mutex>  lk(_mutex);
        _queue.clear();
      }
     
      //----------------------------------------------------------------------
      //!  Returns the max length of the queue.  If 0, no maximum length will
      //!  be enforced.
      //----------------------------------------------------------------------
      uint32_t MaxLength() const
      {
        return(_maxLength);
      }
     
      //----------------------------------------------------------------------
      //!  Sets and returns the max length of the queue.  If 0, no maximum 
      //!  length will be enforced.
      //----------------------------------------------------------------------
      uint32_t MaxLength(uint32_t maxLength)
      {
        _maxLength = maxLength;
        return(_maxLength);
      }

      //----------------------------------------------------------------------
      //!  Returns the current length of the queue.
      //----------------------------------------------------------------------
      typename std::deque<_ValueType>::size_type Length() const
      {
        std::lock_guard<std::mutex>  lk(_mutex);
        return _queue.size();
      }
      
      //----------------------------------------------------------------------
      //!  Inserts \c value on the back of the queue.  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool PushBack(const _ValueType & value)
      {
        bool  rc = false;
        std::lock_guard<std::mutex>  lk(_mutex);
        if ((! _maxLength) ||
            (_queue.size() < _maxLength)) {
          _queue.push_back(value);
          _cv.notify_all();
          rc = true;
        }
        return(rc);
      }

      //----------------------------------------------------------------------
      //!  Inserts \c value on the back of the queue.  Returns true on success,
      //!  false on failure.
      //----------------------------------------------------------------------
      bool PushBack(_ValueType && value)
      {
        bool  rc = false;
        std::lock_guard<std::mutex>  lk(_mutex);
        if ((! _maxLength) ||
            (_queue.size() < _maxLength)) {
          _queue.push_back(value);
          _cv.notify_all();
          rc = true;
        }
        return(rc);
      }
      
      //----------------------------------------------------------------------
      //!  Inserts the values from firstIter to lastIter on the back of the
      //!  queue.  Note that lastIter is excluded, i.e. the range of entries
      //!  inserted is [firstIter,lastIter).  Returns the number of entries
      //!  inserted.
      //----------------------------------------------------------------------
      template <typename InputIterator>
      uint32_t PushBack(InputIterator firstIter, InputIterator lastIter)
      {
        uint32_t  rc = 0;
        if (firstIter != lastIter) {
          std::lock_guard<std::mutex>  lk(_mutex);
          if ((! _maxLength) ||
              (_queue.size() < _maxLength)) {
            uint32_t  oldSize = _queue.size();
            _queue.insert(_queue.end(), firstIter, lastIter);
            rc = _queue.size() - oldSize;
            if (rc) {
              _cv.notify_all();
            }
          }
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
        std::lock_guard<std::mutex>  lk(_mutex);
        if ((! _maxLength) ||
            (_queue.size() < _maxLength)) {
          _queue.push_front(value);
          _cv.notify_all();
          rc = true;
        }
        return(rc);
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      template <typename InputIterator>
      uint32_t PushFront(InputIterator firstIter, InputIterator lastIter)
      {
        uint32_t  rc = 0;
        if (firstIter != lastIter) {
          std::lock_guard<std::mutex>  lk(_mutex);
          if ((! _maxLength) ||
              (_queue.size() < _maxLength)) {
            uint32_t  oldSize = _queue.size();
            _queue.insert(_queue.begin(), firstIter, lastIter);
            rc = _queue.size() - oldSize;
            if (rc) {
              _cv.notify_all();
            }
          }
        }
        return rc;
      }
      
      //----------------------------------------------------------------------
      //!  Unblocks at least one thread waiting on the condition variable.
      //----------------------------------------------------------------------
      void ConditionSignal()
      {
        _cv.notify_one();
      }
     
      //----------------------------------------------------------------------
      //!  Waits for the condition variable to be signalled or broadcasted.
      //----------------------------------------------------------------------
      bool ConditionWait()
      {
        Lock();
        _cv.wait(_lock);
        Unlock();
        return true;
      }
     
      //----------------------------------------------------------------------
      //!  Waits for the condition variable to be signalled or broadcasted
      //!  for timeToWait to pass.  Returns true if the condition variable
      //!  was signalled or broadcasted, else returns false.
      //----------------------------------------------------------------------
      template<class Rep, class Period>
      bool ConditionTimedWait(const std::chrono::duration<Rep,Period> & timeToWait)
      {
        bool rc = false;
        Lock();
        if (_cv.wait_for(_lock, timeToWait) == std::cv_status::no_timeout) {
          rc = true;
        }
        Unlock();
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Pops the entry from the front of the queue and stores it in 
      //!  \c value.  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool PopFront(_ValueType & value)
      {
        bool  rc = false;
        std::lock_guard<std::mutex>  lk(_mutex);
        if (! _queue.empty()) {
          if (std::is_move_assignable<_ValueType>::value) {
            value = std::move(_queue.front());
          }
          else {
            value = _queue.front();
          }
          _queue.pop_front();
          rc = true;
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
        std::lock_guard<std::mutex>  lk(_mutex);
        if (! _queue.empty()) {
          if (std::is_move_assignable<_ValueType>::value) {
            value = std::move(_queue.back());
          }
          else {
            value = _queue.back();
          }
          _queue.pop_back();
          rc = true;
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
        Lock();
        while (_queue.empty()) {
          _cv.wait(_lock);
        }
        rc = true;
        Unlock();
        return(rc);
      }
     
      //----------------------------------------------------------------------
      //!  Waits \c timeToWait for the queue to be non-empty.  Returns true
      //!  if the queue is non-empty, else returns false.
      //----------------------------------------------------------------------
      template <class Rep, class Period>
      bool TimedWaitForNotEmpty(const std::chrono::duration<Rep, Period> & timeToWait)
      {
        bool  rc = false;
        if (_queue.empty()) {
          if (ConditionTimedWait(timeToWait)) {
            if (! _queue.empty()) {
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
        return(_queue.empty());
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void RandomShuffle()
      {
        std::lock_guard<std::mutex>  lk(_mutex);
        random_shuffle(_queue.begin(), _queue.end());
        return;
      }
     
      //----------------------------------------------------------------------
      //!  Copies the contents of the queue to @c c.  Returns the number of
      //!  elements copied.  Note that @c c will always be cleared before
      //!  elements are copied.  Hence if there are no entries in the queue,
      //!  @c c will be empty on return.
      //----------------------------------------------------------------------
      uint32_t Copy(std::deque<_ValueType> & c)
      {
        uint32_t  rc = 0;
        if (! c.empty())
          c.clear();
       
        std::lock_guard<std::mutex>  lk(_mutex);
        typename std::deque<_ValueType>::iterator  iter = _queue.begin();
        for ( ; iter != _queue.end(); ++iter) {
          c.push_back(*iter);
          ++rc;
        }
        return(rc);
      }

      //----------------------------------------------------------------------
      //!  This member is a simple optimization for fetching the contents
      //!  of the queue.  It swaps the encapsulated queue with @c c, then
      //!  clears the contents of the encapsulated queue.  It returns the
      //!  number of entries in @c c after the swap, i.e. the number of
      //!  entries that were in the encapsulated queue before this member
      //!  was called.
      //----------------------------------------------------------------------
      uint32_t Swap(std::deque<_ValueType> & c)
      {
        std::lock_guard<std::mutex>  lk(_mutex);
        if (! _queue.empty()) {
          _queue.swap(c);
          _queue.clear();
        }
        return(c.size());
      }
      
    protected:
      uint32_t                      _maxLength;
      std::deque<_ValueType>        _queue;
      mutable std::mutex            _mutex;
      std::unique_lock<std::mutex>  _lock;
      std::condition_variable       _cv;
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void Lock()
      {
        _lock.lock();
        return;
      }
     
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      void Unlock()
      {
        _lock.unlock();
        return;
      }
     
    };


  }  // namespace Thread
  
}  // namespace Dwm

#endif  // _DWMTHREADQUEUE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
