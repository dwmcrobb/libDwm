//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmSocket.cc 9892 $
// @(#) $Id: TestDwmSocket.cc 9892 2017-12-26 08:16:56Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016
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
//!  \file TestDwmSocket.cc
//!  \brief Unit tests for Dwm::Socket
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
}

#include <atomic>
#include <thread>

#include "DwmSocket.hh"
#include "DwmSvnTag.hh"
#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmSocket.cc 9892 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void UdpClientThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET, SOCK_DGRAM, 0))) {
    if (UnitAssert(s.SendTo("Hello", 0, Ipv4Address("127.0.0.1"), 2190))) {
      string  goodbye("Goodbye"), sr;
      Ipv4Address  srcAddr;
      uint16_t     srcPort;
      if (UnitAssert(s.RecvFrom(sr, 0, srcAddr, srcPort) == goodbye.size())) {
        UnitAssert(sr == goodbye);
        UnitAssert(srcAddr == Ipv4Address("127.0.0.1"));
        UnitAssert(srcPort == 2190);
      }
    }
    UnitAssert(s.Close());
  }
  return;
}

atomic<bool>  g_udpServerReady(false);

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void UdpServerThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET, SOCK_DGRAM, 0))) {
    if (UnitAssert(s.Bind(Ipv4Address("127.0.0.1"), 2190))) {
      g_udpServerReady = true;
      string  hello("Hello"), sr;
      Ipv4Address  srcAddr;
      uint16_t     srcPort;
      ssize_t      bytesReceived = s.RecvFrom(sr, 0, srcAddr, srcPort);
      if (UnitAssert(bytesReceived == hello.size())) {
        UnitAssert(sr == hello);
        UnitAssert(srcAddr == Ipv4Address("127.0.0.1"));
        string  goodbye("Goodbye");
        UnitAssert(s.SendTo(goodbye, 0, srcAddr, srcPort) == goodbye.size());
      }
    }
    UnitAssert(s.Close());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Udp6ClientThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET6, SOCK_DGRAM, 0))) {
    if (UnitAssert(s.SendTo("Hello", 0, Ipv6Address("::1"), 2190))) {
      string  goodbye("Goodbye"), sr;
      Ipv6Address  srcAddr;
      uint16_t     srcPort;
      if (UnitAssert(s.RecvFrom(sr, 0, srcAddr, srcPort) == goodbye.size())) {
        UnitAssert(sr == goodbye);
        UnitAssert(srcAddr == Ipv6Address("::1"));
        UnitAssert(srcPort == 2190);
      }
    }
    UnitAssert(s.Close());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Udp6ServerThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET6, SOCK_DGRAM, 0))) {
    if (UnitAssert(s.Bind(Ipv6Address("::1"), 2190))) {
      g_udpServerReady = true;
      string  hello("Hello"), sr;
      Ipv6Address  srcAddr;
      uint16_t     srcPort;
      ssize_t      bytesReceived = s.RecvFrom(sr, 0, srcAddr, srcPort);
      if (UnitAssert(bytesReceived == hello.size())) {
        UnitAssert(sr == hello);
        UnitAssert(srcAddr == Ipv6Address("::1"));
        string  goodbye("Goodbye");
        UnitAssert(s.SendTo(goodbye, 0, srcAddr, srcPort) == goodbye.size());
      }
    }
    UnitAssert(s.Close());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestUdpClientServer()
{
  g_udpServerReady = false;
  thread  serverThread(UdpServerThread);
  while (! g_udpServerReady) { }
  thread  clientThread(UdpClientThread);
  clientThread.join();
  serverThread.join();

  g_udpServerReady = false;
  thread  server6Thread(Udp6ServerThread);
  while(! g_udpServerReady) { }
  thread  client6Thread(Udp6ClientThread);
  client6Thread.join();
  server6Thread.join();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TcpClientThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET, SOCK_STREAM, 0))) {
    if (UnitAssert(s.Connect(Ipv4Address("127.0.0.1"), 2190))) {
      int  on = 1;
      UnitAssert(s.Setsockopt(IPPROTO_TCP, TCP_NODELAY,
                              &on, sizeof(on)));
      UnitAssert(write(s, "Hello", strlen("Hello")) == strlen("Hello"));
      char  buf[65535] = { 0 };
      UnitAssert(read(s, buf, sizeof(buf)) == strlen("Goodbye"));
    }
    UnitAssert(s.Close());
  }
  return;
}

