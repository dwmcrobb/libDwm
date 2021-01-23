//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file DwmPathUtils.hh
//!  \brief Dwm::PathUtils class definition
//---------------------------------------------------------------------------

#ifndef _DWMPATHUTILS_HH_
#define _DWMPATHUTILS_HH_


#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class just encapsulates miscellaneous utility functions for
  //!  working with UNIX paths (files and directories).
  //--------------------------------------------------------------------------
  class PathUtils
  {
  public:
    //------------------------------------------------------------------------
    //!  Returns true if \c path is a directory.  Note that this will also
    //!  return true if \c path is a symbolic link pointing to a directory.
    //------------------------------------------------------------------------
    static bool IsDirectory(const std::string & path);

    //------------------------------------------------------------------------
    //!  Returns true if \c path is suffixed with \c suffix.  For example,
    //!  HasSuffix("foo.cc",".cc") would return true.
    //------------------------------------------------------------------------
    static bool HasSuffix(const std::string & path, 
                          const std::string & suffix);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::string Basename(const std::string & path);
  };
  
}  // namespace Dwm

#endif  // _DWMPATHUTILS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
