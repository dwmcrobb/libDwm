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
//!  
//----------------------------------------------------------------------------
class Task
{
public:
  Task()
      : _inc(1)
  {}
  
  Task(size_t i)  { _inc = i; }
    
  void operator () ()
  {
    _count += _inc;
    return;
  }

  static size_t Count()
  {
    return _count;
  }
  
private:
  static std::atomic<size_t>  _count;
  size_t                      _inc;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
std::atomic<size_t>  Task::_count = 0;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static std::atomic<size_t>  g_count = 0;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TaskFunction()
{
  ++g_count;
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestWithFunction()
{
  Dwm::ThreadPool<5,std::function<void()>>  tp;
  for (int i = 0; i < 42; ++i) {
    tp.Enqueue(TaskFunction);
  }
  tp.Shutdown();
  UnitAssert(42 == g_count);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::ThreadPool<5,Task>  tp;

  for (int i = 0; i < 25; ++i) {
    tp.Enqueue(Task(5));
  }
  tp.Shutdown();
  UnitAssert((5 * 25) == Task::Count());

  TestWithFunction();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << '\n';
    exit(0);
  }

}
