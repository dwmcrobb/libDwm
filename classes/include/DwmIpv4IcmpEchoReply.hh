//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIpv4IcmpEchoReply.hh
//!  \brief Dwm::Ipv4IcmpEchoReply class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ICMPECHOREPLY_HH_
#define _DWMIPV4ICMPECHOREPLY_HH_

extern "C" {
#include <inttypes.h>
}

#include <sstream>
#include <string>

#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmIpv4Address.hh"
#include "DwmIpv4IcmpMessage.hh"
#include "DwmSocket.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an IPv4 ICMP echo reply.
  //--------------------------------------------------------------------------
  class Ipv4IcmpEchoReply
    : public Ipv4IcmpMessage
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    Ipv4IcmpEchoReply();
    
    //------------------------------------------------------------------------
    //!  Construct from the given identifier @c id, sequence number @c seq
    //!  and @c data.
    //------------------------------------------------------------------------
    Ipv4IcmpEchoReply(uint16_t id, uint16_t seq, const std::string & data);

    Ipv4IcmpEchoReply(uint8_t *buf, uint16_t len);

    //------------------------------------------------------------------------
    //!  Returns the identifier.
    //------------------------------------------------------------------------
    uint16_t Identifier() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the identifier.
    //------------------------------------------------------------------------
    uint16_t Identifier(uint16_t id);
    
    //------------------------------------------------------------------------
    //!  Returns the sequence number.
    //------------------------------------------------------------------------
    uint16_t SequenceNumber() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the identifier.
    //------------------------------------------------------------------------
    uint16_t SequenceNumber(uint16_t seq);
    
    //------------------------------------------------------------------------
    //!  Returns the data as a string.
    //------------------------------------------------------------------------
    std::string Data() const;
    
    //------------------------------------------------------------------------
    //!  Sets the data.
    //------------------------------------------------------------------------
    std::string Data(const std::string & data);

    //------------------------------------------------------------------------
    //!  Sets the data to the given @c data.  @c data must be writable via
    //!  Dwm::StreamIO, which means it must be a supported type or a type
    //!  derived from Dwm::StreamWritable.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename T>
    bool SetData(const T & data)
    {
      bool  rc = false;
      uint32_t  dataLen = IOUtils::StreamedLength(data);
      Reallocate(dataLen + 8);
      std::ostringstream  os;
      if (StreamIO::Write(os, data)) {
        memcpy((char *)_data.second + 8, os.str().c_str(), dataLen);
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Copies the encapsulated data into @c data.  @c data must be readable
    //!  via Dwm::StreamIO, which means it must be a supported type or a type
    //!  derived from Dwm::StreamReadable.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename T>
    bool GetData(T & data) const
    {
      bool  rc = false;
      if (_data.first >= (8 + IOUtils::StreamedLength(data))) {
        std::istringstream  is(std::string((char *)_data.second + 8,
                                           _data.first - 8));
        if (StreamIO::Read(is, data)) {
          rc = true;
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Receives the echo reply from the given Socket @c s.  @c source will
    //!  be set to the address that sent the echo reply.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    bool RecvFrom(Socket & s, Ipv4Address & source);
    
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4ICMPECHOREPLY_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