static atomic<bool>  g_tcpServerReady(false);

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TcpServerThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET, SOCK_STREAM, 0))) {
    int  on = 1;
    UnitAssert(s.Setsockopt(SOL_SOCKET, SO_REUSEADDR,
                            &on, sizeof(on)));
    if (UnitAssert(s.Bind(Ipv4Address("127.0.0.1"), 2190))) {
      if (UnitAssert(s.Listen(10))) {
        g_tcpServerReady = true;
        Ipv4Address  peerAddr;
        uint16_t     peerPort;
        Socket  sa = s.Accept(peerAddr, peerPort);
        if (UnitAssert((int)sa >= 0)) {
          Ipv4Address  myAddr;
          uint16_t     myPort;
          UnitAssert(sa.Getsockname(myAddr, myPort));
          UnitAssert(myAddr == Ipv4Address("127.0.0.1"));
          UnitAssert(myPort == 2190);
          UnitAssert(peerAddr == Ipv4Address("127.0.0.1"));
          UnitAssert(peerPort > 1024);
          Ipv4Address  peerAddr2;
          uint16_t     peerPort2;
          UnitAssert(sa.Getpeername(peerAddr2, peerPort2));
          UnitAssert(peerAddr2 == peerAddr);
          UnitAssert(peerPort2 == peerPort);
          int  noDelay = 1;
          UnitAssert(sa.Setsockopt(IPPROTO_TCP, TCP_NODELAY,
                                   &noDelay, sizeof(noDelay)));
          char  buf[65535] = { 0 };
          UnitAssert(read(sa, buf, sizeof(buf)) == strlen("Hello"));
          UnitAssert(write(sa, "Goodbye", strlen("Goodbye"))
                     == strlen("Goodbye"));
          UnitAssert(sa.Close());
        }
      }
    }
    UnitAssert(s.Close());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Tcp6ClientThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET6, SOCK_STREAM, 0))) {
    if (UnitAssert(s.Connect(Ipv6Address("::1"), 2190))) {
      int  on = 1;
      UnitAssert(s.Setsockopt(IPPROTO_TCP, TCP_NODELAY,
                              &on, sizeof(on)));
      UnitAssert(write(s, "Hello", strlen("Hello")) == strlen("Hello"));
      char  buf[65535] = { 0 };
      UnitAssert(read(s, buf, sizeof(buf)) == strlen("Goodbye"));
    }
    UnitAssert(s.Close());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Tcp6ServerThread()
{
  Socket  s;
  if (UnitAssert(s.Open(PF_INET6, SOCK_STREAM, 0))) {
    int  on = 1;
    UnitAssert(s.Setsockopt(SOL_SOCKET, SO_REUSEADDR,
                            &on, sizeof(on)));
    if (UnitAssert(s.Bind(Ipv6Address("::1"), 2190))) {
      if (UnitAssert(s.Listen(10))) {
        g_tcpServerReady = true;
        Ipv6Address  peerAddr;
        uint16_t     peerPort;
        Socket  sa = s.Accept(peerAddr, peerPort);
        if (UnitAssert((int)sa >= 0)) {
          Ipv6Address  myAddr;
          uint16_t     myPort;
          UnitAssert(sa.Getsockname(myAddr, myPort));
          UnitAssert(myAddr == Ipv6Address("::1"));
          UnitAssert(myPort == 2190);
          UnitAssert(peerAddr == Ipv6Address("::1"));
          UnitAssert(peerPort > 1024);
          Ipv6Address  peerAddr2;
          uint16_t     peerPort2;
          UnitAssert(sa.Getpeername(peerAddr2, peerPort2));
          UnitAssert(peerAddr2 == peerAddr);
          UnitAssert(peerPort2 == peerPort);
          int  noDelay = 1;
          UnitAssert(sa.Setsockopt(IPPROTO_TCP, TCP_NODELAY,
                                   &noDelay, sizeof(noDelay)));
          char  buf[65535] = { 0 };
          UnitAssert(read(sa, buf, sizeof(buf)) == strlen("Hello"));
          UnitAssert(write(sa, "Goodbye", strlen("Goodbye"))
                     == strlen("Goodbye"));
          UnitAssert(sa.Close());
        }
      }
    }
    UnitAssert(s.Close());
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestTcpClientServer()
{
  g_tcpServerReady = false;
  thread  serverThread(TcpServerThread);
  while (! g_tcpServerReady) { }
  thread  clientThread(TcpClientThread);
  clientThread.join();
  serverThread.join();

  g_tcpServerReady = false;
  thread  server6Thread(Tcp6ServerThread);
  while(! g_tcpServerReady) { }
  thread  client6Thread(Tcp6ClientThread);
  client6Thread.join();
  server6Thread.join();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void MulticastClient(const string & sendStr,
                            const string & expectedString)
{
  Socket  recvSock;
  Ipv4Address  bindAddr(INADDR_ANY);
  Ipv4Address  mcastAddr("224.0.168.42");
  Ipv4Address  intfAddr(INADDR_ANY);
  
  if (UnitAssert(recvSock.Open(PF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
    int on = 1;
    recvSock.Setsockopt(SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if (UnitAssert(recvSock.Bind(bindAddr, 2191))) {
      if (UnitAssert(recvSock.JoinMulticastGroup(mcastAddr, intfAddr))) {
        Socket  sendSock;
        if (UnitAssert(sendSock.Open(PF_INET, SOCK_DGRAM, 0))) {
          int   tries = 0;
          bool  otherSeen = false;
          while (tries < 20) {
            if (UnitAssert(sendSock.SendTo(sendStr, 0, mcastAddr, 2191))) {
              string       rs;
              Ipv4Address  srcAddr;
              uint16_t     srcPort;
              if (! recvSock.ReadWouldBlock()) {
                UnitAssert(recvSock.RecvFrom(rs, 0, srcAddr, srcPort) > 0);
                if (rs == expectedString) {
                  otherSeen = true;
                  break;
                }
                ++tries;
              }
            }
          }
          UnitAssert(otherSeen);
          sendSock.Close();
        }
      }
    }
  }
  recvSock.Close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void Multicast6Client(const string & sendStr,
                            const string & expectedString)
{
  Socket  recvSock;
  struct in6_addr  in6Addr = IN6ADDR_ANY_INIT;
  Ipv6Address  bindAddr(in6Addr);
  Ipv6Address  mcastAddr("ff15::1");
  string       intfName("lo0");
  
  if (UnitAssert(recvSock.Open(PF_INET6, SOCK_DGRAM, IPPROTO_UDP))) {
    int on = 1;
    recvSock.Setsockopt(SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    if (UnitAssert(recvSock.Bind(bindAddr, 2191))) {
      if (UnitAssert(recvSock.JoinMulticastGroup(mcastAddr, intfName))) {
        Socket  sendSock;
        if (UnitAssert(sendSock.Open(PF_INET6, SOCK_DGRAM, 0))) {
          int   tries = 0;
          bool  otherSeen = false;
          while (tries < 20) {
            if (UnitAssert(sendSock.SendTo(sendStr, 0, mcastAddr, 2191))) {
              string       rs;
              Ipv6Address  srcAddr;
              uint16_t     srcPort;
              if (! recvSock.ReadWouldBlock()) {
                UnitAssert(recvSock.RecvFrom(rs, 0, srcAddr, srcPort) > 0);
                if (rs == expectedString) {
                  otherSeen = true;
                  break;
                }
                ++tries;
              }
            }
          }
          UnitAssert(otherSeen);
          sendSock.Close();
        }
      }
    }
  }
  recvSock.Close();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestMulticast()
{
  thread  client1(MulticastClient, "sent by first client",
                  "sent by second client");
  thread  client2(MulticastClient, "sent by second client",
                  "sent by first client");
  client1.join();
  client2.join();

  thread  client1_6(Multicast6Client, "sent by first client",
                  "sent by second client");
  thread  client2_6(Multicast6Client, "sent by second client",
                    "sent by first client");
  client1_6.join();
  client2_6.join();
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  //  SysLogger::Open("TestDwmSocket", LOG_PERROR, LOG_USER);
  
  TestUdpClientServer();
  TestTcpClientServer();
  //  TestMulticast();
  
  int rc = 0;
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    rc = 0;
  }
  return rc;  
}
