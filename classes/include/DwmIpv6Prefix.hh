//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2006, 2016, 2023, 2024
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
//!  \file DwmIpv6Prefix.hh
//!  \brief Dwm::Ipv6Prefix class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV6PREFIX_HH_
#define _DWMIPV6PREFIX_HH_

#include "DwmIpv6Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates an IPv6 network prefix.
  //--------------------------------------------------------------------------
  class Ipv6Prefix
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Ipv6Prefix();

    //------------------------------------------------------------------------
    //!  Construct from a string.
    //------------------------------------------------------------------------
    Ipv6Prefix(const std::string & prefix);

    //------------------------------------------------------------------------
    //!  Construct from an IPv6 network address and netmask length.  The
    //!  network address may be a host address; this constructor will
    //!  apply the netmask.
    //------------------------------------------------------------------------
    Ipv6Prefix(const Ipv6Address & network, uint8_t maskLength);

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    Ipv6Prefix(const Ipv6Prefix & prefix);

    //------------------------------------------------------------------------
    //!  Returns the network address.
    //------------------------------------------------------------------------
    Ipv6Address Network() const;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline const in6_addr & In6Addr() const
    { return _addr; }
      
    //------------------------------------------------------------------------
    //!  Returns the netmask.
    //------------------------------------------------------------------------
    Ipv6Address Netmask() const;

    //------------------------------------------------------------------------
    //!  Returns the netmask length.
    //------------------------------------------------------------------------
    inline uint8_t MaskLength() const
    { return _length; }

    //------------------------------------------------------------------------
    //!  Sets and returns the netmask length.
    //------------------------------------------------------------------------
    uint8_t MaskLength(uint8_t maskLen);

    //------------------------------------------------------------------------
    //!  Set the prefix using a network address and netmask length.  Returns
    //!  true on success, false on failure.
    //------------------------------------------------------------------------
    bool Set(const Ipv6Address & network, uint8_t maskLength);

    //------------------------------------------------------------------------
    //!  Less-than operator.
    //------------------------------------------------------------------------
    bool operator < (const Ipv6Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Greater-than operator.
    //------------------------------------------------------------------------
    bool operator > (const Ipv6Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Equal-to operator.
    //------------------------------------------------------------------------
    bool operator == (const Ipv6Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Not-equal-to operator.
    //------------------------------------------------------------------------
    bool operator != (const Ipv6Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Returns true if \c addr falls within the prefix.
    //------------------------------------------------------------------------
    bool Contains(const Ipv6Address & addr) const;
    
    //------------------------------------------------------------------------
    //!  Reads the prefix from a file descriptor.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);

    //------------------------------------------------------------------------
    //!  Writes the prefix to a file descriptor.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f);
    
    //------------------------------------------------------------------------
    //!  Writes the prefix to a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;
    
    //------------------------------------------------------------------------
    //!  Reads the prefix from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Writes the prefix to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from a gzFile.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);

    //------------------------------------------------------------------------
    //!  Writes the prefix to a gzFile.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from a BZFILE pointer.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);

    //------------------------------------------------------------------------
    //!  Writes the prefix to a BZFILE pointer.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Writes the prefix to @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Writes the prefix to @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const;
    
    //------------------------------------------------------------------------
    //!  Reads the prefix from @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Writes the prefix to @c s.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) const;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const
    { return (sizeof(_length) + ((_length + 7) >> 3)); }
    
    //------------------------------------------------------------------------
    //!  Prints a prefix to an ostream in the usual colon-delimited form.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os,
                                       const Ipv6Prefix & prefix);

    //------------------------------------------------------------------------
    //!  Returns a string representation of the prefix in the usual form,
    //!  for example "1234:5678::/32"
    //------------------------------------------------------------------------
    std::string ToString() const;

    //------------------------------------------------------------------------
    //!  Given that the number of IPv6 addresses I typically deal with is
    //!  nowhere near 4 billion, a 32-bit hash is sufficient if its
    //!  distribution is effective.  And it's faster on small 32-bit
    //!  platforms.  Hence I'm currently using XXH32(), and only on the
    //!  non-zero bytes of the address.  Yes, that means I'll have collisions
    //!  for some prefixes but it's a small (near 0) number in my usage.
    //!  I'm looking for average speed here.
    //------------------------------------------------------------------------
    inline uint64_t Hash() const
    {
      return XXH32(_addr.s6_addr, (_length + 7) >> 3, 0);
      // return XXH3_64bits(_addr.s6_addr, 16);
  }
    
  private:
    alignas(8) struct in6_addr  _addr;
    uint8_t                     _length;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline uint8_t NumAddrBytes() const
    {
      return((_length + 7) >> 3);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    bool ASIO_Read(S & s, boost::system::error_code & ec)
    {
      using boost::asio::read, boost::asio::buffer;
      bool  rc = false;
      if ((read(s, buffer(&_length, sizeof(_length)), ec) == sizeof(_length))
          && (! ec)) {
        size_t  len = (_length + 7) >> 3;
        if ((read(s, buffer(_addr.s6_addr, len), ec) == sizeof(_length))
          && (! ec)) {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    bool ASIO_Write(S & s, boost::system::error_code & ec) const
    {
      using boost::asio::write, boost::asio::buffer;
      bool  rc = false;
      if ((write(s, buffer(&_length, sizeof(_length)), ec) == sizeof(_length))
          && (! ec)) {
        size_t  len = (_length + 7) >> 3;
        if ((write(s, buffer(_addr.s6_addr, len), ec) == sizeof(_addr.s6_addr))
            && (! ec)) {
          rc = true;
        }
      }
      return rc;
    }
    
  };

}  // namespace Dwm

#endif  // _DWMIPV6PREFIX_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
