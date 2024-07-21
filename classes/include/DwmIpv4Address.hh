//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2006, 2016, 2023, 2024
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY 
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIpv4Address.hh
//!  \brief Dwm::Ipv4Address class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ADDRESS_HH_
#define _DWMIPV4ADDRESS_HH_

extern "C" {
  #include <inttypes.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <bzlib.h>
  #include <zlib.h>
}

#include <string>
#include <boost/asio.hpp>

namespace Dwm {

  using ipv4addr_t = uint32_t;
  
  //--------------------------------------------------------------------------
  //!  This class encapsulates an IPv4 address.
  //--------------------------------------------------------------------------
  class Ipv4Address
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Ipv4Address();

    //------------------------------------------------------------------------
    //!  Construct from an ipv4addr_t (32-bit value in network byte order).
    //------------------------------------------------------------------------
    inline Ipv4Address(ipv4addr_t addr)
        : _addr(addr)
    {}

    //------------------------------------------------------------------------
    //!  Construct from a dotted-decimal string.
    //------------------------------------------------------------------------
    Ipv4Address(const std::string & dottedAddr);

    //------------------------------------------------------------------------
    //!  Returns an ipv4addr_t representation (32-bit value in network
    //!  byte order).
    //------------------------------------------------------------------------
    inline ipv4addr_t Raw() const
    { return(_addr); }
    
    //------------------------------------------------------------------------
    //!  Returns a dotted-decimal string representation.
    //------------------------------------------------------------------------
    explicit operator std::string () const;
    
    //------------------------------------------------------------------------
    //!  less-than operator.
    //------------------------------------------------------------------------
    inline bool operator < (const Ipv4Address & addr) const
    { return (ntohl(_addr) < ntohl(addr._addr)); }

    //------------------------------------------------------------------------
    //!  Greater-than operator.
    //------------------------------------------------------------------------
    inline bool operator > (const Ipv4Address & addr) const
    { return (ntohl(_addr) > ntohl(addr._addr)); }

    //------------------------------------------------------------------------
    //!  Greater than or equal-to operator.
    //------------------------------------------------------------------------
    inline bool operator >= (const Ipv4Address & addr) const
    {
      return ((ntohl(_addr) > ntohl(addr._addr))
              || (_addr == addr._addr));
    }
    
    //------------------------------------------------------------------------
    //!  Less-than or equal-to operator.
    //------------------------------------------------------------------------
    inline bool operator <= (const Ipv4Address & addr) const
    {
      return ((ntohl(_addr) < ntohl(addr._addr))
              || (_addr == addr._addr));
    }

    //------------------------------------------------------------------------
    //!  Equal-to operator.
    //------------------------------------------------------------------------
    inline bool operator == (const Ipv4Address & addr) const
    { return (_addr == addr._addr); }

    //------------------------------------------------------------------------
    //!  Not-equal-to operator.
    //------------------------------------------------------------------------
    inline bool operator != (const Ipv4Address & addr) const
    { return (_addr != addr._addr); }
    
