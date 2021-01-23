//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmMacAddress.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::MacAddress implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/uio.h>
  #include <unistd.h>
}

#include <cstring>  // for memset(), memcpy()
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>

#include "DwmMacAddress.hh"
#include "DwmSysLogger.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::MacAddress()
  {
    memset(_addr, 0, 6);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::MacAddress(const std::string & s)
  {
    memset(_addr, 0, 6);
    istringstream  is(s);
    string         c;
    uint8_t        i = 0;
    while (getline(is, c, ':')) {
      if (! c.empty()) {
        _addr[i] = strtoul(c.c_str(), 0, 16);
      }
      if (++i > 5)
        break;
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::MacAddress(const MacAddress & addr)
  {
    memcpy(_addr, addr._addr, 6);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress & MacAddress::operator = (const MacAddress & addr)
  {
    if (&addr != this) {
      memcpy(_addr, addr._addr, 6);
    }
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MacAddress::operator == (const MacAddress & addr) const
  {
    return(memcmp(_addr, addr._addr, 6) == 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MacAddress::operator < (const MacAddress & addr) const
  {
    return(memcmp(_addr, addr._addr, 6) < 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::operator std::string () const
  {
    ostringstream  os;
    os << *this;
    return os.str();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & MacAddress::Read(std::istream & is)
  {
    if (is) {
      is.read((char *)_addr, 6);
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & MacAddress::Write(std::ostream & os) const
  {
    if (os) {
      os.write((char *)_addr, 6);
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t MacAddress::Read(int fd)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = read(fd, _addr, 6);
      if (rc != 6) {
        Syslog(LOG_ERR, "MacAddress::Write(%d) failed: %m", fd);
        rc = -1;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t MacAddress::Write(int fd) const
  {
    ssize_t  rc = 6;
    if (fd >= 0) {
      rc = write(fd, _addr, 6);
      if (rc != 6) {
        Syslog(LOG_ERR, "MacAddress::Write(%d) failed: %m", fd);
        rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t MacAddress::Read(FILE *f)
  {
    size_t  rc = 0;
    if (f) {
      rc = fread(_addr, 6, 1, f);
      if (rc != 6)
        Syslog(LOG_ERR, "MacAddress::Read(FILE %p) failed: %m", f);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t MacAddress::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      rc = fwrite(_addr, 6, 1, f);
      if (rc != 6)
        Syslog(LOG_ERR, "MacAddress::Write(FILE %p) failed: %m", f);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::Read(gzFile gzf)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzread(gzf, _addr, 6);
      if (rc != 6)
        Syslog(LOG_ERR, "MacAddress::Read(gzFile %p) failed: %m", gzf);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      rc = gzwrite(gzf, _addr, 6);
      if (rc != 6)
        Syslog(LOG_ERR, "MacAddress::Write(gzFile %p) failed: %m", gzf);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzread(bzf, _addr, 6);
      if (rc != 6)
        Syslog(LOG_ERR, "MacAddress::BZRead(%p) failed: %m", bzf);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      rc = BZ2_bzwrite(bzf, (void *)_addr, 6);
      if (rc != 6)
        Syslog(LOG_ERR, "MacAddress::BZWrite(%p) failed: %m", bzf);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t MacAddress::StreamedLength() const
  {
    return(6);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const MacAddress & addr)
  {
    if (os) {
      ostringstream  ss;
      ss << setfill('0') << hex;
      ss << setw(2) << (uint16_t)addr._addr[0] << ":"
         << setw(2) << (uint16_t)addr._addr[1] << ":"
         << setw(2) << (uint16_t)addr._addr[2] << ":"
         << setw(2) << (uint16_t)addr._addr[3] << ":"
         << setw(2) << (uint16_t)addr._addr[4] << ":"
         << setw(2) << (uint16_t)addr._addr[5];
      os << ss.str();
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream &
  operator >> (std::istream & is, MacAddress & addr)
  {
    memset(addr._addr, 0, 6);
    if (is) {
      is.setf(ios::hex, ios::basefield);
      uint16_t  us;
      for (int i = 0; i < 5; ++i) {
        char      c;
        if (! (is >> setw(2) >> us >> c))
          goto done;
        if (us > 255 || c != ':')
          goto done;
        addr._addr[i] = us;
      }
      if (! (is >> setw(2) >> us))
        goto done;
      addr._addr[5] = us;
    }
  done:
    is.unsetf(ios::basefield);
    return(is);
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
