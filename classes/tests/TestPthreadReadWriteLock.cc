//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestPthreadReadWriteLock.cc 8389 $
// @(#) $Id: TestPthreadReadWriteLock.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file TestPthreadReadWriteLock.cc
//!  \brief Unit tests for Dwm::Pthread::ReadWriteLock
//---------------------------------------------------------------------------

#include "DwmSvnTag.hh"
#include "DwmPthreadReadWriteLock.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestPthreadReadWriteLock.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void *Thread1(void *arg)
{
  Pthread::ReadWriteLock *rwLock = (Pthread::ReadWriteLock *)arg;
  UnitAssert(rwLock->TryReadLock() == EBUSY);
  UnitAssert(rwLock->TryWriteLock() == EBUSY);
  return(arg);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void *Thread2(void *arg)
{
  Pthread::ReadWriteLock *rwLock = (Pthread::ReadWriteLock *)arg;
  UnitAssert(rwLock->ReadLock());
  UnitAssert(rwLock->Unlock());
  UnitAssert(rwLock->TryReadLock() == 0);
  UnitAssert(rwLock->TryWriteLock() == EBUSY);
  UnitAssert(rwLock->Unlock());
  
  return(arg);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void *Thread3(void *arg)
{
  Pthread::ReadWriteLock *rwLock = (Pthread::ReadWriteLock *)arg;
  UnitAssert(rwLock->TryReadLock() == 0);
  UnitAssert(rwLock->Unlock());
  UnitAssert(rwLock->TryWriteLock() == EBUSY);
  
  return(arg);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void *Thread4(void *arg)
{
  Pthread::ReadWriteLock *rwLock = (Pthread::ReadWriteLock *)arg;
  UnitAssert(rwLock->TryReadLock() == EBUSY);
  
  return(arg);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Pthread::ReadWriteLock  rwLock;
  
  pthread_t  thread1;
  UnitAssert(rwLock.WriteLock());
  UnitAssert(pthread_create(&thread1, 0, Thread1, (void *)&rwLock) == 0);
  UnitAssert(pthread_join(thread1, 0) == 0);
  UnitAssert(rwLock.Unlock());

  pthread_t  thread2;
  UnitAssert(rwLock.ReadLock());
  UnitAssert(pthread_create(&thread2, 0, Thread2, (void *)&rwLock) == 0);
  UnitAssert(pthread_join(thread2, 0) == 0);
  UnitAssert(rwLock.Unlock());

  pthread_t  thread3;
  UnitAssert(rwLock.TryReadLock() == 0);
  UnitAssert(pthread_create(&thread3, 0, Thread3, (void *)&rwLock) == 0);
  UnitAssert(pthread_join(thread3, 0) == 0);
  UnitAssert(rwLock.Unlock());
  
  pthread_t  thread4;
  UnitAssert(rwLock.TryWriteLock() == 0);
  UnitAssert(pthread_create(&thread4, 0, Thread4, (void *)&rwLock) == 0);
  UnitAssert(pthread_join(thread4, 0) == 0);
  UnitAssert(rwLock.Unlock());

  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  return(0);
}
