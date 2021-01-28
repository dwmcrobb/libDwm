//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2020
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
//!  \file DwmIpv4UdpHeader.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4UdpHeader class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <arpa/inet.h>
  #include <unistd.h>
}

#include <cassert>
#include <sstream>

#include "DwmIpv4UdpHeader.hh"
#include "DwmIpv4UdpPayload.hh"
#include "DwmIpv4Utils.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader::Ipv4UdpHeader()
      : _ownData(true)
  {
    _data = new Data();
    assert(_data);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader::Ipv4UdpHeader(const Ipv4UdpHeader & udpHeader)
      : _ownData(true)
  {
    _data = new Data();
    assert(_data);
    *_data = *udpHeader._data;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader::Ipv4UdpHeader(const Ipv4PacketHeader & ipHeader)
      : _data((Data *)ipHeader.End()), _ownData(false)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader::Ipv4UdpHeader(struct udphdr *udphdr)
      : _ownData(false)
  {
    _data = (Data *)udphdr;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader::Ipv4UdpHeader(const struct udphdr & udpHdr)
      : _ownData(true)
  {
    _data = new Data();
    assert(_data);
    memcpy(_data, &udpHdr, sizeof(*_data));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader & Ipv4UdpHeader::operator = (const Ipv4UdpHeader & udpHeader)
  {
    if (this != &udpHeader) {
      *_data = *(udpHeader._data);
    }
    return(*this);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4UdpHeader::~Ipv4UdpHeader()
  {
    if (_ownData)
      delete(_data);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::SourcePort() const
  {
    return(ntohs(_data->sourcePort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::SourcePort(uint16_t port)
  {
    _data->sourcePort = htons(port);
    return(ntohs(_data->sourcePort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::DestinationPort() const
  {
    return(ntohs(_data->destinationPort));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::DestinationPort(uint16_t port)
  {
    _data->destinationPort = htons(port);
    return(ntohs(_data->destinationPort));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::UdpLength() const
  {
    return(ntohs(_data->udpLength));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::UdpLength(uint16_t udpLength)
  {
    _data->udpLength = htons(udpLength);
    return(ntohs(_data->udpLength));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4UdpHeader::UdpChecksum() const
  {
    return(ntohs(_data->checksum));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4UdpHeader::SetUdpChecksum(const Ipv4PacketHeader & ipHeader,
                                     const std::string & data)
  {
    _data->checksum = 0;
    
    uint32_t  sum = ipHeader.SourceAddress().IpSum();
    sum += ipHeader.DestinationAddress().IpSum();
    sum += htons(17);
    sum += _data->udpLength;
    sum += Ipv4Utils::Sum((const uint8_t *)_data, sizeof(*_data));
    sum += Ipv4Utils::Sum((const uint8_t *)data.c_str(), data.length());

    Ipv4Utils::AddCarries(sum);
    _data->checksum = (uint16_t)sum;
    
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4UdpHeader::SetUdpChecksum(const Ipv4PacketHeader & ipHeader,
                                     const Ipv4UdpPayload & payload)
  {
    _data->checksum = 0;
    uint32_t  sum = ipHeader.SourceAddress().IpSum();
    sum += ipHeader.DestinationAddress().IpSum();
    sum += htons(17);
    sum += _data->udpLength;
    sum += Ipv4Utils::Sum((const uint8_t *)_data, sizeof(*_data));
    sum += payload.Sum();
  
    Ipv4Utils::AddCarries(sum);
    _data->checksum = (uint16_t)sum;
    
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t *Ipv4UdpHeader::End() const
  {
    return((uint8_t *)_data + sizeof(*_data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Ipv4UdpHeader::Read(std::istream & is)
  {
    if (is) {
      is.read((char *)_data, sizeof(*_data));
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Ipv4UdpHeader::Write(std::ostream & os) const
  {
    if (os) {
      os.write((const char *)_data, sizeof(*_data));
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4UdpHeader::Read(int fd)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = read(fd, _data, sizeof(*_data));
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4UdpHeader::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = write(fd, _data, sizeof(*_data));
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4UdpHeader::Read(FILE * f)
  {
    size_t  rc = 0;
    if (f) {
      rc = fread(_data, sizeof(*_data), 1, f);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4UdpHeader::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      rc = fwrite(_data, sizeof(*_data), 1, f);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t Ipv4UdpHeader::StreamedLength() const
  {
    return(sizeof(*_data));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpHeader::Read(gzFile gzf)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzread(gzf, _data, sizeof(*_data));
      if (rc != sizeof(*_data))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpHeader::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      rc = gzwrite(gzf, _data, sizeof(*_data));
      if (rc != sizeof(*_data))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpHeader::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzread(bzf, (void *)_data, sizeof(*_data));
        if (rc != sizeof(*_data))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4UdpHeader::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzwrite(bzf, (void *)_data, sizeof(*_data));
      if (rc != sizeof(*_data))
        rc = -1;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4UdpHeader::operator == (const Ipv4UdpHeader & udpHeader) const
  {
    return(memcmp(_data, udpHeader._data, sizeof(*_data)) == 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & 
  Ipv4UdpHeader::PrintXML(std::ostream & os, const std::string & pre) const
  {
    if (os) {
      os << pre << "<udpheader>\n"
         << pre << "  <sourcePort>" << ntohs(_data->sourcePort) 
         << "</sourcePort>\n"
         << pre << "  <destinationPort>" << ntohs(_data->destinationPort)
         << "</destinationPort>\n"
         << pre << "  <udpLength>" << ntohs(_data->udpLength) 
         << "</udpLength>\n"
         << pre << "  <cksum>" << ntohs(_data->checksum) << "</cksum>\n"
         << pre << "</udpheader>\n";
    }
    return(os);
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
