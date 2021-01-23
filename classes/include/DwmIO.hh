//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016, 2017, 2020
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIO.hh
//!  \brief Dwm::IO class definition
//---------------------------------------------------------------------------

#ifndef _DWMIO_HH_
#define _DWMIO_HH_

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmIOUtils.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class is now just a wrapper for DescriptorIO, FileIO, StreamIO
  //!  and IOUtils.  It is essentially deprecated due to the introduction of
  //!  theses classes, but was refactored as a simple bucket of wrapper
  //!  function templates to allow older code to compile and work without
  //!  source code changes.
  //--------------------------------------------------------------------------
  class IO
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static std::istream & Read(std::istream & is, T & val)
    {
      return StreamIO::Read(is, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static std::ostream & Write(std::ostream & os, const T & val)
    {
      return StreamIO::Write(os, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Read(int fd, T & val)
    {
      return DescriptorIO::Read(fd, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Write(int fd, const T & val)
    {
      return DescriptorIO::Write(fd, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static size_t Read(FILE *f, T & val)
    {
      return FileIO::Read(f, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static size_t Write(FILE *f, const T & val)
    {
      return FileIO::Write(f, val);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static uint32_t StreamedLength(const T & val)
    {
      return IOUtils::StreamedLength(val);
    }
  };

}  // namespace Dwm


#endif  // _DWMIO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
