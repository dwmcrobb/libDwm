//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4IcmpDestinationUnreachable.cc 11100 $
// @(#) $Id: DwmIpv4IcmpDestinationUnreachable.cc 11100 2020-09-07 22:16:21Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file DwmIpv4IcmpDestinationUnreachable.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4IcmpDestinationUnreachable class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in.h>
}

#include <cassert>
#include <cstring>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4IcmpDestinationUnreachable.hh"
#include "DwmIpv4Utils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4IcmpDestinationUnreachable.cc 11100 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpDestinationUnreachable::Ipv4IcmpDestinationUnreachable()
  {
    Allocate(28);
    Type(3);
    Code(0);
    NexthopMTU(1500);
    Ipv4PacketHeader  ipHeader;
    ipHeader.SetChecksum();
    IpHeader(ipHeader);
    SetChecksum();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpDestinationUnreachable::
  Ipv4IcmpDestinationUnreachable(uint8_t code, uint16_t nexthopMTU,
                                 const Ipv4PacketHeader & ipHeader,
                                 const std::string & data)
  {
    uint16_t len = 
      sizeof(uint8_t)     // type
      + sizeof(uint8_t)   // code
      + sizeof(uint16_t)  // checksum
      + sizeof(uint16_t)  // empty
      + sizeof(uint16_t)  // nexthop MTU
      + ipHeader.HeaderLength();  //  IP header
    if (data.length() > 8)
      Allocate(len + 8);
    else
      Allocate(len + data.length());

    uint8_t *ptr = _data.second;

    //  set the type and code
    *ptr++ = 3;     //  type = 3
    *ptr++ = code;  //  code

    //  remember location of checksum, then skip over it
    uint16_t  *cksum = (uint16_t *)ptr;
    ptr += sizeof(*cksum);

    //  skip over empty field
    ptr += 2;
    
    //  set the nexthop MTU
    *(uint16_t *)ptr = htons(nexthopMTU);    ptr += sizeof(nexthopMTU);

    //  set the IP header
    ostringstream  os;
    ipHeader.Write(os);
    memcpy(ptr, os.str().c_str(), os.str().size());
    ptr += ipHeader.HeaderLength();
    
    //  set the data
    if (! data.empty())
      memcpy(ptr, data.c_str(), data.length() > 8 ? 8 : data.length());

    //  compute and set the checksum
    uint32_t  sum = Ipv4Utils::Sum(_data.second, _data.first);
    Ipv4Utils::AddCarries(sum);

    //  set the checksum
    *cksum = (uint16_t)sum;

    _ownData = true;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4IcmpDestinationUnreachable::
  Ipv4IcmpDestinationUnreachable(uint8_t *buf, uint16_t len)
  {
    _data.first = len;
    _data.second = buf;
    _ownData = false;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4IcmpDestinationUnreachable::NexthopMTU() const
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
  uint16_t Ipv4IcmpDestinationUnreachable::NexthopMTU(uint16_t nexthopMTU)
  {
    if (_data.first < 8)
      Reallocate(8);
    
    uint16_t  u16 = htons(nexthopMTU);
    memcpy(_data.second + 6, &u16, sizeof(u16));
    SetChecksum();
    return(ntohs(*(uint16_t *)(_data.second + 6)));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader Ipv4IcmpDestinationUnreachable::IpHeader() const
  {
    Ipv4PacketHeader  ipHeader;
    if ((_data.first >= 9) && _data.second) {
      if (_data.first >= (8 + ((_data.second[8] & 0x0f) << 2))) {
        istringstream  is(string((char *)_data.second + 8, _data.first - 8));
        // is.rdbuf()->pubsetbuf((char *)_data.second + 8, _data.first - 8);
        ipHeader.Read(is);
      }
    }
    return(ipHeader);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader Ipv4IcmpDestinationUnreachable::
  IpHeader(const Ipv4PacketHeader & ipHeader)
  {
    if (_data.first < 8 + ipHeader.HeaderLength())
      Reallocate(8 + ipHeader.HeaderLength());
    ostringstream  os;
    ipHeader.Write(os);
    memcpy((char *)_data.second + 8, os.str().c_str(), _data.first - 8);
    SetChecksum();
    return(IpHeader());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Ipv4IcmpDestinationUnreachable::Data() const
  {
    Ipv4PacketHeader  ipHeader = IpHeader();
    uint16_t  offset = ipHeader.HeaderLength() + 8;
    if ((_data.first >= offset) && _data.second)
      return(std::string((const char *)_data.second + offset, 
                         _data.first - offset));
    else
      return(std::string(""));
  }

}  // namespace Dwm

