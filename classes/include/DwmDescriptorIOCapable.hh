//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020, 2024
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
//!  \file DwmDescriptorIOCapable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::DescriptorReadable, Dwm::DescriptorWritable and
//!     Dwm::DescriptorIOCapable pure virtual class declarations
//---------------------------------------------------------------------------

#ifndef _DWMDESCRIPTORIOCAPABLE_HH_
#define _DWMDESCRIPTORIOCAPABLE_HH_

extern "C" {
  #include <sys/types.h>
}

#include <cstddef>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  T has a Read(int) member that returns ssize_t.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasDescriptorRead = requires (T t, int fd) {
    { t.Read(fd) } -> std::same_as<ssize_t>;
  };

  //--------------------------------------------------------------------------
  //!  T has a Write(int fd) const member that returns ssize_t.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasDescriptorWrite = requires (T t, int fd) {
    { ((const T)t).Write(fd) } -> std::same_as<ssize_t>;
  };

  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can read their
  //!  contents from a file descriptor.
  //--------------------------------------------------------------------------
  class DescriptorReadable
  {
  public:
    //------------------------------------------------------------------------
    //!  destructor
    //------------------------------------------------------------------------
    virtual ~DescriptorReadable() { }

    //------------------------------------------------------------------------
    //!  Read from a file descriptor.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    virtual ssize_t Read(int fd) = 0;
  };

  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can write their
  //!  contents to a file descriptor.
  //--------------------------------------------------------------------------
  class DescriptorWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    virtual ~DescriptorWritable() { }

    //------------------------------------------------------------------------
    //!  Write to a file descriptor.  Return the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    virtual ssize_t Write(int fd) const = 0;
  };
  
  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can write/read their
  //!  contents to/from a file descriptor.
  //--------------------------------------------------------------------------
  class DescriptorIOCapable
    : public DescriptorReadable, public DescriptorWritable
  {};
  
}  // namespace Dwm

#endif  // _DWMDESCRIPTORIOCAPABLE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
