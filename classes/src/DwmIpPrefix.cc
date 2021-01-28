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
//!  \file DwmIpPrefix.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::IpPrefix class implementation
//---------------------------------------------------------------------------

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmIpPrefix.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpPrefix::IpPrefix()
      : _prefix(Ipv4Prefix())
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpPrefix::IpPrefix(const IpAddress & network, uint8_t maskLength)
  {
    Set(network, maskLength);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpPrefix::IpPrefix(const Ipv4Prefix & prefix)
      : _prefix(prefix)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpPrefix::IpPrefix(const Ipv6Prefix & prefix)
      : _prefix(prefix)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpPrefix::IpPrefix(const std::string & prefix)
  {
    Ipv6Prefix  v6pfx(prefix);
    if ((v6pfx.Network() == Ipv6Address()) && (v6pfx.MaskLength() == 128)) {
      _prefix = Ipv4Prefix(prefix);
    }
    else {
      _prefix = v6pfx;
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int IpPrefix::Family() const
  {
    return k_families[_prefix.index()];
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  IpPrefix::operator std::string () const
  {
    std::string  rc;
    const Ipv6Prefix  *p6 = Prefix<Ipv6Prefix>();
    if (p6) {
      rc = p6->ToString();
    }
    else {
      const Ipv4Prefix  *p4 = Prefix<Ipv4Prefix>();
      if (p4) {
        rc = p4->ToString();
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::Set(const IpAddress & network, uint8_t maskLength)
  {
    bool  rc = false;
    
    if (network.Family() == AF_INET6) {
      Ipv6Prefix  pfx;
      rc = pfx.Set(*(network.Addr<Ipv6Address>()), maskLength);
      if (rc) {
        _prefix = pfx;
      }
    }
    else if (network.Family() == AF_INET) {
      Ipv4Prefix  pfx;
      rc = pfx.Set(*(network.Addr<Ipv4Address>()), maskLength);
      if (rc) {
        _prefix = pfx;
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::Contains(const IpAddress & addr) const
  {
    bool  rc = false;
    switch (addr.Family()) {
      case AF_INET:
        switch (Family()) {
          case AF_INET:
            rc =
              Prefix<Ipv4Prefix>()->Contains(*(addr.Addr<Ipv4Address>()));
            break;
          default:
            break;
        }
        break;
      case AF_INET6:
        switch (Family()) {
          case AF_INET6:
            rc =
              Prefix<Ipv6Prefix>()->Contains(*(addr.Addr<Ipv6Address>()));
            break;
          default:
            break;
        }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::Contains(const Ipv4Address & addr) const
  {
    bool  rc = false;
    const Ipv4Prefix  *pfx = Prefix<Ipv4Prefix>();
    if (nullptr != pfx) {
      rc = pfx->Contains(addr);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::Contains(const Ipv6Address & addr) const
  {
    bool  rc = false;
    const Ipv6Prefix  *pfx = Prefix<Ipv6Prefix>();
    if (nullptr != pfx) {
      rc = pfx->Contains(addr);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::operator < (const IpPrefix & prefix) const
  {
    return (_prefix < prefix._prefix);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::operator > (const IpPrefix & prefix) const
  {
    return (_prefix > prefix._prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::operator == (const IpPrefix & prefix) const
  {
    return (_prefix == prefix._prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IpPrefix::operator != (const IpPrefix & prefix) const
  {
    return (_prefix != prefix._prefix);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream	& operator << (std::ostream & os, const IpPrefix & addr)
  {
    os << (std::string)addr;
    return os;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IpPrefix::Read(int fd)
  {
    return DescriptorIO::Read(fd, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IpPrefix::Write(int fd) const
  {
    return DescriptorIO::Write(fd, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IpPrefix::Read(FILE *f)
  {
    return FileIO::Read(f, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IpPrefix::Write(FILE *f) const
  {
    return FileIO::Write(f, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & IpPrefix::Read(std::istream & is)
  {
    return StreamIO::Read(is, _prefix);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & IpPrefix::Write(std::ostream & os) const
  {
    return StreamIO::Write(os, _prefix);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int IpPrefix::Read(gzFile gzf)
  {
    return GZIO::Read(gzf, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int IpPrefix::Write(gzFile gzf) const
  {
    return GZIO::Write(gzf, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int IpPrefix::BZRead(BZFILE *bzf)
  {
    return BZ2IO::BZRead(bzf, _prefix);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int IpPrefix::BZWrite(BZFILE *bzf) const
  {
    return BZ2IO::BZWrite(bzf, _prefix);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t IpPrefix::StreamedLength() const
  {
    return IOUtils::StreamedLength(_prefix);
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
