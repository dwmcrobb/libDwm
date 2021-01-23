//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007
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
//!  \file DwmSocket.hh
//!  \brief Dwm::Socket class definition
//---------------------------------------------------------------------------

#ifndef _DWMSOCKET_HH_
#define _DWMSOCKET_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/un.h>
  #include <netinet/in.h>
}

#include <string>

#include "DwmDescriptor.hh"
#include "DwmIpv4Address.hh"
#include "DwmIpv6Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class abstracts a UNIX socket descriptor.
  //--------------------------------------------------------------------------
  class Socket
    : public Descriptor
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Socket();
    
    //------------------------------------------------------------------------
    //!  Copy constructor.  Note we use dup() to duplicate the underlying
    //!  UNIX socket descriptor, which allows us to close the underlying
    //!  UNIX socket descriptor in our destructor.
    //------------------------------------------------------------------------
    Socket(const Socket & sock);

    //------------------------------------------------------------------------
    //!  operator = (assignment operator).
    //------------------------------------------------------------------------
    Socket & operator = (const Socket & sock);
    
    operator bool () const;
    
    //------------------------------------------------------------------------
    //!  Opens the socket.  Note this just calls the UNIX socket(2)
    //!  function, and takes the same arguments.  Returns true on success,
    //!  false on failure.
    //------------------------------------------------------------------------
    bool Open(int domain, int type, int protocol);

    int Type() const;
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX accept(2) function.  Returns the resulting socket.
    //------------------------------------------------------------------------
    Socket Accept(struct sockaddr_in & addr);

    //------------------------------------------------------------------------
    //!  Calls the UNIX accept(2) function.  Returns the resulting socket.
    //------------------------------------------------------------------------
    Socket Accept(struct sockaddr_in6 & addr);
    
    //------------------------------------------------------------------------
    //!  Calls accept() on the socket and returns the resulting socket.
    //!  If successful, @c addr will be set to the peer's address and @c port
    //!  will be set to the peer's port.
    //------------------------------------------------------------------------
    Socket Accept(Ipv4Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls accept() on the socket and returns the resulting socket.
    //!  If successful, @c addr will be set to the peer's address and @c port
    //!  will be set to the peer's port.
    //------------------------------------------------------------------------
    Socket Accept(Ipv6Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls the UNIX accept() function for the given socket @c s and
    //!  attaches *this to the return if successful.  Returns true on
    //!  success, false on failure.  @c addr is a result parameter and
    //!  will contain the address of the remote connection on success.
    //------------------------------------------------------------------------
    bool Accept(Socket & s, struct sockaddr_in & addr);

    //------------------------------------------------------------------------
    //!  Calls the UNIX accept() function for the given socket @c s and
    //!  attaches *this to the return if successful.  Returns true on
    //!  success, false on failure.  @c addr is a result parameter and
    //!  will contain the address of the remote connection on success.
    //------------------------------------------------------------------------
    bool Accept(Socket & s, struct sockaddr_in6 & addr);

    //------------------------------------------------------------------------
    //!  Calls the UNIX accept() function for the given socket @c s and
    //!  attaches *this to the return if successful.  Returns true on
    //!  success, false on failure.  @c addr and @c port are result parameters
    //!  and will contain the address and port of the remote connection on
    //!  success.
    //------------------------------------------------------------------------
    bool Accept(Socket & s, Ipv4Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls the UNIX accept() function for the given socket @c s and
    //!  attaches *this to the return if successful.  Returns true on
    //!  success, false on failure.  @c addr and @c port are result parameters
    //!  and will contain the address and port of the remote connection on
    //!  success.
    //------------------------------------------------------------------------
    bool Accept(Socket & s, Ipv6Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls the UNIX accept(2) function.  Returns the resulting socket.
    //------------------------------------------------------------------------
    Socket Accept(struct sockaddr_un & addr);

    bool Accept(Socket & s, struct sockaddr_un & addr);

    //------------------------------------------------------------------------
    //!  Calls the UNIX listen(2) function.  Returns true on success,
    //!  false on failure.
    //------------------------------------------------------------------------
    bool Listen(int backlog);
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX bind(2) function.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Bind(const struct sockaddr_in & addr);

    //------------------------------------------------------------------------
    //!  Calls the UNIX bind(2) function.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Bind(const struct sockaddr_in6 & addr);

    //------------------------------------------------------------------------
    //!  Bind to the given IPv4 address @c addr and @c port.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    bool Bind(const Ipv4Address & addr, uint16_t port);

    //------------------------------------------------------------------------
    //!  Bind to the given IPv6 address @c addr and @c port.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    bool Bind(const Ipv6Address & addr, uint16_t port);

    //------------------------------------------------------------------------
    //!  Calls the UNIX bind(2) function.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Bind(const struct sockaddr_un & addr);
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX connect(2) function.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Connect(const struct sockaddr_in & name);

    //------------------------------------------------------------------------
    //!  Calls the UNIX connect(2) function.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Connect(const struct sockaddr_in6 & name);

    //------------------------------------------------------------------------
    //!  Connect to the given IPv4 address @c dstAddr at the given @c port.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool Connect(const Ipv4Address & dstAddr, uint16_t port);
    
    //------------------------------------------------------------------------
    //!  Connect to the given IPv6 address @c dstAddr at the given @c port.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool Connect(const Ipv6Address & dstAddr, uint16_t port);

    //------------------------------------------------------------------------
    //!  Calls the UNIX connect(2) function.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    bool Connect(const struct sockaddr_un & name);
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX getpeername(2) function.  Returns true on success,
    //  false on failure.
    //------------------------------------------------------------------------
    bool Getpeername(struct sockaddr * name, socklen_t & namelen);

    //------------------------------------------------------------------------
    //!  Calls getpeername() on the socket.  Returns true on success, false
    //!  on failure.  @c addr and @c port are result parameters, and will
    //!  contain the address and port of the peer on success.
    //------------------------------------------------------------------------
    bool Getpeername(Ipv4Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls getpeername() on the socket.  Returns true on success, false
    //!  on failure.  @c addr and @c port are result parameters, and will
    //!  contain the address and port of the peer on success.
    //------------------------------------------------------------------------
    bool Getpeername(Ipv6Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls the UNIX getsockname(2) function.  Returns true on success,
    //  false on failure.
    //------------------------------------------------------------------------
    bool Getsockname(struct sockaddr * name, socklen_t & namelen);

    //------------------------------------------------------------------------
    //!  Calls getsockname() on the socket.  Returns true on success, false
    //!  on failure.  @c addr and @c port are result parameters and will
    //!  contain the address and port of the socket on success.  Note this
    //!  only works on IPv4 sockets, not UNIX domain sockets.
    //------------------------------------------------------------------------
    bool Getsockname(Ipv4Address & addr, uint16_t & port);

    //------------------------------------------------------------------------
    //!  Calls getsockname() on the socket.  Returns true on success, false
    //!  on failure.  @c addr and @c port are result parameters and will
    //!  contain the address and port of the socket on success.  Note this
    //!  only works on IPv6 sockets.
    //------------------------------------------------------------------------
    bool Getsockname(Ipv6Address & addr, uint16_t & port);
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX setsockopt(2) function.  Returns true on success,
    //  false on failure.
    //------------------------------------------------------------------------
    bool Setsockopt(int level, int optname, const void * optval,
                    socklen_t optlen);

    //------------------------------------------------------------------------
    //!  Calls the UNIX setsockopt(2) function.  Returns true on success,
    //  false on failure.
    //------------------------------------------------------------------------
    bool Getsockopt(int level, int optname, void * optval, socklen_t & optlen);
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX recvfrom(2) function.
    //------------------------------------------------------------------------
    ssize_t RecvFrom(void *buf, size_t len, int flags,
                     struct sockaddr_in & from);

    //------------------------------------------------------------------------
    //!  Calls the UNIX recvfrom(2) function.
    //------------------------------------------------------------------------
    ssize_t RecvFrom(void *buf, size_t len, int flags,
                     struct sockaddr_in6 & from);

    //------------------------------------------------------------------------
    //!  Calls recvfrom() on the socket.  A buffer @c buf of length @c len
    //!  must be provided by the caller to receive the data.  @c flags is
    //!  passed to the recvfrom() function (see the recvfrom manpage).  On
    //!  success, returns the number of bytes received and sets @c srcAddr
    //!  and @c srcPort to the source address and port of the received packet.
    //------------------------------------------------------------------------
    ssize_t RecvFrom(void *buf, size_t len, int flags,
                     Ipv4Address & srcAddr, uint16_t & srcPort);

    //------------------------------------------------------------------------
    //!  Calls recvfrom() on the socket.  A buffer @c buf of length @c len
    //!  must be provided by the caller to receive the data.  @c flags is
    //!  passed to the recvfrom() function (see the recvfrom manpage).  On
    //!  success, returns the number of bytes received and sets @c srcAddr
    //!  and @c srcPort to the source address and port of the received packet.
    //------------------------------------------------------------------------
    ssize_t RecvFrom(void *buf, size_t len, int flags,
                     Ipv6Address & srcAddr, uint16_t & srcPort);

    //------------------------------------------------------------------------
    //!  Calls recvfrom() on the socket.  On success, stores the received
    //!  data in @c s, the source address in @c srcAddr and the source port
    //!  in @c srcPort and returns the number of bytes received.  On failure,
    //!  returns -1.  Note that @c may contain embedded nulls and should NOT
    //!  be treated like a C string; use s.size() to determine how much data
    //!  was stored.
    //------------------------------------------------------------------------
    ssize_t RecvFrom(std::string & s, int flags,
                     Ipv4Address & srcAddr, uint16_t & srcPort);

    //------------------------------------------------------------------------
    //!  Calls recvfrom() on the socket.  On success, stores the received
    //!  data in @c s, the source address in @c srcAddr and the source port
    //!  in @c srcPort and returns the number of bytes received.  On failure,
    //!  returns -1.  Note that @c may contain embedded nulls and should NOT
    //!  be treated like a C string; use s.size() to determine how much data
    //!  was stored.
    //------------------------------------------------------------------------
    ssize_t RecvFrom(std::string & s, int flags,
                     Ipv6Address & srcAddr, uint16_t & srcPort);
    
    //------------------------------------------------------------------------
    //!  Calls the UNIX sendto(2) function.
    //------------------------------------------------------------------------
    ssize_t SendTo(const void *buf, size_t len, int flags,
                   const struct sockaddr_in & to);

    //------------------------------------------------------------------------
    //!  Calls the UNIX sendto(2) function.
    //------------------------------------------------------------------------
    ssize_t SendTo(const void *buf, size_t len, int flags,
                   const struct sockaddr_in6 & to);

    //------------------------------------------------------------------------
    //!  Calls sendto() on the socket, sending the given buffer @c buf of
    //!  length @c len to the given destination address and port.  Returns
    //!  the number of bytes sent on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t SendTo(const void *buf, size_t len, int flags,
                   const Ipv4Address & dstAddr, uint16_t dstPort);

    //------------------------------------------------------------------------
    //!  Calls sendto() on the socket, sending the given buffer @c buf of
    //!  length @c len to the given destination address and port.  Returns
    //!  the number of bytes sent on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t SendTo(const void *buf, size_t len, int flags,
                   const Ipv6Address & dstAddr, uint16_t dstPort);

    //------------------------------------------------------------------------
    //!  Calls sendto() on the socket, sending the given string @c s to the
    //!  given destination and port.  @c flags is passed on to sendto() (see
    //!  the sendto() manpage).  On success, returns the number of bytes sent
    //!  which should be the same as @c s.size().  On failure, returns -1.
    //!  Note that @c s can contain binary data, including nulls, but can not
    //!  be longer than 65535 bytes (the maximum size of an IP packet) minus
    //!  the length of the transport header and any IP header options.
    //------------------------------------------------------------------------
    ssize_t SendTo(const std::string & s, int flags,
                   const Ipv4Address & dstAddr, uint16_t dstPort);

    //------------------------------------------------------------------------
    //!  Calls sendto() on the socket, sending the given string @c s to the
    //!  given destination and port.  @c flags is passed on to sendto() (see
    //!  the sendto() manpage).  On success, returns the number of bytes sent
    //!  which should be the same as @c s.size().  On failure, returns -1.
    //!  Note that @c s can contain binary data, including nulls, but can not
    //!  be longer than 65535 bytes (the maximum size of an IP packet) minus
    //!  the length of the transport header and any IP header options.
    //------------------------------------------------------------------------
    ssize_t SendTo(const std::string & s, int flags,
                   const Ipv6Address & dstAddr, uint16_t dstPort);
    
    //------------------------------------------------------------------------
    //!  Join the given multicast group @c groupAddr on the given interface
    //!  address @c intfAddr.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool JoinMulticastGroup(const Ipv4Address & groupAddr,
                            const Ipv4Address & intfAddr);
    
    //------------------------------------------------------------------------
    //!  Join the given multicast group @c groupAddr on the given interface
    //!  @c ifIndex.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool JoinMulticastGroup(const Ipv6Address & groupAddr,
                            unsigned int ifIndex);

    //------------------------------------------------------------------------
    //!  Join the given multicast group @c groupAddr on the interface with
    //!  the given name @c ifName.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool JoinMulticastGroup(const Ipv6Address & groupAddr,
                            const std::string & ifName);

    bool HeaderIncluded() const;
    
  protected:
    int   _type;
    bool  _headerIncluded;
  };
  
}  // namespace Dwm


#endif  // _DWMSOCKET_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
