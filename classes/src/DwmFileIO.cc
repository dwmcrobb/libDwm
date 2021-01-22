//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmFileIO.cc 11084 $
// @(#) $Id: DwmFileIO.cc 11084 2020-09-06 04:57:40Z dwm $
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmFileIO.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::FileIO class implementation
//---------------------------------------------------------------------------

#include <cstdlib>
#include <cassert>

#include "DwmSvnTag.hh"
#include "DwmSysLogger.hh"
#include "DwmFileIO.hh"
#include "DwmPortability.hh"
#include "DwmXDRUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmFileIO.cc 11084 $");

namespace Dwm {

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, char c)
  {
    size_t  rc = 0;
    if (f) {
      rc = fwrite((void *)&c, sizeof(c), 1, f);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, uint8_t c)
  {
    size_t  rc = 0;
    if (f) {
      rc = fwrite((void *)&c, sizeof(c), 1, f);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, bool b)
  {
    uint8_t  c = b;
    return(FileIO::Write(f, c));
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, int16_t val)
  {
    size_t rc = 0;
    if (f) {
      int16_t v = htobe16(val);
      if (fwrite((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
   
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, uint16_t val)
  {
    size_t rc = 0;
    if (f) {
      uint16_t v = htobe16(val);
      if (fwrite((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, int32_t val)
  {
    size_t rc = 0;
    if (f) {
      int32_t v = htobe32(val);
      if (fwrite((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, uint32_t val)
  {
    size_t rc = 0;
    if (f) {
      uint32_t v = htobe32(val);
      if (fwrite((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, const int64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      int64_t  v = htobe64(val);
      if (fwrite((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, const uint64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint64_t  v = htobe64(val);
      if (fwrite((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, float val)
  {
    size_t  rc = 0;
    if (f) {
      char  *buf = 0;
      rc = XDRUtils::Encode(val, &buf);
      if (fwrite((void *)buf, rc, 1, f) != 1) {
        rc = 0;
      }
      free(buf);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, const double & val)
  {
    size_t  rc = 0;
    
    if (f) {
      char  *buf = 0;
      rc = XDRUtils::Encode(val, &buf);
      if (fwrite((void *)buf, rc, 1, f) != 1) {
        rc = 0;
      }
      free(buf);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Write(FILE * f, const std::string & s)
  {
    size_t  rc = 0;
    if (f) {
      uint32_t len = s.size();
      if (Write(f, len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          if (fwrite((void *)s.c_str(), len, 1, f) == 1) {
            rc += len;
          }
          else {
            rc = 0;
          }
        }
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, char & c)
  {
    size_t rc = 0;
    if (f) {
      rc = fread((void *)&c, sizeof(c), 1, f);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, uint8_t & c)
  {
    size_t  rc = 0;
    if (f) {
      rc = fread((void *)&c, sizeof(c), 1, f);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, bool & b)
  {
    uint8_t  c;
    size_t  rc = FileIO::Read(f, c);
    if (rc > 0) {
      b = c ? true : false;
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, int16_t & val)
  {
    size_t  rc = 0;
    if (f) {
      int16_t v;
      if (fread((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, uint16_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint16_t v;
      if (fread((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, int32_t & val)
  {
    size_t  rc = 0;
    if (f) {
      int32_t v;
      if (fread((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, uint32_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint32_t v;
      if (fread((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, int64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint64_t  v;
      if (fread((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
        val = be64toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, uint64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint64_t v;
      if (fread((void *)&v, sizeof(v), 1, f) == 1) {
        rc = sizeof(v);
        val = be64toh(v);
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, float & val)
  {
    size_t  rc = 0;
    
    if (f) {
      char   buf[4];
      if (fread((void *)buf, 4, 1, f) == 1) {
        XDRUtils::Decode(buf, val);
        rc = 4;
      }
    }
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, double & val)
  {
    size_t  rc = 0;
    
    if (f) {
      char   buf[8];
      if (fread((void *)buf, 8, 1, f) == 1) {
        XDRUtils::Decode(buf, val);
        rc = 8;
      }
    }

    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t FileIO::Read(FILE * f, std::string & s)
  {
    s.clear();
    size_t  rc = 0;
    if (f) {
      uint32_t len;
      if (Read(f, len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          try {
            s.resize(len);
            if (fread((void *)s.data(), len , 1, f) == 1) {
              rc += len;
            }
            else {
              s.clear();
              rc = 0;
            }
          }
          catch (const std::exception & ex) {
            Syslog(LOG_ERR, "Exception: %s", ex.what());
            s.clear();
            rc = 0;
          }
          catch (...) {
            Syslog(LOG_ERR, "Exception");
            s.clear();
            rc = 0;
          }
        }
      }
    }
    return(rc);
  }

}  // namespace Dwm
