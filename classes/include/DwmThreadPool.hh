//===========================================================================
// @(#) $DwmPath$
// @(#) $Id$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file DwmThreadPool.hh
//!  \brief Dwm::ThreadPool class template
//---------------------------------------------------------------------------

#ifndef _DWMTHREADPOOL_HH_
#define _DWMTHREADPOOL_HH_

#include <concepts>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulate a trivial thread pool.  @c N is the number of threads and
  //!  @c T is a functor type.  At the moment I only handle functors with no
  //!  arguments.  Hence if you want to call a function that takes arguments,
  //!  you need to wrap it in a class with a function call operator that
  //!  can pass your arguments internally and use that class as @c T.
  //--------------------------------------------------------------------------
  template <size_t N, typename T>
  requires std::invocable<T>
  class ThreadPool
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructs the thread pool and starts the threads.
    //------------------------------------------------------------------------
    ThreadPool()
        : _workers(), _tasks(), _mtx(), _cv(), _run(true)
    {
      for (size_t i = 0; i < N; ++i) {
        _workers.emplace_back(std::thread(&ThreadPool::WorkerThread,this));
      }
    }

    //------------------------------------------------------------------------
    //!  Adds a task to be executed by the thread pool.
    //------------------------------------------------------------------------
    void Enqueue(T task)
    {
      {
        std::lock_guard<std::mutex>  lock(_mtx);
        _tasks.push_back(task);
      }
      _cv.notify_one();
      return;
    }

    //------------------------------------------------------------------------
    //!  Shuts down the thread pool.  Note that we will wait for all tasks
    //!  to be completed.
    //------------------------------------------------------------------------
    void Shutdown()
    {
      {
        std::lock_guard<std::mutex>  lock(_mtx);
        _run = false;
      }
      _cv.notify_all();
      for (std::thread & worker : _workers) {
        worker.join();
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~ThreadPool()
    {
      if (_run) {
        Shutdown();
      }
    }
    
  private:
    std::vector<std::thread>   _workers;
    std::deque<T>              _tasks;
    std::mutex                 _mtx;
    std::condition_variable    _cv;
    bool                       _run;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void WorkerThread()
    {
      //----------------------------------------------------------------------
      //!  Just a lambda we use when waiting on the condition variable.
      //----------------------------------------------------------------------
      auto  waitFor = 
        [this] { return ((! this->_run) || (! this->_tasks.empty())); };
        
      for (;;) {
        T  task;
        {
          std::unique_lock<std::mutex>  lock(this->_mtx);
          this->_cv.wait(lock, waitFor);
          if ((! this->_run) && (this->_tasks.empty())) {
            //  We've been asked to stop and there are no more tasks.
            return;
          }
          //  Get the task at the front of the queue.
          task = this->_tasks.front();
          this->_tasks.pop_front();
        }
        //  Execute the task.
        task();
      }
    }
    
  };
  
}  // namespace Dwm

#endif  // _DWMTHREADPOOL_HH_
