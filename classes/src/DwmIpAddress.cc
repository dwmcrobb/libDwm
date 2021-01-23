//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020
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
//!  \file DwmIpAddress.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::IpAddress class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <arpa/inet.h>
}

#include "DwmIpAddress.hh"
#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpAddress::IpAddress(const IpAddress & addr)
      : _addr(addr._addr)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpAddress::IpAddress(const Ipv6Address & addr)
      : _addr(addr)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpAddress::IpAddress(const Ipv4Address & addr)
      : _addr(addr)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpAddress::IpAddress(const std::string & addr)
  {
    if (! addr.empty()) {
      struct in6_addr  in6Addr;
      if (inet_pton(AF_INET6, addr.c_str(), &in6Addr) == 1) {
        _addr = Ipv6Address(in6Addr);
      }
      else {
        struct in_addr  inAddr;
        if (inet_pton(AF_INET, addr.c_str(), &inAddr) == 1) {
          _addr = Ipv4Address(inAddr.s_addr);
        }
        else {
          _addr = Ipv4Address(INADDR_NONE);
        }
      }
    }
    else {
      _addr = Ipv4Address(INADDR_NONE);
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpAddress & IpAddress::operator = (const IpAddress & addr)
  {
    if (&addr != this) {
      _addr = addr._addr;
    }
    return *this;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpAddress::operator std::string () const
  {
    std::string   rc;
    const Ipv6Address  *p6 = std::get_if<Ipv6Address>(&_addr);
    if (nullptr != p6) {
      rc = (std::string)(*p6);
    }
    else {
      const Ipv4Address  *p4 = std::get_if<Ipv4Address>(&_addr);
      if (nullptr != p4) {
        rc = (std::string)(*p4);
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpAddress::operator < (const IpAddress & addr) const
  {
    return (_addr < addr._addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpAddress::operator == (const IpAddress & addr) const
  {
    return (_addr == addr._addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const IpAddress & addr)
  {
    os << (std::string)addr;
    return os;
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  uint32_t IpAddress::StreamedLength() const
  {
    return IOUtils::StreamedLength(_addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IpAddress::Read(std::istream & is)
  {
    return StreamIO::Read(is, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IpAddress::Write(std::ostream & os) const
  {
    return StreamIO::Write(os, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IpAddress::Read(int fd)
  {
    return DescriptorIO::Read(fd, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IpAddress::Write(int fd) const
  {
    return DescriptorIO::Write(fd, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IpAddress::Read(FILE * f)
  {
    return FileIO::Read(f, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IpAddress::Write(FILE * f) const
  {
    return FileIO::Write(f, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int IpAddress::Read(gzFile gzf)
  {
    return GZIO::Read(gzf, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int IpAddress::Write(gzFile gzf) const
  {
    return GZIO::Write(gzf, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int IpAddress::BZRead(BZFILE *bzf)
  {
    return BZ2IO::BZRead(bzf, _addr);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int IpAddress::BZWrite(BZFILE *bzf) const
  {
    return BZ2IO::BZWrite(bzf, _addr);
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
