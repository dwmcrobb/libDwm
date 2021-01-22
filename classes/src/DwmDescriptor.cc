//===========================================================================
// @(#) $Name$
// @(#) $Id: DwmDescriptor.cc 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007
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
//!  \file DwmDescriptor.cc
//!  \brief Dwm::Descriptor class definition
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <sys/time.h>
}

#include "DwmSysLogger.hh"
#include "DwmDescriptor.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmDescriptor.cc 11087 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Descriptor::Descriptor()
    : _fd(-1)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Descriptor::Descriptor(const Descriptor & d)
  {
    _fd = dup(d._fd);
    Syslog(LOG_DEBUG, "descriptor %d duped from %d", _fd, d._fd);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Descriptor::Descriptor(int fd)
  {
    _fd = fd;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Descriptor & Descriptor::operator = (const Descriptor & d)
  {
    if (&d != this) {
      if (_fd >= 0) {
        Close();
      }
      if (d._fd >= 0) {
        _fd = dup(d._fd);
        Syslog(LOG_DEBUG, "descriptor %d duped from %d", _fd, d._fd);
      }
    }
    return *this;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Descriptor::~Descriptor()
  {
    if (this->_fd >= 0)
      this->Close();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::Attach(int fd)
  {
    bool  rc = false;
    
    if (this->_fd >= 0)
      this->Close();
    
    if (fd >= 0) {
      this->_fd = fd;
      rc = true;
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::Detach()
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      this->_fd = -1;
      rc = true;
    }
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::Close()
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      if (close(this->_fd) == 0) {
        Syslog(LOG_DEBUG, "Closed descriptor %d", this->_fd);
        rc = true;
        this->_fd = -1;
      }
      else {
        Syslog(LOG_ERR, "Failed to close descriptor %d", this->_fd);
      }
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::SetNonBlocking()
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      int  fdFlags = fcntl(this->_fd, F_GETFL, 0);
      fdFlags |= O_NONBLOCK;
      if (fcntl(this->_fd, F_SETFL, fdFlags) == 0) {
        rc = true;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::SetBlocking()
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      int  fdFlags = fcntl(this->_fd, F_GETFL, 0);
      fdFlags &= (~ O_NONBLOCK);
      if (fcntl(this->_fd, F_SETFL, fdFlags) == 0) {
        rc = true;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::FdSet(fd_set & fdSet)
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      FD_SET(this->_fd,& fdSet);
      rc = true;
    }
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::FdIsSet(fd_set & fdSet)
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      if (FD_ISSET(this->_fd, &fdSet)) {
        rc = true;
      }
    }
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::FdClr(fd_set & fdSet) 
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      FD_CLR(this->_fd, &fdSet);
      rc = true;
    }
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Descriptor::ReadWouldBlock()
  {
    bool  rc = false;
    int  fdFlags = fcntl(this->_fd, F_GETFL, 0);
    if (! (fdFlags & O_NONBLOCK)) {
      struct timeval  stv;
      stv.tv_sec = 0;
      stv.tv_usec = 0;
      fd_set  fdSet;
      FD_ZERO(&fdSet);
      FD_SET(this->_fd, &fdSet);
      
      int selectrc = select(this->_fd + 1, &fdSet, 0, 0, &stv);
      if (selectrc == 0) {
        rc = true;
      }
      else {
        if (selectrc > 0) {
          if (! FD_ISSET(this->_fd, &fdSet)) {
            rc = true;
          }
        }
      }
    }

    return(rc);
  }
  
}  // namespace Dwm
