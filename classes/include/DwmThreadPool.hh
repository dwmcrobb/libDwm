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

#include <array>
#include <concepts>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <type_traits>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulate a trivial thread pool.  @c N is the number of threads and
  //!  @c T is a functor type that takes @c Args when invoked.
  //!
  //!  Work is added to the pool via AddTask().
  //--------------------------------------------------------------------------
  template <size_t N, typename F, typename... Args>
  requires (std::invocable<F,Args...> && std::is_copy_assignable_v<F>
            && (std::is_copy_assignable_v<Args> && ...))
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
        _workers[i] = std::thread(&ThreadPool::WorkerThread, this);
      }
    }

    //------------------------------------------------------------------------
    //!  Adds a task to be executed by the thread pool.  @c fn is a function
    //!  object and @c args are the arguments to the function.
    //------------------------------------------------------------------------
    void AddTask(F fn, Args ...args)
    {
      {
        std::lock_guard<std::mutex>  lock(_mtx);
        _tasks.emplace_back(Task(fn,std::make_tuple(args...)));
      }
      _cv.notify_one();
      return;
    }

    //------------------------------------------------------------------------
    //!  Shuts down the threads.  Note that we will wait for all tasks
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
    //------------------------------------------------------------------------
    //!  Encapsulate a task to be executed by a worker thread.  Just need to
    //!  hold the function object and the function arguments.
    //------------------------------------------------------------------------
    class Task
    {
    public:
      Task() = default;
      Task(const Task & t) = default;
      Task(const Task && t) = default;
      Task(F f, std::tuple<Args...> && a) : _fn(f), _args(a) { }
      Task & operator = (const Task &) = default;
      Task & operator = (Task &&) = default;
      
      //----------------------------------------------------------------------
      //!
      //----------------------------------------------------------------------
      inline void Execute()
      {
        return CallFunc(std::make_index_sequence<sizeof...(Args)>());
      }
      
      //----------------------------------------------------------------------
      //!
      //----------------------------------------------------------------------
      template<size_t ...S>
      void CallFunc(std::index_sequence<S...>)
      {
        _fn(std::get<S>(_args) ...);
        return;
      }

    private:
      F                    _fn;
      std::tuple<Args...>  _args;
    };

    std::array<std::thread,N>  _workers;
    std::deque<Task>           _tasks;
    std::mutex                 _mtx;
    std::condition_variable    _cv;
    bool                       _run;

    //------------------------------------------------------------------------
    //!  The thread we'll run for each worker.
    //------------------------------------------------------------------------
    void WorkerThread()
    {
      //  Just a lambda we use when waiting on the condition variable.
      //  We want to wait for a request to shut down or work to do.
      auto  waitFor = 
        [this] { return ((! this->_run) || (! this->_tasks.empty())); };
        
      for (;;) {
        Task  task;
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
        task.Execute();
      }
    }
    
  };
  
}  // namespace Dwm

#endif  // _DWMTHREADPOOL_HH_
