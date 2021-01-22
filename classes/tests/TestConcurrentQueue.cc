//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestConcurrentQueue.cc 8389 $
// @(#) $Id: TestConcurrentQueue.cc 8389 2016-04-17 04:31:36Z dwm $
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
//!  \file TestConcurrentQueue.cc
//!  \brief Unit tests for Dwm::Thread::ConcurrentQueue class template
//---------------------------------------------------------------------------

#include <iostream>

#include "DwmConcurrentQueue.hh"
#include "DwmUnitAssert.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestConcurrentQueue.cc 8389 $");

#define NUM_ENTRIES_PER_CPU_TO_TEST  5000000

using namespace std;
using Dwm::Thread::ConcurrentQueue;

//----------------------------------------------------------------------------
atomic<uint64_t>  consumed_count(0);

int g_numCpus = 4;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void cthr_fn(ConcurrentQueue<uint64_t> & cq, const string & name)
{
  vector<uint64_t>  entries;
  
  while (consumed_count < ((g_numCpus/2) * NUM_ENTRIES_PER_CPU_TO_TEST)) {
    if (cq.PopFront(entries)) {
      consumed_count += entries.size();
    }
    else {
      usleep(1000);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void pthr_fn(ConcurrentQueue<uint64_t> & cq, const string & name)
{
  const int         numEntries = 1000;
  vector<uint64_t>  entries(numEntries);
  
  for (uint64_t i = 0; i < NUM_ENTRIES_PER_CPU_TO_TEST/numEntries; ++i) {
    for (int j = 0; j < numEntries; ++j) {
      entries[j] = i * numEntries + j;
    }
    cq.PushBack(entries);
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestMultiProducerConsumer()
{
  consumed_count = 0;

  int                        numProducers = g_numCpus / 2;
  vector<thread>             producers(numProducers);
  vector<thread>             consumers(numProducers);
  ConcurrentQueue<uint64_t>  cq;
  
  for (int i = 0; i < numProducers; ++i) {
    consumers[i] = thread(cthr_fn, ref(cq),
                               string("cthr" + to_string(i)));
    producers[i] = thread(pthr_fn, ref(cq),
                               string("pthr" + to_string(i)));
  }
  for (int i = 0; i < numProducers; ++i) {
    UnitAssert(producers[i].joinable());
    UnitAssert(consumers[i].joinable());
  }
  for (int i = 0; i < numProducers; ++i) {
    producers[i].join();
    consumers[i].join();
  }
  UnitAssert(consumed_count == (NUM_ENTRIES_PER_CPU_TO_TEST * numProducers));
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  g_numCpus = thread::hardware_concurrency();
  if (g_numCpus < 4) {
    g_numCpus = 4;
  }
  int  numQueues = g_numCpus / 2;
  
  vector<ConcurrentQueue<uint64_t> >  cqs(numQueues);
  vector<thread>                      push_threads(numQueues);
  vector<thread>                      pop_threads(numQueues);
  
  for (int i = 0; i < numQueues; ++i) {
    pop_threads[i] = thread(cthr_fn, ref(cqs[i]),
                            string("cthr" + to_string(i)));
    push_threads[i] = thread(pthr_fn, ref(cqs[i]),
                             string("pthr" + to_string(i)));
  }
  for (int i = 0; i < numQueues; ++i) {
    UnitAssert(push_threads[i].joinable());
    UnitAssert(pop_threads[i].joinable());
  }
  for (int i = 0; i < numQueues; ++i) {
    push_threads[i].join();
    pop_threads[i].join();
  }

  UnitAssert(consumed_count == (NUM_ENTRIES_PER_CPU_TO_TEST * numQueues));

  TestMultiProducerConsumer();
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Dwm::Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
