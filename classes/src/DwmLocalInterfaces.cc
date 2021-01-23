//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2020
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
//!  \file DwmLocalInterfaces.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::LocalInterfaces class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <net/if.h>
  #include <errno.h>
  #include <ifaddrs.h>
  #include <unistd.h>

#if (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__))
    #include <ifaddrs.h>
    #include <net/if_dl.h>
    #include <net/ethernet.h>
  #else
    #ifdef __linux__
      #include <linux/sockios.h>
    #endif
    #ifdef __APPLE_CC__
      #include <sys/sockio.h>
    #endif
  #endif
  #include <sys/ioctl.h>

  #ifndef OSIOCGIFCONF
    #define OSIOCGIFCONF SIOCGIFCONF
  #endif
}

#include <cstring>
#include <iomanip>
#include <string>

#include "DwmLocalInterfaces.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  LocalInterfaceAlias::LocalInterfaceAlias()
      : _addr(0), _mask("255.255.255.255")
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  LocalInterfaceAlias::LocalInterfaceAlias(const Ipv4Address & address,
                                           const Ipv4Address & mask)
      : _addr(address), _mask(mask)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4Address & LocalInterfaceAlias::Addr() const
  {
    return(_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4Address & LocalInterfaceAlias::Addr(const Ipv4Address & addr)
  {
    _addr = addr;
    return(_addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4Address & LocalInterfaceAlias::Mask() const
  {
    return(_mask);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const Ipv4Address & LocalInterfaceAlias::Mask(const Ipv4Address & mask)
  {
    _mask = mask;
    return(_mask);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool
  LocalInterfaceAlias::operator < (const LocalInterfaceAlias & alias) const
  {
    bool  rc = false;
    
    if (_addr < alias._addr) {
      rc = true;
    }
    else {
      if (_addr == alias._addr) {
        if (_mask < alias._mask) {
          rc = true;
        }
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool
  LocalInterfaceAlias::operator == (const LocalInterfaceAlias & alias) const
  {
    return((_addr == alias._addr) && (_mask == alias._mask));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & operator << (ostream & os, const LocalInterfaceAlias & alias)
  {
    if (os) {
      os << "addr " << setiosflags(ios::left) << setw(16) << alias._addr
         << "mask " << setw(16) << alias._mask
         << resetiosflags(ios::left);
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  LocalInterface::LocalInterface()
      : _name(), _addr(), _mask(), _mtu(), _aliases()
  {}
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const std::string & LocalInterface::Name() const
  {
    return(_name);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const std::string & LocalInterface::Name(const std::string & name)
  {
    _name = name;
    return(_name);
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const Ipv4Address & LocalInterface::Addr() const
  {
    return(_addr);
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const Ipv4Address & LocalInterface::Addr(const Ipv4Address & addr)
  {
    _addr = addr;
    return(_addr);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const Ipv4Address & LocalInterface::Mask() const
  {
    return(_mask);
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  const Ipv4Address & LocalInterface::Mask(const Ipv4Address & mask)
  {
    _mask = mask;
    return(_mask);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int LocalInterface::Mtu() const
  {
    return(_mtu);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int LocalInterface::Mtu(int mtu)
  {
    _mtu = mtu;
    return(_mtu);
  }
  
#if (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__APPLE__))
        
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool LocalInterface::IsUp() const
  {
    bool  rc = false;
        
    struct ifaddrs  *ifAddrs;
    if (getifaddrs(&ifAddrs) == 0) {
      struct ifaddrs  *ifAddr = ifAddrs;
      while (ifAddr) {
        if (ifAddr->ifa_addr) {
          if (ifAddr->ifa_addr->sa_family == AF_INET) {
            if (_name == std::string(ifAddr->ifa_name)) {
              if (ifAddr->ifa_flags & IFF_UP) {
                rc = true;
              }
              break;
            }
          }
        }
        ifAddr = ifAddr->ifa_next;
      }
      freeifaddrs(ifAddrs);
    }

    return(rc);
  }
  

#else  // linux?
        
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool LocalInterface::IsUp() const
  {
    bool  rc = false;
        
    int  sockfd = socket(PF_INET,SOCK_DGRAM,0);
    if (sockfd >= 0) {
      struct ifconf  ifc;
      struct ifreq  *ifr;
      ifc.ifc_buf = 0;
      int    numreqs = 30;
      for (;;) {
        ifc.ifc_len = sizeof(struct ifreq) * numreqs;
        ifc.ifc_buf = (char *)realloc(ifc.ifc_buf, ifc.ifc_len);
        memset(ifc.ifc_buf, 0, ifc.ifc_len);
        if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
          break;
        if ((uint32_t)ifc.ifc_len == sizeof(struct ifreq) * numreqs) {
          //  assume it overflowed and try again
          numreqs += 10;
          continue;
        }
        break;
      }
      ifr = ifc.ifc_req;
      for (int n = 0; n < ifc.ifc_len; n += sizeof(struct ifreq)) {
        if (_name == std::string(ifr->ifr_name)) {
          if (ioctl(sockfd, SIOCGIFFLAGS, ifr) >= 0) {
            if (ifr->ifr_flags & IFF_UP) {
              rc = true;
            }
          }
          break;
        }
#ifdef _SIZEOF_ADDR_IFREQ
        ifr = (struct ifreq *)((char *)ifr + _SIZEOF_ADDR_IFREQ(*ifr));
#else
		++ifr;
#endif
      }
      if (ifc.ifc_buf)
        free(ifc.ifc_buf);
      close(sockfd);
    }
    return(rc);
  }

#endif

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool LocalInterface::IsRunning() const
  {
    bool  rc = false;
        
    int  sockfd = socket(PF_INET,SOCK_DGRAM,0);
    if (sockfd >= 0) {
      struct ifconf  ifc;
      struct ifreq  *ifr;
      ifc.ifc_buf = 0;
      int    numreqs = 30;
      for (;;) {
        ifc.ifc_len = sizeof(struct ifreq) * numreqs;
        ifc.ifc_buf = (char *)realloc(ifc.ifc_buf, ifc.ifc_len);
        memset(ifc.ifc_buf, 0, ifc.ifc_len);
        if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
          break;
        if ((uint32_t)ifc.ifc_len == sizeof(struct ifreq) * numreqs) {
          //  assume it overflowed and try again
          numreqs += 10;
          continue;
        }
        break;
      }
      ifr = ifc.ifc_req;
      for (int n = 0; n < ifc.ifc_len; n += sizeof(struct ifreq)) {
        if (_name == std::string(ifr->ifr_name)) {
          if (ioctl(sockfd, SIOCGIFFLAGS, ifr) >= 0) {
            if (ifr->ifr_flags & IFF_RUNNING) {
              rc = true;
            }
          }
          break;
        }
#ifdef _SIZEOF_ADDR_IFREQ
        ifr = (struct ifreq *)((char *)ifr + _SIZEOF_ADDR_IFREQ(*ifr));
#else
		++ifr;
#endif
      }
      if (ifc.ifc_buf)
        free(ifc.ifc_buf);
      close(sockfd);
    }
    return(rc);
  }

#ifndef __linux__

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool LocalInterface::AddAlias(const Ipv4Address & address, 
                                const Ipv4Address & netmask)
  {
    bool  rc = false;
    
    if (_aliases.find(LocalInterfaceAlias(address, netmask)) != _aliases.end())
      return(false);
    
    struct ifaliasreq  aliasRequest;
    memset(&aliasRequest, 0, sizeof(aliasRequest));
    
    strncpy(aliasRequest.ifra_name, _name.c_str(), IFNAMSIZ);

    //  the address
    struct sockaddr_in  *sin = (struct sockaddr_in *)(&aliasRequest.ifra_addr);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = PF_INET;
    sin->sin_addr.s_addr = address.Raw();

    //  the netmask
    sin = (struct sockaddr_in *)(&aliasRequest.ifra_mask);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = PF_INET;
    sin->sin_addr.s_addr = netmask.Raw();

    //  the broadcast
    sin = (struct sockaddr_in *)(&aliasRequest.ifra_broadaddr);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = PF_INET;
    sin->sin_addr.s_addr = address.Raw() | (~(netmask.Raw()));

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd >= 0) {
      if (ioctl(sockfd, SIOCAIFADDR, &aliasRequest) == 0) {
        rc = true;
        _aliases.insert(LocalInterfaceAlias(address, netmask));
      }
      close(sockfd);
    }

    return(rc);
  }

#endif

#ifndef __linux__
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool LocalInterface::RemoveAlias(const Ipv4Address & address,
                                   const Ipv4Address & netmask)
  {
    bool  rc = false;
    
    if (_aliases.find(LocalInterfaceAlias(address, netmask)) == _aliases.end())
      return(false);

    struct ifaliasreq  aliasRequest;
    memset(&aliasRequest, 0, sizeof(aliasRequest));
    
    strncpy(aliasRequest.ifra_name, _name.c_str(), IFNAMSIZ);

    //  the address
    struct sockaddr_in  *sin = (struct sockaddr_in *)(&aliasRequest.ifra_addr);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = PF_INET;
    sin->sin_addr.s_addr = address.Raw();

    //  the netmask
    sin = (struct sockaddr_in *)(&aliasRequest.ifra_mask);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = PF_INET;
    sin->sin_addr.s_addr = netmask.Raw();

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd >= 0) {
      if (ioctl(sockfd, SIOCDIFADDR, &aliasRequest) == 0) {
        _aliases.erase(LocalInterfaceAlias(address, netmask));
        rc = true;
      }
      close(sockfd);
    }
    
    return(rc);
  }

#endif

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::set<LocalInterfaceAlias> & LocalInterface::Aliases() const
  {
    return(_aliases);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::set<LocalInterfaceAlias> & LocalInterface::Aliases()
  {
    return(_aliases);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & operator << (ostream & os, const LocalInterface & li)
  {
    if (os) {
      os << "name " << li._name << " addr " << setiosflags(ios::left)
         << setw(16) << li._addr << "mask " << setw(16) << li._mask
         << " mtu " << setw(5) << li._mtu;
      if (li.IsUp()) {
        os << " UP";
      }
      os << '\n';
      if (! li._aliases.empty()) {
        string::size_type  nameWidth = li._name.length();
        set<LocalInterfaceAlias>::const_iterator  ai;
        for (ai = li._aliases.begin(); ai != li._aliases.end(); ++ai) {
          os << setw(6 + nameWidth) << " " << *ai << '\n';
        }
      }
    }
    return(os);
  }
  
#ifdef __linux__

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool GetLocalInterfaces(map<string,LocalInterface> & interfaces)
  {
    if (! interfaces.empty())
      interfaces.clear();
        
    int  sockfd = socket(PF_INET,SOCK_DGRAM,0);
    if (sockfd >= 0) {
      struct ifconf  ifc;
      struct ifreq  *ifr;
      ifc.ifc_buf = 0;
      int  numreqs = 30;
      for (;;) {
        ifc.ifc_len = sizeof(struct ifreq) * numreqs;
        ifc.ifc_buf = (char *)realloc(ifc.ifc_buf, ifc.ifc_len);
        if (ioctl(sockfd, OSIOCGIFCONF, &ifc) < 0)
          break;
        if ((uint32_t)ifc.ifc_len == sizeof(struct ifreq) * numreqs) {
          //  assume it overflowed and try again
          numreqs += 10;
          continue;
        }
        break;
      }
      
      ifr = ifc.ifc_req;
#ifdef _SIZEOF_ADDR_IFREQ
      for (int n = 0; n < ifc.ifc_len; n += _SIZEOF_ADDR_IFREQ(*ifr)) {
#else
      for (int n = 0; n < ifc.ifc_len; n += sizeof(*ifr)) {
#endif
        if (ifr->ifr_name) {
          LocalInterface  localIntf;
          
          localIntf.Name(ifr->ifr_name);
          
          localIntf.Addr(0);
          ipv4addr_t  intfIP = 0;
          if (ioctl(sockfd, SIOCGIFADDR, ifr) >= 0) {
            if (ifr->ifr_addr.sa_family == AF_INET) {
              intfIP = ((struct sockaddr_in *)&(ifr->ifr_addr))->sin_addr.s_addr;
              localIntf.Addr(intfIP);
            }
          }
          
          localIntf.Mask(0);
          ipv4addr_t    intfMask = 0;
          if (ioctl(sockfd, SIOCGIFNETMASK, ifr) >= 0) {
            if (ifr->ifr_addr.sa_family == AF_INET) {
              intfMask = ((struct sockaddr_in *)&(ifr->ifr_addr))->sin_addr.s_addr;
              localIntf.Mask(intfMask);
            }
          }
          
          localIntf.Mtu(0);
          if (ioctl(sockfd, SIOCGIFMTU, ifr) >= 0)
            localIntf.Mtu(ifr->ifr_mtu);
            
          interfaces[ifr->ifr_name] = localIntf;
        }

        ++ifr;
      }
      if (ifc.ifc_buf)
        free(ifc.ifc_buf);
      close(sockfd);
    }
    return(! interfaces.empty());
  }

#elif defined(__FreeBSD__) || defined(__APPLE__)
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool GetLocalInterfaces(std::map<string, LocalInterface> & interfaces)
  {
    if (! interfaces.empty())
      interfaces.clear();

    struct ifaddrs  *ifAddrs;
    if (getifaddrs(&ifAddrs) == 0) {
      struct ifaddrs  *ifAddr = ifAddrs;
      while (ifAddr) {
        if (ifAddr->ifa_addr) {
          if (ifAddr->ifa_addr->sa_family == AF_INET) {
            if (interfaces.find(ifAddr->ifa_name) == interfaces.end()) {
              interfaces[ifAddr->ifa_name].Name(ifAddr->ifa_name);
              const struct sockaddr_in  *sin = 
                (const struct sockaddr_in *)(ifAddr->ifa_addr);
              interfaces[ifAddr->ifa_name].Addr(sin->sin_addr.s_addr);
              sin = (const struct sockaddr_in *)(ifAddr->ifa_netmask);
              interfaces[ifAddr->ifa_name].Mask(sin->sin_addr.s_addr);
            }
            else {
              //  assume it's an alias
              const struct sockaddr_in  *sinAddr = 
                (const struct sockaddr_in *)(ifAddr->ifa_addr);
              const struct sockaddr_in  *sinMask = 
                (const struct sockaddr_in *)(ifAddr->ifa_netmask);
              interfaces[ifAddr->ifa_name].AddAlias(sinAddr->sin_addr.s_addr,
                                                    sinMask->sin_addr.s_addr);
            }
          }
        }
        ifAddr = ifAddr->ifa_next;
      }
      freeifaddrs(ifAddrs);
    }
    
    if (getifaddrs(&ifAddrs) == 0) {
      struct ifaddrs  *ifAddr = ifAddrs;
      while (ifAddr) {
        if (ifAddr->ifa_addr) {
          if (ifAddr->ifa_addr->sa_family == AF_LINK) {
            const struct if_data  *ifData = 
              (const struct if_data *)(ifAddr->ifa_data);
            if (ifData) {
              if (interfaces.find(ifAddr->ifa_name) != interfaces.end()) {
                interfaces[ifAddr->ifa_name].Mtu(ifData->ifi_mtu);
              }
            }
          }
        }
        ifAddr = ifAddr->ifa_next;
      }
      freeifaddrs(ifAddrs);
    }
    
    return (! interfaces.empty());
  }
      
#else

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool GetLocalInterfaces(std::map<string, LocalInterface> & interfaces)
  {
    if (! interfaces.empty())
      interfaces.clear();
        
    int  sockfd = socket(PF_INET,SOCK_DGRAM,0);
    if (sockfd >= 0) {
      struct ifconf  ifc;
      struct ifreq  *ifr;
      ifc.ifc_buf = 0;
      int  numreqs = 30;
      for (;;) {
        ifc.ifc_len = sizeof(struct ifreq) * numreqs;
        ifc.ifc_buf = (char *)realloc(ifc.ifc_buf, ifc.ifc_len);
        if (ioctl(sockfd, OSIOCGIFCONF, &ifc) < 0)
          break;
        if ((uint32_t)ifc.ifc_len == sizeof(struct ifreq) * numreqs) {
          //  assume it overflowed and try again
          numreqs += 10;
          continue;
        }
        break;
      }
      
      ifr = ifc.ifc_req;
#ifdef _SIZEOF_ADDR_IFREQ
      for (int n = 0; n < ifc.ifc_len; n += _SIZEOF_ADDR_IFREQ(*ifr)) {
#else
      for (int n = 0; n < ifc.ifc_len; n += sizeof(*ifr)) {
#endif
        interfaces[ifr->ifr_name].Name(ifr->ifr_name);
        
        ipv4addr_t  intfIP = 0;
        if (ioctl(sockfd, SIOCGIFADDR, ifr) >= 0) {
          if (ifr->ifr_addr.sa_family == AF_INET) {
            intfIP = ((struct sockaddr_in *)&(ifr->ifr_addr))->sin_addr.s_addr;
          }
        }
        
        ipv4addr_t    intfMask = 0;
        if (ioctl(sockfd, SIOCGIFNETMASK, ifr) >= 0) {
#if defined(__OpenBSD__)
          if (ifr->ifr_addr.sa_family == AF_INET ||
              ifr->ifr_addr.sa_family == 0) {
#else
          if (ifr->ifr_addr.sa_family == AF_INET) {
#endif
            intfMask = ((struct sockaddr_in *)&(ifr->ifr_addr))->sin_addr.s_addr;
          }
          
        }
     
        if (interfaces[ifr->ifr_name].Addr().Raw() == 0) {
            
        //  XXX - what is magic about flags == 0x12 on BSD and OS X?
          // if (ifr->ifr_flags == 0x12) {
          interfaces[ifr->ifr_name].Addr(intfIP);
          interfaces[ifr->ifr_name].Mask(intfMask);
          interfaces[ifr->ifr_name].Mtu(0);
          if (ioctl(sockfd, SIOCGIFMTU, ifr) >= 0)
            interfaces[ifr->ifr_name].Mtu(ifr->ifr_mtu);
        }
        else {
          if (interfaces[ifr->ifr_name].Addr() != intfIP) {
            LocalInterfaceAlias  intfAlias;
            intfAlias.Addr(intfIP);
            intfAlias.Mask(intfMask);
            interfaces[ifr->ifr_name].Aliases().insert(intfAlias);
          }
        }

#if (defined(__OpenBSD__))
        ++ifr;
#else
        ifr = (struct ifreq *)((char *)ifr + _SIZEOF_ADDR_IFREQ(*ifr));
#endif
      }
      if (ifc.ifc_buf)
        free(ifc.ifc_buf);
      close(sockfd);
    }
    return(! interfaces.empty());
  }

#endif


}  // namespace Dwm

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
