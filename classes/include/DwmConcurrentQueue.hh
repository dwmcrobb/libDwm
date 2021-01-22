//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmConcurrentQueue.hh 9014 $
// @(#) $Id: DwmConcurrentQueue.hh 9014 2017-04-11 09:53:02Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016
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
//!  \file DwmConcurrentQueue.hh
//!  \brief Dwm::Thread::ConcurrentQueue class template definition
//---------------------------------------------------------------------------

#ifndef _DWMCONCURRENTQUEUE_HH_
#define _DWMCONCURRENTQUEUE_HH_

extern "C" {
  #include <unistd.h>
}

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace Dwm {

  namespace Thread {

    //------------------------------------------------------------------------
    //!  Concurrent queue template.  This is based on an old article from
    //!  Herb Sutter in Dr. Dobb's Journal, with modifications to support
    //!  pushing and popping more than one entry at a time and reduction of
    //!  the number of pointers.
    //!  
    //!  The operations here are not wait-free, but I've tried to minimize
    //!  contention.  There is no contention between a producer
    //!  (caller of PushBack()) and a consumer (caller of PopFront()).
    //!  Short periods of contention will exist among producers.  Short
    //!  periods of contention will exist among consumers.
    //!  
    //!  In my typical use, I only have one producer and one consumer
    //!  per Queue.  Occasionally I have the need for one producer and
    //!  multiple consumers (for example, one producer feeding 'work' to
    //!  multiple 'worker' threads).  I rarely have a need for a single
    //!  queue with multiple producers.  If I need multiple producers, I
    //!  almost always need (or want) multiple consumers.  In those
    //!  cases it almost always makes sense to have multiple queues,
    //!  each with a single producer and a single consumer.  For
    //!  multiple wait-free producers, you've already thrown out the
    //!  notion of ordered processing since you're not synchronizing the
    //!  producers (else they'd be waiting for synchronization points).
    //!  In most such cases, the solution with the most concurrency is
    //!  multiple queues.
    //!
    //!  Of course there are exceptions.  For example, you might want one
    //!  producer to feed a set of consumers who then act as producers to
    //!  feed one consumer (break a problem apart for multiple worker
    //!  threads, then put it back together with a single consumer thread).
    //!  But again, you've already thrown out strict ordering; that final
    //!  consumer might as well be reading from multiple queues.
    //------------------------------------------------------------------------
    template <typename T>
    class ConcurrentQueue
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructor.  @c maxLength is the maximum depth of the queue.
      //!  When the queue reaches this depth, we will sleep @c pushSleepUsecs
      //!  at a time until the queue is shallower than @c maxLength.  This
      //!  allows a consuming thread to catch up with PopFront().
      //----------------------------------------------------------------------
      ConcurrentQueue(uint64_t maxLength = 10000,
                      useconds_t pushSleepUsecs = 1000)
          :  _length(0), _maxLength(maxLength),
             _pushSleepUsecs(pushSleepUsecs)
      {
        _front = _back = new Entry(T());
        _pushingLocked = _poppingLocked = false;
      }
      
      //----------------------------------------------------------------------
      //!  Destructor.  Walks the queue and deletes all entries.
      //----------------------------------------------------------------------
      ~ConcurrentQueue()
      {
        while (_front != nullptr) {
          Entry *tmp = _front;
          _front = tmp->next;
          delete tmp;
        }
      }
      
      //----------------------------------------------------------------------
      //!  If the queue is not empty, pops the front entry into @c result
      //!  and returns true.  If the queue is empty, returns false.  A caller
      //!  may choose to sleep a short period of time when false is returned.
      //----------------------------------------------------------------------
      bool PopFront(T & result)
      {
        while (_poppingLocked.exchange(true));
        if (_front->next != nullptr) {
          //  queue is not empty.
          Entry  *oldFirst = _front;
          _front = _front->next;
          result = _front->value;
          _poppingLocked = false;
          //  delete the old front Entry.
          delete oldFirst;
          //  decrement the queue length.
          --_length;
          return true;
        }
        _poppingLocked = false;
        return false;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool PopFront(std::vector<T> & result)
      {
        typename std::vector<T>::size_type  numEntries = 0;
        //  lcok against others popping from the queue
        while (_poppingLocked.exchange(true));
        if (_front->next != nullptr) {
          //  queue is not empty.  Save the existing front.
          Entry  *oldFront = _front;
          //  Move front to the end of the queue since we're going to take
          //  all entries from the queue.  Keep track of how many entries
          //  we'll take, and adjust _length at end.
          while (_front->next != nullptr) {
            _front = _front->next;
            ++numEntries;
          }
          _length -= numEntries;
          //  Save the new location of _front.  This is where we'll stop
          //  popping entries.  Once we unlock popping, another thread
          //  could come in and move _front.
          Entry  *newFront = _front;
          //  Unlock popping so other threads can pop entries.
          _poppingLocked = false;
          //  Resize the output vector to hold the popped entries.
          result.resize(numEntries);
          //  Populate the output vector and delete the old entries.
          typename std::vector<T>::size_type  i = 0;
          Entry  *entry = oldFront->next;
          while (entry != newFront) {
            Entry  *delEntry = entry;
            result[i] = entry->value;
            entry = entry->next;
            ++i;
            delete delEntry;
          }
          //  Delete the old front entry.
          delete oldFront;
          return true;
        }
        _poppingLocked = false;
        return false;
      }

      //----------------------------------------------------------------------
      //!  Waits for the queue to not be full (see the Queue constructor),
      //!  then pushes @c t onto the back of the queue and returns true.
      //----------------------------------------------------------------------
      bool PushBack(const T & t)
      {
        while (_length > _maxLength) { usleep(_pushSleepUsecs); };
        Entry *tmp = new Entry(t);
        while (_pushingLocked.exchange(true));
        _back->next = tmp;
        _back = tmp;
        _pushingLocked = false;
        ++_length;
        return true;
      }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool PushBack(const std::vector<T> & t)
      {
        while (_length > _maxLength) { usleep(_pushSleepUsecs); }
        std::vector<Entry *> newEntries(t.size());
        typename std::vector<T>::size_type i = 0;
        newEntries[i] = new Entry(t[i]);
        ++i;
        for (; i < t.size(); ++i) {
          newEntries[i] = new Entry(t[i]);
          newEntries[i-1]->next = newEntries[i];
        }
        while (_pushingLocked.exchange(true));
        _back->next = newEntries[0];
        _back = newEntries[t.size() - 1];
        _pushingLocked = false;
        _length += t.size();
        return true;
      }

    private:
      //----------------------------------------------------------------------
      //!  Class to encapsulate a single entry in the queue.
      //----------------------------------------------------------------------
      class Entry
      {
      public:
        Entry(const T & val) 
            : value(val), next(nullptr) 
        {}
        T                      value;
        std::atomic<Entry *>   next;
      };

      Entry                  *_front;
      std::atomic<bool>       _poppingLocked;
      Entry                  *_back;
      std::atomic<bool>       _pushingLocked;
      std::atomic<uint64_t>   _length;
      uint64_t                _maxLength;
      useconds_t              _pushSleepUsecs;
    };
    

  }  // namespace Thread
  
}  // namespace Dwm

#endif  // _DWMCONCURRENTQUEUE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
