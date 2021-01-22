//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestThreadQueue.cc 11256 $
// @(#) $Id: TestThreadQueue.cc 11256 2020-11-15 22:52:10Z dwm $
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
//!  \file TestThreadQueue.cc
//!  \brief Unit tests for ThreadQueue class
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <thread>
#include <vector>

#include "DwmOptArgs.hh"
#include "DwmSvnTag.hh"
#include "DwmSysLogger.hh"
#include "DwmTimeValue.hh"
#include "DwmUnitAssert.hh"
#include "DwmThreadQueue.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestThreadQueue.cc 11256 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void PushPopProducer(Thread::Queue<int> & tq)
{
  for (int i = 0; i < 1000; ++i) {
    UnitAssert(tq.PushBack(i));
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void PushPopFrontProducer(Thread::Queue<int> & tq)
{
  for (int i = 0; i < 1000; ++i) {
    UnitAssert(tq.PushFront(i));
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void PushPopConsumer(Thread::Queue<int> & tq)
{
  int  i = 0;
  int  value;
  //  This is a while() loop because PopFront() might not have anything
  //  to consume if this thread gets more time than the producer thread.
  //  We only increment the loop termination condition when we actually
  //  consumed a member of the queue.
  while (i < 1000) {
    if (tq.PopFront(value)) {
      UnitAssert(value == i);
      ++i;
    }
  }
  UnitAssert(i == 1000);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void PushPopBackConsumer(Thread::Queue<int> & tq)
{
  int  i = 0;
  int  value;
  //  This is a while() loop because PopBack() might not have anything
  //  to consume if this thread gets more time than the producer thread.
  //  We only increment the loop termination condition when we actually
  //  consumed a member of the queue.
  while (i < 1000) {
    if (tq.PopBack(value)) {
      UnitAssert(value == i);
      ++i;
    }
  }
  UnitAssert(i == 1000);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestPushPop()
{
  Thread::Queue<int>  tq;
  thread  producerThread(PushPopProducer, std::ref(tq));
  thread  consumerThread(PushPopConsumer, std::ref(tq));
  producerThread.join();
  consumerThread.join();
  thread  frontProducerThread(PushPopFrontProducer, std::ref(tq));
  thread  backConsumerThread(PushPopBackConsumer, std::ref(tq));
  frontProducerThread.join();
  backConsumerThread.join();
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void WaitNotEmptyConsumer(Thread::Queue<int> & tq)
{
  int  value;
  int  i = 0;
  for ( ; i < 1000; ++i) {
    UnitAssert(tq.WaitForNotEmpty());
    UnitAssert(tq.PopFront(value));
    UnitAssert(value == i);
  }
#if 0
  while (tq.PopFront(value)) {
    UnitAssert(value == i);
    ++i;
  }
#endif
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void WaitNotEmptyProducer(Thread::Queue<int> & tq)
{
  for (int i = 0; i < 1000; ++i) {
    UnitAssert(tq.PushBack(i));
    usleep(1000);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestWaitForNotEmpty()
{
  Thread::Queue<int>  tq;
  thread  consumerThread(WaitNotEmptyConsumer, std::ref(tq));
  thread  producerThread(WaitNotEmptyProducer, std::ref(tq));
  producerThread.join();
  consumerThread.join();
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TimedWaitConsumer(Thread::Queue<int> & tq)
{
  //  The producer thread won't signal the condition until it's been
  //  running for 3 seconds.  Hence our first check should time out.
  UnitAssert(! tq.ConditionTimedWait(std::chrono::milliseconds(100)));
  //  And our second check should indicate the condition was signalled
  //  because we wait long enough.
  UnitAssert(tq.ConditionTimedWait(std::chrono::seconds(5)));
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TimedWaitProducer(Thread::Queue<int> & tq)
{
  sleep(3);
  tq.ConditionSignal();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestConditionTimedWait()
{
  Thread::Queue<int>  tq;
  thread  consumerThread(TimedWaitConsumer, std::ref(tq));
  thread  producerThread(TimedWaitProducer, std::ref(tq));
  producerThread.join();
  consumerThread.join();
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void SimplePopFn(Thread::Queue<uint64_t> * queue, const string & name)
{
  uint64_t  i;
  uint64_t  processed = 0;
  while (processed < 1000000) {
    if (queue->PopFront(i)) {
      ++processed;
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void SimplePushFn(Thread::Queue<uint64_t> * queue, const string & name)
{
  uint64_t  pushed = 0;
  while (pushed < 1000000) {
    if (queue->PushBack(pushed)) {
      ++pushed;
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void BulkPopFn(Thread::Queue<uint64_t> * queue, const string & name)
{
  uint64_t  i;
  uint64_t  processed = 0;
  deque<uint64_t>  entries;
  while (processed < 1000000) {
    processed += queue->Swap(entries);
    entries.clear();
  }
  UnitAssert(1000000 == processed);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void BulkPushFn(Thread::Queue<uint64_t> * queue, const string & name)
{
  uint64_t  pushed = 0;
  vector<uint64_t>  entries(1000);
  while (pushed < 1000000) {
    if (queue->PushBack(entries.begin(), entries.end())) {
      pushed += entries.size();
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void BulkPushFrontFn(Thread::Queue<uint64_t> * queue, const string & name)
{
  uint64_t  pushed = 0;
  vector<uint64_t>  entries(1000);
  while (pushed < 1000000) {
    if (queue->PushFront(entries.begin(), entries.end())) {
      pushed += entries.size();
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestBulkPushFront()
{
  Thread::Queue<uint64_t>  tq;
  thread  pushThread(BulkPushFrontFn, &tq, "bulk_push_front");
  thread  popThread(BulkPopFn, &tq, "pop");
  pushThread.join();
  popThread.join();
  UnitAssert(tq.Empty());
  return;
}
                    
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestSimplePerformance()
{
  const uint64_t  numThreads = 4;
  thread  push_threads[numThreads];
  thread  pop_threads[numThreads];
  Thread::Queue<uint64_t>  *queues[numThreads];

  TimeValue  startTime(true);
  for (int i = 0; i < numThreads; ++i) {
    queues[i] = new Thread::Queue<uint64_t>();
    queues[i]->MaxLength(10000);
    pop_threads[i] = thread(SimplePopFn, queues[i],
                            string("popthr" + to_string(i)));
    push_threads[i] = thread(SimplePushFn, queues[i],
                             string("pushthr" + to_string(i)));
  }
  for (int i = 0; i < numThreads; ++i) {
    pop_threads[i].join();
    push_threads[i].join();
  }
  for (int i = 0; i < numThreads; ++i) {
    delete queues[i];
  }
  TimeValue runTime(true);

  runTime -= startTime;
  cout << "Processed " << (numThreads * 1000000) / (double)runTime
       << " per second\n";
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestBulkPerformance()
{
  const uint64_t  numThreads = 4;
  thread  push_threads[numThreads];
  thread  pop_threads[numThreads];
  Thread::Queue<uint64_t>  *queues[numThreads];

  TimeValue  startTime(true);
  for (int i = 0; i < numThreads; ++i) {
    queues[i] = new Thread::Queue<uint64_t>();
    queues[i]->MaxLength(10000);
    pop_threads[i] = thread(SimplePopFn, queues[i],
                            string("popthr" + to_string(i)));
    push_threads[i] = thread(SimplePushFn, queues[i],
                             string("pushthr" + to_string(i)));
  }
  for (int i = 0; i < numThreads; ++i) {
    pop_threads[i].join();
    push_threads[i].join();
  }
  for (int i = 0; i < numThreads; ++i) {
    delete queues[i];
  }
  TimeValue runTime(true);

  runTime -= startTime;
  cout << "Bulk processed " << (numThreads * 1000000) / (double)runTime
       << " per second\n";
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  OptArgs  optargs;
  optargs.AddOptArg("p", "performance", false, "false",
                    "run performance tests");
  optargs.Parse(argc, argv);
  
  TestPushPop();
  TestWaitForNotEmpty();
  TestConditionTimedWait();
  TestBulkPushFront();
  
  if (optargs.Get<bool>('p')) {
    Dwm::SysLogger::Open("TestThreadQueue", LOG_PERROR, LOG_USER);
    TestSimplePerformance();
    TestBulkPerformance();
  }

  return Assertions::PrintAndReturn();
}
