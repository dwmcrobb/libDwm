//===========================================================================
// @(#) $Name$
// @(#) $Id: DwmIO.cc 11084 2020-09-06 04:57:40Z dwm $
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

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <fcntl.h>
}

#include <cassert>

#include "DwmSvnTag.hh"
#include "DwmSysLogger.hh"
#include "DwmIO.hh"
#include "DwmPortability.hh"
#include "DwmXDRUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIO.cc 11084 $");

namespace Dwm {

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, char c)
  {
    if (os)
      os.write(&c,sizeof(c));
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, uint8_t c)
  {
    if (os)
      os.write((caddr_t)&c,sizeof(c));
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, bool b)
  {
    uint8_t  v = b;
    return(IO::Write(os, v));
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, int16_t val)
  {
    if (os) {
      int16_t  v = htobe16(val);
      os.write((caddr_t)&v,sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, uint16_t val)
  {
    if (os) {
      uint16_t v = htobe16(val);
      os.write((caddr_t)&v,sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, int32_t val)
  {
    if (os) {
      int32_t v = htobe32(val);
      os.write((caddr_t)&v,sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, uint32_t val)
  {
    if (os) {
      uint32_t v = htobe32(val);
      os.write((caddr_t)&v,sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, const int64_t & val)
  {
    if (os) {
      uint64_t v = htobe64(val);
      os.write((caddr_t)&v,sizeof(v));
    }
    return(os);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, const uint64_t & val)
  {
    if (os) {
      uint64_t v = htobe64(val);
      os.write((caddr_t)&v,sizeof(v));
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & IO::Write(std::ostream & os, float val)
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
  std::ostream & IO::Write(std::ostream & os, const double & val)
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
  std::ostream & IO::Write(std::ostream & os, const std::string & s)
  {
    if (os) {
      uint32_t  len = s.size();
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
  std::istream & IO::Read(std::istream & is, char & c)
  {
    if (is)
      is.read(&c,sizeof(c));
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, uint8_t & c)
  {
    if (is)
      is.read((caddr_t)&c,sizeof(c));
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, bool & b)
  {
    uint8_t  c;
    if (IO::Read(is, c))
      b = (c ? true : false);
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, int16_t & val)
  {
    if (is) {
      int16_t  v;
      if (is.read((caddr_t)&v,sizeof(v)))
        val = be16toh(v);
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, uint16_t & val)
  {
    if (is) {
      uint16_t  v;
      if (is.read((caddr_t)&v,sizeof(v)))
        val = be16toh(v);
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, int32_t & val)
  {
    if (is) {
      int32_t  v;
      if (is.read((caddr_t)&v,sizeof(v)))
        val = be32toh(v);
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, uint32_t & val)
  {
    if (is) {
      uint32_t  v;
      if (is.read((caddr_t)&v,sizeof(v)))
        val = be32toh(v);
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, int64_t & val)
  {
    if (is) {
      int64_t  v;
      if (is.read((caddr_t)&v,sizeof(v))) {
        val = be64toh(v);
      }
    }
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, uint64_t & val)
  {
    if (is) {
      uint64_t  v;
      if (is.read((caddr_t)&v,sizeof(v))) {
        val = be64toh(v);
      }
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & IO::Read(std::istream & is, float & val)
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
  std::istream & IO::Read(std::istream & is, double & val)
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
  std::istream & IO::Read(std::istream & is, std::string & s)
  {
    s.clear();
    if (is) {
      uint32_t  len;
      IO::Read(is, len);
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
    return(is);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, char c)
  {
    ssize_t  rc = -1;
    if (fd >= 0)
      rc = write(fd,(void *)&c,sizeof(c));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, uint8_t c)
  {
    ssize_t  rc = -1;
    if (fd >= 0)
      rc = write(fd,(void *)&c,sizeof(c));
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Write(int fd, bool b)
  {
    uint8_t  c = b;
    return(IO::Write(fd, c));
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, int16_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int16_t v = htobe16(val);
      rc = write(fd,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, uint16_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint16_t v = htobe16(val);
      rc = write(fd,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, int32_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int32_t v = htobe32(val);
      rc = write(fd,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, uint32_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint32_t v = htobe32(val);
      rc = write(fd,(void *)&v,sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, const int64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int64_t v = htobe64(val);
      rc = write(fd,(void *)&v,sizeof(v));
      if (rc != sizeof(v))
        rc = -1;
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, const uint64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t v = htobe64(val);
      rc = write(fd,(void *)&v,sizeof(v));
      if (rc != sizeof(v))
        rc = -1;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Write(int fd, float val)
  {
    if (fd < 0)
      return(-1);
    
    char     *buf = 0;
    uint32_t  xe = XDRUtils::Encode(val, &buf);
    ssize_t   rc = write(fd, buf, xe);
    free(buf);
    
    if (rc < 4)
      rc = -1;

    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Write(int fd, const double & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      char  *buf = 0;
      uint32_t  xe = XDRUtils::Encode(val, &buf);
      rc = write(fd, buf, xe);
      free(buf);
      if (rc < 8) {
        rc = -1;
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Write(int fd, const std::string & s)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint32_t  len = s.size();
      if (Write(fd,len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          if (Write(fd,(const void *)s.c_str(),len) == len) {
            rc += len;
          }
          else {
            rc = -1;
          }
        }
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, char & c)
  {
    ssize_t  rc = -1;
    if (fd >= 0)
      rc = read(fd,(void *)&c,sizeof(c));
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, uint8_t & c)
  {
    ssize_t  rc = -1;
    if (fd >= 0)
      rc = read(fd,(void *)&c,sizeof(c));
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Read(int fd, bool & b)
  {
    uint8_t  c;
    ssize_t  rc = IO::Read(fd, c);
    if (rc > 0)
      b = c ? true : false;
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, int16_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int16_t  v;
      rc = read(fd,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be16toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, uint16_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint16_t  v;
      rc = read(fd,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be16toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, int32_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int32_t  v;
      rc = read(fd,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be32toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, uint32_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint32_t  v;
      rc = read(fd,(void *)&v,sizeof(v));
      if (rc == sizeof(v))
        val = be32toh(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, int64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t  v;
      if (read(fd,(void *)&v,sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, uint64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t  v;
      if (read(fd,(void *)&v,sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Read(int fd, float & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      char     buf[4];
      rc = read(fd,buf,4);
      if (rc == 4) {
        XDRUtils::Decode(buf,val);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Read(int fd, double & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      char     buf[8];
      rc = read(fd,buf,8);
      if (rc == 8) {
        XDRUtils::Decode(buf,val);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t IO::Read(int fd, std::string & s)
  {
    s.clear();
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint32_t  len;
      if (Read(fd, len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          try {
            s.resize(len);
            ssize_t  bytesRead = Read(fd, s.data(), len);
            if (bytesRead == len) {
              rc += len;
            }
            else {
              s.clear();
              rc = -1;
            }
          }
          catch (const std::exception & ex) {
            Syslog(LOG_ERR, "Exception: %s", ex.what());
            rc = -1;
          }
          catch (...) {
            Syslog(LOG_ERR, "Exception");
            rc = -1;
          }
        }
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, char c)
  {
    size_t  rc = 0;
    if (f)
      rc = fwrite((void *)&c,sizeof(c),1,f);
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, uint8_t c)
  {
    size_t  rc = 0;
    if (f)
      rc = fwrite((void *)&c,sizeof(c),1,f);
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IO::Write(FILE * f, bool b)
  {
    uint8_t  c = b;
    return(IO::Write(f, c));
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, int16_t val)
  {
    size_t rc = 0;
    if (f) {
      int16_t v = htobe16(val);
      if (fwrite((void *)&v,sizeof(v),1,f) == 1)
        rc = sizeof(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, uint16_t val)
  {
    size_t rc = 0;
    if (f) {
      uint16_t v = htobe16(val);
      if (fwrite((void *)&v,sizeof(v),1,f) == 1)
        rc = sizeof(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, int32_t val)
  {
    size_t rc = 0;
    if (f) {
      int32_t v = htobe32(val);
      if (fwrite((void *)&v,sizeof(v),1,f) == 1)
        rc = sizeof(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, uint32_t val)
  {
    size_t rc = 0;
    if (f) {
      uint32_t v = htobe32(val);
      if (fwrite((void *)&v,sizeof(v),1,f) == 1)
        rc = sizeof(v);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, const int64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      int64_t  v = htobe64(val);
      if (fwrite((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, const uint64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint64_t  v = htobe64(val);
      if (fwrite((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IO::Write(FILE * f, float val)
  {
    size_t  rc = 0;
    if (f) {
      char  *buf = 0;
      rc = XDRUtils::Encode(val,&buf);
      if (fwrite((void *)buf,rc,1,f) != 1)
        rc = 0;
      free(buf);
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IO::Write(FILE * f, const double & val)
  {
    size_t  rc = 0;
    
    if (f) {
      char  *buf = 0;
      rc = XDRUtils::Encode(val,&buf);
      if (fwrite((void *)buf,rc,1,f) != 1)
        rc = 0;
      free(buf);
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Write(FILE * f, const std::string & s)
  {
    size_t  rc = 0;
    if (f) {
      uint32_t len = s.size();
      if (Write(f,len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          if (fwrite((void *)s.c_str(),len,1,f) == 1) {
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
  size_t IO::Read(FILE * f, char & c)
  {
    size_t rc = 0;
    if (f) 
      rc = fread((void *)&c,sizeof(c),1,f);
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, uint8_t & c)
  {
    size_t  rc = 0;
    if (f) 
      rc = fread((void *)&c,sizeof(c),1,f);
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IO::Read(FILE * f, bool & b)
  {
    uint8_t  c;
    size_t  rc = IO::Read(f, c);
    if (rc > 0) 
      b = c ? true : false;
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, int16_t & val)
  {
    size_t  rc = 0;
    if (f) {
      int16_t v;
      if (fread((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, uint16_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint16_t v;
      if (fread((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, int32_t & val)
  {
    size_t  rc = 0;
    if (f) {
      int32_t v;
      if (fread((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, uint32_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint32_t v;
      if (fread((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, int64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint64_t  v;
      if (fread((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
        val = be64toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, uint64_t & val)
  {
    size_t  rc = 0;
    if (f) {
      uint64_t v;
      if (fread((void *)&v,sizeof(v),1,f) == 1) {
        rc = sizeof(v);
        val = be64toh(v);
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IO::Read(FILE * f, float & val)
  {
    size_t  rc = 0;
    
    if (f) {
      char   buf[4];
      if (fread((void *)buf,4,1,f) == 1) {
        XDRUtils::Decode(buf,val);
        rc = 4;
      }
    }
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t IO::Read(FILE * f, double & val)
  {
    size_t  rc = 0;
    
    if (f) {
      char   buf[8];
      if (fread((void *)buf,8,1,f) == 1) {
        XDRUtils::Decode(buf,val);
        rc = 8;
      }
    }

    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t IO::Read(FILE * f, std::string & s)
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

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Read(int fd, void *buf, size_t buflen)
  {
    ssize_t  totalBytesRead = 0;
    if (buf && (buflen > 0) && (fd >= 0)) {
      uint8_t  *bufp = (uint8_t *)buf;
      while (totalBytesRead < buflen) {
        ssize_t  bytesRead = read(fd, (void *)bufp, buflen - totalBytesRead);
        if (bytesRead == buflen) {
          totalBytesRead = bytesRead;
          break;
        }
        else if (bytesRead <= 0) {
          break;
        }
        else {
          bufp += bytesRead;
          totalBytesRead += bytesRead;
        }
      }
    }
    return ((totalBytesRead == buflen) ? buflen : -1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t IO::Write(int fd, const void *buf, size_t buflen)
  {
    ssize_t  totalBytesWritten = 0;
    if (buf && (buflen > 0) && (fd >= 0)) {
      const uint8_t  *bufp = (const uint8_t *)buf;
      while (totalBytesWritten < buflen) {
        ssize_t  bytesWritten = write(fd, (void *)bufp,
                                      buflen - totalBytesWritten);
        if (bytesWritten == buflen) {
          totalBytesWritten = bytesWritten;
          break;
        }
        else if (bytesWritten < 0) {
          break;
        }
        else {
          bufp += bytesWritten;
          totalBytesWritten += bytesWritten;
        }
      }
    }
    return ((totalBytesWritten == buflen) ? buflen : -1);
  }
  

}  // namespace Dwm
