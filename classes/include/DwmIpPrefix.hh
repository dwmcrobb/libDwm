//===========================================================================
// @(#) $DwmPath$
// @(#) $Id$
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
//!  \file DwmIpPrefix.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::IpPrefix class declaration
//---------------------------------------------------------------------------

#ifndef _DWMIPPREFIX_HH_
#define _DWMIPPREFIX_HH_

extern "C" {
  #include <inttypes.h>
  #include <sys/socket.h>
}

#include <variant>

#include "DwmIpAddress.hh"
#include "DwmIpv4Prefix.hh"
#include "DwmIpv6Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  class IpPrefix
    : public StreamIOCapable, public FileIOCapable, public DescriptorIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    IpPrefix();
    IpPrefix(const IpAddress & network, uint8_t maskLength);
    IpPrefix(const Ipv4Prefix & prefix);
    IpPrefix(const Ipv6Prefix & prefix);
    IpPrefix(const std::string & prefix);
    
    int Family() const;

    template <typename T>
    const T * Prefix() const  { return std::get_if<T>(&_prefix); }

    operator std::string () const;
    
    bool Set(const IpAddress & network, uint8_t maskLength);
    
    bool Contains(const IpAddress & addr) const;
    bool Contains(const Ipv4Address & addr) const;
    bool Contains(const Ipv6Address & addr) const;

    bool operator < (const IpPrefix & prefix) const;
    bool operator > (const IpPrefix & prefix) const;
    bool operator == (const IpPrefix & prefix) const;
    bool operator != (const IpPrefix & prefix) const;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const IpPrefix & addr);
    
    //------------------------------------------------------------------------
    //!  Reads the prefix from a file descriptor.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Writes the prefix to a file descriptor.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads the prefix from a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;
    
    //------------------------------------------------------------------------
    //!  Writes the prefix to a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the prefix from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes the prefix to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the prefix from a gzFile.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;

    //------------------------------------------------------------------------
    //!  Writes the prefix to a gzFile.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;

    //------------------------------------------------------------------------
    //!  Reads the prefix from a BZFILE pointer.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;

    //------------------------------------------------------------------------
    //!  Writes the prefix to a BZFILE pointer.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if the prefix
    //!  was written to a file descriptor or ostream.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override;

  private:
    std::variant<Ipv6Prefix,Ipv4Prefix>  _prefix;

    static constexpr int  k_families[2] = { AF_INET6, AF_INET };
  };
    
}  // namespace Dwm

#endif  // _DWMIPPREFIX_HH_
