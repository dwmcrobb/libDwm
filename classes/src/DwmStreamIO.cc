//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2013, 2017, 2020
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY 
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmStreamIO.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::StreamIO class implementation
//---------------------------------------------------------------------------

#include <cassert>
#include <cstdint>
#include <cstdlib>

#include "DwmSysLogger.hh"
#include "DwmStreamIO.hh"
#include "DwmPortability.hh"
#include "DwmXDRUtils.hh"

namespace Dwm {

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, char c)
  {
    if (os)
      os.write(&c, sizeof(c));
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, uint8_t c)
  {
    if (os)
      os.write((caddr_t)&c, sizeof(c));
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, bool b)
  {
    uint8_t  v = b;
    return(StreamIO::Write(os, v));
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, int16_t val)
  {
    if (os) {
      int16_t  v = htobe16(val);
      os.write((caddr_t)&v, sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, uint16_t val)
  {
    if (os) {
      uint16_t v = htobe16(val);
      os.write((caddr_t)&v, sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, int32_t val)
  {
    if (os) {
      int32_t v = htobe32(val);
      os.write((caddr_t)&v, sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, uint32_t val)
  {
    if (os) {
      uint32_t v = htobe32(val);
      os.write((caddr_t)&v, sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, const int64_t & val)
  {
    if (os) {
      uint64_t v = htobe64(val);
      os.write((caddr_t)&v, sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, const uint64_t & val)
  {
    if (os) {
      uint64_t v = htobe64(val);
      os.write((caddr_t)&v, sizeof(v));
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, float val)
  {
    if (os) {
      char     *buf = 0;
      uint32_t  rc = XDRUtils::Encode(val, &buf);
      os.write(buf, rc);
      free(buf);
    }
                
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, const double & val)
  {
    if (os) {
      char     *buf = 0;
      uint32_t  rc = XDRUtils::Encode(val, &buf);
      os.write(buf, rc);
      free(buf);
    }

    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & StreamIO::Write(std::ostream & os, const std::string & s)
  {
    if (os) {
      uint64_t  len = s.size();
      if (Write(os, len)) {
        if (len > 0) {
          os.write(s.c_str(), len);
        }
      }
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, char & c)
  {
    if (is) {
      is.read(&c, sizeof(c));
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, uint8_t & c)
  {
    if (is) {
      is.read((caddr_t)&c, sizeof(c));
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, bool & b)
  {
    uint8_t  c;
    if (StreamIO::Read(is, c)) {
      b = (c ? true : false);
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, int16_t & val)
  {
    if (is) {
      int16_t  v;
      if (is.read((caddr_t)&v, sizeof(v))) {
        val = be16toh(v);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, uint16_t & val)
  {
    if (is) {
      uint16_t  v;
      if (is.read((caddr_t)&v, sizeof(v))) {
        val = be16toh(v);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, int32_t & val)
  {
    if (is) {
      int32_t  v;
      if (is.read((caddr_t)&v, sizeof(v))) {
        val = be32toh(v);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, uint32_t & val)
  {
    if (is) {
      uint32_t  v;
      if (is.read((caddr_t)&v, sizeof(v))) {
        val = be32toh(v);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, int64_t & val)
  {
    if (is) {
      int64_t  v;
      if (is.read((caddr_t)&v, sizeof(v))) {
        val = be64toh(v);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, uint64_t & val)
  {
    if (is) {
      uint64_t  v;
      if (is.read((caddr_t)&v, sizeof(v))) {
        val = be64toh(v);
      }
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, float & val)
  {
    if (is) {
      char  buf[4];
      if (is.read(buf, 4)) {
        XDRUtils::Decode(buf, val);
      }
    }
    return(is);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, double & val)
  {
    if (is) {
      char  buf[8];
      if (is.read(buf, 8)) {
        XDRUtils::Decode(buf, val);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & StreamIO::Read(std::istream & is, std::string & s)
  {
    s.clear();
    if (is) {
      uint64_t  len;
      if (StreamIO::Read(is, len)) {
        if (len > 0) {
          try {
            s.resize(len);
            if (! is.read(s.data(), len)) {
              s.clear();
            }
          }
          catch (const std::exception & ex) {
            Syslog(LOG_ERR, "Exception: %s", ex.what());
            is.setstate(std::ios_base::failbit);
          }
          catch (...) {
            Syslog(LOG_ERR, "Exception");
            is.setstate(std::ios_base::failbit);
          }
        }
      }
    }
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
