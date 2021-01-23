//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2020
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
//!  \file DwmIpv4UdpPacket.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4UdpPacket class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
}

#include "DwmIpv4UdpPacket.hh"
#include "DwmSysLogger.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPacket::Ipv4UdpPacket()
      : _ipHeader(), _udpHeader(), _udpPayload()
  {
    _ownData = true;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPacket::Ipv4UdpPacket(uint8_t *ptr)
      : _ipHeader((struct ip *)ptr), 
        _udpHeader(_ipHeader),
        _udpPayload(_udpHeader),
        _ownData(false)
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpPacket::~Ipv4UdpPacket()
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4PacketHeader & Ipv4UdpPacket::IpHeader() const
  {
    return(_ipHeader);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader & Ipv4UdpPacket::IpHeader()
  {
    return(_ipHeader);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4UdpHeader & Ipv4UdpPacket::UdpHeader() const
  {
    return(_udpHeader);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader & Ipv4UdpPacket::UdpHeader()
  {
    return(_udpHeader);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4UdpPacket::Read(std::istream & is)
  {
    if (is) {
      if (_ipHeader.Read(is)) {
        if (_udpHeader.Read(is)) {
          _udpPayload.Read(is, _udpHeader.UdpLength() - 8);
        }
        else {
          Syslog(LOG_ERR, "Failed to read Ipv4UdpHeader");
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
  std::ostream & Ipv4UdpPacket::Write(std::ostream & os) const
  {
    if (os) {
      if (_ipHeader.Write(os)) {
        if (_udpHeader.Write(os)) {
          _udpPayload.Write(os);
        }
        else {
          Syslog(LOG_ERR, "Failed to write Ipv4UdpHeader");
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
  ssize_t Ipv4UdpPacket::Read(int fd)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesRead = _ipHeader.Read(fd);
      if (bytesRead != _ipHeader.StreamedLength())
        goto err;
      rc = bytesRead;
      bytesRead = _udpHeader.Read(fd);
      if (bytesRead != _udpHeader.StreamedLength())
        goto err;
      rc += bytesRead;
      if (_udpHeader.UdpLength() > 8) {
        bytesRead = _udpPayload.Read(fd, _udpHeader.UdpLength() - 8);
        if (bytesRead != _udpHeader.UdpLength() - 8)
          goto err;
        rc += bytesRead;
      }
      else {
        _udpPayload.Clear();
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::Read(%d) failed", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4UdpPacket::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesWritten = _ipHeader.Write(fd);
      if (bytesWritten == _ipHeader.StreamedLength()) {
        rc = bytesWritten;
        bytesWritten = _udpHeader.Write(fd);
        if (bytesWritten == _udpHeader.StreamedLength()) {
          rc += bytesWritten;
          bytesWritten = _udpPayload.Write(fd);
          if (bytesWritten == _udpPayload.StreamedLength()) {
            rc += bytesWritten;
          }
          else {
            rc = -1;
            Syslog(LOG_ERR, "_udpPayload.Write(%d) failed: %m", fd);
          }
        }
        else {
          rc = -1;
            Syslog(LOG_ERR, "_udpHeader.Write(%d) failed: %m", fd);
        }
      }
      else {
        rc = -1;
        Syslog(LOG_ERR, "_ipHeader.Write(%d) failed: %m", fd);
      }
    }
    else {
      Syslog(LOG_ERR, "Bad descriptor %d", fd);
    }
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4UdpPacket::Read(FILE *f)
  {
    size_t  rc = 0;
    if (f) {
      if (! _ipHeader.Read(f))
        goto err;
      if (! _udpHeader.Read(f))
        goto err;
      if (_udpHeader.UdpLength() > 8) {
        if (! _udpPayload.Read(f, _udpHeader.UdpLength() - 8))
          goto err;
      }
      else {
        _udpPayload.Clear();
      }
      rc = 1;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::Read(FILE %p) failed", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //! 
  //--------------------------------------------------------------------------
  size_t Ipv4UdpPacket::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      if (! _ipHeader.Write(f))
        goto err;
      if (! _udpHeader.Write(f))
        goto err;
      if (_udpPayload.StreamedLength() > 0)
        if (! _udpPayload.Write(f))
          goto err;
      rc = 1;
    }
    
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::Write(FILE %p) failed", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPacket::Read(gzFile gzf)
  {
    int  rc = -1;
    if (gzf) {
      int  bytesRead = _ipHeader.Read(gzf);
      if (bytesRead < 0)
        goto err;
      rc = bytesRead;
      bytesRead = _udpHeader.Read(gzf);
      if (bytesRead != _udpHeader.StreamedLength())
        goto err;
      rc += bytesRead;
      if (_udpHeader.UdpLength() > 8) {
        bytesRead = _udpPayload.Read(gzf, _udpHeader.UdpLength() - 8);
        if (bytesRead != _udpHeader.UdpLength() - 8)
          goto err;
        rc += bytesRead;
      }
      else {
        _udpPayload.Clear();
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::Read(%p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPacket::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      int  bytesWritten = _ipHeader.Write(gzf);
      if (bytesWritten != _ipHeader.StreamedLength())
        goto err;
      rc = bytesWritten;
      bytesWritten = _udpHeader.Write(gzf);
      if (bytesWritten != _udpHeader.StreamedLength())
        goto err;
      rc += bytesWritten;
      bytesWritten = _udpPayload.Write(gzf);
      if (bytesWritten != _udpPayload.StreamedLength())
        goto err;
      rc += bytesWritten;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::Write(%p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPacket::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    if (bzf) {
      int  bytesRead = _ipHeader.BZRead(bzf);
      if (bytesRead != _ipHeader.StreamedLength())
        goto err;
      rc = bytesRead;
      bytesRead = _udpHeader.BZRead(bzf);
      if (bytesRead != _udpHeader.StreamedLength())
        goto err;
      rc += bytesRead;
      if (_udpHeader.UdpLength() > 8) {
        bytesRead = _udpPayload.BZRead(bzf, _udpHeader.UdpLength() - 8);
        if (bytesRead != _udpHeader.UdpLength() - 8)
          goto err;
        rc += bytesRead;
      }
      else {
        _udpPayload.Clear();
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::BZRead(%p) failed", bzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpPacket::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      int  bytesWritten = _ipHeader.BZWrite(bzf);
      if (bytesWritten != _ipHeader.StreamedLength())
        goto err;
      rc = bytesWritten;
      bytesWritten = _udpHeader.BZWrite(bzf);
      if (bytesWritten != _udpHeader.StreamedLength())
        goto err;
      rc += bytesWritten;
      bytesWritten = _udpPayload.BZWrite(bzf);
      if (bytesWritten != _udpPayload.StreamedLength())
        goto err;
      rc += bytesWritten;
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "Ipv4UdpPacket::BZWrite(%p) failed", bzf);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4UdpPacket::StreamedLength() const
  {
    return(_ipHeader.StreamedLength() +
           _udpHeader.StreamedLength() +
           _udpPayload.StreamedLength());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4UdpPacket::SendTo(int fd) const
  {
    bool  rc = false;
    if (fd >= 0) {
      std::ostringstream  os;
      _ipHeader.Write(os);
      _udpHeader.Write(os);
      _udpPayload.Write(os);
      struct sockaddr_in  toAddr;
      memset(&toAddr, 0, sizeof(toAddr));
#ifndef __linux__
      toAddr.sin_len = sizeof(toAddr);
#endif
      toAddr.sin_family = PF_INET;
      // toAddr.sin_port = htons(_udpHeader.DestinationPort());
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
  bool Ipv4UdpPacket::SendTo(Socket & s) const
  {
    bool  rc = false;
    if (s) {
      std::ostringstream  os;
      if (s.HeaderIncluded()) {
        _ipHeader.Write(os);
        _udpHeader.Write(os);
      }
      _udpPayload.Write(os);
      struct sockaddr_in  toAddr;
      memset(&toAddr, 0, sizeof(toAddr));
#ifndef __linux__
      toAddr.sin_len = sizeof(toAddr);
#endif
      toAddr.sin_family = PF_INET;
      toAddr.sin_port = htons(_udpHeader.DestinationPort());
      toAddr.sin_addr.s_addr = _ipHeader.DestinationAddress().Raw();
      if (s.SendTo(os.str().c_str(), os.str().length(), 0, toAddr) > 0) {
        rc = true;
      }
      else {
        Syslog(LOG_ERR, "SendTo(%p, %u, 0, %s:%hu) failed: %m",
               os.str().c_str(), os.str().length(), 
               ((std::string)_ipHeader.DestinationAddress()).c_str(),
               _udpHeader.DestinationPort());
      }
    }
    return(rc);
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
