//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmIpv4TcpPayload.hh
//!  \brief Dwm::Ipv4TcpPayload class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4TCPPAYLOAD_HH_
#define _DWMIPV4TCPPAYLOAD_HH_

extern "C" {
  #include <inttypes.h>
}

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "DwmIpv4TcpHeader.hh"
#include "DwmBZ2IOCapable.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a TCP payload.
  //!  @note We don't inherit from Readable, GZReadable and BZ@Readable
  //!  because our read members need to be passed a length.
  //--------------------------------------------------------------------------
  class Ipv4TcpPayload
    : public DescriptorWritable, public FileWritable, public StreamWritable,
      public StreamedLengthCapable, public GZWritable, public BZ2Writable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Ipv4TcpPayload();

    //------------------------------------------------------------------------
    //!  Construct based on an Ipv4PacketHeader and  Ipv4TcpHeader that point
    //!  into a TCP packet, which may have a payload following the TCP 
    //!  header.  The underlying memory will not be copied, we'll just point
    //!  to it.  Hence the caller should not free the underlying memory while
    //!  the Ipv4TcpPayload is still in use.
    //------------------------------------------------------------------------
    Ipv4TcpPayload(const Ipv4PacketHeader & ipHeader,
                   const Ipv4TcpHeader & tcpHeader);

    //------------------------------------------------------------------------
    //!  Construct based on a pointer that points to the first byte of the
    //!  payload in a TCP packet.  The underlying memory will not be copied,
    //!  we'll just point to it.  Hence the caller should not free the
    //!   underlying memory while the Ipv4TcpPayload is still in use.
    //------------------------------------------------------------------------
    Ipv4TcpPayload(uint8_t *buf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Copy the given string into the payload.
    //------------------------------------------------------------------------
    Ipv4TcpPayload(const std::string & s);

    //------------------------------------------------------------------------
    //!  Destructor.
    //------------------------------------------------------------------------
    ~Ipv4TcpPayload();
    
    //------------------------------------------------------------------------
    //!  Returns a shallow copy of the payload.
    //------------------------------------------------------------------------
    std::pair<uint16_t,const uint8_t *> Payload() const;

    //------------------------------------------------------------------------
    //!  Returns the payload as a string.
    //------------------------------------------------------------------------
    std::string ToString() const;

    //------------------------------------------------------------------------
    //!  Returns the IP sum of the payload, which can be used as part
    //!  of TCP checksum computation.
    //------------------------------------------------------------------------
    uint32_t Sum() const;
    
    //------------------------------------------------------------------------
    //!  Copies the payload into @c payload.  Returns true on success, false
    //!  on failure.  Note that payload must be a type natively supported by
    //!  Dwm::StreamIO or a type derived from Dwm::StreamReadable.
    //------------------------------------------------------------------------
    template <typename T>
    bool GetPayload(T & payload) const
    {
      bool  rc = false;
      if (_payload.first) {
        std::istringstream  is(std::string((const char *)_payload.second,
                                           _payload.first));
        rc = StreamIO::Read(is, payload);
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Sets the payload to @c payload.  Returns true on success, false on
    //!  failure.  Note that payload must be a type natively supported by
    //!  Dwm::StreamIO or a type derived from Dwm::StreamWritable.
    //------------------------------------------------------------------------
    template <typename T>
    bool SetPayload(const T & payload)
    {
      bool  rc = false;
      Free();

      uint16_t  maxPayloadLen = 65535 - (20 + 8);
      uint64_t  payloadLen = IOUtils::StreamedLength(payload);
      if (payloadLen <= maxPayloadLen) {
        std::ostringstream  os;
        if (StreamIO::Write(os, payload)) {
          _payload.first = payloadLen;
          _payload.second = (uint8_t *)calloc(1, _payload.first);
          assert(_payload.second);
          memcpy(_payload.second, os.str().c_str(), _payload.first);
          rc = true;
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Clears the payload.
    //------------------------------------------------------------------------
    void Clear();
    
    //------------------------------------------------------------------------
    //!  Reads @c len bytes of the payload from the given istream @c is.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given ostream @c os.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;
    
    //------------------------------------------------------------------------
    //!  Reads @c len bytes of the payload from the given descriptor @c fd.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;
    
    //------------------------------------------------------------------------
    //!  Reads @c len bytes of the payload from the given FILE @c f.
    //!  Returns 1 on success, 0 on failure (fread() semantics).
    //------------------------------------------------------------------------
    size_t Read(FILE *f, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given FILE @c f.  Returns 1 on success,
    //!  0 on failure (fread() semantics).
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;
    
    //------------------------------------------------------------------------
    //!  Reads @c len bytes of the payload from the given gzFile @c gzf.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given gzFile @c gzf.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Reads @c len bytes of the payload from the given BZFILE @c bzf.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Writes the payload to the given BZFILE @c bzf.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called one
    //!  of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override;
    
  private:
    std::pair<uint16_t, uint8_t *>  _payload;
    bool                            _ownData;

    void Free();
    void Allocate(uint16_t len);
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4TCPPAYLOAD_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
