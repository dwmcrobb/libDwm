//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmLoadBalancer.cc 10170 $
// @(#) $Id: TestDwmLoadBalancer.cc 10170 2018-07-23 10:06:25Z dwm $
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
//!  \file TestDwmLoadBalancer.cc
//!  \brief Unit tests for Dwm::LoadBalancer class template
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <numeric>
#include <atomic>

#include "DwmLoadBalancer.hh"
#include "DwmOptArgs.hh"
#include "DwmSvnTag.hh"
#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmLoadBalancer.cc 10170 $");

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
class MyWorker
  : public Dwm::LoadBalancer<uint64_t>::Worker

{
public:
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MyWorker(const std::string & name)
      : Dwm::LoadBalancer<uint64_t>::Worker(std::chrono::microseconds(500)),
      _name(name), _itemsProcessed(0)
  {
    MaxWork(10000);
    Start();
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void ProcessWork(uint64_t & item)
  {
    ++_itemsProcessed;
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  inline bool ProcessWork(std::deque<uint64_t> & items)
  {
    _itemsProcessed += items.size();
    return true;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t ItemsProcessed() const
  {
    return _itemsProcessed;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Name() const
  {
    return _name;
  }
  
private:
  std::string  _name;
  uint64_t     _itemsProcessed;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
class MyPointerWorker
  : public Dwm::LoadBalancer<uint64_t *>::Worker

{
public:
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MyPointerWorker(const std::string & name)
      : Dwm::LoadBalancer<uint64_t *>::Worker(std::chrono::microseconds(500)),
      _name(name), _itemsProcessed(0)
  {
    MaxWork(10000);
    Start();
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void ProcessWork(uint64_t * & item)
  {
    if (nullptr != item) {
      ++_itemsProcessed;
      delete item;
      item = nullptr;
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  inline bool ProcessWork(std::deque<uint64_t *> & items)
  {
    _itemsProcessed += items.size();
    for (auto & it : items) {
      if (nullptr != it) {
        delete it;
        it = nullptr;
      }
    }
    return true;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t ItemsProcessed() const
  {
    return _itemsProcessed;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Name() const
  {
    return _name;
  }
  
private:
  std::string  _name;
  uint64_t     _itemsProcessed;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool TestPointerSingle(unsigned int numWorkers, bool printPerformance)
{
  bool  rc = true;
  //--------------------------------------------------------------------------
  // Do the same, but with pointers as work items
  //--------------------------------------------------------------------------
  Dwm::LoadBalancer<uint64_t *>  ptrLoadBalancer;

  //  Create the workers and add them to the load balancer.
  for (unsigned int i = 0; i < numWorkers; ++i) {
    MyPointerWorker  *worker = new MyPointerWorker("lw" + std::to_string(i+1));
    ptrLoadBalancer.AddWorker(worker);
  }

  //  Test that all workers are running
  for (auto & worker : ptrLoadBalancer.Workers()) {
    rc &= UnitAssert(worker->IsRunning());
  }

  Dwm::TimeValue  startTime(true);

  //  Feed the work to the load balancer.
  for (uint64_t i = 0; i < 100000; ++i) {
    rc &= UnitAssert(ptrLoadBalancer.AddWork(new uint64_t(i)));
  }

  //  Stop the load balancer (hence all of the workers)
  ptrLoadBalancer.Stop();

  //  Record the elapsed time to do the work.
  Dwm::TimeValue  elapsedTime(true);
  elapsedTime -= startTime;

  //  Test that all of the workers have been stopped.
  for (auto & worker : ptrLoadBalancer.Workers()) {
    rc &= UnitAssert(! worker->IsRunning());
    UnitAssert(! worker->ReadyForWork());
  }

  //  Test that all of the work was done.
  auto  total = std::accumulate(ptrLoadBalancer.Workers().begin(),
                                ptrLoadBalancer.Workers().end(),
                                0,
                                [&] (const uint64_t & cur,
                                     const std::unique_ptr<Dwm::LoadBalancer<uint64_t *>::Worker> & worker)
                                { const MyPointerWorker *wp =
                                    static_cast<const MyPointerWorker *>(worker.get());
                                  return cur + wp->ItemsProcessed(); 
                                });
  rc &= UnitAssert(100000 == total);
  
  if (printPerformance) {
    //  Print some performance data.
    std::cout.imbue(std::locale(""));
    std::cout << "PointerSingle " << (double)elapsedTime
              << " seconds elapsed, "
              << (uint64_t)(((uint64_t)10000 * 10000) / (double)elapsedTime)
              << " entries/second\n";
    for (auto & worker : ptrLoadBalancer.Workers()) {
      auto  myWorker = static_cast<const MyPointerWorker *>(worker.get());
      std::cout << myWorker->Name() << ": "
                << myWorker->ItemsProcessed() << " items\n";
    }
  }

  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool TestPointerBulk(unsigned int numWorkers, bool printPerformance)
{
  bool  rc = true;
  
  Dwm::LoadBalancer<uint64_t *>  ptrLoadBalancer;

  //  Create the workers and add them to the load balancer.
  for (unsigned int i = 0; i < numWorkers; ++i) {
    MyPointerWorker  *worker = new MyPointerWorker("lw" + std::to_string(i+1));
    ptrLoadBalancer.AddWorker(worker);
  }

  //  Test that all workers are running
  for (auto & worker : ptrLoadBalancer.Workers()) {
    rc &= UnitAssert(worker->IsRunning());
  }

  Dwm::TimeValue  startTime(true);

  //  Feed the work to the load balancer.
  std::vector<uint64_t *>  nums(1000);
  for (uint64_t i = 0; i < 1000; ++i) {
    for (auto & ni : nums) {
      ni = new uint64_t(i);
    }
    rc &= UnitAssert(ptrLoadBalancer.AddWork(nums.begin(), nums.end()));
  }

  //  Stop the load balancer (hence all of the workers)
  ptrLoadBalancer.Stop();

  //  Record the elapsed time to do the work.
  Dwm::TimeValue  elapsedTime(true);
  elapsedTime -= startTime;

  //  Test that all of the workers have been stopped.
  for (auto & worker : ptrLoadBalancer.Workers()) {
    rc &= UnitAssert(! worker->IsRunning());
    UnitAssert(! worker->ReadyForWork());
  }

  //  Test that all of the work was done.
  auto  total = std::accumulate(ptrLoadBalancer.Workers().begin(),
                                ptrLoadBalancer.Workers().end(),
                                0,
                                [&] (const uint64_t & cur,
                                     const std::unique_ptr<Dwm::LoadBalancer<uint64_t *>::Worker> & worker)
                                { const MyPointerWorker *wp =
                                    static_cast<const MyPointerWorker *>(worker.get());
                                  return cur + wp->ItemsProcessed(); 
                                });
  rc &= UnitAssert(1000000 == total);
  
  if (printPerformance) {
    //  Print some performance data.
    std::cout.imbue(std::locale(""));
    std::cout << "PointerBulk " << (double)elapsedTime
              << " seconds elapsed, "
              << (uint64_t)(((uint64_t)10000 * 1000) / (double)elapsedTime)
              << " entries/second\n";
    for (auto & worker : ptrLoadBalancer.Workers()) {
      auto  myWorker = static_cast<const MyPointerWorker *>(worker.get());
      std::cout << myWorker->Name() << ": "
                << myWorker->ItemsProcessed() << " items\n";
    }
  }

  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::OptArgs  optargs;
  optargs.AddOptArg("p", "performance", false, "false",
                    "run performance tests");
  optargs.Parse(argc, argv);

  Dwm::LoadBalancer<uint64_t>  loadBalancer;
  unsigned int  numWorkers = std::thread::hardware_concurrency();
  if (numWorkers < 4) {
    numWorkers = 4;
  }
  else {
    --numWorkers;
  }

  //  Create the workers and add them to the load balancer.
  for (unsigned int i = 0; i < numWorkers; ++i) {
    MyWorker  *worker = new MyWorker("lw" + std::to_string(i+1));
    loadBalancer.AddWorker(worker);
  }

  //  Test that all workers are running
  for (auto & worker : loadBalancer.Workers()) {
    UnitAssert(worker->IsRunning());
  }

  //  Create a vector of work items
  std::vector<uint64_t>  nums(10000);
  srand(12379);
  for (int j = 0; j < 10000; ++j) {
    nums[j] = rand();
  }

  Dwm::TimeValue  startTime(true);

  //  Feed the work to the load balancer.
  for (int i = 0; i < 10000; ++i) {
    UnitAssert(loadBalancer.AddWork(nums.begin(), nums.end()));
  }
  //  Stop the load balancer (hence all of the workers)
  loadBalancer.Stop();

  //  Record the elapsed time to do the work.
  Dwm::TimeValue  elapsedTime(true);
  elapsedTime -= startTime;

  //  Test that all of the workers have been stopped.
  for (auto & worker : loadBalancer.Workers()) {
    UnitAssert(! worker->IsRunning());
    UnitAssert(! worker->ReadyForWork());
  }

  //  Test that all of the work was done.
  auto  total = std::accumulate(loadBalancer.Workers().begin(),
                                loadBalancer.Workers().end(),
                                0,
                                [&] (const uint64_t & cur,
                                     const std::unique_ptr<Dwm::LoadBalancer<uint64_t>::Worker> & worker)
                                { const MyWorker *wp =
                                    static_cast<const MyWorker *>(worker.get());
                                  return cur + wp->ItemsProcessed(); 
                                });
  UnitAssert(100000000 == total);
  
  if (optargs.Get<bool>('p')) {
    //  Print some performance data.
    std::cout.imbue(std::locale(""));
    std::cout << (double)elapsedTime << " seconds elapsed, "
              << (uint64_t)(((uint64_t)10000 * 10000) / (double)elapsedTime)
              << " entries/second\n";
    for (auto & worker : loadBalancer.Workers()) {
      auto  myWorker = static_cast<const MyWorker *>(worker.get());
      std::cout << myWorker->Name() << ": "
                << myWorker->ItemsProcessed() << " items\n";
    }
  }

  UnitAssert(TestPointerSingle(numWorkers, optargs.Get<bool>('p')));
  UnitAssert(TestPointerBulk(numWorkers, optargs.Get<bool>('p')));

  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }

  return(0);
}
