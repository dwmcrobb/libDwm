//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004, 2024-2026
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmIpAddress.hh
//!  \brief Dwm::IpAddress class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPADDRESS_HH_
#define _DWMIPADDRESS_HH_

extern "C" {
  #include <sys/socket.h>
}

#include <cstdint>
#include <string>
#include <variant>

#include "DwmIpv4Address.hh"
#include "DwmIpv6Address.hh"
#include "DwmDescriptorIO.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates an IP address (IPv4 or IPv6).
  //--------------------------------------------------------------------------
  class IpAddress
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor.
    //------------------------------------------------------------------------
    IpAddress()
        : _addr(Ipv4Address(INADDR_NONE))
    {}

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    IpAddress(const IpAddress & addr);

    //------------------------------------------------------------------------
    //!  Copy assignment operator.
    //------------------------------------------------------------------------
    IpAddress & operator = (const IpAddress & addr);
    
    //------------------------------------------------------------------------
    //!  Construct from an Ipv6Address.
    //------------------------------------------------------------------------
    IpAddress(const Ipv6Address & addr);

    //------------------------------------------------------------------------
    //!  Construct from an Ipv4Address.
    //------------------------------------------------------------------------
    IpAddress(const Ipv4Address & addr);

    //------------------------------------------------------------------------
    //!  Consturct from a string.
    //------------------------------------------------------------------------
    IpAddress(const std::string & addr);
    
    //------------------------------------------------------------------------
    //!  Returns the address family (AF_INET or AF_INET6) of the address.
    //------------------------------------------------------------------------
    int Family() const  { return k_families[_addr.index()]; }

    //------------------------------------------------------------------------
    //!  Returns true if the contained address is an IPv4 address.
    //------------------------------------------------------------------------
    bool IsV4() const  { return (1 == _addr.index()); }

    //------------------------------------------------------------------------
    //!  Returns true if the contained address is an IPv6 address.
    //------------------------------------------------------------------------
    bool IsV6() const  { return (0 == _addr.index()); }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    const T * Addr() const  { return std::get_if<T>(&_addr); };
    
    //------------------------------------------------------------------------
    //!  Returns a string representation.
    //------------------------------------------------------------------------
    operator std::string () const;

    //------------------------------------------------------------------------
    //!  Less-than operator.
    //------------------------------------------------------------------------
    bool operator < (const IpAddress & addr) const;

    //------------------------------------------------------------------------
    //!  Equality operator.
    //------------------------------------------------------------------------
    bool operator == (const IpAddress & addr) const;

    //------------------------------------------------------------------------
    //!  ostream output operator.
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const IpAddress & addr);
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  one of the non-compressing Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;
    
    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    inline std::istream & Read(std::istream & is)
    { return StreamIO::Read(is, _addr); }
    
    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    inline std::ostream & Write(std::ostream & os) const
    { return StreamIO::Write(os, _addr); }

    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    inline std::istream & NRead(std::istream & is)
    { return Read(is); }
    
    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & NWrite(std::ostream & os) const
    { return Write(os); }
    
    //------------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes read
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline ssize_t Read(int fd)
    { return DescriptorIO::Read(fd, _addr); }
    
    //------------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline ssize_t Write(int fd) const
    { return DescriptorIO::Write(fd, _addr); }

    //------------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes read
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline ssize_t NRead(int fd)
    { return Read(fd); }
    
    //------------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline ssize_t NWrite(int fd) const
    { return Write(fd); }
    
    //------------------------------------------------------------------------
    //!  Reads from a FILE pointer.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE * f);
    
    //------------------------------------------------------------------------
    //!  Writes to a FILE pointer.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE * f) const;

    //------------------------------------------------------------------------
    //!  Reads from a gzFile.  Returns the number of bytes read
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);
    
    //------------------------------------------------------------------------
    //!  Writes to a gzFile.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;
    
    //------------------------------------------------------------------------
    //!  Reads from a BZFILE pointer.  Returns the number of bytes read
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);
    
    //------------------------------------------------------------------------
    //!  Writes to a BZFILE pointer.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;
    
  private:
    std::variant<Ipv6Address,Ipv4Address>  _addr;

    static constexpr int  k_families[2] = { AF_INET6, AF_INET };
  };
  
}  // namespace Dwm

#if __has_include(<format>)                                                                             
namespace std {
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <>
  struct formatter<Dwm::IpAddress> 
  {
    constexpr auto parse(format_parse_context & ctx) 
    { return ctx.begin(); }
    
    auto format(const Dwm::IpAddress & addr, format_context & ctx) const
    {
      return format_to(ctx.out(), "{}", (string)addr);
    }
  };
}

#else

#  if __has_include(<fmt/format.h>)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template <>
struct fmt::formatter<Dwm::IpAddress> 
{
  constexpr auto parse(fmt::format_parse_context & ctx) 
  { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Dwm::IpAddress & addr, FormatContext & ctx) 
  {
    return fmt::format_to(ctx.out(), "{}", (std::string)addr);
  }
};

#  endif

#endif  // if __has_include(<format>)

#endif  // _DWMIPADDRESS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
