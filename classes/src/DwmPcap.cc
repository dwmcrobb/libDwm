//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016, 2020
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
//!  \file DwmPcap.cc
//!  \author Daniel W. McRobb
//!  \brief Pcap class implementation
//---------------------------------------------------------------------------

#include "DwmPortability.hh"

#include <cassert>
#include <cstdlib>
#include <cstring>

#include "DwmLocalInterfaces.hh"
#include "DwmSysLogger.hh"
#include "DwmPcap.hh"

extern "C" {
#if (defined(HAVE_NET_PFVAR_H) && defined(HAVE_NET_IF_PFLOG_H))
  #include <net/if.h>
  #include <net/pfvar.h>
  #include <net/if_pflog.h>
#endif
}

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Pcap::Pcap()
      : _pcap(nullptr), _errbuf()
  { }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Pcap::~Pcap()
  {
    if (_pcap)
      Close();
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int	Pcap::SetBufferSize(int bufferSize)
  {
    if (_pcap != nullptr) {
      return pcap_set_buffer_size(_pcap, bufferSize);
    }
    else {
      return PCAP_ERROR_ACTIVATED;
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::Create(const string & device)
  {
    bool  rc = false;
    if (! _pcap) {
      if (! device.empty()) {
        char    errbuf[PCAP_ERRBUF_SIZE];
        _pcap = pcap_create((char *)device.c_str(), errbuf);
        if (_pcap) {
          rc = true;
        }
        else {
          FSyslog(LOG_ERR, "Pcap::Create({}) failed: {}",
                  device, errbuf);
          _errbuf = errbuf;
        }
      }
      else {
        FSyslog(LOG_ERR, "Pcap::Create() invalid (empty) device name");
      }
    }
    else {
      FSyslog(LOG_ERR, "Pcap::Create(): pcap handle already open!");
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::Activate()
  {
    bool  rc = false;
    if (_pcap) {
      int  res = pcap_activate(_pcap);
      if (0 == res) {
        rc = true;
      }
      else if (0 < res) {
        switch (res) {
          case PCAP_WARNING_PROMISC_NOTSUP:
            FSyslog(LOG_WARNING,
                    "Pcap::Activate(): promiscuous mode not supported ({})",
                    pcap_geterr(_pcap));
            break;
          case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
            FSyslog(LOG_WARNING,
                    "Pcap::Activate(): timestamp type not supported");
            break;
          case PCAP_WARNING:
            FSyslog(LOG_WARNING,
                    "");
            break;
          default:
            break;
        }
        rc = true;
      }
      else {
        switch (res) {
          case PCAP_ERROR_ACTIVATED:
            FSyslog(LOG_ERR, "Pcap::Activate(): device already activated");
            break;
          case PCAP_ERROR_NO_SUCH_DEVICE:
            FSyslog(LOG_ERR, "Pcap::Activate(): no such device ({})",
                    pcap_geterr(_pcap));
            break;
          case PCAP_ERROR_PERM_DENIED:
            FSyslog(LOG_ERR, "Pcap::Activate(): permission denied ({})",
                    pcap_geterr(_pcap));
            break;
          case PCAP_ERROR_PROMISC_PERM_DENIED:
            FSyslog(LOG_ERR,
                    "Pcap::Activate(): promiscuous permission denied");
            break;
          case PCAP_ERROR_RFMON_NOTSUP:
            FSyslog(LOG_ERR, "Pcap::Activate(): monintor mode not supported");
            break;
          case PCAP_ERROR_IFACE_NOT_UP:
            FSyslog(LOG_ERR, "Pcap::Activate(): interface not up");
            break;
          case PCAP_ERROR:
            FSyslog(LOG_ERR, "Pcap::Activate(): {}", pcap_geterr(_pcap));
            break;
          default:
            break;
        }
      }
    }
    else {
      FSyslog(LOG_ERR, "Pcap::Activate(): device not open");
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::SetImmediateMode(bool enable)
  {
    bool  rc = false;
    if (_pcap) {
      int  pcrc = pcap_set_immediate_mode(_pcap, enable ? 1 : 0);
      switch (pcrc) {
        case 0:
          rc = true;
          break;
        case PCAP_ERROR_ACTIVATED:
          FSyslog(LOG_ERR, "Pcap::SetImmediateMode(): device activated");
          break;
        default:
          break;
      }
    }
    else {
      FSyslog(LOG_ERR, "Pcap::SetImmediateMode(): device not open");
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Pcap::GetSelectableFd()
  {
    int  rc = -1;
    if (_pcap) {
      rc = pcap_get_selectable_fd(_pcap);
      if (0 > rc) {
        FSyslog(LOG_ERR, "Pcap::GetSelectableFd() failed");
      }
    }
    else {
      FSyslog(LOG_ERR, "Pcap::GetSelectableFd(): device not open");
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::SetSnaplen(int snaplen)
  {
    bool  rc = false;
    if (_pcap) {
      int pcrc = pcap_set_snaplen(_pcap, snaplen);
      switch (pcrc) {
        case 0:
          rc = true;
          break;
        case PCAP_ERROR_ACTIVATED:
          FSyslog(LOG_ERR, "Pcap::SetSnaplen(): device already activated");
          break;
        default:
          FSyslog(LOG_ERR, "Pcap::SetSnaplen({}) failed", snaplen);
          break;
      }
    }
    else {
      FSyslog(LOG_ERR, "Pcap::SetSnaplen(): device not open");
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::OpenLive(const string & device, int snaplen,
                      bool promisc, int timeout_msecs)
  {
    string  intfName = device;
    bool    rc = true;
    char    errbuf[PCAP_ERRBUF_SIZE];

    if (intfName.empty()) {
      map<string,LocalInterface>  interfaces;
      if ((! GetLocalInterfaces(interfaces))
          || interfaces.empty()) {
        Syslog(LOG_ERR, "Pcap::OpenLive() failed to find an interface");
        return(false);
      }
      map<string,LocalInterface>::const_iterator  i;
      for (i = interfaces.begin(); i != interfaces.end(); ++i) {
        if (i->second.IsUp() && i->second.IsRunning()) {
          intfName = i->first;
          break;
        }
      }
      if (intfName.empty()) {
        Syslog(LOG_ERR, "Pcap::OpenLive(): no suitable interface found");
        return(false);
      }
    }
      
    _pcap = pcap_open_live((char *)intfName.c_str(), snaplen, promisc,
                           timeout_msecs, errbuf);
    if (! _pcap) {
      rc = false;
      _errbuf = errbuf;
      Syslog(LOG_ERR, "Pcap::OpenLive(\"%s\") failed", intfName.c_str());
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::SetDirection(pcap_direction_t direction)
  {
    bool  rc = false;
    if (_pcap) {
      if (pcap_setdirection(_pcap, direction) == 0) {
        rc = true;
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::OpenOffline(const string & fname)
  {
    bool  rc = true;
    char  errbuf[PCAP_ERRBUF_SIZE];
    if (_pcap)
      Close();
    _pcap = pcap_open_offline(fname.c_str(), errbuf);
    if (!_pcap) {
      rc = false;
      _errbuf = errbuf;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::SetFilter(const string & filter, bool optimize)
  {
    bool  rc = false;

    if (! filter.empty()) {
      //  sigh, libpcap is not const-correct
      char *filt = (char *)strdup(filter.c_str());
      
      struct bpf_program  bpfProg;
      memset(&bpfProg, 0, sizeof(bpfProg));
      if (pcap_compile(_pcap, &bpfProg, filt, optimize, 0) >= 0) {
        if (pcap_setfilter(_pcap, &bpfProg) == 0) {
          rc = true;
        }
        pcap_freecode(&bpfProg);
      }
      free(filt);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::Close()
  {
    bool  rc = false;
    if (_pcap) {
      pcap_close(_pcap);
      _pcap = 0;
      rc = true;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Pcap::Dispatch(int count, pcap_handler callback, uint8_t *user)
  {
    assert(_pcap);
    return(pcap_dispatch(_pcap, count, callback, user));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Pcap::Loop(int count, pcap_handler callback, uint8_t *user)
  {
    return(pcap_loop(_pcap, count, callback, user));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Pcap::BreakLoop()
  {
    return pcap_breakloop(_pcap);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::RecvPacketReady(struct timeval timeout)
  {
    bool rc = false;
    if (_pcap) {
      int  fd = pcap_get_selectable_fd(_pcap);
      if (0 <= fd) {
        fd_set  fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        int  selectrc = select(fd+1, &fds, nullptr, nullptr, &timeout);
        if (selectrc > 0) {
          if (FD_ISSET(fd, &fds)) {
            rc = true;
          }
        }
      }
      else {
        FSyslog(LOG_ERR, "pcap_get_selectable_fd() failed");
      }
    }
    else {
      FSyslog(LOG_ERR, "capture device not open");
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Pcap::SendPacket(const uint8_t *buf, int size)
  {
    bool  rc = false;
    if (_pcap) {
      int  pcrc = pcap_sendpacket(_pcap, buf, size);
      switch (pcrc) {
        case 0:
          rc = true;
          break;
        case PCAP_ERROR_NOT_ACTIVATED:
          FSyslog(LOG_ERR, "Pcap::SendPacket(): device not activated");
          break;
        case PCAP_ERROR:
          FSyslog(LOG_ERR, "Pcap::SendPacket(): {}", pcap_geterr(_pcap));
          break;
        default:
          FSyslog(LOG_ERR, "Pcap::SendPacket(): unknown error");
          break;
      }
    }
    else {
      FSyslog(LOG_ERR, "Pcap::SendPacket(): device not open");
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & Pcap::LastError() const
  {
    return(_errbuf);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Pcap::DataLinkOffset() const
  {
    int  rc = -1;
    std::map<int,int>::const_iterator  iter = 
      _dataLinkOffsets.find(pcap_datalink(_pcap));
    if (iter != _dataLinkOffsets.end())
      rc = iter->second;
    return(rc);
  }
  
  std::map<int,int>  Pcap::_dataLinkOffsets = {
#if (defined(DLT_PFLOG) && defined(PFLOG_HDRLEN))
    { DLT_PFLOG,   PFLOG_HDRLEN },
#endif
    { DLT_NULL,     4 },
    { DLT_EN10MB,  14 },
    { DLT_IEEE802, 22 },
    { DLT_FDDI,    21 },
    { DLT_LOOP,    4 }
  };
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
