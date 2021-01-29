//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2021
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
//!  \file DwmWriteLockedReference.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::WriteLockedReference class template declaration
//---------------------------------------------------------------------------

#ifndef _DWMWRITELOCKEDREFERENCE_HH_
#define _DWMWRITELOCKEDREFERENCE_HH_

#include <mutex>
#include <shared_mutex>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Just a small utility template that allows classes to expose a piece
  //!  of their data (via mutable reference) along with a unique_lock
  //!  constructed with a shared_mutex that the class uses internally to
  //!  protect that data.  A class would return an instance of this class
  //!  after constructing it with a reference to their shared_mutex and a
  //!  reference to their data.
  //!
  //!  Since this class holds the lock, it will retain it until destructed,
  //!  at which point the lock will be released automatically.  It is
  //!  particularly useful for classes that encapsulate containers which
  //!  need to be protected while also allowing access to (for example)
  //!  iterators and standard algorithms.  Instances of this template
  //!  should only be used in RAII fashion, as temporary stack members;
  //!  otherwise you're back to object lifetime potentially creating
  //!  deadlock.
  //--------------------------------------------------------------------------
  template <typename F, typename LockedType>
  class WriteLockedReference
  {
  public:
    //------------------------------------------------------------------------
    //!  No default constructor.
    //------------------------------------------------------------------------
    WriteLockedReference() = delete;
    
    //------------------------------------------------------------------------
    //!  No copy constructor.
    //------------------------------------------------------------------------
    WriteLockedReference(const WriteLockedReference &) = delete;
    
    //------------------------------------------------------------------------
    //!  No move constructor.
    //------------------------------------------------------------------------
    WriteLockedReference(WriteLockedReference &&) = delete;
    
    //------------------------------------------------------------------------
    //!  No assigment operator.
    //------------------------------------------------------------------------
    WriteLockedReference & oiperator = (const WriteLockedReference &) = delete;

    //------------------------------------------------------------------------
    //!  Returns a mutable reference to the write-locked data.
    //------------------------------------------------------------------------
    LockedType & Data()  { return _data; }

  private:
    std::unique_lock<std::shared_mutex>   _lock;
    LockedType                           &_data;

    //------------------------------------------------------------------------
    //!  Construct from a reference to a mutex @c mtx and the @c data
    //!  protected by the mutex.
    //------------------------------------------------------------------------
    WriteLockedReference(std::shared_mutex & mtx, LockedType & data)
      : _lock(mtx), _data(data)
    { }
  };
  
}  // namespace Dwm

#endif  // _DWMWRITELOCKEDREFERENCE_HH_
