//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2016, 2020
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
//!  \file DwmIpv4UdpHeader.hh
//!  \brief Dwm::Ipv4UdpHeader class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4UDPHEADER_HH_
#define _DWMIPV4UDPHEADER_HH_

extern "C" {
  #include <sys/types.h>
  #ifdef __linux__
    #define __FAVOR_BSD 1
  #endif
  #include <netinet/udp.h>
}

#include <cstdint>
#include <string>

#include "DwmIpv4PacketHeader.hh"

namespace Dwm {

  class Ipv4UdpPayload;
  
  //--------------------------------------------------------------------------
  //!  Encapsulates an IPv4 UDP header.
  //--------------------------------------------------------------------------
  class Ipv4UdpHeader
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  The new object will own the underlying memory.
    //------------------------------------------------------------------------
    Ipv4UdpHeader();

    //------------------------------------------------------------------------
    //!  Construct from the given IPv4 packet header @c ipHeader.
    //------------------------------------------------------------------------
    Ipv4UdpHeader(const Ipv4PacketHeader & ipHeader);

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    Ipv4UdpHeader(const Ipv4UdpHeader & udpHeader);
    
    //------------------------------------------------------------------------
    //!  Construct from a pointer to an existing udphdr.  We will not copy
    //!  the contents of @c udpHdr, it will continue to be owned by the
    //!  caller.  This is useful when the caller wishes to use this class
    //!  for manipulating an existing udphdr.
    //------------------------------------------------------------------------
    Ipv4UdpHeader(struct udphdr *udpHdr);

    //------------------------------------------------------------------------
    //!  Constructor.  Copies the contents of @c udpHdr.  The new object will
    //   own the underlying memory.
    //------------------------------------------------------------------------
    Ipv4UdpHeader(const struct udphdr & udpHdr);

    //------------------------------------------------------------------------
    //!  operator =
    //------------------------------------------------------------------------
    Ipv4UdpHeader & operator = (const Ipv4UdpHeader & udpHeader);

    //------------------------------------------------------------------------
    //!  Destructor.  If the object owns the underlying memory, the 
    //!  underlying memory will be freed.
    //------------------------------------------------------------------------
    ~Ipv4UdpHeader();

    //------------------------------------------------------------------------
    //!  Returns the UDP source port in host byte order.
    //------------------------------------------------------------------------
    uint16_t SourcePort() const;

    //------------------------------------------------------------------------
    //!  Sets the UDP source port to @c port, which must be in host byte
    //!  order.  Returns the UDP source port in host byte order.
    //------------------------------------------------------------------------
    uint16_t SourcePort(uint16_t port);

    //------------------------------------------------------------------------
    //!  Returns the UDP destination port in host byte order.
    //------------------------------------------------------------------------
    uint16_t DestinationPort() const;

    //------------------------------------------------------------------------
    //!  Sets the UDP destination port to @c port, which must be in host byte
    //!  order.  Returns the UDP destination port in host byte order.
    //------------------------------------------------------------------------
    uint16_t DestinationPort(uint16_t port);

    //------------------------------------------------------------------------
    //!  Returns the UDP length in host byte order.  The UDP length field
    //!  includes the bytes for both the UDP header and the UDP payload.
    //------------------------------------------------------------------------
    uint16_t UdpLength() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the UDP length in host byte order.  The UDP length
    //!  field includes the bytes for both the UDP header and the UDP payload.
    //------------------------------------------------------------------------
    uint16_t UdpLength(uint16_t udpLength);

    //------------------------------------------------------------------------
    //!  Returns the UDP checksum field.
    //------------------------------------------------------------------------
    uint16_t UdpChecksum() const;
    
    //------------------------------------------------------------------------
    //!  Calculates and sets the UDP checksum field, which requires the
    //!  UDP payload @c data and some fields from the IP header.
    //------------------------------------------------------------------------
    void SetUdpChecksum(const Ipv4PacketHeader & ipHeader,
                        const std::string & data);

    //------------------------------------------------------------------------
    //!  Calculates and sets the UDP checksum field, which requires the
    //!  UDP payload @c data and some fields from the IP header.
    //------------------------------------------------------------------------
    void SetUdpChecksum(const Ipv4PacketHeader & ipHeader,
                        const Ipv4UdpPayload & data);
    
    uint8_t *End() const;
    
    //------------------------------------------------------------------------
    //!  Reads the UDP header from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Writes the UDP header to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Reads the UDP header from a file descriptor.  Returns the number of
    //!  bytes read on success (should be equal to StreamedLength()), -1 on 
    //!  failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);

    //------------------------------------------------------------------------
    //!  Writes the UDP header to a file descriptor.  Returns the number of
    //!  bytes written on success (should be equal to StreamedLength()),
    //!  -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads the UDP header from a FILE.  Returns 1 on success, 0 on 
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE * f);

    //------------------------------------------------------------------------
    //!  Writes the UDP header to a FILE.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called one
    //!  of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;

    //------------------------------------------------------------------------
    //!  Reads the UDP header from a gzFile.  Returns the number of bytes
    //!  read on success (should be equal to StreamedLength()), -1 on
    //!  failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);

    //------------------------------------------------------------------------
    //!  Writes the UDP header to a gzFile.  Returns the number of bytes
    //!  written on success (should be equal to StreamedLength()), -1 on
    //!  failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads the UDP header from a BZFILE.  Returns the number of bytes
    //!  read on success (should be equal to StreamedLength()), -1 on
    //!  failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);

    //------------------------------------------------------------------------
    //!  Writes the UDP header to a BZFILE.  Returns the number of bytes
    //!  written on success (should be equal to StreamedLength()), -1 on
    //!  failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const Ipv4UdpHeader & udpHeader) const;

    //------------------------------------------------------------------------
    //!  Print the UDP header to an ostream in pseudo-XML, with each line
    //!  prefixed by @c pre.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & PrintXML(std::ostream & os, 
                            const std::string & pre = "") const;
    
  private:
    typedef struct {
      uint16_t  sourcePort;
      uint16_t  destinationPort;
      uint16_t  udpLength;
      uint16_t  checksum;
    } Data;

    Data  *_data;
    bool   _ownData;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4UDPHEADER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
