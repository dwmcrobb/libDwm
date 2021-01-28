//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2016
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
//!  \file DwmIpv4PacketHeader.hh
//!  \brief Dwm::Ipv4PacketHeader class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4PACKETHEADER_HH_
#define _DWMIPV4PACKETHEADER_HH_

extern "C" {
#include <sys/types.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
}

#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmBZ2IOCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmIpv4Address.hh"

namespace Dwm {
  
  //--------------------------------------------------------------------------
  //!  Encapsulates an IPv4 packet header.
  //--------------------------------------------------------------------------
  class Ipv4PacketHeader
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  The new object will own the underlying memory.
    //------------------------------------------------------------------------
    Ipv4PacketHeader();

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    Ipv4PacketHeader(const Ipv4PacketHeader & ipHeader);
    
    //------------------------------------------------------------------------
    //!  Construct from a struct ip.  The new object will own the underlying 
    //!  memory.
    //------------------------------------------------------------------------
    Ipv4PacketHeader(const struct ip & ip);

    //------------------------------------------------------------------------
    //!  Construct from a pointer to a struct ip.  The caller will still own
    //!  the memory at @c ip, and @c ip must remain unfreed while the new
    //!  Ipv4PacketHeader object is in use.
    //------------------------------------------------------------------------
    Ipv4PacketHeader(struct ip *ip);

    //------------------------------------------------------------------------
    //!  Destructor.  If the underlying memory is owned by the object,
    //!  it will be freed.
    //------------------------------------------------------------------------
    ~Ipv4PacketHeader();

    //------------------------------------------------------------------------
    //!  operator =
    //------------------------------------------------------------------------
    Ipv4PacketHeader & operator = (const Ipv4PacketHeader & ipHeader);
    
    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const Ipv4PacketHeader & ipHeader) const;
    
    //------------------------------------------------------------------------
    //!  Copy the contents of @c ip into the object.
    //------------------------------------------------------------------------
    void Set(const struct ip & ip);

    //------------------------------------------------------------------------
    //!  Returns the IP header length.
    //------------------------------------------------------------------------
    uint8_t HeaderLength() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the IP header length.
    //------------------------------------------------------------------------
    uint8_t HeaderLength(uint8_t headerLength);

    //------------------------------------------------------------------------
    //!  Returns the type-of-service.
    //------------------------------------------------------------------------
    uint8_t TypeOfService() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the type-of-service.
    //------------------------------------------------------------------------
    uint8_t TypeOfService(uint8_t typeOfService);

    //------------------------------------------------------------------------
    //!  Returns the total length field in host byte order.
    //------------------------------------------------------------------------
    uint16_t TotalLength() const;

    //------------------------------------------------------------------------
    //!  Sets and returns total length field in host byte order.
    //------------------------------------------------------------------------
    uint16_t TotalLength(uint16_t totalLength);

    //------------------------------------------------------------------------
    //!  Returns the IP ID field in host byte order.
    //------------------------------------------------------------------------
    uint16_t Id() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the IP ID field in host byte order.
    //------------------------------------------------------------------------
    uint16_t Id(uint16_t id);

    //------------------------------------------------------------------------
    //!  Returns the fragment offset field in host byte order.
    //------------------------------------------------------------------------
    uint16_t FragmentOffset() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the fragment offset field in host byte order.
    //------------------------------------------------------------------------
    uint16_t FragmentOffset(uint16_t fragmentOffset);

    //------------------------------------------------------------------------
    //!  Returns the time-to-live field.
    //------------------------------------------------------------------------
    uint8_t TimeToLive() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the time-to-live field.
    //------------------------------------------------------------------------
    uint8_t TimeToLive(uint8_t timeToLive);

    //------------------------------------------------------------------------
    //!  Returns the protocol field.
    //------------------------------------------------------------------------
    uint8_t Protocol() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the protocol field.
    //------------------------------------------------------------------------
    uint8_t Protocol(uint8_t protocol);

    //------------------------------------------------------------------------
    //!  Returns the checksum field in host byte order.
    //------------------------------------------------------------------------
    uint16_t Checksum() const;

    //------------------------------------------------------------------------
    //!  Returns the source IP address.
    //------------------------------------------------------------------------
    Ipv4Address SourceAddress() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the source IP address.
    //------------------------------------------------------------------------
    Ipv4Address SourceAddress(const Ipv4Address & sourceAddress);

    //------------------------------------------------------------------------
    //!  Returns the destination IP address.
    //------------------------------------------------------------------------
    Ipv4Address DestinationAddress() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the destination IP address.
    //------------------------------------------------------------------------
    Ipv4Address DestinationAddress(const Ipv4Address & destinationAddress);

    //------------------------------------------------------------------------
    //!  Computes and stores the IP header checksum.
    //------------------------------------------------------------------------
    void SetChecksum();
    
    //------------------------------------------------------------------------
    //!  Reads the header from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes the header to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the header from a file descriptor.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Writes the header to a file descriptor.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads the header from a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes the header to a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we called one
    //!  of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override;

    //------------------------------------------------------------------------
    //!  Reads the header from a gzFile.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;

    //------------------------------------------------------------------------
    //!  Writes the header to a gzFile.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;

    //------------------------------------------------------------------------
    //!  Reads the header from a BZFILE.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;

    //------------------------------------------------------------------------
    //!  Writes the header to a BZFILE.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;

    //------------------------------------------------------------------------
    //!  Prints the header is pseudo-XML.  Each line will be prefixed by
    //!  @c pre.
    //------------------------------------------------------------------------
    std::ostream & PrintXML(std::ostream & os, 
                            const std::string & pre = "") const;

    uint8_t *End() const;
    
  private:
    struct ip  *_ip;
    bool        _ownData;
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4PACKETHEADER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
