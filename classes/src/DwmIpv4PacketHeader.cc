//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file DwmIpv4PacketHeader.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4PacketHeader class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmIpv4PacketHeader.hh"
#include "DwmSysLogger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4PacketHeader.cc 11107 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader::Ipv4PacketHeader()
      : _ownData(true)
  {
    _ip = (struct ip *)calloc(1, sizeof(struct ip));
    assert(_ip);
    _ip->ip_v = 4;
    _ip->ip_hl = (20 >> 2);
    _ip->ip_len = 20;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader::Ipv4PacketHeader(const Ipv4PacketHeader & ipHeader)
      : _ownData(true)
  {
    _ip = (struct ip *)calloc(1, sizeof(struct ip));
    assert(_ip);
    Set(*ipHeader._ip);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader::Ipv4PacketHeader(const struct ip & ip)
      : _ownData(true)
  {
    _ip = (struct ip *)calloc(1, sizeof(struct ip));
    assert(_ip);
    Set(ip);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader::Ipv4PacketHeader(struct ip *ip)
      : _ip(ip), _ownData(false)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader::~Ipv4PacketHeader()
  {
    if (_ownData)
      free(_ip);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4PacketHeader & 
  Ipv4PacketHeader::operator = (const Ipv4PacketHeader & ipHeader)
  {
    if (this != &ipHeader) {
      if (_ownData) {
        free(_ip);
      }
      _ip = (struct ip *)malloc(sizeof(struct ip));
      assert(_ip);
      _ownData = true;
      Set(*ipHeader._ip);
    }
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4PacketHeader::operator == (const Ipv4PacketHeader & ipHeader) const
  {
    bool  rc = false;
    if (_ip == ipHeader._ip) {
      //  pointing to same memory
      rc = true;
    }
    else {
      if (_ip && ipHeader._ip) {
        if (memcmp(_ip, ipHeader._ip, sizeof(*_ip)) == 0) {
          rc = true;
        }
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4PacketHeader::Set(const struct ip & ip)
  {
    memcpy(_ip, &ip, sizeof(*_ip));
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::HeaderLength() const
  {
    return(_ip->ip_hl << 2);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::HeaderLength(uint8_t headerLength)
  {
    //  header length must be between 20 and 60 bytes, 
    //  and a multiple of 4 bytes
    assert(headerLength >= 20 && headerLength <= 60);
    assert(headerLength % 4 == 0);

    _ip->ip_hl = headerLength >> 2;
    return(headerLength);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::TypeOfService() const
  {
    return(_ip->ip_tos);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::TypeOfService(uint8_t typeOfService)
  {
    _ip->ip_tos = typeOfService;
    return(_ip->ip_tos);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::TotalLength() const
  {
    return(ntohs(_ip->ip_len));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::TotalLength(uint16_t totalLength)
  {
    _ip->ip_len = htons(totalLength);
    return(ntohs(_ip->ip_len));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::Id() const
  {
    return(ntohs(_ip->ip_id));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::Id(uint16_t id)
  {
    _ip->ip_id = htons(id);
    return(ntohs(_ip->ip_id));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::FragmentOffset() const
  {
    return(ntohs(_ip->ip_off));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::FragmentOffset(uint16_t fragmentOffset)
  {
    _ip->ip_off = htons(fragmentOffset);
    return(ntohs(_ip->ip_off));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::TimeToLive() const
  {
    return(_ip->ip_ttl);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::TimeToLive(uint8_t timeToLive)
  {
    _ip->ip_ttl = timeToLive;
    return(_ip->ip_ttl);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::Protocol() const
  {
    return(_ip->ip_p);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t Ipv4PacketHeader::Protocol(uint8_t protocol)
  {
    _ip->ip_p = protocol;
    return(_ip->ip_p);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t Ipv4PacketHeader::Checksum() const
  {
    return(ntohs(_ip->ip_sum));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address Ipv4PacketHeader::SourceAddress() const
  {
    return(_ip->ip_src.s_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address 
  Ipv4PacketHeader::SourceAddress(const Ipv4Address & sourceAddress)
  {
    _ip->ip_src.s_addr = sourceAddress.Raw();
    return(_ip->ip_src.s_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address Ipv4PacketHeader::DestinationAddress() const
  {
    return(_ip->ip_dst.s_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address
  Ipv4PacketHeader::DestinationAddress(const Ipv4Address & destinationAddress)
  {
    _ip->ip_dst.s_addr = destinationAddress.Raw();
    return(_ip->ip_dst.s_addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  istream & Ipv4PacketHeader::Read(istream & is)
  {
    if (is) {
      is.read((char *)_ip, sizeof(*_ip));
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & Ipv4PacketHeader::Write(ostream & os) const
  {
    if (os) {
      os.write((const char *)_ip, sizeof(*_ip));
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4PacketHeader::Read(int fd)
  {
    ssize_t  rc = -1;
    
    if (fd >= 0) {
      rc = read(fd, _ip, sizeof(*_ip));
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Ipv4PacketHeader::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = write(fd, _ip, sizeof(*_ip));
      if (rc != sizeof(*_ip)) {
        Syslog(LOG_ERR, "Ipv4PacketHeader::Write(%d) failed: %m", fd);
        rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4PacketHeader::Read(FILE *f)
  {
    size_t  rc = 0;
    if (f) {
      rc = fread(_ip, sizeof(*_ip), 1, f);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4PacketHeader::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      rc = fwrite(_ip, sizeof(*_ip), 1, f);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4PacketHeader::Read(gzFile gzf)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzread(gzf, _ip, sizeof(*_ip));
      if (rc != sizeof(*_ip))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4PacketHeader::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      rc = gzwrite(gzf, _ip, sizeof(*_ip));
      if (rc != sizeof(*_ip))
        rc = -1;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4PacketHeader::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzread(bzf,(void *)_ip,sizeof(*_ip));
        if (rc != sizeof(*_ip))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Ipv4PacketHeader::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzwrite(bzf,(void *)_ip,sizeof(*_ip));
      if (rc != sizeof(*_ip))
        rc = -1;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Ipv4PacketHeader::StreamedLength() const
  {
    return(sizeof(*_ip));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static string Ipv4PacketHeaderOffsetString(uint16_t offset)
  {
    string  rc;
    switch (ntohs(offset)) {
      case 0x2000:
        rc = "MF";
        break;
      case 0x4000:
        rc = "DF";
        break;
      case 0x8000:
        rc = "RF";
        break;
      default:
        {
          ostringstream  os;
          os << "0x" << hex << ntohs(offset);
          rc = os.str();
        }
        break;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & 
  Ipv4PacketHeader::PrintXML(ostream & os, const string & pre) const
  {
    if (os) {
      os << pre << "<ipv4header>\n"
         << pre << "  <hlen>" << (_ip->ip_hl << 2) << "</hlen>\n"
         << hex << setfill('0')
         << pre << "  <tos>0x" << setw(2) << (uint16_t)_ip->ip_tos << "</tos>\n"
         << dec << setfill(' ')
         << pre << "  <len>" << ntohs(_ip->ip_len) << "</len>\n"
         << pre << "  <id>" << ntohs(_ip->ip_id) << "</id>\n"
         << hex << setfill('0')
         << pre << "  <offset>" << Ipv4PacketHeaderOffsetString(_ip->ip_off) << "</offset>\n"
         << dec << setfill(' ')
         << pre << "  <ttl>" << (uint16_t)_ip->ip_ttl << "</ttl>\n"
         << pre << "  <protocol>" << (uint16_t)_ip->ip_p << "</protocol>\n"
         << pre << "  <cksum>" << ntohs(_ip->ip_sum) << "</cksum>\n"
         << pre << "  <src>" << inet_ntoa(_ip->ip_src) << "</src>\n";
      os << pre << "  <dst>" << inet_ntoa(_ip->ip_dst) << "</dst>\n"
         << pre << "</ipv4header>\n";
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4PacketHeader::SetChecksum()
  {
    _ip->ip_sum = 0;
    uint32_t   sum = 0;
    uint16_t  *bufp = (uint16_t *)(_ip);
    uint16_t  *endp = (uint16_t *)((uint8_t *)bufp + (_ip->ip_hl << 2));
    
    // make 16 bit words out of every two adjacent 8 bit words in the packet
    // and add them up
    while (bufp < endp) {
      sum += (uint32_t)(*bufp);
      ++bufp;
    }
    
    // take only 16 bits out of the 32 bit sum and add up the carries
    while (sum >> 16)
      sum = (sum & 0xFFFF) + (sum >> 16);

    // one's complement the result
    _ip->ip_sum = (uint16_t)(~sum);

    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t *Ipv4PacketHeader::End() const
  {
    uint8_t  *rc = 0;
    if (_ip->ip_hl) {
      uint8_t  headerLength = _ip->ip_hl << 2;
      //  header length must be between 20 and 60 bytes,
      //  and a multiple of 4 bytes
      assert(headerLength >= 20 && headerLength <= 60);
      assert(headerLength % 4 == 0);
      rc = (uint8_t *)(_ip) + headerLength;
    }
    else {
      rc = (uint8_t *)(_ip) + sizeof(*_ip);
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
