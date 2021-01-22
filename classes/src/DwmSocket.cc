//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmSocket.cc 11134 $
// @(#) $Id: DwmSocket.cc 11134 2020-09-07 23:44:48Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007, 2016
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
//!  \file DwmSocket.cc
//!  \author Daniel W. McRobb
//!  \brief Socket class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>
  #include <sys/param.h>
  #include <sys/socket.h>
  #include <net/if.h>
}

#include <cstdlib>
#include <cstring>
#include <string>

#include "DwmIpv4PacketHeader.hh"
#include "DwmSysLogger.hh"
#include "DwmSocket.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmSocket.cc 11134 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket::Socket()
      : Descriptor(), _type(0), _headerIncluded(false)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket::operator bool () const
  {
    return(_fd >= 0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket::Socket(const Socket & sock)
      : Descriptor((const Descriptor &)sock), _type(sock._type),
        _headerIncluded(sock._headerIncluded)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket & Socket::operator = (const Socket & sock)
  {
    if (&sock != this) {
      _type = sock._type;
      _headerIncluded = sock._headerIncluded;
      if (_fd >= 0) {
        close(_fd);
        _fd = -1;
        if (sock._fd >= 0) {
          _fd = dup(sock._fd);
        }
      }
    }
    return *this;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Open(int domain, int type, int protocol)
  {
    bool  rc = false;

    if (this->_fd >= 0) {
      this->Close();
    }
    _type = 0;
    
    this->_fd = socket(domain, type, protocol);
    if (this->_fd >= 0) {
      _type = type;
      rc = true;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Socket::Type() const
  {
    return(_type);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket Socket::Accept(struct sockaddr_in & addr)
  {
    Socket     rc;
    socklen_t  addrLen = sizeof(addr);
    rc._fd = accept(this->_fd, (struct sockaddr *)&addr, &addrLen);
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket Socket::Accept(struct sockaddr_in6 & addr)
  {
    Socket  rc;
    socklen_t  addrLen = sizeof(addr);
    rc._fd = accept(this->_fd, (struct sockaddr *)&addr, &addrLen);
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket Socket::Accept(Ipv4Address & addr, uint16_t & port)
  {
    struct sockaddr_in  sockAddr;
    Socket  rc = Accept(sockAddr);
    if (rc._fd >= 0) {
      addr = Ipv4Address(sockAddr.sin_addr.s_addr);
      port = ntohs(sockAddr.sin_port);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket Socket::Accept(Ipv6Address & addr, uint16_t & port)
  {
    struct sockaddr_in6  sockAddr;
    Socket  rc = Accept(sockAddr);
    if (rc._fd >= 0) {
      addr = Ipv6Address(sockAddr.sin6_addr);
      port = ntohs(sockAddr.sin6_port);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Accept(Socket & s, struct sockaddr_in & addr)
  {
    bool       rc = false;
    socklen_t  addrLen = sizeof(addr);
    int        fd = accept(this->_fd, (struct sockaddr *)&addr, &addrLen);
    if (fd >= 0) {
      s.Attach(fd);
      rc = true;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Accept(Socket & s, struct sockaddr_in6 & addr)
  {
    bool       rc = false;
    socklen_t  addrLen = sizeof(addr);
    int        fd = accept(this->_fd, (struct sockaddr *)&addr, &addrLen);
    if (fd >= 0) {
      s.Attach(fd);
      rc = true;
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Accept(Socket & s, Ipv4Address & addr, uint16_t & port)
  {
    struct sockaddr_in  inAddr;
    memset(&inAddr, 0, sizeof(inAddr));
    bool  rc = Accept(s, inAddr);
    if (rc) {
      addr = Ipv4Address(inAddr.sin_addr.s_addr);
      port = ntohs(inAddr.sin_port);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Accept(Socket & s, Ipv6Address & addr, uint16_t & port)
  {
    struct sockaddr_in6  inAddr;
    memset(&inAddr, 0, sizeof(inAddr));
    bool  rc = Accept(s, inAddr);
    if (rc) {
      addr = Ipv6Address(inAddr.sin6_addr);
      port = ntohs(inAddr.sin6_port);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Socket Socket::Accept(struct sockaddr_un & addr)
  {
    Socket     rc;
    socklen_t  addrLen = sizeof(addr);
    rc._fd = accept(this->_fd, (struct sockaddr *)&addr, &addrLen);
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Accept(Socket & s, struct sockaddr_un & addr)
  {
    bool       rc = false;
    socklen_t  addrLen = sizeof(addr);
    int        fd = accept(this->_fd, (struct sockaddr *)&addr, &addrLen);
    if (fd >= 0) {
      s.Attach(fd);
      rc = true;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Listen(int backlog)
  {
    bool  rc = false;
    if (this->_fd >= 0) {
      if (listen(this->_fd, backlog) == 0) {
        rc = true;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Bind(const struct sockaddr_in & addr)
  {
    bool  rc = false;
    if (bind(this->_fd, (const struct sockaddr *)&addr, sizeof(addr)) == 0) {
      rc = true;
    }
    else {
      Syslog(LOG_ERR, "bind(%d,%p,%d) failed: %m",
             this->_fd, &addr, sizeof(addr));
    }
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Bind(const struct sockaddr_in6 & addr)
  {
    bool  rc = false;
    if (bind(this->_fd, (const struct sockaddr *)&addr, sizeof(addr)) == 0) {
      rc = true;
    }
    else {
      Syslog(LOG_ERR, "bind(%d,%p,%d) failed: %m",
             this->_fd, &addr, sizeof(addr));
    }
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Bind(const Ipv4Address & addr, uint16_t port)
  {
    struct sockaddr_in  sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
#ifndef __linux__
    sockAddr.sin_len = sizeof(sockAddr);
#endif
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = addr.Raw();
    sockAddr.sin_port = htons(port);
    return Bind(sockAddr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Bind(const Ipv6Address & addr, uint16_t port)
  {
    struct sockaddr_in6  sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
#ifndef __linux__
    sockAddr.sin6_len = sizeof(sockAddr);
#endif
    sockAddr.sin6_family = AF_INET6;
    sockAddr.sin6_addr = addr;
    sockAddr.sin6_port = htons(port);
    return Bind(sockAddr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::RecvFrom(void *buf, size_t len, int flags,
                           struct sockaddr_in & from)
  {
    ssize_t  rc = -1;
    if (this->_fd >= 0) {
      socklen_t  fromLen = sizeof(from);
      rc = recvfrom(this->_fd, buf, len, flags, (struct sockaddr *)&from,
                    &fromLen);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::RecvFrom(void *buf, size_t len, int flags,
                           struct sockaddr_in6 & from)
  {
    ssize_t  rc = -1;
    if (this->_fd >= 0) {
      socklen_t  fromLen = sizeof(from);
      rc = recvfrom(this->_fd, buf, len, flags, (struct sockaddr *)&from,
                    &fromLen);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::RecvFrom(void *buf, size_t len, int flags,
                           Ipv4Address & srcAddr, uint16_t & srcPort)
  {
    struct sockaddr_in  fromAddr;
    ssize_t  rc = RecvFrom(buf, len, flags, fromAddr);
    if (rc >= 0) {
      srcAddr = fromAddr.sin_addr.s_addr;
      srcPort = ntohs(fromAddr.sin_port);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::RecvFrom(void *buf, size_t len, int flags,
                           Ipv6Address & srcAddr, uint16_t & srcPort)
  {
    struct sockaddr_in6  fromAddr;
    ssize_t  rc = RecvFrom(buf, len, flags, fromAddr);
    if (rc >= 0) {
      srcAddr = fromAddr.sin6_addr;
      srcPort = ntohs(fromAddr.sin6_port);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::RecvFrom(std::string & s, int flags,
                           Ipv4Address & srcAddr, uint16_t & srcPort)
  {
    s.clear();
    char     buf[IP_MAXPACKET];
    ssize_t  rc = RecvFrom(buf, IP_MAXPACKET, flags, srcAddr, srcPort);
    if (rc > 0) {
      s.assign(buf, rc);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::RecvFrom(std::string & s, int flags,
                           Ipv6Address & srcAddr, uint16_t & srcPort)
  {
    s.clear();
    char     buf[IP_MAXPACKET];
    ssize_t  rc = RecvFrom(buf, IP_MAXPACKET, flags, srcAddr, srcPort);
    if (rc > 0) {
      s.assign(buf, rc);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::SendTo(const void *buf, size_t len, int flags,
                         const struct sockaddr_in & to)
  {
    ssize_t  rc = -1;
    if (this->_fd >= 0) {
#if (defined(__FreeBSD__) && (__FreeBSD_version < 1100000)) || defined(__APPLE__)
      if (_headerIncluded && (_type == SOCK_RAW)) {
        Ipv4PacketHeader  ipHeader((struct ip *)buf);
        ipHeader.TotalLength(ntohs(ipHeader.TotalLength()));
        ipHeader.FragmentOffset(ntohs(ipHeader.FragmentOffset()));
      }
#endif
      socklen_t  toLen = sizeof(to);
      rc = sendto(this->_fd, buf, len, flags, (struct sockaddr *)&to, toLen);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::SendTo(const void *buf, size_t len, int flags,
                         const struct sockaddr_in6 & to)
  {
    ssize_t  rc = -1;
    if (this->_fd >= 0) {
      socklen_t  toLen = sizeof(to);
      rc = sendto(this->_fd, buf, len, flags, (struct sockaddr *)&to, toLen);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::SendTo(const void *buf, size_t len, int flags,
                         const Ipv4Address & dstAddr, uint16_t dstPort)
  {
    sockaddr_in  addr;
    memset(&addr, 0, sizeof(addr));
#ifndef __linux__
    addr.sin_len = sizeof(addr);
#endif
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = dstAddr.Raw();
    addr.sin_port = htons(dstPort);
    return SendTo(buf, len, flags, addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::SendTo(const void *buf, size_t len, int flags,
                         const Ipv6Address & dstAddr, uint16_t dstPort)
  {
    sockaddr_in6  addr;
    memset(&addr, 0, sizeof(addr));
#ifndef __linux__
    addr.sin6_len = sizeof(addr);
#endif
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = dstAddr;
    addr.sin6_port = htons(dstPort);
    return SendTo(buf, len, flags, addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::SendTo(const std::string & s, int flags,
                         const Ipv4Address & dstAddr, uint16_t dstPort)
  {
    return SendTo(s.data(), s.size(), flags, dstAddr, dstPort);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Socket::SendTo(const std::string & s, int flags,
                         const Ipv6Address & dstAddr, uint16_t dstPort)
  {
    return SendTo(s.data(), s.size(), flags, dstAddr, dstPort);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Bind(const struct sockaddr_un & addr)
  {
    bool       rc = false;
    socklen_t  sockLen = strlen(addr.sun_path) + sizeof(addr.sun_family) + 1;
    if (bind(this->_fd, (const struct sockaddr *)&addr, sockLen) == 0)
      rc = true;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Connect(const struct sockaddr_in & name)
  {
    bool  rc = false;
    if (connect(this->_fd, (const struct sockaddr *)&name, sizeof(name)) == 0)
      rc = true;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Connect(const struct sockaddr_in6 & name)
  {
    bool  rc = false;
    if (connect(this->_fd, (const struct sockaddr *)&name, sizeof(name)) == 0)
      rc = true;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Connect(const Ipv4Address & dstAddr, uint16_t port)
  {
    struct sockaddr_in  addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = dstAddr.Raw();
    addr.sin_port = htons(port);
    return Connect(addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Connect(const Ipv6Address & dstAddr, uint16_t port)
  {
    struct sockaddr_in6  addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = dstAddr;
    addr.sin6_port = htons(port);
    return Connect(addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Connect(const struct sockaddr_un & name)
  {
    bool  rc = false;
    if (connect(this->_fd, (const struct sockaddr *)&name, sizeof(name)) == 0)
      rc = true;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getpeername(struct sockaddr * name, socklen_t & namelen)
  {
    bool  rc = false;
    if (getpeername(this->_fd, name, &namelen) == 0)
      rc = true;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getpeername(Ipv4Address & addr, uint16_t & port)
  {
    bool  rc = false;
    struct sockaddr_in  sockAddr;
    socklen_t           sockLen = sizeof(sockAddr);
    if (Getpeername((struct sockaddr *)&sockAddr, sockLen)) {
      if (sockAddr.sin_family == AF_INET) {
        addr = Ipv4Address(sockAddr.sin_addr.s_addr);
        port = ntohs(sockAddr.sin_port);
        rc = true;
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getpeername(Ipv6Address & addr, uint16_t & port)
  {
    bool  rc = false;
    struct sockaddr_in6  sockAddr;
    socklen_t            sockLen = sizeof(sockAddr);
    if (Getpeername((struct sockaddr *)&sockAddr, sockLen)) {
      if (sockAddr.sin6_family == AF_INET6) {
        addr = Ipv6Address(sockAddr.sin6_addr);
        port = ntohs(sockAddr.sin6_port);
        rc = true;
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getsockname(struct sockaddr * name, socklen_t & namelen)
  {
    bool  rc = false;
    if (getsockname(this->_fd, name, &namelen) == 0)
      rc = true;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getsockname(Ipv4Address & addr, uint16_t & port)
  {
    bool  rc = false;
    struct sockaddr_in  sockAddr;
    socklen_t           sockLen = sizeof(sockAddr);
    if (Getsockname((struct sockaddr *)&sockAddr, sockLen)) {
      if (sockAddr.sin_family == AF_INET) {
        addr = Ipv4Address(sockAddr.sin_addr.s_addr);
        port = ntohs(sockAddr.sin_port);
        rc = true;
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getsockname(Ipv6Address & addr, uint16_t & port)
  {
    bool  rc = false;
    struct sockaddr_in6  sockAddr;
    socklen_t            sockLen = sizeof(sockAddr);
    if (Getsockname((struct sockaddr *)&sockAddr, sockLen)) {
      if (sockAddr.sin6_family == AF_INET6) {
        addr = Ipv6Address(sockAddr.sin6_addr);
        port = ntohs(sockAddr.sin6_port);
        rc = true;
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Setsockopt(int level, int optname, const void * optval,
                          socklen_t optlen)
  {
    bool  rc = false;
    if (setsockopt(this->_fd, level, optname, optval, optlen) == 0) {
      rc = true;
      if (level == IPPROTO_IP && optname == IP_HDRINCL) {
        _headerIncluded = *(int *)optval ? true : false;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::Getsockopt(int level, int optname, void * optval,
                          socklen_t & optlen)
  {
    bool  rc = false;
    if (getsockopt(this->_fd, level, optname, optval, &optlen) == 0)
      rc = true;
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::JoinMulticastGroup(const Ipv4Address & groupAddr,
                                  const Ipv4Address & intfAddr)
  {
    bool  rc = false;
    struct ip_mreq  mreq;
    mreq.imr_multiaddr.s_addr = groupAddr.Raw();
    mreq.imr_interface.s_addr = intfAddr.Raw();
    return Setsockopt(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::JoinMulticastGroup(const Ipv6Address & groupAddr,
                                  unsigned int ifIndex)
  {
    bool  rc = false;
    struct ipv6_mreq  mreq;
    mreq.ipv6mr_multiaddr = groupAddr;
    mreq.ipv6mr_interface = ifIndex;
    return Setsockopt(IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::JoinMulticastGroup(const Ipv6Address & groupAddr,
                                  const std::string & ifName)
  {
    bool  rc = false;
    if (! ifName.empty()) {
      unsigned int  ifIndex = if_nametoindex(ifName.c_str());
      if (ifIndex > 0) {
        rc = JoinMulticastGroup(groupAddr, ifIndex);
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Socket::HeaderIncluded() const
  {
    return(_headerIncluded);
  }

  
}  // namespace Dwm
