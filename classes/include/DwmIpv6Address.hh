//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2006, 2016, 2020, 2023
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
//!  \file DwmIpv6Address.hh
//!  \brief Dwm::Ipv6Address class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV6ADDRESS_HH_
#define _DWMIPV6ADDRESS_HH_

extern "C" {
#include <inttypes.h>
#include <netinet/in.h>
}

#include <string>

#define XXH_INLINE_ALL
#include <xxhash.h>

#include "DwmASIO.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmBZ2IOCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates an IPv6 address.
  //--------------------------------------------------------------------------
  class Ipv6Address
    : public ASIOCapable, public StreamIOCapable, public FileIOCapable,
      public DescriptorIOCapable, public StreamedLengthCapable,
      public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Sets address to 0::
    //------------------------------------------------------------------------
    Ipv6Address();
    
    //------------------------------------------------------------------------
    //!  Construct from a string.
    //------------------------------------------------------------------------
    Ipv6Address(const std::string & addr);

    //------------------------------------------------------------------------
    //!  Construct from an in6_addr.
    //------------------------------------------------------------------------
    Ipv6Address(const struct in6_addr & addr);

    //------------------------------------------------------------------------
    //!  Returns a string representation.
    //------------------------------------------------------------------------
    operator std::string () const;

    //------------------------------------------------------------------------
    //!  Returns an in6_addr representation.
    //------------------------------------------------------------------------
    operator struct in6_addr () const;
    
    //------------------------------------------------------------------------
    //!  Less-than operator.
    //------------------------------------------------------------------------
    bool operator < (const Ipv6Address & addr) const;

    //------------------------------------------------------------------------
    //!  Greater-than operator.
    //------------------------------------------------------------------------
    bool operator > (const Ipv6Address & addr) const;
    
    //------------------------------------------------------------------------
    //!  Equal-to operator.
    //------------------------------------------------------------------------
    bool operator == (const Ipv6Address & addr) const;

    //------------------------------------------------------------------------
    //!  Not-equal-to operator.
    //------------------------------------------------------------------------
    inline bool operator != (const Ipv6Address & addr) const
    { return (! (*this == addr)); }
    
    //------------------------------------------------------------------------
    //!  Mask operator.
    //------------------------------------------------------------------------
    Ipv6Address & operator &= (const Ipv6Address & netmask);
    
    //------------------------------------------------------------------------
    //!  Mask operator.
    //------------------------------------------------------------------------
    Ipv6Address operator & (const Ipv6Address & netmask) const;
    
    //------------------------------------------------------------------------
    //!  Returns true if the address is link-local.
    //------------------------------------------------------------------------
    bool IsLinkLocal() const;

    //------------------------------------------------------------------------
    //!  Returns true if the address is site-local.
    //------------------------------------------------------------------------
    bool IsSiteLocal() const;
    
    //------------------------------------------------------------------------
    //!  Returns a pointer to the memory containing the address.
    //------------------------------------------------------------------------
    inline const uint8_t *RawPointer() const
    {
      return (const uint8_t *)&_addr;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline const in6_addr & In6Addr() const
    {
      return _addr;
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  one of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override;
    
    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes read
    //!  (16 on success).
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;
    
    //------------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes written
    //!  (16 on success).
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads from a FILE pointer.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE * f) override;
    
    //------------------------------------------------------------------------
    //!  Writes to a FILE pointer.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE * f) const override;
    
    //------------------------------------------------------------------------
    //!  Reads from a gzFile.  Returns the number of bytes read
    //!  (16 on success).
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes to a gzFile.  Returns the number of bytes written
    //!  (16 on success).
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;

    //------------------------------------------------------------------------
    //!  Reads from a BZFILE pointer.  Returns the number of bytes read
    //!  (16 on success).
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes to a BZFILE pointer.  Returns the number of bytes written
    //!  (16 on success).
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;

    //------------------------------------------------------------------------
    //!  Function template intended for boost::asio stream sockets.
    //------------------------------------------------------------------------
    template <typename ST>
    requires IsSupportedASIOSocket<ST>
    bool ASIO_Read(ST & s, boost::system::error_code & ec)
    {
      using boost::asio::read, boost::asio::buffer;
      return ((read(s, buffer(_addr.s6_addr, 16), ec) == 16) && (! ec));
    }

    //------------------------------------------------------------------------
    //!  Function template intended for boost::asio stream sockets.
    //------------------------------------------------------------------------
    template <typename ST>
    requires IsSupportedASIOSocket<ST>
    bool ASIO_Write(ST & s, boost::system::error_code & ec) const
    {
      using boost::asio::write, boost::asio::buffer;
      return ((write(s, buffer(_addr.s6_addr, 16), ec) == 16) && (! ec));
    }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6Address from @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec) override
    { return ASIO_Read(s, ec); }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6Address to @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const override
    { return ASIO_Write(s, ec); }

    //------------------------------------------------------------------------
    //!  Reads the Ipv6Address from @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec) override
    { return ASIO_Read(s, ec); }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6Address to @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const override
    { return ASIO_Write(s, ec); }
      
    //------------------------------------------------------------------------
    //!  Reads the Ipv6Address from @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec) override
    { return ASIO_Read(s, ec); }

    //------------------------------------------------------------------------
    //!  Writes the Ipv6Address to @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) const override
    { return ASIO_Write(s, ec); }
      
    //------------------------------------------------------------------------
    //!  Prints an Ipv6Address to an ostream in human-readable form.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, 
                                       const Ipv6Address & addr);

    //------------------------------------------------------------------------
    //!  Returns a 64-bit hash value for the Ipv6Address.
    //------------------------------------------------------------------------
    inline uint64_t Hash() const
    {
      return XXH64(_addr.s6_addr, 16, 0);
    }
    
  private:
    alignas(8) struct in6_addr  _addr;
  };

}  // namespace Dwm

#endif  // _DWMIPV6ADDRESS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
