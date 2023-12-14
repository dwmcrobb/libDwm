//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2020, 2023
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIpv6Prefix.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv6Prefix class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
}

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <sstream>

#include "DwmASIO.hh"
#include "DwmIpv6Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void
  SetBits(struct in6_addr & addr, uint8_t numBits)
  {
    assert(128 >= numBits);
    memset(addr.s6_addr, 0, sizeof(addr.s6_addr));
    uint8_t  *b = addr.s6_addr;
    while (numBits >= 8) {
      *b++ = 0xFF;
      numBits -= 8;
    }
    if (numBits) {
      *b++ = ~(0xFF >> numBits);
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static inline void MaskBits(struct in6_addr & addr, uint8_t numBits)
  {
    if (numBits < 128) {
      uint8_t  byte = numBits >> 3;
      uint8_t  bits = numBits - (byte << 3);
      if (bits) {
        addr.s6_addr[byte] &= ~(0xFF >> bits);
        ++byte;
      }
      memset(&(addr.s6_addr[byte]), 0, sizeof(addr.s6_addr) - byte);
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Prefix::Ipv6Prefix()
  {
    _addr = in6addr_any;
    _length = 128;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Prefix::Ipv6Prefix(const std::string & prefix)
  {
    static std::regex
      rgx("([0-9a-fA-F:]+)([/]([0-9]|[1-9][0-9]|1[01][0-9]|12[0-8])){0,1}",
          std::regex::ECMAScript | std::regex::optimize);

    std::smatch  sm;
    if (regex_match(prefix, sm, rgx)) {
      if (4 == sm.size()) {
        _addr = Ipv6Address(sm[1].str());
        if (! sm[3].str().empty()) {
          _length = atoi(sm[3].str().c_str());
          MaskBits(_addr, _length);
        }
        else {
          _length = 128;
        }
      }
      else {
        _addr = Ipv6Address();
        _length = 128;
      }
    }
    else {
      _addr = Ipv6Address();
      _length = 128;
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Prefix::Ipv6Prefix(const Ipv6Address & network, uint8_t maskLength)
  {
    _addr = network;
    _length = maskLength;
    MaskBits(_addr, _length);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Prefix::Ipv6Prefix(const Ipv6Prefix & prefix)
  {
    _addr = prefix._addr;
    _length = prefix._length;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address Ipv6Prefix::Network() const
  {
    return(_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address Ipv6Prefix::Netmask() const
  {
    in6_addr  addr;
    SetBits(addr, _length);
    return(addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv6Prefix::MaskLength(uint8_t maskLen)
  {
    _length = maskLen;
    MaskBits(_addr, _length);
    return _length;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::Set(const Ipv6Address & network, uint8_t maskLength)
  {
    _addr = network;
    _length = maskLength;
    return(true);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::operator < (const Ipv6Prefix & prefix) const
  {
    bool  rc = false;
    int   cmp = memcmp(_addr.s6_addr, prefix._addr.s6_addr,
                       sizeof(_addr.s6_addr));
    if (cmp < 0)
      rc = true;
    else
      if (cmp == 0)
        if (_length < prefix._length)
          rc = true;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::operator > (const Ipv6Prefix & prefix) const
  {
    bool  rc = false;
    int   cmp = memcmp(_addr.s6_addr, prefix._addr.s6_addr,
                       sizeof(_addr.s6_addr));
    if (cmp > 0)
      rc = true;
    else 
      if (cmp == 0)
        if (_length > prefix._length)
          rc = true;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::operator == (const Ipv6Prefix & prefix) const
  {
    if (_length == prefix._length) {
      return (memcmp(_addr.s6_addr, prefix._addr.s6_addr,
                     (_length + 7) >> 3) == 0);
    }
    return false;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::operator != (const Ipv6Prefix & prefix) const
  {
    bool  rc = false;
    if (_length != prefix._length) {
      rc = true;
    }
    else {
      if (memcmp(_addr.s6_addr, prefix._addr.s6_addr,
                 sizeof(_addr.s6_addr)) != 0) {
        rc = true;
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::Contains(const Ipv6Address & addr) const
  {
    Ipv6Address  a = addr & Netmask();
    if (a == this->Network())
      return(true);
    else
      return(false);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv6Prefix::Read(int fd)
  {
    ssize_t  rc = -1;
    memset(_addr.s6_addr, 0, sizeof(_addr.s6_addr));
    if (fd >= 0) {
      int  bytesRead = read(fd, &_length, sizeof(_length));
      if (bytesRead == sizeof(_length)) {
        rc = bytesRead;
        uint8_t  addrBytes = NumAddrBytes();
        bytesRead = read(fd, _addr.s6_addr, addrBytes);
        if (bytesRead == addrBytes)
          rc += bytesRead;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv6Prefix::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int  bytesWritten = write(fd, &_length, sizeof(_length));
      if (bytesWritten == sizeof(_length)) {
        rc = bytesWritten;
        uint8_t  addrBytes = NumAddrBytes();
        bytesWritten = write(fd, _addr.s6_addr, addrBytes);
        if (bytesWritten == addrBytes)
          rc += bytesWritten;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv6Prefix::Read(FILE *f)
  {
    size_t  rc = 0;
    memset(_addr.s6_addr, 0, sizeof(_addr.s6_addr));
    if (f) {
      if (fread(&_length, sizeof(_length), 1, f) == 1) {
        uint8_t  addrBytes = NumAddrBytes();
        if (fread(_addr.s6_addr, addrBytes, 1, f) == 1) {
          rc = 1;
        }
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv6Prefix::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      if (fwrite(&_length, sizeof(_length), 1, f) == 1) {
        uint8_t  addrBytes = NumAddrBytes();
        if (fwrite(_addr.s6_addr, addrBytes, 1, f) == 1) {
          rc = 1;
        }
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv6Prefix::Read(std::istream & is)
  {
    memset(_addr.s6_addr, 0, sizeof(_addr.s6_addr));
    if (is) {
      if (is.read((char *)&_length, sizeof(_length)))
        is.read((char *)_addr.s6_addr, NumAddrBytes());
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv6Prefix::Write(std::ostream & os) const
  {
    if (os) {
      if (os.write((const char *)&_length, sizeof(_length)))
        os.write((const char *)_addr.s6_addr, NumAddrBytes());
    }
    return(os);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv6Prefix::Read(gzFile gzf)
  {
    int  rc = -1;
    memset(_addr.s6_addr, 0, sizeof(_addr.s6_addr));
    if (gzf) {
      int  bytesRead = gzread(gzf, &_length, sizeof(_length));
      if (bytesRead == sizeof(_length)) {
        rc = bytesRead;
        uint8_t  addrBytes = NumAddrBytes();
        bytesRead = gzread(gzf, _addr.s6_addr, addrBytes);
        if (bytesRead == addrBytes)
          rc += bytesRead;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv6Prefix::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      int  bytesWritten = gzwrite(gzf, (void *)(&_length), sizeof(_length));
      if (bytesWritten == sizeof(_length)) {
        rc = bytesWritten;
        uint8_t  addrBytes = NumAddrBytes();
        bytesWritten = gzwrite(gzf, (void *)(_addr.s6_addr), addrBytes);
        if (bytesWritten == addrBytes)
          rc += bytesWritten;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv6Prefix::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    memset(_addr.s6_addr, 0, sizeof(_addr.s6_addr));
    if (bzf) {
      int  bytesRead = BZ2_bzread(bzf, &_length, sizeof(_length));
      if (bytesRead == sizeof(_length)) {
        rc = bytesRead;
        uint8_t  addrBytes = NumAddrBytes();
        bytesRead = BZ2_bzread(bzf, _addr.s6_addr, addrBytes);
        if (bytesRead == addrBytes)
          rc += bytesRead;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv6Prefix::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      int  bytesWritten = BZ2_bzwrite(bzf, (void *)(&_length),
                                      sizeof(_length));
      if (bytesWritten == sizeof(_length)) {
        rc = bytesWritten;
        uint8_t  addrBytes = NumAddrBytes();
        bytesWritten = BZ2_bzwrite(bzf, (void *)(_addr.s6_addr), addrBytes);
        if (bytesWritten == addrBytes)
          rc += bytesWritten;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::Read(boost::asio::ip::tcp::socket & s)
  { return ASIO_Read(s); }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::Read(boost::asio::local::stream_protocol::socket & s)
  { return ASIO_Read(s); }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::Write(boost::asio::ip::tcp::socket & s) const
  { return ASIO_Write(s); }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Prefix::Write(boost::asio::local::stream_protocol::socket & s) const
  { return ASIO_Write(s); }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Ipv6Prefix::ToString() const
  {
    std::ostringstream  os;
    os << (std::string)Ipv6Address(_addr) << "/" << (uint16_t)_length;
    return(os.str());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os,
                              const Ipv6Prefix & prefix)
  {
    if (os) {
      os << prefix.ToString();
    }
    
    return(os);
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
