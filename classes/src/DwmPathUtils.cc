//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmPathUtils.cc 11123 $
// @(#) $Id: DwmPathUtils.cc 11123 2020-09-07 23:18:45Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007
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
//!  \file DwmPathUtils.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::PathUtils class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/stat.h>
}

#include "DwmSvnTag.hh"
#include "DwmPathUtils.hh"
#include "DwmStringUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmPathUtils.cc 11123 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PathUtils::IsDirectory(const std::string & path)
  {
    bool  rc = false;
    struct stat  statBuf;
    if (stat(path.c_str(), &statBuf) == 0)
      if (S_ISDIR(statBuf.st_mode))
        rc = true;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool PathUtils::HasSuffix(const std::string & path,
                            const std::string & suffix)
  {
    return(StringUtils::HasSuffix(path, suffix));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string PathUtils::Basename(const std::string & path)
  {
    std::string rc = path;
    std::string::size_type  idx = path.find_last_of('/');
    if (idx != rc.npos)
      rc = rc.substr(idx+1,rc.length() - 1);
    return(rc);
  }
    
}  // namespace Dwm
