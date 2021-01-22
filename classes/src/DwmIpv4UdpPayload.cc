//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4UdpPayload.cc 11114 $
// @(#) $Id: DwmIpv4UdpPayload.cc 11114 2020-09-07 22:53:15Z dwm $
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
//!  \file DwmIpv4UdpPayload.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4UdpPayload class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstring>

#include "DwmSvnTag.hh"
#include "DwmIpv4UdpPayload.hh"
#include "DwmIpv4Utils.hh"
#include "DwmSysLogger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4UdpPayload.cc 11114 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPayload::Ipv4UdpPayload()
      : _payload(0,(uint8_t *)0), _ownData(true)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPayload::Ipv4UdpPayload(const Ipv4UdpHeader & udpHeader)
      : _payload(0,(uint8_t *)0), _ownData(false)
  {
    if (udpHeader.UdpLength() > 8) {
      _payload.first = udpHeader.UdpLength() - 8;
      _payload.second = udpHeader.End();
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPayload::Ipv4UdpPayload(uint8_t *buf, uint16_t len)
      : _ownData(false)
  {
    _payload.first = len;
    _payload.second = buf;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPayload::Ipv4UdpPayload(const std::string & s)
      : _payload(0,(uint8_t *)0), _ownData(true)
  {
    _payload.first = s.length();
    if (_payload.first) {
      Allocate(_payload.first);
      assert(_payload.second);
      memcpy(_payload.second, s.c_str(), _payload.first);
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPayload::~Ipv4UdpPayload()
  {
    Free();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4UdpPayload::Clear()
  {
    Free();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4UdpPayload::Free()
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
  void Ipv4UdpPayload::Allocate(uint16_t len)
  {
    assert(len > 0);
    Free();
    _payload.first = len;
    _payload.second = (uint8_t *)calloc(1, _payload.first);
    assert(_payload.second);
    _ownData = true;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::pair<uint16_t,const uint8_t *> Ipv4UdpPayload::Payload() const
  {
    return(_payload);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Ipv4UdpPayload::ToString() const
  {
    return(std::string((const char *)_payload.second, _payload.first));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4UdpPayload::Sum() const
  {
    return(Ipv4Utils::Sum(_payload.second, _payload.first));
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4UdpPayload::Read(std::istream & is, uint16_t len)
  {
    Free();
    if (is && len) {
      assert(len <= (65535 - (20 + 8)));
      Allocate(len);
      assert(_payload.second);
      is.read((char *)_payload.second, _payload.first);
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv4UdpPayload::Write(std::ostream & os) const
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
  ssize_t Ipv4UdpPayload::Read(int fd, uint16_t len)
  {
    ssize_t  rc = -1;
    
    Free();
    if (fd >= 0 && len) {
      assert(len <= (65535 - (20 + 8)));
      Allocate(len);
      rc = read(fd, _payload.second, _payload.first);
      if (rc != _payload.first)
        goto err;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::Read(%d, %hu) failed", fd, len);
    return(-1);
  }
      
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4UdpPayload::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = write(fd, _payload.second, _payload.first);
      if (rc != _payload.first)
        goto err;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::Write(%d) failed", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4UdpPayload::Read(FILE *f, uint16_t len)
  {
    size_t  rc = 0;
    Free();
    if (f && len) {
      assert(len <= 65535 - (20 + 8));
      Allocate(len);
      rc = fread(_payload.second, _payload.first, 1, f);
      if (! rc)
        goto err;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::Read(%p, %hu) failed: %m", f, len);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4UdpPayload::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      rc = fwrite(_payload.second, _payload.first, 1, f);
      if (! rc)
        goto err;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::Write(%p) failed", f);
    return(0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPayload::Read(gzFile gzf, uint16_t len)
  {
    int  rc = -1;
    Free();
    if (gzf && len) {
      assert(len <= (65535 - (20 + 8)));
      Allocate(len);
      rc = gzread(gzf, _payload.second, _payload.first);
      if (rc != _payload.first)
        goto err;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::Read(%p, %hu) failed", gzf, len);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPayload::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      rc = gzwrite(gzf, _payload.second, _payload.first);
      if (rc != _payload.first)
        goto err;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::Write(%p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPayload::BZRead(BZFILE *bzf, uint16_t len)
  {
    int  rc = -1;
    Free();
    if (bzf && len) {
      assert(len <= (65535 - (20 + 8)));
      Allocate(len);
      rc = BZ2_bzread(bzf, (void *)_payload.second, _payload.first);
      if (rc != _payload.first)
        goto err;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::BZRead(%p) failed", bzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPayload::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzwrite(bzf, _payload.second, _payload.first);
      if (rc != _payload.first)
        goto err;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4UdpPayload::BZWrite(%p) failed", bzf);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4UdpPayload::StreamedLength() const
  {
    return(_payload.first);
  }
  

}  // namespace Dwm
