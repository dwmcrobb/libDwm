//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004
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
  #include <inttypes.h>
  #include <sys/socket.h>
}

#include <string>
#include <variant>

#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmBZ2IOCapable.hh"

#include "DwmIpv4Address.hh"
#include "DwmIpv6Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates an IP address.
  //--------------------------------------------------------------------------
  class IpAddress
    : public StreamIOCapable, public FileIOCapable, public DescriptorIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    IpAddress()
        : _addr(Ipv4Address(INADDR_NONE))
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    IpAddress(const IpAddress & addr);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    IpAddress & operator = (const IpAddress & addr);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    IpAddress(const Ipv6Address & addr);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    IpAddress(const Ipv4Address & addr);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    IpAddress(const std::string & addr);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int Family() const      { return k_families[_addr.index()]; }

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
    //!  
    //------------------------------------------------------------------------
    bool operator < (const IpAddress & addr) const;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool operator == (const IpAddress & addr) const;

    //------------------------------------------------------------------------
    //!  
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
    
  private:
    std::variant<Ipv6Address,Ipv4Address>  _addr;

    static constexpr int  k_families[2] = { AF_INET6, AF_INET };
  };
  
}  // namespace Dwm

#endif  // _DWMIPADDRESS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
