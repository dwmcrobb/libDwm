//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2020
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
//!  \file DwmBZ2IO.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::BZ2IO class implementation
//---------------------------------------------------------------------------

extern "C" {
#if (defined(__unix__) || defined(unix) || defined(__unix)) || (defined(__APPLE__))
  #include <sys/types.h>
  #include <unistd.h>
#endif
}

#include <cassert>
#include <cstdlib>

#include "DwmBZ2IO.hh"
#include "DwmPortability.hh"
#include "DwmXDRUtils.hh"

namespace Dwm {

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, char c)
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzwrite(bzf,(void *)&c,sizeof(c));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, uint8_t c)
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzwrite(bzf,(void *)&c,sizeof(c));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, int16_t val)
  {
    int  rc = -1;
    if (bzf) {
      int16_t v = htobe16(val);
      rc = BZ2_bzwrite(bzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, uint16_t val)
  {
    int  rc = -1;
    if (bzf) {
      uint16_t v = htobe16(val);
      rc = BZ2_bzwrite(bzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, int32_t val)
  {
    int  rc = -1;
    if (bzf) {
      int32_t v = htobe32(val);
      rc = BZ2_bzwrite(bzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, uint32_t val)
  {
    int  rc = -1;
    if (bzf) {
      uint32_t v = htobe32(val);
      rc = BZ2_bzwrite(bzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, const int64_t & val)
  {
    int  rc = -1;
    if (bzf) {
      int64_t v = htobe64(val);
      rc = BZ2_bzwrite(bzf,(void *)&v,sizeof(v));
      if (rc != sizeof(v))
        rc = -1;
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, const uint64_t & val)
  {
    int  rc = -1;
    if (bzf) {
      uint64_t v = htobe64(val);
      rc = BZ2_bzwrite(bzf,(void *)&v,sizeof(v));
      if (rc != sizeof(v))
        rc = -1;
    }
    return(rc);
  }

#if (defined(__unix__) || defined(unix) || defined(__unix)) || (defined(__APPLE__))

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, float val)
  {
    int  rc = -1;
    if (bzf) {
      char     *buf = 0;
      uint32_t  xe = XDRUtils::Encode(val, &buf);
      rc = BZ2_bzwrite(bzf, buf, xe);
      free(buf);
      if (rc < 4) {
        rc = -1;
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, const double & val)
  {
    int  rc = -1;
    if (bzf) {
      char  *buf = 0;
      uint32_t  xe = XDRUtils::Encode(val, &buf);
      rc = BZ2_bzwrite(bzf, buf, xe);
      if (rc < 8) {
        rc = -1;
      }
    }
    return rc;
  }

#elif (defined(_WIN32) || defined(_WIN64))
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, float val)
  {
    const uint32_t  *p = (const uint32_t *)&val;
    return BZ2IO::Write(bzf, *p);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, const double & val)
  {
    const uint64_t  *p = (const uint64_t *)&val;
    return BZ2IO::Write(bzf, *p);
  }
  
#endif
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZWrite(BZFILE *bzf, const std::string & s)
  {
    int  rc = -1;
    
    if (bzf) {
      int32_t  len = s.length();
      if (BZWrite(bzf,len) == sizeof(len)) {
        rc = sizeof(len);
        if (BZ2_bzwrite(bzf,(void *)s.c_str(),len) == len)
          rc += len;
        else
          rc = -1;
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, char & c)
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzread(bzf,(void *)&c,sizeof(c));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, uint8_t & c)
  {
    int  rc = -1;
    if (bzf)
      rc = BZ2_bzread(bzf,(void *)&c,sizeof(c));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, int16_t & val)
  {
    int  rc = -1;
    if (bzf) {
      int16_t  v;
      rc = BZ2_bzread(bzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be16toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, uint16_t & val)
  {
    int  rc = -1;
    if (bzf) {
      uint16_t  v;
      rc = BZ2_bzread(bzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be16toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, int32_t & val)
  {
    int  rc = -1;
    if (bzf) {
      int32_t  v;
      rc = BZ2_bzread(bzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be32toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, uint32_t & val)
  {
    int  rc = -1;
    if (bzf) {
      uint32_t  v;
      rc = BZ2_bzread(bzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be32toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, int64_t & val)
  {
    int  rc = -1;
    if (bzf) {
      uint64_t  v;
      if (BZ2_bzread(bzf,(void *)&v,sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, uint64_t & val)
  {
    int  rc = -1;
    if (bzf) {
      uint64_t  v;
      if (BZ2_bzread(bzf,(void *)&v,sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }

#if (defined(__unix__) || defined(unix) || defined(__unix)) || (defined(__APPLE__))
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, float & val)
  {
    int  rc = -1;
    if (bzf) {
      char buf[4];
      rc = BZ2_bzread(bzf, buf, 4);
      if (rc == 4) {
        XDRUtils::Decode(buf, val);
      }
      else {
        rc = -1;
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, double & val)
  {
    int  rc = -1;
    if (bzf) {
      char  buf[8];
      rc = BZ2_bzread(bzf,buf,8);
      if (rc == 8) {
        XDRUtils::Decode(buf, val);
      }
      else {
        rc = -1;
      }
    }
    return rc;
  }

#elif (defined(_WIN32) || defined(_WIN64))

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, float & val)
  {
    uint32_t  *p = (uint32_t *)&val;
    return BZRead(bzf, *p);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, double & val)
  {
    uint64_t  *p = (uint64_t *)&val;
    return BZRead(bzf, *p);
  }
  
#endif
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int BZ2IO::BZRead(BZFILE *bzf, std::string & s)
  {
    int  rc = -1;
    if (bzf) {
      int32_t  len;
      if (BZRead(bzf,len) == sizeof(len)) {
        rc = sizeof(len);
        char  *buf = (char *)calloc(1,len);
        if (buf) {
          if (BZ2_bzread(bzf,(void *)buf,len) == len) {
            rc += len;
            s.assign(buf,len);
          }
          else {
            rc = -1;
          }
          free(buf);
        }
      }
    }
    return(rc);
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
