//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4TcpPacket.cc 11110 $
// @(#) $Id: DwmIpv4TcpPacket.cc 11110 2020-09-07 22:50:08Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006
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
//!  \file DwmIpv4TcpPacket.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4TcpPacket class implementation
//---------------------------------------------------------------------------

extern "C" {
  #ifdef __linux__
    #define __FAVOR_BSD 1
  #endif
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
}

#include "DwmSvnTag.hh"
#include "DwmIpv4TcpPacket.hh"
#include "DwmSysLogger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4TcpPacket.cc 11110 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPacket::Ipv4TcpPacket()
      : _ipHeader(), _tcpHeader(), _tcpPayload()
  {
    _ownData = true;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPacket::Ipv4TcpPacket(uint8_t *ptr)
      : _ipHeader((struct ip *)ptr), 
        _tcpHeader(_ipHeader),
        _tcpPayload(_ipHeader, _tcpHeader),
        _ownData(false)
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpPacket::~Ipv4TcpPacket()
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4PacketHeader & Ipv4TcpPacket::IpHeader() const
  {
    return(_ipHeader);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader & Ipv4TcpPacket::IpHeader()
  {
    return(_ipHeader);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4TcpHeader & Ipv4TcpPacket::TcpHeader() const
  {
    return(_tcpHeader);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4TcpHeader & Ipv4TcpPacket::TcpHeader()
  {
    return(_tcpHeader);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4TcpPacket::Read(std::istream & is)
  {
    _tcpPayload.Clear();
    
    if (is) {
      if (_ipHeader.Read(is)) {
        if (_tcpHeader.Read(is)) {
          _tcpPayload.Read(is, 
                           _ipHeader.TotalLength() - (_ipHeader.HeaderLength() + _tcpHeader.DataOffset()));
        }
        else {
          Syslog(LOG_ERR, "Failed to read Ipv4TcpHeader");
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to read Ipv4PacketHeader");
      }
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv4TcpPacket::Write(std::ostream & os) const
  {
    if (os) {
      if (_ipHeader.Write(os)) {
        if (_tcpHeader.Write(os)) {
          _tcpPayload.Write(os);
        }
        else {
          Syslog(LOG_ERR, "Failed to write Ipv4TcpHeader");
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to write Ipv4PacketHeader");
      }
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4TcpPacket::Read(int fd)
  {
    ssize_t  rc = -1;
    _tcpPayload.Clear();
    if (fd >= 0) {
      ssize_t  bytesRead = _ipHeader.Read(fd);
      if (bytesRead != _ipHeader.StreamedLength())
        goto err;
      rc = bytesRead;
      bytesRead = _tcpHeader.Read(fd);
      if (bytesRead != _tcpHeader.StreamedLength())
        goto err;
      rc += bytesRead;
      uint16_t  payloadLen = _ipHeader.TotalLength();
      payloadLen -= _ipHeader.HeaderLength();
      payloadLen -= _tcpHeader.DataOffset();
      if (payloadLen) {
        bytesRead = _tcpPayload.Read(fd, payloadLen);
        if (bytesRead != payloadLen)
          goto err;
        rc += bytesRead;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::Read(%d) failed", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4TcpPacket::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesWritten = _ipHeader.Write(fd);
      if (bytesWritten != _ipHeader.StreamedLength())
        goto err;
      rc = bytesWritten;
      bytesWritten = _tcpHeader.Write(fd);
      if (bytesWritten != _tcpHeader.StreamedLength())
        goto err;
      rc += bytesWritten;
      if (_tcpPayload.StreamedLength()) {
        bytesWritten = _tcpPayload.Write(fd);
        if (bytesWritten != _tcpPayload.StreamedLength())
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::Write(%d) failed: %m", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4TcpPacket::Read(FILE *f)
  {
    size_t  rc = 0;
    _tcpPayload.Clear();
    
    if (f) {
      if (! _ipHeader.Read(f))
        goto err;
      if (! _tcpHeader.Read(f))
        goto err;
      uint16_t  payloadLen = _ipHeader.TotalLength();
      payloadLen -= _ipHeader.HeaderLength();
      payloadLen -= _tcpHeader.DataOffset();
      if (payloadLen)
        if (! _tcpPayload.Read(f, payloadLen))
          goto err;
      rc = 1;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::Read(FILE %p) failed", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4TcpPacket::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      if (! _ipHeader.Write(f))
        goto err;
      if (! _tcpHeader.Write(f))
        goto err;
      if (_tcpPayload.StreamedLength())
        if (! _tcpPayload.Write(f))
          goto err;
      rc = 1;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::Write(FILE %p) failed", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPacket::Read(gzFile gzf)
  {
    int  rc = -1;
    _tcpPayload.Clear();
    if (gzf) {
      int  bytesRead = _ipHeader.Read(gzf);
      if (bytesRead != _ipHeader.StreamedLength())
        goto err;
      rc = bytesRead;
      bytesRead = _tcpHeader.Read(gzf);
      if (bytesRead != _tcpHeader.StreamedLength())
        goto err;
      rc += bytesRead;
      uint16_t  payloadLen = _ipHeader.TotalLength();
      payloadLen -= _ipHeader.HeaderLength();
      payloadLen -= _tcpHeader.DataOffset();
      if (payloadLen) {
        bytesRead = _tcpPayload.Read(gzf, payloadLen);
        if (bytesRead != payloadLen)
          goto err;
        rc += bytesRead;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::Read(gzFile %p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPacket::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      int  bytesWritten = _ipHeader.Write(gzf);
      if (bytesWritten != _ipHeader.StreamedLength())
        goto err;
      rc = bytesWritten;
      bytesWritten = _tcpHeader.Write(gzf);
      if (bytesWritten != _tcpHeader.StreamedLength())
        goto err;
      rc += bytesWritten;
      if (_tcpPayload.StreamedLength()) {
        bytesWritten = _tcpPayload.Write(gzf);
        if (bytesWritten != _tcpPayload.StreamedLength())
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::Write(gzFile %p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPacket::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    _tcpPayload.Clear();
    if (bzf) {
      int  bytesRead = _ipHeader.BZRead(bzf);
      if (bytesRead != _ipHeader.StreamedLength())
        goto err;
      rc = bytesRead;
      bytesRead = _tcpHeader.BZRead(bzf);
      if (bytesRead != _tcpHeader.StreamedLength())
        goto err;
      rc += bytesRead;
      uint16_t  payloadLen = _ipHeader.TotalLength();
      payloadLen -= _ipHeader.HeaderLength();
      payloadLen -= _tcpHeader.DataOffset();
      if (payloadLen) {
        bytesRead = _tcpPayload.BZRead(bzf, payloadLen);
        if (bytesRead != payloadLen)
          goto err;
        rc += bytesRead;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::BZRead(%p) failed", bzf);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4TcpPacket::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      int  bytesWritten = _ipHeader.BZWrite(bzf);
      if (bytesWritten != _ipHeader.StreamedLength())
        goto err;
      rc = bytesWritten;
      bytesWritten = _tcpHeader.BZWrite(bzf);
      if (bytesWritten != _tcpHeader.StreamedLength())
        goto err;
      rc += bytesWritten;
      if (_tcpPayload.StreamedLength()) {
        bytesWritten = _tcpPayload.BZWrite(bzf);
        if (bytesWritten != _tcpPayload.StreamedLength())
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4TcpPacket::BZWrite(%p) failed", bzf);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4TcpPacket::StreamedLength() const
  {
    return(_ipHeader.StreamedLength() +
           _tcpHeader.StreamedLength() +
           _tcpPayload.StreamedLength());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4TcpPacket::SendTo(int fd) const
  {
    bool  rc = false;
    if (fd >= 0) {
      std::ostringstream  os;
      _ipHeader.Write(os);
      _tcpHeader.Write(os);
      _tcpPayload.Write(os);
      struct sockaddr_in  toAddr;
      memset(&toAddr, 0, sizeof(toAddr));
#ifndef __linux__
      toAddr.sin_len = sizeof(toAddr);
#endif
      toAddr.sin_family = PF_INET;
      // toAddr.sin_port = htons(_tcpHeader.DestinationPort());
      toAddr.sin_addr.s_addr = _ipHeader.DestinationAddress().Raw();
      if (sendto(fd, os.str().c_str(), os.str().length(), 0, 
                 (struct sockaddr *)&toAddr, sizeof(toAddr)) > 0) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "sendto(%d, %p, %u, 0, %p, %u) failed: %m",
               fd, os.str().c_str(), os.str().length(), &toAddr, 
               sizeof(toAddr));
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4TcpPacket::SendTo(Socket & s) const
  {
    bool  rc = false;
    if (s) {
      std::ostringstream  os;
      if (s.HeaderIncluded()) {
        _ipHeader.Write(os);
        _tcpHeader.Write(os);
      }
      _tcpPayload.Write(os);
      struct sockaddr_in  toAddr;
      memset(&toAddr, 0, sizeof(toAddr));
#ifndef __linux__
      toAddr.sin_len = sizeof(toAddr);
#endif
      toAddr.sin_family = PF_INET;
      toAddr.sin_port = htons(_tcpHeader.DestinationPort());
      toAddr.sin_addr.s_addr = _ipHeader.DestinationAddress().Raw();
      if (s.SendTo(os.str().c_str(), os.str().length(), 0, toAddr) > 0) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "SendTo(%p, %u, 0, %s:%hu) failed: %m",
               os.str().c_str(), os.str().length(), 
               ((std::string)_ipHeader.DestinationAddress()).c_str(),
               _tcpHeader.DestinationPort());
      }
    }
    return(rc);
  }

  
}  // namespace Dwm
