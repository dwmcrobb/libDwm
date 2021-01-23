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
//!  \file DwmIpv4TcpPacket.hh
//!  \brief Dwm::Ipv4TcpPacket class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4TCPPACKET_HH_
#define _DWMIPV4TCPPACKET_HH_

#include <sstream>
#include <utility>

#include "DwmBZ2IOCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmIpv4PacketHeader.hh"
#include "DwmIpv4TcpHeader.hh"
#include "DwmIpv4TcpPayload.hh"
#include "DwmSocket.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a TCP packet, including the IP header.
  //--------------------------------------------------------------------------
  class Ipv4TcpPacket
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Ipv4TcpPacket();

    //------------------------------------------------------------------------
    //!  Construct from a pointer to the start of IP header in a TCP packet.
    //!  We don't copy, so the caller still owns the memory at @c ptr.
    //------------------------------------------------------------------------
    Ipv4TcpPacket(uint8_t *ptr);

    //------------------------------------------------------------------------
    //!  Destructor.  If we own the underlying data, we free it.
    //------------------------------------------------------------------------
    ~Ipv4TcpPacket();
    
    //------------------------------------------------------------------------
    //!  Returns a const reference to the IP header.
    //------------------------------------------------------------------------
    const Ipv4PacketHeader & IpHeader() const;
    
    //------------------------------------------------------------------------
    //!  Returns a mutable reference to the IP header.
    //------------------------------------------------------------------------
    Ipv4PacketHeader & IpHeader();

    //------------------------------------------------------------------------
    //!  Returns a const refernece to the TCP header.
    //------------------------------------------------------------------------
    const Ipv4TcpHeader & TcpHeader() const;

    //------------------------------------------------------------------------
    //!  Returns a mutable reference to the TCP header.
    //------------------------------------------------------------------------
    Ipv4TcpHeader & TcpHeader();

    //------------------------------------------------------------------------
    //!  Returns the payload as a string.
    //------------------------------------------------------------------------
    std::string Payload() const
    {
      return(_tcpPayload.ToString());
    }
    
    //------------------------------------------------------------------------
    //!  Copies the packet payload into @c payload.  Note that payload must
    //!  be natively supported by Dwm::StreamIO or a type derived from
    //!  DWm::StreamReadable.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename T>
    bool GetPayload(T & payload) const
    {
      return(_tcpPayload.GetPayload<T>(payload));
    }
    
    //------------------------------------------------------------------------
    //!  Sets the packet payload to @c payload.  Note that @c payload must
    //!  be natively supported by Dwm::StreamIO or a type derived from
    //!  Dwm::StreamWritable.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename T>
    bool SetPayload(const T & payload)
    {
      bool  rc = _tcpPayload.SetPayload<T>(payload);
     
      if (rc) {
        _ipHeader.TotalLength(_ipHeader.HeaderLength() 
                              + _tcpHeader.DataOffset() + 
                              _tcpPayload.StreamedLength());
      }
      return(rc);
    }
  
    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads the packet from the given FILE pointer @c f.  Returns 1 on
    //!  success, 0 on failure (fread() semantics).
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes the packet to the given FILE pointer @c f.  Returns 1 on
    //!  success, 0 on failure (fwrite() semantics).
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Reads the packet from the given gzFile @c gzf.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes the packet to the given gzFile @c gzf.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the packet from the given BZFILE @c bzf.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes the packet to the given BZFILE @c bzf.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the numbers of bytes that should be written if one
    //!  of the Write() members is called.
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override;

    //------------------------------------------------------------------------
    //!  Sends the packet via the given descriptor @c fd.  Returns true
    //!  on success, false on failure.
    //------------------------------------------------------------------------
    bool SendTo(int fd) const;
    
    //------------------------------------------------------------------------
    //!  Sends the packet via the given Socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    bool SendTo(Socket & s) const;
    
  private:
    Ipv4PacketHeader  _ipHeader;
    Ipv4TcpHeader     _tcpHeader;
    Ipv4TcpPayload    _tcpPayload;
    bool              _ownData;

    void FreePayload();
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4TCPPACKET_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
