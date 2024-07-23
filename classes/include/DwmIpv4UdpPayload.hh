//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016, 2020, 2024
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
//!  \file DwmIpv4UdpPayload.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4UdpPayload class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4UDPPAYLOAD_HH_
#define _DWMIPV4UDPPAYLOAD_HH_

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmIpv4UdpHeader.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a UDP payload.
  //--------------------------------------------------------------------------
  class Ipv4UdpPayload
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Ipv4UdpPayload();

    //------------------------------------------------------------------------
    //!  Construct based on a Ipv4UdpHeader that points into a UDP packet,
    //!  which may have a payload following the header.  The underlying
    //!  memory will not be copied, we'll just point to it.  Hence the
    //!  caller should not free the underlying memory while the 
    //!  Ipv4UdpPayload is still in use.
    //------------------------------------------------------------------------
    Ipv4UdpPayload(const Ipv4UdpHeader & udpHeader);

    //------------------------------------------------------------------------
    //!  Construct based on a pointer that points to the first byte of the
    //!  payload in a UDP packet.  The underlying memory will not be copied,
    //!  we'll just point to it.  Hence the caller should not free the
    //!   underlying memory while the Ipv4UdpPayload is still in use.
    //------------------------------------------------------------------------
    Ipv4UdpPayload(uint8_t *buf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Copy the given string into the payload.
    //------------------------------------------------------------------------
    Ipv4UdpPayload(const std::string & s);

    //------------------------------------------------------------------------
    //!  Destructor.
    //------------------------------------------------------------------------
    ~Ipv4UdpPayload();
    
    std::pair<uint16_t,const uint8_t *> Payload() const;

    //------------------------------------------------------------------------
    //!  Returns the payload as a string.
    //------------------------------------------------------------------------
    std::string ToString() const;

    //------------------------------------------------------------------------
    //!  Returns the IP sum of the payload, which can be used as part
    //!  of UDP checksum computation.
    //------------------------------------------------------------------------
    uint32_t Sum() const;
    
    //------------------------------------------------------------------------
    //!  Copies the payload into @c payload.  Returns true on success, false
    //!  on failure.  Note that @c payload must be of a type natively
    //!  supported by Dwm:StreamIO or a type derived from Dwm::StreamReadable.
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
    //!  failure.  Note that @c payload must be of a type natively
    //!  supported by Dwm:StreamIO or a type derived from Dwm::StreamWritable.
    //------------------------------------------------------------------------
    template <typename T>
    bool SetPayload(const T & payload)
    {
      bool  rc = false;
      Free();

      uint16_t  maxPayloadLen = 65535 - (20 + 8);
      uint32_t payloadLen = IOUtils::StreamedLength(payload);
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
    std::ostream & Write(std::ostream & os) const;
    
    //------------------------------------------------------------------------
    //!  Reads len bytes of the payload from the given descriptor @c fd.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;
    
    //------------------------------------------------------------------------
    //!  Reads len bytes of the payload from the given FILE @c f.
    //!  Returns 1 on success, 0 on failure (fread() semantics).
    //------------------------------------------------------------------------
    size_t Read(FILE *f, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given descriptor @c fd.  Returns 1 on
    //!  success, 0 on failure (fwrite() semantics).
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;
    
    //------------------------------------------------------------------------
    //!  Reads len bytes of the payload from the given gzFile @c gzf.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the payload to the given gzFile @c gzf.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;
    
    //------------------------------------------------------------------------
    //!  Reads len bytes of the payload from the given BZFILE @c bzf.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Writes the payload to the given BZFILE @c bzf.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if one of the
    //!  Write() members were called.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;
    
  private:
    std::pair<uint16_t, uint8_t *>  _payload;
    bool                            _ownData;

    void Free();
    void Allocate(uint16_t len);
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4UDPPAYLOAD_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
