//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmIpv4TcpHeader.hh 11087 $
// @(#) $Id: DwmIpv4TcpHeader.hh 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016, 2020
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
//!  \file DwmIpv4TcpHeader.hh
//!  \brief Dwm::Ipv4TcpHeader class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4TCPHEADER_HH_
#define _DWMIPV4TCPHEADER_HH_

extern "C" {
  #include <sys/types.h>
  #ifdef __linux__
    #define __FAVOR_BSD 1
  #endif
  #include <netinet/tcp.h>
  #include <inttypes.h>
}

#include <cassert>

#include "DwmIpv4PacketHeader.hh"
#include "DwmBZ2IOCapable.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a TCP header.
  //--------------------------------------------------------------------------
  class Ipv4TcpHeader
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Ipv4TcpHeader();

    //------------------------------------------------------------------------
    //!  Construct from an Ipv4PacketHeader which points at the IP header
    //!  in a TCP packet.  DO NOT FREE THE UNDERLYING MEMORY WHILE THE
    //!  Ipv4TcpHeader IS STILL IN USE!
    //------------------------------------------------------------------------
    Ipv4TcpHeader(const Ipv4PacketHeader & ipHeader);

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    Ipv4TcpHeader(const Ipv4TcpHeader & tcpHeader);

    //------------------------------------------------------------------------
    //!  Construct from a pointer to a struct tcphdr.  We don't copy,
    //!  so don't free the data into which @c tcpHdr points until after
    //!  the Ipv4TcpHeader is no longer in use.
    //------------------------------------------------------------------------
    Ipv4TcpHeader(struct tcphdr *tcpHdr);

    //------------------------------------------------------------------------
    //!  Construct by copying from a struct tcphdr.
    //------------------------------------------------------------------------
    Ipv4TcpHeader(const struct tcphdr & tcpHdr);

    //------------------------------------------------------------------------
    //!  operator =
    //------------------------------------------------------------------------
    Ipv4TcpHeader & operator = (const Ipv4TcpHeader & tcpHeader);

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~Ipv4TcpHeader();
    
    //------------------------------------------------------------------------
    //!  Returns the source port.
    //------------------------------------------------------------------------
    uint16_t SourcePort() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the source port.
    //------------------------------------------------------------------------
    uint16_t SourcePort(uint16_t port);

    //------------------------------------------------------------------------
    //!  Returns the destination port.
    //------------------------------------------------------------------------
    uint16_t DestinationPort() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the destination port.
    //------------------------------------------------------------------------
    uint16_t DestinationPort(uint16_t port);

    //------------------------------------------------------------------------
    //!  Returns the sequence number.
    //------------------------------------------------------------------------
    uint32_t SequenceNumber() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the sequence number.
    //------------------------------------------------------------------------
    uint32_t SequenceNumber(uint32_t seq);

    //------------------------------------------------------------------------
    //!  Returns the ACK nuber.
    //------------------------------------------------------------------------
    uint32_t AckNumber() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the ACK number.
    //------------------------------------------------------------------------
    uint32_t AckNumber(uint32_t ack);

    //------------------------------------------------------------------------
    //!  The number of 32 bit words in the TCP header.  This indicates where
    //!  the data begins.  The TCP header (even one including options) is an
    //!  integral number of 32 bits long.
    //!  @note since the offset field is only 4 bits, a TCP header can't
    //!  be more than 60 bytes long (15 * 4).
    //------------------------------------------------------------------------
    uint8_t DataOffset() const;

    //------------------------------------------------------------------------
    //!  Sets the number of 32 bit words in the TCP header.  This indicates
    //!  where the data begins.  The TCP header (even one including options)
    //!  is an integral number of 32 bits long.
    //!  @note since the offset field is only 4 bits, a TCP header can't
    //!  be more than 60 bytes long (15 * 4).
    //------------------------------------------------------------------------
    uint8_t DataOffset(uint8_t offset);

    //------------------------------------------------------------------------
    //!  Returns the flags.
    //------------------------------------------------------------------------
    uint8_t Flags() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the flags.
    //------------------------------------------------------------------------
    uint8_t Flags(uint8_t flags);

    //------------------------------------------------------------------------
    //!  Returns the window.
    //------------------------------------------------------------------------
    uint16_t Window() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the window.
    //------------------------------------------------------------------------
    uint16_t Window(uint16_t window);

    //------------------------------------------------------------------------
    //!  Returns the checksum.
    //------------------------------------------------------------------------
    uint16_t Checksum() const;

    //------------------------------------------------------------------------
    //!  Sets the checksum, for which we need the IP header and the data.
    //------------------------------------------------------------------------
    void SetChecksum(const Ipv4PacketHeader & ipHeader,
                     const std::string & data);
    
    //------------------------------------------------------------------------
    //!  Returns the urgent pointer.
    //------------------------------------------------------------------------
    uint16_t UrgentPointer() const;

    //------------------------------------------------------------------------
    //!  Sets the urgent pointer.
    //------------------------------------------------------------------------
    uint16_t UrgentPointer(uint16_t urgent);
    
    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads from the given file descriptor @c fd.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;
    
    //------------------------------------------------------------------------
    //!  Writes to the given file descriptor @c fd.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;
    
    //------------------------------------------------------------------------
    //!  Reads from the given FILE pointer @c f.  Returns 1 on success, 0
    //!  on failure (fread() semantics).
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes to the given FILE pointer @c f.  Returns 1 on success, 0
    //!  on failure (fwrite() semantics).
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;
    
    //------------------------------------------------------------------------
    //!  Reads from the given gzFile @c gzf.  Returns the number of bytes
    //!  read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes to the given gzFile @c gzf.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Reads from the given BZFILE pointer @c bzf.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes to the given BZFILE pointer @c bzf.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if one of the
    //!  Write() members were called.
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override;

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const Ipv4TcpHeader & tcpHeader) const;

    //------------------------------------------------------------------------
    //!  Prints the TCP header to an ostream as pseudo-XML.  Each line will
    //!  be prefixed with @c pre.
    //------------------------------------------------------------------------
    std::ostream & 
    PrintXML(std::ostream & os, const std::string & pre = "") const;

    //------------------------------------------------------------------------
    //!  Returns the address of the end of the TCP header.  If our TCP
    //!  header points into an existing TCP packet, this will be the
    //!  first byte of the user data.
    //------------------------------------------------------------------------
    uint8_t *End() const;
    
  private:
    typedef struct __attribute__((packed)) {
      uint16_t   sourcePort;
      uint16_t   destinationPort;
      uint32_t   sequenceNumber;
      uint32_t   ackNumber;
#if BYTE_ORDER == LITTLE_ENDIAN
      uint8_t    _x:4,
                 dataOffset:4;
#elif BYTE_ORDER == BIG_ENDIAN
      uint8_t    dataOffset:4,
                 _x:4;
#else
      #error "BYTE_ORDER not set"
#endif
      uint8_t    flags;
      uint16_t   window;
      uint16_t   checksum;
      uint16_t   urgentPointer;
    } Data;
    
    Data    *_data;
    uint8_t  *_options;
    bool     _ownData;
    bool     _ownOptions;
    
    void FreeOptions();
    void AllocateOptions(uint8_t len);
  };

  
}  // namespace Dwm

#endif  // _DWMIPV4TCPHEADER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
