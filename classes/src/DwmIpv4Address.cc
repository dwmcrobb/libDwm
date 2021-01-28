//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIpv4Address.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4Address class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
}

#include "DwmIpv4Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address::Ipv4Address()
      : _addr(0)
  { }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address::Ipv4Address(const std::string & dottedAddr)
      : _addr(0)
  {
    if (! dottedAddr.empty())
      _addr = inet_addr(dottedAddr.c_str());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address::operator std::string () const
  {
    struct in_addr  inAddr;
    inAddr.s_addr = _addr;
    char  buf[16] = { '\0' };
    inet_ntop(AF_INET, &inAddr, buf, 16);
    return(buf);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t Ipv4Address::StreamedLength() const
  {
    return(sizeof(_addr));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4Address::Read(std::istream & is)
  {
    if (is)
      is.read((char *)&_addr,sizeof(_addr));
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv4Address::Write(std::ostream & os) const
  {
    if (os)
      os.write((char *)&_addr,sizeof(_addr));
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4Address::Read(int fd) 
  {
    ssize_t  rc = -1;
    if (fd >= 0)
      rc = read(fd,(void *)&_addr,sizeof(_addr));
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4Address::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0)
      rc = write(fd,(void *)&_addr,sizeof(_addr));
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4Address::Read(FILE * f)
  {
    size_t  rc = 0;
    if (f)
      rc = fread((void *)&_addr,sizeof(_addr),1,f);
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4Address::Write(FILE * f) const
  {
    size_t  rc = 0;
    if (f)
      rc = fwrite((void *)&_addr,sizeof(_addr),1,f);
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4Address::Read(gzFile gzf) 
  {
    int  rc = -1;
    if (gzf)
      rc = gzread(gzf,(void *)&_addr,sizeof(_addr));
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4Address::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf)
      rc = gzwrite(gzf,(void *)&_addr,sizeof(_addr));
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4Address::BZRead(BZFILE *bzf) 
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzread(bzf,(void *)&_addr,sizeof(_addr));
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4Address::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzwrite(bzf,(void *)&_addr,sizeof(_addr));
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4Address::IpSum() const
  {
    return((_addr >> 16) + (_addr & 0xFFFF));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const Ipv4Address & addr)
  {
    if (os) {
      struct in_addr  inAddr;
      inAddr.s_addr = addr._addr;
      os << inet_ntoa(inAddr);
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address operator & (const Ipv4Address & addr,
                          const Ipv4Address & mask)
  {
    Ipv4Address  rc(addr);
    rc._addr &= mask._addr;
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