    //------------------------------------------------------------------------
    //!  Mask operator.
    //------------------------------------------------------------------------
    inline Ipv4Address & operator &= (const Ipv4Address & mask)
    {
      _addr &= mask._addr;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Post-increment operator
    //------------------------------------------------------------------------
    inline Ipv4Address operator ++ (int)
    {
      Ipv4Address rc(*this);
      uint32_t  a = ntohl(_addr);
      if (a != 0xFFFFFFFF) {
        _addr = htonl(a + 1);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Pre-increment operator
    //------------------------------------------------------------------------
    inline Ipv4Address & operator ++ ()
    {
      uint32_t  a = ntohl(_addr);
      if (a != 0xFFFFFFFF) {
        _addr = htonl(a + 1);
      }
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Post-decrement operator
    //------------------------------------------------------------------------
    inline Ipv4Address operator -- (int)
    {
      Ipv4Address rc(*this);
      uint32_t  a = ntohl(_addr);
      if (a != 0) {
        _addr = htonl(a - 1);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Pre-decrement operator
    //------------------------------------------------------------------------
    inline Ipv4Address & operator -- ()
    {
      uint32_t  a = ntohl(_addr);
      if (a != 0) {
        _addr = htonl(a - 1);
      }
      return *this;
    }

    //------------------------------------------------------------------------
    //!  -= operator.  Protects against roll-under (will not decrement past
    //!  0.0.0.0).  Note that the right hand side must be in host byte order.
    //------------------------------------------------------------------------
    inline Ipv4Address & operator -= (uint32_t i)
    {
      uint32_t  a = ntohl(_addr);
      if (a >= i) {
        _addr = htonl(a - i);
      }
      else {
        _addr = htonl(0);
      }
      return *this;
    }

    //------------------------------------------------------------------------
    //!  += operator.  Protects against roll-over (will not increment past
    //!  255.255.255.255).  Note that the right hand side must be in host
    //!  byte order.
    //------------------------------------------------------------------------
    inline Ipv4Address & operator += (uint32_t i)
    {
      uint32_t  a = ntohl(_addr);
      if ((uint32_t)(a + i) >= a) {
        _addr = htonl(a + i);
      }
      else {
        _addr = htonl(0xFFFFFFFF);
      }
      return *this;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline bool BitIsSet(int8_t bit) const
    { return (((ntohl(_addr) >> bit) & 1) != 0); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int GetBit(int bit) const
    { return (ntohl(_addr) >> (31 - bit)) & 1; }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static inline uint8_t MaxKeyBits()
    { return 32; }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  one of the non-compressing Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;
    
    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes read
    //!  (4 on success).
    //------------------------------------------------------------------------
    ssize_t Read(int fd);
    
    //------------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes written
    //!  (4 on success).
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

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
    //!  (4 on success).
    //------------------------------------------------------------------------
    int Read(gzFile gzf);
    
    //------------------------------------------------------------------------
    //!  Writes to a gzFile.  Returns the number of bytes written
    //!  (4 on success).
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;
    
    //------------------------------------------------------------------------
    //!  Reads from a BZFILE pointer.  Returns the number of bytes read
    //!  (4 on success).
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);
    
    //------------------------------------------------------------------------
    //!  Writes to a BZFILE pointer.  Returns the number of bytes written
    //!  (4 on success).
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;

    //------------------------------------------------------------------------
    //!  Reads from a boost::asio::ip::tcp::socket.  Returns true on success.
    //!  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec)
    { return ASIO_Read(s, ec); }

    //------------------------------------------------------------------------
    //!  Writes to a boost::asio::ip::tcp::socket.  Returns true on success.
    //!  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const;
    
    //------------------------------------------------------------------------
    //!  Reads from a boost::asio::local::stream_protocol::socket.  Returns
    //!  true on success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes to a boost::asio::local::stream_protocol::socket.  Returns
    //!  true on success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const;

    //------------------------------------------------------------------------
    //!  Reads from a boost::asio::generic::stream_protocol::socket.  Returns
    //!  true on success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec)
    { return ASIO_Read(s, ec); }

    //------------------------------------------------------------------------
    //!  Writes to a boost::asio::generic::stream_protocol::socket.  Returns
    //!  true on success.  Returns false and sets @c ec on failure.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,       
               boost::system::error_code & ec) const;

    //------------------------------------------------------------------------
    //!  Prints an Ipv4Address to an ostream in human-readable form.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, 
                                       const Ipv4Address & addr);

    //------------------------------------------------------------------------
    //!  Apply a @c mask (bitwise AND) to the given @c addr and return
    //!  the result.
    //------------------------------------------------------------------------
    friend Ipv4Address operator & (const Ipv4Address & addr,
                                   const Ipv4Address & mask);

    //------------------------------------------------------------------------
    //!  Returns the partial sum of the IP address, as would be used when
    //!  calculating the IP header checksum or UDP checksum (as part of the
    //!  'pseudo-header' used for UDP checksums).
    //------------------------------------------------------------------------
    uint32_t IpSum() const;
    
  protected:
    uint32_t  _addr;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename Protocol, typename Executor>
    bool ASIO_Read(boost::asio::basic_stream_socket<Protocol,Executor> & s,
              boost::system::error_code & ec)
    {
      using boost::asio::read, boost::asio::buffer;
      return ((read(s, buffer(&_addr, sizeof(_addr)), ec) == sizeof(_addr))
              && (! ec));
    }
  };

}  // namespace Dwm

#endif  // _DWMIPV4ADDRESS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
