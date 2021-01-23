//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file DwmIpv4IcmpMessage.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4IcmpMessage class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "DwmIpv4IcmpMessage.hh"
#include "DwmIpv4Utils.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpMessage::Ipv4IcmpMessage()
      : _data(0,(uint8_t *)0), _ownData(false)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpMessage::~Ipv4IcmpMessage()
  {
    Free();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4IcmpMessage::Type() const
  {
    assert(_data.first && _data.second);
    return(_data.second[0]);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4IcmpMessage::Type(uint8_t type)
  {
    if (! _data.first)
      Reallocate(1);
    _data.second[0] = type;
    return(_data.second[0]);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4IcmpMessage::Code() const
  {
    assert((_data.first > 1) && _data.second);
    return(_data.second[1]);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4IcmpMessage::Code(uint8_t code)
  {
    if (_data.first < 2)
      Reallocate(2);
    _data.second[1] = code;
    return(_data.second[1]);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4IcmpMessage::Checksum()
  {
    assert((_data.first > 3) && _data.second);
    return(ntohs(((uint16_t)_data.second[2] << 8) | _data.second[3]));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4IcmpMessage::SetChecksum()
  {
    if (_data.first < 4)
      Reallocate(4);
    uint16_t  *cksum = (uint16_t *)(_data.second + 2);
    *cksum = 0;
    
    //  compute and set the checksum
    uint32_t  sum = Ipv4Utils::Sum(_data.second, _data.first);
    Ipv4Utils::AddCarries(sum);
    //  set the cheecksum
    *cksum = (uint16_t)sum;

    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4IcmpMessage::Read(std::istream & is, uint16_t len)
  {
    Free();
    if (is && len) {
      Allocate(len);
      is.read((char *)_data.second, _data.first);
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv4IcmpMessage::Write(std::ostream & os) const
  {
    if (os && _data.first && _data.second) {
      os.write((const char *)_data.second, _data.first);
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4IcmpMessage::Read(int fd, uint16_t len)
  {
    ssize_t  rc = -1;
    Free();
    if (fd >= 0 && len) {
      Allocate(len);
      ssize_t  bytesRead = read(fd, _data.second, _data.first);
      if (bytesRead == _data.first)
        rc = bytesRead;
    }
    return(rc);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4IcmpMessage::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      if (! _data.first) {
        rc = 0;
      }
      else {
        ssize_t  bytesWritten = write(fd, _data.second, _data.first);
        if (bytesWritten == _data.first)
          rc = bytesWritten;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4IcmpMessage::Read(FILE *f, uint16_t len)
  {
    size_t  rc = 0;
    Free();
    if (f && len) {
      Allocate(len);
      if (fread(_data.second, _data.first, 1, f)) {
        rc = 1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4IcmpMessage::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      if (_data.first) {
        if (fwrite(_data.second, _data.first, 1, f)) {
          rc = 1;
        }
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4IcmpMessage::Read(gzFile gzf, uint16_t len)
  {
    int  rc = -1;
    Free();
    if (gzf && len) {
      Allocate(len);
      int  bytesRead = gzread(gzf, _data.second, _data.first);
      if (bytesRead == _data.first)
        rc = bytesRead;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4IcmpMessage::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      if (! _data.first)
        return(0);
      else {
        int  bytesWritten = gzwrite(gzf, _data.second, _data.first);
        if (bytesWritten == _data.first) {
          rc = bytesWritten;
        }
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4IcmpMessage::BZRead(BZFILE *bzf, uint16_t len)
  {
    int  rc = -1;
    if (bzf && len) {
      Allocate(len);
      int  bytesRead = BZ2_bzread(bzf, _data.second, _data.first);
      if (bytesRead == _data.first) {
        rc = bytesRead;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4IcmpMessage::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      if (! _data.first)
        return(0);
      else {
        int  bytesWritten = BZ2_bzwrite(bzf, _data.second, _data.first);
        if (bytesWritten == _data.first) {
          rc = bytesWritten;
        }
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4IcmpMessage::StreamedLength() const
  {
    return(_data.first);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4IcmpMessage::operator == (const Ipv4IcmpMessage & msg) const
  {
    bool  rc = false;
    if (_data.first == msg._data.first) {
      if (_data.second == msg._data.second) {
        rc = true;
      }
      else {
        if (memcmp(_data.second, msg._data.second, _data.first) == 0) {
          rc = true;
        }
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4IcmpMessage::Free()
  {
    if (_ownData) {
      if (_data.first && _data.second) {
        free(_data.second);
      }
    }
    _data.first = 0;
    _data.second = 0;
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4IcmpMessage::Allocate(uint16_t len)
  {
    Free();
    assert(len && (len <= (65535 - 20)));
    
    _data.first = len;
    _data.second = (uint8_t *)calloc(1, _data.first);
    assert(_data.second);
    _ownData = true;
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4IcmpMessage::Reallocate(uint16_t len)
  {
    if (len != _data.first) {
      if (_ownData) {
        _data.second = (uint8_t *)realloc(_data.second, len);
        if (len > _data.first)
          memset(_data.second + _data.first, 0, (len - _data.first));
        _data.first = len;
      }
      else {
        uint8_t  *oldData = _data.second;
        _data.second = (uint8_t *)calloc(1, len);
        if (oldData)
          memcpy(_data.second, oldData, _data.first);
        _data.first = len;
        _ownData = true;
      }
    }
    return;
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
