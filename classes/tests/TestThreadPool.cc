//===========================================================================
// @(#) $DwmPath$
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
//!  \file TestThreadPool.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::ThreadPool unit tests
//---------------------------------------------------------------------------

#include <functional>
#include <iostream>

#include "DwmUnitAssert.hh"
#include "DwmThreadPool.hh"

using namespace std;
using namespace Dwm;


//----------------------------------------------------------------------------
//!  A function object with a non-default constructor to store some state,
//!  but no arguments to call operator.
//----------------------------------------------------------------------------
class VoidTask
{
public:
  VoidTask() : _inc(1)   { }
  VoidTask(size_t i)     { _inc = i; }
  void operator () ()    { _count += _inc;  return; }
  static size_t Count()  { return _count; }
  
private:
  static inline std::atomic<size_t>  _count = 0;
  size_t                             _inc;
};

//----------------------------------------------------------------------------
//!  A function object with a 2-argument call operator.
//----------------------------------------------------------------------------
class TwoArgTask
{
public:
  TwoArgTask()                     { }
  void operator () (int a, int b)  { _count += (a + b);  return; }
  static size_t Count()            { return _count; }
  
private:
  static inline std::atomic<size_t>  _count = 0;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static std::atomic<size_t>               g_count = 0;
static unordered_map<thread::id,size_t>  g_threadCalls;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void VoidTaskFunction()
{
  static mutex  mtx;
  ++g_count;

  lock_guard<mutex>  lock(mtx);
  ++g_threadCalls[this_thread::get_id()];
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestVoidFunction()
{
  Dwm::ThreadPool<5,std::function<decltype(VoidTaskFunction)>>  tp;
  for (int i = 0; i < 4001; ++i) {
    tp.AddTask(VoidTaskFunction);
  }
  UnitAssert(! tp.Start());  //  Start() should fail, threads are running
  tp.Stop();
  UnitAssert(4001 == g_count);

  //  Check that every thread processed at least one task, and that the sum
  //  of tasks processed by each thread equals the expected total.
  size_t  totalCalls = 0;
  for (auto thr : g_threadCalls) {
    UnitAssert(thr.second > 0);
    totalCalls += thr.second;
  }
  UnitAssert(4001 == totalCalls);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestStopStart()
{
  Dwm::ThreadPool<5,std::function<decltype(VoidTaskFunction)>>  tp;
  tp.Stop();
  UnitAssert(tp.Start());
  g_count = 0;
  for (int i = 0; i < 41; ++i) {
    tp.AddTask(VoidTaskFunction);
  }
  tp.Stop();
  UnitAssert(41 == g_count);
}

static atomic<size_t> g_twoArgCount = 0;
static string         g_twoArgString;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TwoArgFunction(int a, const string & s)
{
  static mutex          mtx;

  g_twoArgCount += a;
  lock_guard<mutex>  lock(mtx);
  g_twoArgString += s;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestTwoArgFunction()
{
  Dwm::ThreadPool<5,std::function<decltype(TwoArgFunction)>,int,string>  tp;
  size_t  total = 0;
  for (int i = 0; i < 43; ++i) {
    tp.AddTask(TwoArgFunction, i, "hi");
    total += i;
  }
  tp.Stop();
  UnitAssert(g_twoArgCount == total);
  UnitAssert(g_twoArgString.size() == 86);
  UnitAssert(g_twoArgString.front() == 'h');
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestTwoArgTask()
{
  Dwm::ThreadPool<5,TwoArgTask,int,int>  tp;
  size_t  total = 0;
  for (int i = 0; i < 401; ++i) {
    tp.AddTask(TwoArgTask(), i, i+1);
    total += (i + (i + 1));
  }
  tp.Stop();
  UnitAssert(total == TwoArgTask::Count());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestVoidTask()
{
  Dwm::ThreadPool<5,VoidTask>  tp;

  for (int i = 0; i < 25; ++i) {
    tp.AddTask(VoidTask(5));
  }
  tp.Stop();
  UnitAssert((5 * 25) == VoidTask::Count());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
class MemberFuncTask
{
public:
  void Increment(size_t incr)   { _count += incr; }
  size_t Count()                { return _count; }
  
private:
  std::atomic<size_t>  _count = 0;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestMemberFunctionViaLambda()
{
  Dwm::ThreadPool<5,std::function<void(size_t)>,size_t>  tp;
  MemberFuncTask   mft;
  for (int i = 0; i < 25; ++i) {
    tp.AddTask([&](size_t incr) { mft.Increment(incr); }, 42);
  }
  tp.Stop();
  UnitAssert((42 * 25) == mft.Count());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestMemberFunction()
{
  Dwm::ThreadPool<5,std::function<void(size_t)>,size_t>  tp;
  MemberFuncTask   mft;
  for (int i = 0; i < 25; ++i) {
    std::function<void(size_t)>  task =
      std::bind(&MemberFuncTask::Increment, &mft, std::placeholders::_1);
    tp.AddTask(task, 42);
  }
  tp.Stop();
  UnitAssert((42 * 25) == mft.Count());
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestVoidTask();
  TestVoidFunction();
  TestStopStart();
  TestTwoArgTask();
  TestTwoArgFunction();
  TestMemberFunctionViaLambda();
  TestMemberFunction();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << '\n';
    exit(0);
  }

}
