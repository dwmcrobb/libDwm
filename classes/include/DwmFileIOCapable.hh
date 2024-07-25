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
//!  \file DwmFileIOCapable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::HasFileRead and Dwm::HasFileWrite concepts.
//!    Dwm::FileIOCapable pure virtual class declaration.
//---------------------------------------------------------------------------

#ifndef _DWMFILEIOCAPABLE_HH_
#define _DWMFILEIOCAPABLE_HH_

#include <cstdio>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  T has a Read(FILE *) member that returns size_t (1 on success, 0
  //!  on failure).
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasFileRead = requires(T & t, FILE *f) {
    { t.Read(f) } -> std::same_as<size_t>;
  };

  //--------------------------------------------------------------------------
  //!  T has a Write(FILE *) const member that returns size_t (1 on success,
  //!  0 on failure).
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasFileWrite = requires(const T & t, FILE *f) {
    { t.Write(f) } -> std::same_as<size_t>;
  };

  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can read their
  //!  contents from a FILE pointer.
  //--------------------------------------------------------------------------
  class FileReadable
  {
  public:
    //------------------------------------------------------------------------
    //!  destructor
    //------------------------------------------------------------------------
    virtual ~FileReadable() { }

    //------------------------------------------------------------------------
    //!  Read from a FILE pointer.  Returns 1 on success, 0 on failure
    //!  (fread() semantics).
    //------------------------------------------------------------------------
    virtual size_t Read(FILE * f) = 0;
  };
  
  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can write their
  //!  contents to a FILE pointer.
  //--------------------------------------------------------------------------
  class FileWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    virtual ~FileWritable() { }

    //------------------------------------------------------------------------
    //!  Write to a FILE pointer.  Return 1 on success, 0 on failure
    //! (fwrite() semantics).
    //------------------------------------------------------------------------
    virtual size_t Write(FILE *f) const = 0;
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  class FileIOCapable : public FileReadable, public FileWritable { };
  
}  // namespace Dwm

#endif  // _DWMFILEIOCAPABLE_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
