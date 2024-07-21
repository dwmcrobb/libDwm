//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2006, 2023, 2024
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
//!  \file DwmIpv4Prefix.hh
//!  \brief Dwm::Ipv4Prefix class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4PREFIX_HH_
#define _DWMIPV4PREFIX_HH_

#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
#include <boost/asio.hpp>

#include "DwmIpv4Address.hh"
#include "DwmASIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates an IPv4 address and netmask.
  //--------------------------------------------------------------------------
  class Ipv4Prefix
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Initializes the prefix to 255.255.255.255/32
    //------------------------------------------------------------------------
    Ipv4Prefix();
    
    //------------------------------------------------------------------------
    //!  Constructor accepting an IP address and a netmask length.
    //------------------------------------------------------------------------
    Ipv4Prefix(const Ipv4Address & network, uint8_t maskLength);

    //------------------------------------------------------------------------
    //!  Construct from an IP address and netmask.
    //------------------------------------------------------------------------
    Ipv4Prefix(const Ipv4Address & network, const Ipv4Address & netmask);
    
    //------------------------------------------------------------------------
    //!  Construct from a string of the form a.b.c.d/n.  Note we accept
    //!  short addresses, for example 128.35/16 or 10/8.  We also accept
    //!  host addresses with netmasks, and will properly apply the netmask.
    //   For example, 128.35.104.1/16 will result in 128.35/16.
    //------------------------------------------------------------------------
    Ipv4Prefix(const std::string & prefix);

    //------------------------------------------------------------------------
    //!  Construct from an Ipv4Address.  Sets the netmask to 255.255.255.255
    //------------------------------------------------------------------------
    Ipv4Prefix(const Ipv4Address & addr);
    
    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    inline Ipv4Prefix(const Ipv4Prefix & prefix)
    {
      memcpy(_data,prefix._data,5);
    }

    //------------------------------------------------------------------------
    //!  operator =
    //------------------------------------------------------------------------
    inline Ipv4Prefix & operator = (const Ipv4Prefix & prefix)
    {
      if (&prefix != this) {
        memcpy(_data,prefix._data,5);
      }
      return(*this);
    }
    
    //------------------------------------------------------------------------
    //!  Destructor.
    //------------------------------------------------------------------------
    virtual ~Ipv4Prefix();
    
    //------------------------------------------------------------------------
    //!  Returns the network portion of the prefix.
    //------------------------------------------------------------------------
    inline Ipv4Address Network() const
    {
      return(*(ipv4addr_t *)this->_data);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline ipv4addr_t NetworkRaw() const
    {
      return(((ipv4addr_t *)this->_data)[0]);
    }
    
    //------------------------------------------------------------------------
    //!  Returns the netmask portion of the prefix.
    //------------------------------------------------------------------------
    Ipv4Address Netmask() const;
    
    //------------------------------------------------------------------------
    //!  Returns the length of the netmask (number of significant bits).
    //------------------------------------------------------------------------
    inline uint8_t MaskLength() const
    { return this->_data[4]; }
    
    //------------------------------------------------------------------------
    //!  Changes the length of the netmask (number of significant bits).
    //------------------------------------------------------------------------
    inline uint8_t MaskLength(uint8_t maskLength)
    {
      if (maskLength < _data[4]) {
        if (maskLength == 0) {
          *(uint32_t *)_data = 0;
        }
        else {
          //  need to zero some bits
          uint32_t  mask = htonl(0xffffffff << (32 - maskLength));
          *(uint32_t *)_data &= mask;
        }
        _data[4] = maskLength;
      }
      else if (maskLength < 33) {
        _data[4] = maskLength;
      }
      return(_data[4]);
    }

    //------------------------------------------------------------------------
    //!  Returns the first IPv4 address in the prefix's range.
    //------------------------------------------------------------------------
    Ipv4Address FirstAddress() const;

    //------------------------------------------------------------------------
    //!  Returns the last IPv4 address in the prefix's range.
    //------------------------------------------------------------------------
    Ipv4Address LastAddress() const;
      
    //------------------------------------------------------------------------
    //!  Initializes the prefix using an IP address and a netmask length.
    //------------------------------------------------------------------------
    bool Set(const Ipv4Address & network, uint8_t maskLength);

    //------------------------------------------------------------------------
    //!  Initializes the prefix using an IP address and a netmask.
    //------------------------------------------------------------------------
    void Set(const Ipv4Address & network, const Ipv4Address & netmask);
    
    //------------------------------------------------------------------------
    //!  Returns true if \c this is less than \c prefix.  The comparison
    //!  uses the typical ordering: if the IP address of \c this is less
    //!  than the IP address of \c prefix, returns true.  If the IP address
    //!  of \c this is equal to the IP address of \c prefix, returns true
    //!  if the netmask length of \c this is lower than the netmask length
    //!  of \c prefix.  In all other cases, returns false.
    //------------------------------------------------------------------------
    bool operator < (const Ipv4Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Returns true if \c this is greater than \c prefix.  The comparison
    //!  uses the typical ordering: if the IP address of \c this is greater
    //!  than the IP address of \c prefix, returns true.  If the IP address
    //!  of \c this is equal to the IP address of \c prefix, returns true
    //!  if the netmask length of \c this is higher than the netmask length
    //!  of \c prefix.  In all other cases, returns false.
    //------------------------------------------------------------------------
    bool operator > (const Ipv4Prefix & prefix) const;;
    
    //------------------------------------------------------------------------
    //!  Equal-to operator.
    //------------------------------------------------------------------------
    bool operator == (const Ipv4Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Not-equal-to operator.
    //------------------------------------------------------------------------
    bool operator != (const Ipv4Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Pre-increment operator.
    //------------------------------------------------------------------------
    Ipv4Prefix & operator ++ ();

    //------------------------------------------------------------------------
    //!  Post-increment operator.
    //------------------------------------------------------------------------
    Ipv4Prefix operator ++ (int);

    //------------------------------------------------------------------------
    //!  Pre-decrement operator.
    //------------------------------------------------------------------------
    Ipv4Prefix & operator -- ();
    
    //------------------------------------------------------------------------
    //!  Post-decrement operator.
    //------------------------------------------------------------------------
    Ipv4Prefix operator -- (int);

    //------------------------------------------------------------------------
    //!  Returs true if \c address falls within the prefix.
    //------------------------------------------------------------------------
    bool Contains(const Ipv4Address & address) const;
    
    //------------------------------------------------------------------------
    //!  Returns true if \c prefix is a subnet of this.
    //------------------------------------------------------------------------
    bool Contains(const Ipv4Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  Returns true if the given @c prefix is adjacent.
    //------------------------------------------------------------------------
    bool Adjacent(const Ipv4Prefix & prefix) const;

    //------------------------------------------------------------------------
    //!  If the given @c prefix can be combined with this prefix to form a
    //!  single prefix, returns [true,combinedPrefix].  Else returns
    //!  [false,255.255.255.255/32].  Prefixes may be combined if one contains
    //!  the other or they are adjacent and have the same netmask length.
    //------------------------------------------------------------------------
    std::pair<bool,Ipv4Prefix> Combine(const Ipv4Prefix & prefix) const;
    
    //------------------------------------------------------------------------
    //!  Reads a prefix from a file descriptor.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);

    //------------------------------------------------------------------------
    //!  Writes a prefix to a file descriptor.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads a prefix from a FILE pointer.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f);
    
    //------------------------------------------------------------------------
    //!  Writes a prefix to a FILE pointer.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;
    
    //------------------------------------------------------------------------
    //!  Reads a prefix from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);
    
    //------------------------------------------------------------------------
    //!  Writes a prefix to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;
    
    //------------------------------------------------------------------------
    //!  Reads a prefix from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);

    //------------------------------------------------------------------------
    //!  Writes a prefix to a gzFile.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads a prefix from a BZFILE pointer.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);

    //------------------------------------------------------------------------
    //!  Writes a prefix to a BZFILE pointer.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from @c s.  Returns true on success, false on
    //!  failure.  On failure @c ec will be set to the error code.
    //------------------------------------------------------------------------
    bool Read(boost::asio::ip::tcp::socket & s,
              boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Writes the prefix to @c s.  Returns true on success, false on
    //!  failure.  On failure @c ec will be set to the error code.
    //------------------------------------------------------------------------
    bool Write(boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) const;
    
    //------------------------------------------------------------------------
    //!  Reads the prefix from @c s.  Returns true on success, false on
    //!  failure.  On failure @c ec will be set to the error code.
    //------------------------------------------------------------------------
    bool Read(boost::asio::local::stream_protocol::socket & s,
              boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes the prefix to @c s.  Returns true on success, false on
    //!  failure.  On failure @c ec will be set to the error code.
    //------------------------------------------------------------------------
    bool Write(boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) const;

    //------------------------------------------------------------------------
    //!  Reads the prefix from @c s.  Returns true on success, false on
    //!  failure.  On failure @c ec will be set to the error code.
    //------------------------------------------------------------------------
    bool Read(boost::asio::generic::stream_protocol::socket & s,
              boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes the prefix to @c s.  Returns true on success, false on
    //!  failure.  On failure @c ec will be set to the error code.
    //------------------------------------------------------------------------
    bool Write(boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) const;

    //------------------------------------------------------------------------
    //!  Prints an Ipv4Prefix to an ostream in 'a.b.c.d/n' form.  Returns
    //!  the ostream.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, 
                                       const Ipv4Prefix & prefix);

    //------------------------------------------------------------------------
    //!  Returns a string representation of the prefix in 'a.b.c.d/n' form.
    //------------------------------------------------------------------------
    std::string ToString() const;

    //------------------------------------------------------------------------
    //!  Returns a string representation of the prefix in 'a.b.c.d/n' form,
    //!  but with unnecessary trailing zeros removed from the IP address.
    //------------------------------------------------------------------------
    std::string ToShortString() const;
      
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if
    //!  a Write() member was called.
    //------------------------------------------------------------------------
    inline uint64_t StreamedLength() const
    { return(5); }

    //------------------------------------------------------------------------
    //!  Returns a pointer to the 5-byte data.  This should not be used
    //!  by typical applications; it is only needed by things like hash
    //!  algorithms (such as that used by the Ipv4Routes template class).
    //------------------------------------------------------------------------
    inline const uint8_t *Data() const
    { return(_data); }

  private:
    alignas(4) uint8_t  _data[5];

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    bool ASIO_Read(S & s, boost::system::error_code & ec)
    {
      using boost::asio::read, boost::asio::buffer;
      return ((read(s, buffer(_data, sizeof(_data)), ec) == sizeof(_data))
              && (! ec));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    bool ASIO_Write(S & s, boost::system::error_code & ec) const
    {
      using boost::asio::write, boost::asio::buffer;
      return ((write(s, buffer(_data, sizeof(_data)), ec) == sizeof(_data))
              && (! ec));
    }
  };

  //--------------------------------------------------------------------------
  //!  Returns the minimal set of IPv4 prefixes that cover the entire range
  //!  between @c addr1 and @c addr2.  This is mainly used for IDS/IPS types
  //!  of applications which want to map from registry ranges to pf filters.
  //--------------------------------------------------------------------------
  std::vector<Ipv4Prefix> Ipv4RangePrefixes(const Ipv4Address & addr1,
                                            const Ipv4Address & addr2);
  
}  // namespace Dwm

#endif  // _DWMIPV4PREFIX_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
