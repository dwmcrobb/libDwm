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
//!  \file DwmStreamIOCapable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::HasStreamRead and Dwm::HasStreamWrite concepts.
//!    Dwm::StreamReadable, Dwm::StreamWritable and Dwm::StreamIOCapable
//!    pure virtual class declarations.
//---------------------------------------------------------------------------

#ifndef _DWMSTREAMIOCAPABLE_HH_
#define _DWMSTREAMIOCAPABLE_HH_

#include <iostream>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  T has a Read(std::istream &) method that returns the std::istream
  //!  reference that was passed in as its only parameter.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasStreamRead = requires(T & t, std::istream & is) {
    { t.Read(is) } -> std::same_as<std::istream &>;
  };

  //--------------------------------------------------------------------------
  //!  T has a Write(std::ostream &) const method that returns the
  //!  std::ostream reference that was passed in as its only parameter.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasStreamWrite = requires(const T & t, std::ostream & os) {
    { t.Write(os) } -> std::same_as<std::ostream &>;
  };
  
  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can read their
  //!  contents from an istream.
  //--------------------------------------------------------------------------
  class StreamReadable
  {
  public:
    //------------------------------------------------------------------------
    //!  destructor
    //------------------------------------------------------------------------
    virtual ~StreamReadable() { }

    //------------------------------------------------------------------------
    //!  Read from an istream.  Return the istream.
    //------------------------------------------------------------------------
    virtual std::istream & Read(std::istream & is) = 0;
  }; 

  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can write their
  //!  contents to an ostream.
  //--------------------------------------------------------------------------
  class StreamWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    virtual ~StreamWritable() { }

    //------------------------------------------------------------------------
    //!  Write to an ostream.  Return the ostream.
    //------------------------------------------------------------------------
    virtual std::ostream & Write(std::ostream & os) const = 0;
  };
  
  //--------------------------------------------------------------------------
  //!  Interface for classes with iostream read and write capabilities.
  //--------------------------------------------------------------------------
  class StreamIOCapable
    : public StreamReadable, public StreamWritable
  {};
  
}  // namespace Dwm

#endif  // _DWMSTREAMIOCAPABLE_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
