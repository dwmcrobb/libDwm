//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008, 2009, 2016, 2018
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
//!  \file DwmLoadBalancer.hh
//!  \brief Dwm::LoadBalancer class template definition
//---------------------------------------------------------------------------

#ifndef _DWMLOADBALANCER_HH_
#define _DWMLOADBALANCER_HH_

#include <algorithm>
#include <atomic>
#include <deque>
#include <chrono>
#include <thread>
#include <vector>

#include "DwmThreadQueue.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A simple load balancer class template which balances work across
  //!  Worker objects that each run in their own thread.
  //!
  //!  A user would derive a worker class from LoadBalancer<ItemT>::Worker,
  //!  and implement the Worker::ProcessWork(ItemT) member.  The worker can
  //!  be added to a LoadBalancer via the LoadBalancer::AddWorker() member.
  //!  Work can be added to the LoadBalancer with AddWork(ItemT).  A unit of
  //!  work is encapsulated in the ItemT template parameter.  This is a
  //!  template so that any copyable type can be used as a unit of work.
  //!
  //!  Today, this class template may not be all that useful (we have
  //!  the concurrency features of C++11 and C++14).  I originally wrote
  //!  this class using pthreads, before we had C++11 and C++14.  It's
  //!  still in use in some of my code, but I may revisit this soon and
  //!  modernize it.  For now the only modernization is the migration to
  //!  std::thread.
  //--------------------------------------------------------------------------
  template <typename ItemT>
  class LoadBalancer
  {
  public:
    //------------------------------------------------------------------------
    //!  Worker class for LoadBalancer.
    //------------------------------------------------------------------------
    class Worker
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructs the worker.  @c waitForNotEmptyMicroseconds is the
      //!  period of time to wait for the work queue to become non-empty
      //!  before checking again whether or not to continue.
      //----------------------------------------------------------------------
      Worker(std::chrono::microseconds waitNotEmptyMicroseconds = std::chrono::microseconds(1000))
          : _workQueue(), _keepRunning(false), _thread(),
            _waitUsecs(waitNotEmptyMicroseconds)
      {}

      //----------------------------------------------------------------------
      //!  Destructor.  Stops the worker thread.
      //----------------------------------------------------------------------
      virtual ~Worker()
      {
        Stop();
      }
      
      //----------------------------------------------------------------------
      //!  Adds a work item for the worker.  Returns true on success, false
      //!  on failure.  A reason for failure: the work queue is full.
      //----------------------------------------------------------------------
      bool AddWork(const ItemT & item)
      {
        return _workQueue.PushBack(item);
      }
      
      //----------------------------------------------------------------------
      //!  Adds work items for the worker.  Returns true on success, false     
      //!  on failure.  Reasons for failure: the work queue is full, or
      //!  @c lastIter is equal to @c firstIter.
      //----------------------------------------------------------------------
      template <typename InputIterator>
      bool AddWork(InputIterator firstIter, InputIterator lastIter)
      {
        return (_workQueue.PushBack(firstIter, lastIter) > 0);
      }
      
      //----------------------------------------------------------------------
      //!  Returns the current length of the worker's work queue.
      //----------------------------------------------------------------------
      uint32_t QueueLength() const
      {
        return _workQueue.Length();
      }
      
      //----------------------------------------------------------------------
      //!  Returns true if the worker is ready for more work (has room in
      //!  its work queue and is running).
      //----------------------------------------------------------------------
      bool ReadyForWork() const
      {
        return (_keepRunning
                && ((! _workQueue.MaxLength())
                    || (_workQueue.Length() < _workQueue.MaxLength())));
      }

      //----------------------------------------------------------------------
      //!  Returns true if the worker is ready for @c numEntries units of
      //!  work.
      //----------------------------------------------------------------------
      bool ReadyForWork(size_t numEntries) const
      {
        return (_keepRunning
                && ((! _workQueue.MaxLength())
                    || ((_workQueue.Length() + numEntries)
                        <= _workQueue.MaxLength())));
      }

      //----------------------------------------------------------------------
      //!  Sets the maximum length of the worker's work queue.
      //----------------------------------------------------------------------
      void MaxWork(uint32_t maxItems)
      {
        _workQueue.MaxLength(maxItems);
      }
      
      //----------------------------------------------------------------------
      //!  Starts the worker.
      //----------------------------------------------------------------------
      bool Start()
      {
        bool  rc = false;
        _keepRunning = true;
        _thread = std::thread(&Worker::Run, this);
        rc = true;
        return rc;
      }
      
      //----------------------------------------------------------------------
      //!  Stops the worker.
      //----------------------------------------------------------------------
      void Stop()
      {
        _keepRunning = false;
        if (_thread.joinable()) {
          _thread.join();
        }
        return;
      }

      //----------------------------------------------------------------------
      //!  Returns true if the worker's thread is running.
      //----------------------------------------------------------------------
      bool IsRunning()
      {
        return (_thread.joinable());
      }
      
      //----------------------------------------------------------------------
      //!  Runs the worker thread.
      //----------------------------------------------------------------------
      void Run()
      {
        while (_keepRunning || (_workQueue.Length())) {
          if (_workQueue.TimedWaitForNotEmpty(_waitUsecs.load())) {
            std::deque<ItemT>  myCopy;
            _workQueue.Swap(myCopy);
            //  try processing in bulk
            if (! ProcessWork(myCopy)) {
              //  else fall back to one at a time.
              for (auto i : myCopy) {
                ProcessWork(i);
              }
            }
          }
        }
        return;
      }
      
      //----------------------------------------------------------------------
      //!  Pure virtual member to process a single work item.  Obviously
      //!  must be implemented by derived classes.
      //----------------------------------------------------------------------
      virtual void ProcessWork(ItemT & item) = 0;

      //----------------------------------------------------------------------
      //!  Process a deque of work items.  The default implementation does
      //!  nothing and returns @c false.  If you impelement this in your
      //!  derived class, it should return @c true.
      //----------------------------------------------------------------------
      virtual bool ProcessWork(std::deque<ItemT> & items)
      {
        return false;
      }

      //----------------------------------------------------------------------
      //!  Returns the microseconds we'll wait for queue to be non-empty
      //!  in our worker thread.  This is basically the longest possible
      //!  time that our worker thread will be idle.
      //----------------------------------------------------------------------
      std::chrono::microseconds WaitUsecs() const
      {
        return _waitUsecs;
      }
      
    protected:
      Thread::Queue<ItemT>                    _workQueue;
      std::atomic<bool>                       _keepRunning;
      std::thread                             _thread;
      std::atomic<std::chrono::microseconds>  _waitUsecs;
    };

    //------------------------------------------------------------------------
    //!  Adds the given worker to the load balancer.  Note that on return,
    //!  the caller no longer owns @c worker.  It is assigned to a
    //!  unique_ptr that is owned by the LoadBalancer.
    //------------------------------------------------------------------------
    void AddWorker(Worker *worker)
    {
      std::lock_guard<std::mutex>  lock(_mtx);
      _workers.push_back(std::unique_ptr<Worker>(worker));
      return;
    }

    //------------------------------------------------------------------------
    //!  Adds work to be done with load balancing.  Returns true on success,
    //!  false on failure.
    //------------------------------------------------------------------------
    bool AddWork(ItemT item)
    {
      bool  workAdded = false;
      {
        std::lock_guard<std::mutex>  lock(_mtx);
        if (_workers.empty()) {
          //  No workers to perform work!!!
          return false;
        }
      }
      while (! workAdded) {
        {
          //  in separate scope so we don't hold lock for too long
          std::lock_guard<std::mutex>  lock(_mtx);
          auto  w = std::min_element(_workers.begin(), _workers.end(),
                                     [&] (const std::unique_ptr<Worker> & a,
                                          const std::unique_ptr<Worker> & b)
                                     { return (a->QueueLength()
                                               < b->QueueLength()); });
          if (w != _workers.end()) {
            workAdded = (*w)->AddWork(item);
          }
        }
        if (! workAdded) {
          std::this_thread::sleep_for(MinimumWaitForWorkerReady());
        }
      }
      return workAdded;
    }

    //------------------------------------------------------------------------
    //!  Adds work to be done with load balancing.
    //------------------------------------------------------------------------
    template <typename InputIterator>
    bool AddWork(InputIterator firstIter, InputIterator lastIter)
    {
      bool  workAdded = false;
      {
        std::lock_guard<std::mutex>  lock(_mtx);
        if (_workers.empty()) {
          //  No workers to perform work!!!
          return false;
        }
      }
      while (! WorkerReady(lastIter - firstIter)) {
        std::this_thread::sleep_for(std::chrono::microseconds(MinimumWaitForWorkerReady()));
      }

      std::lock_guard<std::mutex>  lock(_mtx);
      auto  w = std::min_element(_workers.begin(), _workers.end(),
                                 [&] (const std::unique_ptr<Worker> & a,
                                      const std::unique_ptr<Worker> & b)
                                 { return (a->QueueLength() < b->QueueLength()); });
      return (*w)->AddWork(firstIter, lastIter);
    }

    //------------------------------------------------------------------------
    //!  Calls Worker::Stop() on all encapsulated Worker objects.
    //------------------------------------------------------------------------
    void Stop()
    {
      std::lock_guard<std::mutex>  lock(_mtx);
      for (auto & w : _workers) {
        w->Stop();
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  Returns a const reference to the encapsulated workers.  NOT thread
    //!  safe!  This is mostly just for unit testing.
    //------------------------------------------------------------------------
    const std::vector<std::unique_ptr<Worker>> & Workers() const
    {
      return _workers;
    }
    
  private:
    mutable std::mutex                    _mtx;
    std::vector<std::unique_ptr<Worker>>  _workers;
    
    //------------------------------------------------------------------------
    //!  Returns true if there is a worker ready to do work.
    //------------------------------------------------------------------------
    bool WorkerReady()
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lock(_mtx);
      for (auto & w : _workers) {
        if (w->ReadyForWork()) {
          rc = true;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Returns true if there's a worker that is ready for @c numEntries
    //!  of work.
    //------------------------------------------------------------------------
    bool WorkerReady(size_t numEntries) const
    {
      bool  rc = false;
      std::lock_guard<std::mutex>  lock(_mtx);
      for (auto & w : _workers) {
        if (w->ReadyForWork(numEntries)) {
          rc = true;
          break;
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Returns the shortest wait time among our workers.
    //------------------------------------------------------------------------
    std::chrono::microseconds MinimumWaitForWorkerReady() const
    {
      std::chrono::microseconds  rc(1000);
      std::lock_guard<std::mutex>  lock(_mtx);
      auto  w = std::min_element(_workers.begin(), _workers.end(),
                                 [&] (const std::unique_ptr<Worker> & a,
                                      const std::unique_ptr<Worker> & b)
                                 { return (a->WaitUsecs() < b->WaitUsecs())\
; });
      if (w != _workers.end()) {
        rc = std::chrono::microseconds((*w)->WaitUsecs());
      }
      return rc;
    }
    
  };
  
}  // namespace Dwm


#endif  // _DWMLOADBALANCER_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
