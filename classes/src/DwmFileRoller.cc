//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file DwmFileRoller.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
}

#include <cassert>
#include <sstream>

#include "DwmBZ2IO.hh"
#include "DwmSvnTag.hh"
#include "DwmFileRoller.hh"
#include "DwmGZIO.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmFileRoller.cc 8388 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  FileRoller::FileRoller(const std::string & dir, const std::string & prefix,
                         size_t maxSize, size_t numFiles, 
                         CompressionEnum compression)
      : _dir(dir), _prefix(prefix), _maxSize(maxSize), _numFiles(numFiles),
        _compression(compression)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::FilesNeedRolling() const
  {
    bool    rc = false;
    string  firstFile;
    if (GetFilename(0, firstFile)) {
      struct stat  statbuf;
      if (stat(firstFile.c_str(), &statbuf) == 0) {
        if (statbuf.st_size >= _maxSize) {
          rc = true;
        }
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::FilesNeedRolling(int fd) const
  {
    bool  rc = false;
    
    if (fd >= 0) {
      off_t  pos = lseek(fd, 0, SEEK_CUR);
      if (pos != -1) {
        if (pos >= _maxSize) {
          rc = true;
        }
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::FilesNeedRolling(std::ostream & os) const
  {
    bool  rc = false;
    if (os) {
      std::ostream::pos_type  pos = os.tellp();
      if (pos >= _maxSize)
        rc = true;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::FilesNeedRolling(FILE *f) const
  {
    bool  rc = false;
    if (f) {
      long  pos = ftell(f);
      if (pos >= (long)_maxSize) {
        rc = true;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileRoller::MaxSize() const
  {
    return(_maxSize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileRoller::MaxSize(size_t maxSize)
  {
    _maxSize = maxSize;
    return(_maxSize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileRoller::NumFiles() const
  {
    return(_numFiles);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileRoller::NumFiles(size_t numFiles)
  {
    _numFiles = numFiles;
    return(_numFiles);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::RollFiles()
  {
    for (size_t fileNum = _numFiles + 1; fileNum > 0; --fileNum) {
      string  filename;
      if (GetFilename(fileNum, filename)) {
        struct stat  statbuf;
        if (stat(filename.c_str(), &statbuf) == 0) {
          //  remove file
          unlink(filename.c_str());
        }

        if ((fileNum != 1) || (_compression == e_compressionNone)) {
          string  prevFilename;
          if (GetFilename(fileNum - 1, prevFilename)) {
            if (stat(prevFilename.c_str(), &statbuf) == 0) {
              //  link to the previous file
              link(prevFilename.c_str(), filename.c_str());
            }
          }
        }
        else {
          string  prevFilename;
          if (GetFilename(fileNum - 1, prevFilename)) {
            switch (_compression) {
              case e_compressionGzip:
                GzipFile(prevFilename.c_str(), filename.c_str());
                break;
              case e_compressionBzip2:
                BzipFile(prevFilename.c_str(), filename.c_str());
                break;
              default:
                assert((_compression == e_compressionGzip) ||
                       (_compression == e_compressionBzip2));
                break;
            }
          }
        }
      }
    }

    string  firstFile;
    if (GetFilename(0, firstFile)) {
      struct stat  statbuf;
      if (stat(firstFile.c_str(), &statbuf) == 0) {
        //  remove the first file
        unlink(firstFile.c_str());

        //  create the first file
        int  fd = open(firstFile.c_str(), O_WRONLY|O_CREAT, statbuf.st_mode);
        if (fd >= 0) {
          close(fd);
        }
      }
    }
    return(true);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::GzipFile(const string & src, const string & dst) const
  {
    bool  rc = false;
    
    unlink(dst.c_str());
    
    int  rfd = open(src.c_str(), O_RDONLY);
    if (rfd >= 0) {
      gzFile  gzf = gzopen(dst.c_str(), "wb");
      if (gzf) {
        char     buf[65536];
        ssize_t  bytesRead = 0;
        do {
          ssize_t  bytesRead = read(rfd, buf, 65536);
          if (bytesRead > 0) {
            gzwrite(gzf, buf, bytesRead);
          }
        } while (bytesRead > 0);
        gzclose(gzf);
        rc = true;
      }
      close(rfd);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::BzipFile(const string & src, const string & dst) const
  {
    bool  rc = false;

    unlink(dst.c_str());
    
    int  rfd = open(src.c_str(), O_RDONLY);
    if (rfd >= 0) {
      BZFILE  *bzf = BZ2_bzopen(dst.c_str(), "wb");
      if (bzf) {
        char     buf[65536];
        ssize_t  bytesRead;
        do {
          bytesRead = read(rfd, buf, 65536);
          if (bytesRead > 0) {
            BZ2_bzwrite(bzf, buf, bytesRead);
          }
        } while (bytesRead > 0);
        BZ2_bzclose(bzf);
        rc = true;
      }
      close(rfd);
    }
    return(rc);
  };
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileRoller::GetFilename(size_t fileNum, std::string & filename) const
  {
    filename = "";
    if (fileNum <= _numFiles) {
      if (fileNum == 0) {
        filename = _dir + string("/") + _prefix;
      }
      else {
        ostringstream  os;
        os << _dir << "/" << _prefix << "." << fileNum - 1;
        switch (_compression) {
          case e_compressionGzip:
            os << ".gz";
            break;
          case e_compressionBzip2:
            os << ".bz2";
            break;
          default:
            break;
        }
        
        filename = os.str();
      }
    }
    return(! filename.empty());
  }

  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
