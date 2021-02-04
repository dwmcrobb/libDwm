//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIpv4Prefix.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4Prefix class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/param.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
}

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <regex>
#include <sstream>
#include <string>

#include "DwmIpv4Prefix.hh"
#include "DwmStringUtils.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  Ipv4Prefix::Ipv4Prefix()
  {
    this->_data[0] = 0xFF;
    this->_data[1] = 0xFF;
    this->_data[2] = 0xFF;
    this->_data[3] = 0xFF;
    this->_data[4] = 32;
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  Ipv4Prefix::Ipv4Prefix(const Ipv4Address & network, uint8_t maskLength)
  {
    this->Set(network, maskLength);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix::Ipv4Prefix(const Ipv4Address & network,
                         const Ipv4Address & netmask)
  {
    this->Set(network, netmask);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix::Ipv4Prefix(const string & prefix)
  {
    static const regex
    rgx("([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])"
        "([.]([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]))?"
        "([.]([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]))?"
        "([.]([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]))?"
        "(\\/([0-9]|[12][0-9]|[3][012]))?",
        std::regex::ECMAScript | std::regex::optimize);
    smatch  sm;
    if (regex_match(prefix, sm, rgx) && (sm.size() == 10)) {
      memset(_data, 0, sizeof(_data));
      uint8_t  *p = _data;
      for (int i = 1; i < 9 && (sm[i].length() > 0); i += 2) {
        *p++ = stoul(sm[i].str());
        _data[4] += 8;
      }
      if (! sm[9].str().empty()) {
        _data[4] = stoul(sm[9].str());
        if (_data[4] == 0) {
          memset(_data, 0, sizeof(_data[0]) * 4);
        }
        else if (_data[4] < 32) {
          uint32_t  mask = htonl(0xffffffff << (32 - _data[4]));
          *(uint32_t *)_data &= mask;
        }
      }
    }
    else {
      memset(_data, 0xFF, sizeof(_data[0]) * 4);
      _data[4] = 32;
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix::Ipv4Prefix(const Ipv4Address & addr)
  {
    ipv4addr_t  rawAddr = addr.Raw();
    memcpy(this->_data, &rawAddr, sizeof(rawAddr));
    this->_data[4] = 32;
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  Ipv4Prefix::~Ipv4Prefix()
  {
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  Ipv4Address Ipv4Prefix::Netmask() const
  {
    ipv4addr_t  val;
    switch (this->_data[4]) {
      case 0:
        val = 0;
        break;
      case 32:
        val = 0xFFFFFFFF;
        break;
      default:
        val = htonl(~(0xFFFFFFFF >> this->_data[4]));
        break;
    }
    
    return(val);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address Ipv4Prefix::FirstAddress() const
  {
    return Network();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Address Ipv4Prefix::LastAddress() const
  {
    ipv4addr_t   lastAddrRaw = ntohl(Network().Raw());
    if (MaskLength() != 32) {
      lastAddrRaw |= (0xFFFFFFFF >> MaskLength());
    }
    return Ipv4Address(htonl(lastAddrRaw));
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::Set(const Ipv4Address & network, uint8_t maskLength)
  {
    if (maskLength > 32)
      return(false);

    ipv4addr_t  mask = 0xffffffff;
    ipv4addr_t  net = network.Raw();
    if (maskLength != 32) {
      if (maskLength == 0) {
        net = 0;
      }
      else {
        mask = htonl(0xffffffff << (32 - maskLength));
      }
    }
    
    net &= mask;
    memcpy(this->_data,&net,sizeof(net));
    this->_data[4] = maskLength;
    
    return(true);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Ipv4Prefix::Set(const Ipv4Address & network,
                       const Ipv4Address & netmask)
  {
    ipv4addr_t  mask = ntohl(netmask.Raw());
    ipv4addr_t  net = network.Raw();
    int8_t      maskLen = 32;
    while (maskLen > 0) {
      if ((mask >> (32 - maskLen)) & 0x1) {
        break;
      }
      --maskLen;
    }
    int8_t  remainingBits = maskLen;
    while (remainingBits > 0) {
      assert((mask >> (32 - remainingBits)) & 0x1);
      --remainingBits;
    }
    net &= htonl(mask);
    memcpy(this->_data, &net, sizeof(net));
    this->_data[4] = maskLen;
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::operator < (const Ipv4Prefix & prefix) const
  {
    bool  rc = false;
    ipv4addr_t  myAddr = ntohl(*(uint32_t *)this->_data);
    ipv4addr_t  othAddr = ntohl(*(uint32_t *)prefix._data);
    if (myAddr < othAddr) {
      rc = true;
    }
    else {
      if (myAddr == othAddr) {
        if (this->_data[4] < prefix._data[4]) {
          rc = true;
        }
      }
    }
    return(rc);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::operator > (const Ipv4Prefix & prefix) const
  {
    bool        rc = false;
    ipv4addr_t  myAddr = ntohl(*(uint32_t *)this->_data);
    ipv4addr_t  othAddr = ntohl(*(uint32_t *)prefix._data);
    if (myAddr > othAddr) {
      rc = true;
    }
    else {
      if (myAddr == othAddr) {
        if (this->_data[4]  > prefix._data[4]) {
          rc = true;
        }
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::operator == (const Ipv4Prefix & prefix) const
  {
    return (memcmp(this->_data, prefix._data, sizeof(this->_data)) == 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::operator != (const Ipv4Prefix & prefix) const
  {
    return (memcmp(this->_data, prefix._data, sizeof(this->_data)) != 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::Contains(const Ipv4Address & address) const
  {
    return((address.Raw() & this->Netmask().Raw()) == this->Network().Raw());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::Contains(const Ipv4Prefix & prefix) const
  {
    bool  rc = false;
    if (prefix._data[4] >= _data[4]) {
      if ((prefix.Network().Raw() & this->Netmask().Raw()) == this->Network().Raw()) {
        rc = true;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Ipv4Prefix::Adjacent(const Ipv4Prefix & prefix) const
  {
    return ((++(Ipv4Address(LastAddress())) == prefix.FirstAddress())
            || (FirstAddress() == ++(Ipv4Address(prefix.LastAddress()))));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::pair<bool,Ipv4Prefix>
  Ipv4Prefix::Combine(const Ipv4Prefix & prefix) const
  {
    std::pair<bool,Ipv4Prefix>  rc(false, Ipv4Prefix("255.255.255.255/32"));
    if (Adjacent(prefix) && (_data[4] == prefix._data[4])) {
      Ipv4Prefix  newpfx(prefix.Network(), _data[4] - 1);
      if (newpfx.Contains(*this) && newpfx.Contains(prefix)) {
        rc.first = true;
        rc.second = newpfx;
      }
    }
    else if (Contains(prefix)) {
      rc.first = true;
      rc.second = *this;
    }
    else if (prefix.Contains(*this)) {
      rc.first = true;
      rc.second = prefix;
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix & Ipv4Prefix::operator ++ ()
  {
    Ipv4Address  lastAddr = LastAddress();
    if (lastAddr != Ipv4Address(0xFFFFFFFF)) {
      ++lastAddr;
      Set(lastAddr, MaskLength());
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix Ipv4Prefix::operator ++ (int)
  {
    Ipv4Prefix  rc(*this);
    Ipv4Address  lastAddr = LastAddress();
    if (lastAddr != Ipv4Address(0xFFFFFFFF)) {
      ++lastAddr;
      Set(lastAddr, MaskLength());
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix & Ipv4Prefix::operator -- ()
  {
    Ipv4Address  firstAddr = FirstAddress();
    if (firstAddr != Ipv4Address(0x00000000)) {
      --firstAddr;
      Set(firstAddr, MaskLength());
    }
    return *this;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Ipv4Prefix Ipv4Prefix::operator -- (int)
  {
    Ipv4Prefix  rc(*this);
    Ipv4Address  firstAddr = FirstAddress();
    if (firstAddr != Ipv4Address(0x00000000)) {
      --firstAddr;
      Set(firstAddr, MaskLength());
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  ssize_t Ipv4Prefix::Read(int fd)
  {
    return(::read(fd,this->_data,sizeof(this->_data)));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4Prefix::Read(FILE *f)
  {
    return(::fread(this->_data,sizeof(this->_data),1,f));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  istream & Ipv4Prefix::Read(istream & is)
  {
    if (is)
      is.read((caddr_t)this->_data,sizeof(this->_data));
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  int Ipv4Prefix::Read(gzFile gzf)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzread(gzf,this->_data,sizeof(this->_data));
      if (rc != sizeof(this->_data))
        rc = -1;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  int Ipv4Prefix::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzread(bzf,(void *)this->_data,sizeof(this->_data));
      if (rc != sizeof(this->_data))
        rc = -1;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  ssize_t Ipv4Prefix::Write(int fd) const
  {
    return(::write(fd,this->_data,sizeof(this->_data)));
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t Ipv4Prefix::Write(FILE *f) const
  {
    return(::fwrite(this->_data,sizeof(this->_data),1,f));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & Ipv4Prefix::Write(ostream & os) const
  {
    if (os)
      os.write((caddr_t)this->_data,sizeof(this->_data));
    return(os);
  }

  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  int Ipv4Prefix::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      rc = gzwrite(gzf,(void *)this->_data,sizeof(this->_data));
      if (rc != sizeof(this->_data))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  int Ipv4Prefix::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzwrite(bzf,(void *)this->_data,sizeof(this->_data));
      if (rc != sizeof(this->_data))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //  
  //--------------------------------------------------------------------------
  ostream & operator << (ostream & os, const Ipv4Prefix & prefix)
  {
    struct in_addr  inAddr;
    inAddr.s_addr = prefix.Network().Raw();
    ostringstream  oss;
    oss << inet_ntoa(inAddr) << "/" << (int)prefix._data[4];
    os << oss.str();
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string Ipv4Prefix::ToString() const
  {
    char  buf[20];
    
    struct in_addr  inAddr;
    memcpy(&inAddr.s_addr,this->_data,sizeof(inAddr.s_addr));
    snprintf(buf,sizeof(buf),"%s/%d",inet_ntoa(inAddr),this->_data[4]);
    return(string(buf));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string Ipv4Prefix::ToShortString() const
  {
    ostringstream  os;
    ipv4addr_t  a = *((const uint32_t *)_data);
    a = ntohl(a);
    os << (a >> 24);
    if (_data[4] > 8) {
      os << "." << ((a >> 16) & 0xFF);
      if (_data[4] > 16) {
        os << "." << ((a >> 8) & 0xFF);
        if (_data[4] > 24) {
          os << "." << (a & 0xFF);
        }
      }
    }
    os << "/" << (uint16_t)(_data[4]);
    return os.str();
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void Ipv4RangePrefixes(vector<Ipv4Prefix> & rc,
                                const Ipv4Address & startAddr,
                                const Ipv4Address & endAddr)
  {
    Ipv4Prefix  pfx(startAddr, 32);
    Ipv4Prefix  p = pfx;
    while ((pfx.FirstAddress() >= startAddr)
           && (pfx.LastAddress() <= endAddr)) {
      p = pfx;
      pfx.Set(startAddr, pfx.MaskLength() - 1);
    }
    rc.push_back(p);
    if (pfx.FirstAddress() > startAddr) {
      Ipv4Address  s(p.FirstAddress());
      Ipv4RangePrefixes(rc, startAddr, --s);
    }
    if (p.LastAddress() < endAddr) {
      Ipv4Address  e(p.LastAddress());
      Ipv4RangePrefixes(rc, ++e, endAddr);
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  vector<Ipv4Prefix> Ipv4RangePrefixes(const Ipv4Address & addr1,
                                       const Ipv4Address & addr2)
  {
    vector<Ipv4Prefix>  rc;
    if (addr2 > addr1) {
      Ipv4RangePrefixes(rc, addr1, addr2);
    }
    else {
      Ipv4RangePrefixes(rc, addr2, addr1);
    }
    return rc;
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
