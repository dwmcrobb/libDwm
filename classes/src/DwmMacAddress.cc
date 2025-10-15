//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2024-2025
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

#include "DwmMacAddress.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmBZ2IO.hh"
#include "DwmSysLogger.hh"

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::MacAddress()
      : _addr{0,0,0,0,0,0}
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::MacAddress(const std::string & s)
  {
    _addr = {0,0,0,0,0,0};
    istringstream  is(s);
    string         c;
    uint8_t        i = 0;
    while (getline(is, c, ':')) {
      if (! c.empty()) {
        _addr[i] = strtoul(c.c_str(), 0, 16);
      }
      if (++i > (sizeof(_addr) / sizeof(_addr[0])) - 1)
      {
        break;
      }
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress::MacAddress(const MacAddress & addr)
  {
    _addr = addr._addr;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MacAddress & MacAddress::operator = (const MacAddress & addr)
  {
    if (&addr != this) {
      _addr = addr._addr;
    }
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MacAddress::operator == (const MacAddress & addr) const
  {
    return(_addr == addr._addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MacAddress::operator < (const MacAddress & addr) const
  {
    return(_addr < addr._addr);
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
  size_t MacAddress::Read(FILE *f)
  {
    return FileIO::Read(f, _addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t MacAddress::Write(FILE *f) const
  {
    return FileIO::Write(f, _addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::Read(gzFile gzf)
  {
    return GZIO::Read(gzf, _addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::Write(gzFile gzf) const
  {
    return GZIO::Write(gzf, _addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::BZRead(BZFILE *bzf)
  {
    return BZ2IO::BZRead(bzf, _addr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MacAddress::BZWrite(BZFILE *bzf) const
  {
    return BZ2IO::BZWrite(bzf, _addr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t MacAddress::StreamedLength() const
  {
    return(_addr.size());
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
    addr._addr = {0,0,0,0,0,0};
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
