//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2020
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
//!  \file DwmIpv4IcmpEchoReply.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4IcmpEchoReply class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in.h>
}

#include <cassert>
#include <cstring>

#include "DwmIpv4IcmpEchoReply.hh"
#include "DwmIpv4Utils.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpEchoReply::Ipv4IcmpEchoReply()
  {
    Allocate(8);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpEchoReply::Ipv4IcmpEchoReply(uint16_t id, uint16_t seq,
                                       const std::string & data)
  {
    uint16_t  len = 
      sizeof(uint8_t)     // type
      + sizeof(uint8_t)   // code
      + sizeof(uint16_t)  // checksum
      + sizeof(uint16_t)  // identifier
      + sizeof(uint16_t)  // sequence number
      + data.length();    // user data
    Allocate(len);

    uint8_t *ptr = _data.second;

    //  set the type and code
    *ptr++ = 0;  //  type = 0
    *ptr++ = 0;  //  code = 0

    //  remember location of checksum, then skip over it
    uint16_t  *cksum = (uint16_t *)ptr;
    ptr += sizeof(*cksum);

    //  set the identifier
    *(uint16_t *)ptr = htons(id);    ptr += sizeof(id);

    //  set the sequence number
    *(uint16_t *)ptr = htons(seq);   ptr += sizeof(seq);

    //  set the data
    if (! data.empty())
      memcpy(ptr, data.c_str(), data.length());

    //  compute and set the checksum
    uint32_t  sum = Ipv4Utils::Sum(_data.second, _data.first);
    Ipv4Utils::AddCarries(sum);

    //  set the cheecksum
    *cksum = (uint16_t)sum;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpEchoReply::Ipv4IcmpEchoReply(uint8_t *buf, uint16_t len)
  {
    _data.first = len;
    _data.second = buf;
    _ownData = false;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4IcmpEchoReply::Identifier() const
  {
    uint16_t  rc = 0;
    if ((_data.first >= 6) && _data.second) {
      rc = ntohs(*(uint16_t *)(_data.second + 4));
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4IcmpEchoReply::Identifier(uint16_t id)
  {
    assert((_data.first >= 6) && _data.second);
    uint16_t  u16 = htons(id);
    memcpy(_data.second + 4, &u16, sizeof(u16));
    return(ntohs(*(uint16_t *)(_data.second + 4)));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4IcmpEchoReply::SequenceNumber() const
  {
    uint16_t  rc = 0;
    if ((_data.first >= 8) && _data.second) {
      rc = ntohs(*(uint16_t *)(_data.second + 6));
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4IcmpEchoReply::SequenceNumber(uint16_t seq)
  {
    assert((_data.first >= 8) && _data.second);
    uint16_t  u16 = htons(seq);
    memcpy(_data.second + 6, &u16, sizeof(u16));
    return(ntohs(*(uint16_t *)(_data.second + 6)));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Ipv4IcmpEchoReply::Data() const
  {
    if ((_data.first > 8) && _data.second)
      return(std::string((const char *)_data.second + 8, _data.first - 8));
    else
      return(std::string(""));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Ipv4IcmpEchoReply::Data(const std::string & data)
  {
    Reallocate(8 + data.length());
    memcpy(_data.second + 8, data.c_str(), _data.first - 8);
    return(Data());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4IcmpEchoReply::RecvFrom(Socket & s, Ipv4Address & source)
  {
    bool  rc = false;
    if (s) {
      const size_t  buflen = 2048;
      uint8_t       buf[buflen];
      struct sockaddr_in  sockAddr;
      ssize_t  bytesReceived = s.RecvFrom(buf, buflen, 0, sockAddr);
      //  Note the recvfrom() on a raw socket gets the IP header too,
      //  so we need to skip over it to get to the ICMP data.
      if (bytesReceived >= 28) {
        uint16_t  iphl = (buf[0] & 0x0f) << 2;
        if (bytesReceived >= iphl + 8) {
          std::istringstream  is;
          is.rdbuf()->pubsetbuf((char *)buf + iphl, bytesReceived - iphl);
          if (Read(is, bytesReceived - iphl)) {
            source = sockAddr.sin_addr.s_addr;
            rc = true;
          }
        }
      }
    }
    
    return(rc);
  }
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
