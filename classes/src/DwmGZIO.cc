//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2020, 2024
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
//!  \file DwmGZIO.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::GZIO class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <netinet/in.h>
}

#include <cassert>

#include "DwmGZIO.hh"
#include "DwmPortability.hh"
#include "DwmXDRUtils.hh"

namespace Dwm {

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, char c)
  {
    int  rc = -1;
    if (gzf) { 
      rc = gzwrite(gzf,(void *)&c,sizeof(c));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, uint8_t c)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzwrite(gzf,(void *)&c,sizeof(c));
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, bool b)
  {
    if (gzf) {
      uint8_t  c = (b ? 1 : 0);
      return gzwrite(gzf, (void *)&c, sizeof(c));
    }
    return -1;
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, int16_t val)
  {
    int  rc = -1;
    if (gzf) {
      int16_t v = htobe16(val);
      rc = gzwrite(gzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, uint16_t val)
  {
    int  rc = -1;
    if (gzf) {
      uint16_t v = htobe16(val);
      rc = gzwrite(gzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, int32_t val)
  {
    int  rc = -1;
    if (gzf) {
      int32_t v = htobe32(val);
      rc = gzwrite(gzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, uint32_t val)
  {
    int  rc = -1;
    if (gzf) {
      uint32_t v = htobe32(val);
      rc = gzwrite(gzf,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, const int64_t & val)
  {
    int  rc = -1;
    if (gzf) {
      int64_t v = htobe64(val);
      rc = gzwrite(gzf,(void *)&v,sizeof(v));
      if (rc != sizeof(v))
        rc = -1;
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, const uint64_t & val)
  {
    int  rc = -1;
    if (gzf) {
      uint64_t v = htobe64(val);
      rc = gzwrite(gzf,(void *)&v,sizeof(v));
      if (rc != sizeof(v))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, float val)
  {
    if (! gzf)
      return(-1);
    
    char     *buf = 0;
    uint32_t  xe = XDRUtils::Encode(val,&buf);
    int   rc = gzwrite(gzf,buf,xe);
    free(buf);
    
    if (rc < 4)
      rc = -1;

    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, const double & val)
  {
    if (! gzf)
      return(-1);
    
    char  *buf = 0;
    uint32_t  xe = XDRUtils::Encode(val,&buf);
    int   rc = gzwrite(gzf,buf,xe);
    free(buf);
    
    if (rc < 8)
      rc = -1;

    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Write(gzFile gzf, const std::string & s)
  {
    int  rc = -1;
    
    if (gzf) {
      int64_t  len = s.length();
      if (Write(gzf,len) == sizeof(len)) {
        rc = sizeof(len);
        if (gzwrite(gzf,(void *)s.c_str(),len) == len)
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
  int GZIO::Read(gzFile gzf, char & c)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzread(gzf,(void *)&c,sizeof(c));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, uint8_t & c)
  {
    int  rc = -1;
    if (gzf) {
      rc = gzread(gzf,(void *)&c,sizeof(c));
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, bool & b)
  {
    if (gzf) {
      uint8_t  c;
      if (gzread(gzf, (void *)&c, sizeof(c)) == sizeof(c)) {
        b = (c ? true : false);
        return sizeof(c);
      }
    }
    return -1;
  }
      
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, int16_t & val)
  {
    int  rc = -1;
    if (gzf) {
      int16_t  v;
      rc = gzread(gzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v)) {
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, uint16_t & val)
  {
    int  rc = -1;
    if (gzf) {
      uint16_t  v;
      rc = gzread(gzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v)) {
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, int32_t & val)
  {
    int  rc = -1;
    if (gzf) {
      int32_t  v;
      rc = gzread(gzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v)) {
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, uint32_t & val)
  {
    int  rc = -1;
    if (gzf) {
      uint32_t  v;
      rc = gzread(gzf,(void *)&v,sizeof(v));
      if (rc == sizeof(v)) {
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, int64_t & val)
  {
    int  rc = -1;
    if (gzf) {
      uint64_t  v;
      if (gzread(gzf,(void *)&v,sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, uint64_t & val)
  {
    int  rc = -1;
    if (gzf) {
      uint64_t  v;
      if (gzread(gzf,(void *)&v,sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, float & val)
  {
    int  rc = -1;
    if (gzf) {
      char     buf[4];
      rc = gzread(gzf, buf, 4);
      if (rc == 4) {
        XDRUtils::Decode(buf, val);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, double & val)
  {
    int  rc = -1;
    if (gzf) {
      char     buf[8];
      rc = gzread(gzf, buf, 8);
      if (rc == 8) {
        XDRUtils::Decode(buf, val);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int GZIO::Read(gzFile gzf, std::string & s)
  {
    int  rc = -1;
    s = "";
    
    if (gzf) {
      int64_t  len;
      if (Read(gzf,len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          char  *buf = (char *)calloc(1,len);
          if (buf) {
            if (gzread(gzf,(void *)buf,len) == len) {
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
