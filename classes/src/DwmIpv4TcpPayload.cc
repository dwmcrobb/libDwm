//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2020
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
//!  \file DwmIpv4TcpPayload.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4TcpPayload class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstring>

#include "DwmIpv4TcpPayload.hh"
#include "DwmIpv4Utils.hh"
#include "DwmSysLogger.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPayload::Ipv4TcpPayload()
      : _payload(0,(uint8_t *)0), _ownData(true)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPayload::Ipv4TcpPayload(const Ipv4PacketHeader & ipHeader,
                                 const Ipv4TcpHeader & tcpHeader)
      : _payload(0,(uint8_t *)0), _ownData(false)
  {
    if (ipHeader.TotalLength() 
        >= (ipHeader.HeaderLength() + tcpHeader.DataOffset())) {
      _payload.first = ipHeader.TotalLength() 
        - (ipHeader.HeaderLength() + tcpHeader.DataOffset());
      _payload.second = tcpHeader.End();
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPayload::Ipv4TcpPayload(uint8_t *buf, uint16_t len)
      : _ownData(false)
  {
    _payload.first = len;
    _payload.second = buf;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPayload::Ipv4TcpPayload(const std::string & s)
      : _payload(0,(uint8_t *)0), _ownData(true)
  {
    Allocate(s.length());
    if (_payload.first) {
      memcpy(_payload.second, s.c_str(), _payload.first);
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPayload::~Ipv4TcpPayload()
  {
    Free();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4TcpPayload::Free()
  {
    if (_ownData) {
      if (_payload.second) {
        free(_payload.second);
        _payload.second = 0;
      }
      _payload.first = 0;
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4TcpPayload::Allocate(uint16_t len)
  {
    Free();
    if (len) {
      _payload.first = len;
      _payload.second = (uint8_t *)calloc(1, _payload.first);
      assert(_payload.second);
      _ownData = true;
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::pair<uint16_t,const uint8_t *> Ipv4TcpPayload::Payload() const
  {
    return(_payload);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Ipv4TcpPayload::ToString() const
  {
    return(std::string((const char *)_payload.second, _payload.first));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpPayload::Sum() const
  {
    return(Ipv4Utils::Sum(_payload.second, _payload.first));
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4TcpPayload::Clear()
  {
    Free();
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4TcpPayload::Read(std::istream & is, uint16_t len)
  {
    Free();
    if (is && len) {
      assert(len <= (65535 - (20 + 20)));
      Allocate(len);
      is.read((char *)_payload.second, _payload.first);
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv4TcpPayload::Write(std::ostream & os) const
  {
    if (os) {
      if (_payload.first) {
        os.write((char *)_payload.second, _payload.first);
      }
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4TcpPayload::Read(int fd, uint16_t len)
  {
    ssize_t  rc = -1;
    Free();
    if (fd >= 0 && len) {
      assert(len <= (65535 - (20 + 20)));
      Allocate(len);
      int bytesRead = read(fd, _payload.second, _payload.first);
      if (bytesRead != _payload.first)
        goto err;
      rc = bytesRead;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::Read(%d, %hu) failed", fd, len);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4TcpPayload::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesWritten = write(fd, _payload.second, _payload.first);
      if (bytesWritten != _payload.first)
        goto err;
      rc = bytesWritten;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::Write(%d) failed", fd);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4TcpPayload::Read(FILE *f, uint16_t len)
  {
    size_t  rc = 0;
    Free();
    if (f && len) {
      assert(len <= (65535 - (20 + 20)));
      Allocate(len);
      if (! fread(_payload.second, _payload.first, 1, f))
        goto err;
      rc = 1;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::Read(FILE %p, %hu) failed", f, len);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4TcpPayload::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      if (_payload.first) {
        if (! fwrite(_payload.second, _payload.first, 1, f))
          goto err;
        rc = 1;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::Write(FILE %p) failed", f);
    return(0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPayload::Read(gzFile gzf, uint16_t len)
  {
    int  rc = -1;
    Free();
    if (gzf && len) {
      assert(len <= (65535 - (20 + 20)));
      Allocate(len);
      int  bytesRead = gzread(gzf, _payload.second, _payload.first);
      if (bytesRead != _payload.first)
        goto err;
      rc = bytesRead;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::Read(gzFile %p, %hu) failed", gzf, len);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPayload::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      if (_payload.first) {
        int  bytesWritten = gzwrite(gzf, _payload.second, _payload.first);
        if (bytesWritten != _payload.first)
          goto err;
        rc = bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::Write(gzFile %p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPayload::BZRead(BZFILE *bzf, uint16_t len)
  {
    int  rc = -1;
    Free();
    if (bzf && len) {
      assert(len <= (65535 - (20 + 20)));
      Allocate(len);
      int  bytesRead = BZ2_bzread(bzf, _payload.second, _payload.first);
      if (bytesRead != _payload.first)
        goto err;
      rc = bytesRead;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::BZRead(%p, %hu) failed", bzf, len);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPayload::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      if (_payload.first) {
        int  bytesWritten = BZ2_bzwrite(bzf, _payload.second, _payload.first);
        if (bytesWritten != _payload.first)
          goto err;
        rc = bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPayload::BZWrite(%p) failed", bzf);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpPayload::StreamedLength() const
  {
    return(_payload.first);
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
