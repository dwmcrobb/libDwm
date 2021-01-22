//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv6Address.cc 11347 $
// @(#) $Id: DwmIpv6Address.cc 11347 2020-12-08 00:53:09Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2020
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
//!  \file DwmIpv6Address.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv6Address class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>  // for inet_ntop()/inet_pton()
  #include <sys/uio.h>     // for read()/write()
  #include <netinet/in.h>  // for inet_ntop()/inet_pton()
  #include <arpa/inet.h>   // for inet_ntop()/inet_pton()
  #include <stdio.h>       // for fread()/fwrite()
  #include <string.h>      // for memset()/memcpy()
  #include <unistd.h>      // for read()/write()
}

#include <cassert>

#include "DwmSvnTag.hh"
#include "DwmIpv6Address.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv6Address.cc 11347 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address::Ipv6Address()
  {
    memset(&_addr, 0, sizeof(_addr));
  }
  
  //------------------------------------------------------------------------
  //!  Construct from a string.
  //------------------------------------------------------------------------
  Ipv6Address::Ipv6Address(const std::string & addr)
  {
    memset(&_addr, 0, sizeof(_addr));
    if (! addr.empty()) {
      if (inet_pton(AF_INET6, addr.c_str(), &_addr) != 1)
        memset(&_addr, 0, sizeof(_addr));
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address::Ipv6Address(const struct in6_addr & addr)
  {
    memcpy(&(_addr.s6_addr), &(addr.s6_addr), sizeof(_addr.s6_addr));
  }
  
  //------------------------------------------------------------------------
  //!  Returns a dotted-decimal string representation.
  //------------------------------------------------------------------------
  Ipv6Address::operator std::string () const
  {
    std::string  rc;
    
    char  buf[64];
    if (inet_ntop(AF_INET6, &_addr, buf, 64))
      rc = buf;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address::operator struct in6_addr () const
  {
    in6_addr  rc = _addr;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Address::operator < (const Ipv6Address & addr) const
  {
    return(memcmp(_addr.s6_addr, addr._addr.s6_addr, sizeof(_addr.s6_addr)) < 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Address::operator > (const Ipv6Address & addr) const
  {
    return(memcmp(_addr.s6_addr, addr._addr.s6_addr, sizeof(_addr.s6_addr)) > 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Address::operator == (const Ipv6Address & addr) const
  {
    return(memcmp(_addr.s6_addr,addr._addr.s6_addr,sizeof(_addr.s6_addr)) == 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address & Ipv6Address::operator &= (const Ipv6Address & netmask)
  {
    for (uint8_t i = 0; i < sizeof(_addr.s6_addr); ++i)
      _addr.s6_addr[i] &= netmask._addr.s6_addr[i];
    return(*this);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv6Address Ipv6Address::operator & (const Ipv6Address & netmask) const
  {
    Ipv6Address  rc(*this);
    for (uint8_t i = 0; i < sizeof(rc._addr.s6_addr); ++i)
      rc._addr.s6_addr[i] &= netmask._addr.s6_addr[i];
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Address::IsLinkLocal() const
  {
    return ((_addr.s6_addr[0] == 0xFE)
            && ((_addr.s6_addr[1] & 0xC0) == 0x80));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv6Address::IsSiteLocal() const
  {
    return ((_addr.s6_addr[0] == 0xFE)
            && ((_addr.s6_addr[1] & 0xC0) == 0xC0));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv6Address::StreamedLength() const
  {
    return(sizeof(_addr));
  }

  //------------------------------------------------------------------------
  //!  Reads from an istream.  Returns the istream.
  //------------------------------------------------------------------------
  std::istream & Ipv6Address::Read(std::istream & is)
  {
    if (is)
      is.read((char *)&_addr, sizeof(_addr));
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  Writes to an ostream.  Returns the ostream.
  //------------------------------------------------------------------------
  std::ostream & Ipv6Address::Write(std::ostream & os) const
  {
    if (os)
      os.write((char *)&_addr, sizeof(_addr));
    return(os);
  }

  //------------------------------------------------------------------------
  //!  Reads from a file descriptor.  Returns the number of bytes read
  //!  (16 on success).
  //------------------------------------------------------------------------
  ssize_t Ipv6Address::Read(int fd)
  {
    ssize_t  rc = 0;
    if (fd >= 0)
      rc = read(fd, &_addr, sizeof(_addr));
    return(rc);
  }
    
  //------------------------------------------------------------------------
  //!  Writes to a file descriptor.  Returns the number of bytes written
  //!  (4 on success).
  //------------------------------------------------------------------------
  ssize_t Ipv6Address::Write(int fd) const
  {
    ssize_t  rc = 0;
    if (fd >= 0)
      rc = write(fd, &_addr, sizeof(_addr));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t Ipv6Address::Read(FILE * f)
  {
    size_t  rc = 0;
    if (f)
      if (fread(&_addr, sizeof(_addr), 1, f) == 1)
        rc = 1;
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t Ipv6Address::Write(FILE * f) const
  {
    size_t  rc = 0;
    if (f)
      if (fwrite(&_addr, sizeof(_addr), 1, f) == 1)
        rc = 1;
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  Reads from a gzFile.  Returns the number of bytes read
  //!  (16 on success).
  //------------------------------------------------------------------------
  int Ipv6Address::Read(gzFile gzf)
  {
    int  rc = -1;
    if (gzf)
      rc = gzread(gzf, &_addr, sizeof(_addr));
    return(rc);
  }
    
  //------------------------------------------------------------------------
  //!  Writes to a gzFile.  Returns the number of bytes written
  //!  (4 on success).
  //------------------------------------------------------------------------
  int Ipv6Address::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf)
      rc = gzwrite(gzf, (void *)(&_addr), sizeof(_addr));
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  Reads from a BZFILE pointer.  Returns the number of bytes read
  //!  (16 on success).
  //------------------------------------------------------------------------
  int Ipv6Address::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzread(bzf, &_addr, sizeof(_addr));
    return(rc);
  }
    
  //------------------------------------------------------------------------
  //!  Writes to a BZFILE pointer.  Returns the number of bytes written
  //!  (4 on success).
  //------------------------------------------------------------------------
  int Ipv6Address::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzwrite(bzf, (void *)&_addr, sizeof(_addr));
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const Ipv6Address & addr)
  {
    if (os) {
      os << (std::string)addr;
    }
    return os;
  }
  
}  // namespace Dwm
