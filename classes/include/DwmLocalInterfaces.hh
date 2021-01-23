//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005, 2016
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
//!  \file DwmLocalInterfaces.hh
//!  \brief Dwm::LocalInterfaceAlias, Dwm::LocalInterface and
//!  Dwm::GetLocalInterfaces() definitions
//---------------------------------------------------------------------------

#ifndef _DWMLOCALINTERFACES_HH_
#define _DWMLOCALINTERFACES_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
}

#include <set>
#include <string>
#include <map>

#include "DwmIpv4Address.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Holds an interface alias, which is just IP address and netmask.
  //--------------------------------------------------------------------------
  class LocalInterfaceAlias
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    LocalInterfaceAlias();

    //------------------------------------------------------------------------
    //!  Construct from an address and netmask
    //------------------------------------------------------------------------
    LocalInterfaceAlias(const Ipv4Address & address, const Ipv4Address & mask);

    //------------------------------------------------------------------------
    //!  Returns the IP address
    //------------------------------------------------------------------------
    const Ipv4Address & Addr() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the IP address
    //------------------------------------------------------------------------
    const Ipv4Address & Addr(const Ipv4Address & addr);

    //------------------------------------------------------------------------
    //!  Returns the netmask
    //------------------------------------------------------------------------
    const Ipv4Address & Mask() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the netmask
    //------------------------------------------------------------------------
    const Ipv4Address & Mask(const Ipv4Address & mask);

    //------------------------------------------------------------------------
    //!  operator <
    //------------------------------------------------------------------------
    bool operator < (const LocalInterfaceAlias & alias) const;

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const LocalInterfaceAlias & alias) const;
    
    //------------------------------------------------------------------------
    //!  Prints a LocalInterfaceAlias to an ostream
    //------------------------------------------------------------------------
    friend std::ostream & 
    operator << (std::ostream & os, const LocalInterfaceAlias & alias);
    
  private:
    Ipv4Address  _addr;
    Ipv4Address  _mask;
  };
  
  //------------------------------------------------------------------------
  //!  Simple class to hold the name and IPv4 address of a local
  //!  interface.
  //------------------------------------------------------------------------
  class LocalInterface
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    LocalInterface();
    
    //--------------------------------------------------------------------
    //!  Returns the interface name.
    //--------------------------------------------------------------------
    const std::string & Name() const;

    //--------------------------------------------------------------------
    //!  Sets and returns the interface name.
    //--------------------------------------------------------------------
    const std::string & Name(const std::string & name);

    //--------------------------------------------------------------------
    //!  Returns the interface address.
    //--------------------------------------------------------------------
    const Ipv4Address & Addr() const;

    //--------------------------------------------------------------------
    //!  Sets and returns the interface address.
    //--------------------------------------------------------------------
    const Ipv4Address & Addr(const Ipv4Address & addr);

    //--------------------------------------------------------------------
    //!  Returns the interface netmask.
    //--------------------------------------------------------------------
    const Ipv4Address & Mask() const;

    //--------------------------------------------------------------------
    //!  Sets and returns the interface netmask.
    //--------------------------------------------------------------------
    const Ipv4Address & Mask(const Ipv4Address & mask);

    //------------------------------------------------------------------------
    //!  Returns the MTU
    //------------------------------------------------------------------------
    int Mtu() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the MTU
    //------------------------------------------------------------------------
    int Mtu(int mtu);
    
    //--------------------------------------------------------------------
    //!  Returns true if the interface is administratively 'up'.
    //!  This says nothing about the link state.
    //--------------------------------------------------------------------
    bool IsUp() const;

    bool IsRunning() const;

    //------------------------------------------------------------------------
    //!  Adds an alias with the given \c address and \c netmask.  Returns
    //!  true on success, false on failure.
    //------------------------------------------------------------------------
    bool AddAlias(const Ipv4Address & address, const Ipv4Address & netmask);

    //------------------------------------------------------------------------
    //!  Removes an alias with the given \c address and \c netmask.  Returns
    //!  true on success, false on failure.
    //------------------------------------------------------------------------
    bool RemoveAlias(const Ipv4Address & address, const Ipv4Address & netmask);

    //------------------------------------------------------------------------
    //!  Returns a const reference to the aliases.
    //------------------------------------------------------------------------
    const std::set<LocalInterfaceAlias> & Aliases() const;
    
    //------------------------------------------------------------------------
    //!  Returns a mutable reference to the aliases.
    //------------------------------------------------------------------------
    std::set<LocalInterfaceAlias> & Aliases();

    //------------------------------------------------------------------------
    //!  Prints a LocalInterface to an ostream.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, 
                                       const LocalInterface & li);
    
  private:
    std::string                    _name;
    Ipv4Address                    _addr;
    Ipv4Address                    _mask;
    int                            _mtu;
    std::set<LocalInterfaceAlias>  _aliases;
  };

  //------------------------------------------------------------------------
  //!  Fetches local interfaces with IP addresses into \c interfaces.
  //!  Returns true on success (1 or more interfaces found).
  //!  This _should_ work on OpenBSD, FreeBSD and Linux.
  //------------------------------------------------------------------------
  bool GetLocalInterfaces(std::map<std::string,LocalInterface> & interfaces);
    
}  // namespace Dwm

#endif  // _DWMLOCALINTERFACES_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
