//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020
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
//!  \file DwmGZIOCapable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::GZReadable, Dwm::GZWritable and Dwm::GZIOCapable pure
//!    virtual class declarations
//---------------------------------------------------------------------------

#ifndef _DWMGZIOCAPABLE_HH_
#define _DWMGZIOCAPABLE_HH_

extern "C" {
  #include <sys/types.h>
  #include <stdio.h>
  #include <zlib.h>
}

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  T has a Read(gzFile) member that returns int.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasGZRead = requires(T & t, gzFile gzf) {
    { t.Read(gzf) } -> std::same_as<int>;
  };
  
  //--------------------------------------------------------------------------
  //!  T has a Write(gzFile) const member that returns int.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasGZWrite = requires(const T & t, gzFile gzf) {
    { t.Write(gzf) } -> std::same_as<int>;
  };
      
  //--------------------------------------------------------------------------
  //!  This class is a pure virtual class, defining an interface for
  //!  classes that can read their contents from a gzFile.
  //--------------------------------------------------------------------------
  class GZReadable
  {
  public:
    //------------------------------------------------------------------------
    //!  destructor
    //------------------------------------------------------------------------
    virtual ~GZReadable() { }

    //------------------------------------------------------------------------
    //!  Read from a gzFile.  Should return the number of bytes read on
    //!  success (-1 on failure?)
    //------------------------------------------------------------------------
    virtual int Read(gzFile gzf) = 0;
  };

  //--------------------------------------------------------------------------
  //!  This class is a pure virtual class, defining an interface for
  //!  classes that can write their contents to a gzFile.
  //--------------------------------------------------------------------------
  class GZWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  destructor
    //------------------------------------------------------------------------
    virtual ~GZWritable() { }

    //------------------------------------------------------------------------
    //!  Write to a gzFile.  Should return the number of bytes written on
    //!  success (-1 on failure?)
    //------------------------------------------------------------------------
    virtual int Write(gzFile gzf) const = 0;
  };
  
  //--------------------------------------------------------------------------
  //!  Interface for classes with GZIO read and write capabilities.
  //--------------------------------------------------------------------------
  class GZIOCapable
    : public GZReadable, public GZWritable 
  {};
  
}  // namespace Dwm

#endif  // _DWMGZIOENABLED_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
