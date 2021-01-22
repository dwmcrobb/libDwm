//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmFileRoller.hh 8401 $
// @(#) $Id: DwmFileRoller.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmFileRoller.hh
//!  \brief Dwm::FileRoller class definition
//---------------------------------------------------------------------------

#ifndef _DWMFILEROLLER_HH_
#define _DWMFILEROLLER_HH_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class can be used to roll over files such as log files.
  //!  Typically an application logging to a file will periodically call
  //!  one of the FilesNeedRolling() members, and if it returns true,
  //!  close the logging file and then call RollFiles().
  //--------------------------------------------------------------------------
  class FileRoller
  {
  public:
    //------------------------------------------------------------------------
    //!  Available compression types.
    //------------------------------------------------------------------------
    typedef enum {
      e_compressionNone,
      e_compressionGzip,
      e_compressionBzip2
    } CompressionEnum;
    
    //------------------------------------------------------------------------
    //!  Constructor.  \c dir is the directory in which the files are
    //!  stored.  \c prefix is the name of the first file, which presumably
    //!  your application is using for logging or the like.  \c maxSize is
    //!  the file size (in bytes) at which we'd like to roll over files.
    //!  \c numFiles is the number of files we'd like to keep beyond the
    //!  first file.  \c compression may be set to use gzip or bzip2
    //!  compression.
    //------------------------------------------------------------------------
    FileRoller(const std::string & dir,
               const std::string & prefix,
               size_t maxSize, size_t numFiles, 
               CompressionEnum compression = e_compressionNone);

    //------------------------------------------------------------------------
    //!  Returns true if the active file (identified by the \c dir and 
    //!  \c prefix in the call to our constructor) is larger than our 
    //!  maximum file size (set with \c maxSize in our constructor).
    //------------------------------------------------------------------------
    bool FilesNeedRolling() const;

    //------------------------------------------------------------------------
    //!  Returns true if the current write position of \c fd is 
    //!  larger than our maximum file size (set with \c maxSize in our 
    //!  constructor).  Here we assume that \c fd is a descriptor being
    //!  used to write to the active file (identified by the \c dir and
    //!  \c prefix in the call to our constructor).
    //!  @note This is a convenience routine that avoids the overhead of
    //!  calling stat().
    //------------------------------------------------------------------------
    bool FilesNeedRolling(int fd) const;

    //------------------------------------------------------------------------
    //!  Returns true if the current write position of \c os is 
    //!  larger than our maximum file size (set with \c maxSize in our 
    //!  constructor).  Here we assume that \c os is an ostream being
    //!  used to write to the active file (identified by the \c dir and
    //!  \c prefix in the call to our constructor).
    //!  @note This is a convenience routine that avoids the overhead of
    //!  calling stat().
    //------------------------------------------------------------------------
    bool FilesNeedRolling(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Returns true if the current write position of \c f is 
    //!  larger than our maximum file size (set with \c maxSize in our 
    //!  constructor).  Here we assume that \c f is a FILE pointer being
    //!  used to write to the active file (identified by the \c dir and
    //!  \c prefix in the call to our constructor).
    //!  @note This is a convenience routine that avoids the overhead of
    //!  calling stat().
    //------------------------------------------------------------------------
    bool FilesNeedRolling(FILE *f) const;
    
    //------------------------------------------------------------------------
    //!  Rolls the files.   The active file should be closed before calling
    //!  this function.
    //------------------------------------------------------------------------
    bool RollFiles();

    //------------------------------------------------------------------------
    //!  Returns the max file size.
    //------------------------------------------------------------------------
    size_t MaxSize() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the max file size.
    //------------------------------------------------------------------------
    size_t MaxSize(size_t maxSize);

    //------------------------------------------------------------------------
    //!  Returns the max file size.
    //------------------------------------------------------------------------
    size_t NumFiles() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the max file size.
    //------------------------------------------------------------------------
    size_t NumFiles(size_t numFiles);
    
  private:
    std::string      _dir;
    std::string      _prefix;
    off_t            _maxSize;
    size_t           _numFiles;
    CompressionEnum  _compression;

    bool GzipFile(const std::string & src, const std::string & dst) const;
    bool BzipFile(const std::string & src, const std::string & dst) const;
    bool GetFilename(size_t fileNum, std::string & filename) const;
    
  };
  
}  // namespace Dwm

#endif  // _DWMFILEROLLER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
