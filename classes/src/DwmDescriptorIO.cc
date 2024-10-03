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
//!  \file DwmDescriptorIO.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::DescriptorIO class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <fcntl.h>
}

#include <cstdlib>
#include <cassert>

#include "DwmSysLogger.hh"
#include "DwmDescriptorIO.hh"
#include "DwmPortability.hh"
#include "DwmXDRUtils.hh"

namespace Dwm {

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, char c)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = write(fd,(void *)&c,sizeof(c));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, int8_t c)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = write(fd, (void *)&c, sizeof(c));
    }
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, uint8_t c)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = write(fd, (void *)&c, sizeof(c));
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, bool b)
  {
    uint8_t  c = b;
    return(DescriptorIO::Write(fd, c));
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, int16_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int16_t v = htobe16(val);
      rc = write(fd, (void *)&v, sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, uint16_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint16_t v = htobe16(val);
      rc = write(fd, (void *)&v, sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, int32_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int32_t v = htobe32(val);
      rc = write(fd, (void *)&v, sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, uint32_t val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint32_t v = htobe32(val);
      rc = write(fd, (void *)&v, sizeof(v));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, const int64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int64_t v = htobe64(val);
      rc = write(fd, (void *)&v, sizeof(v));
      if (rc != sizeof(v)) {
        rc = -1;
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, const uint64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t v = htobe64(val);
      rc = write(fd, (void *)&v, sizeof(v));
      if (rc != sizeof(v)) {
        rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, float val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      char     *buf = 0;
      uint32_t  xe = XDRUtils::Encode(val, &buf);
      rc = write(fd, buf, xe);
      free(buf);
      if (rc < 4) {
        rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Write(int fd, const double & val)
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
  ssize_t DescriptorIO::Write(int fd, const std::string & s)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t  len = s.size();
      if (Write(fd,len) == sizeof(len)) {
        rc = sizeof(len);
        if (len > 0) {
          if (Write(fd, (const void *)s.c_str(), len) == len) {
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
  ssize_t DescriptorIO::Read(int fd, char & c)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = read(fd, (void *)&c, sizeof(c));
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, int8_t & c)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = read(fd, (void *)&c, sizeof(c));
    }
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, uint8_t & c)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      rc = read(fd, (void *)&c, sizeof(c));
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, bool & b)
  {
    uint8_t  c;
    ssize_t  rc = DescriptorIO::Read(fd, c);
    if (rc > 0) {
      b = c ? true : false;
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, int16_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int16_t  v;
      rc = read(fd, (void *)&v, sizeof(v));
      if (rc == sizeof(v)) {
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, uint16_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint16_t  v;
      rc = read(fd, (void *)&v, sizeof(v));
      if (rc == sizeof(v)) {
        val = be16toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, int32_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      int32_t  v;
      rc = read(fd, (void *)&v, sizeof(v));
      if (rc == sizeof(v)) {
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, uint32_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint32_t  v;
      rc = read(fd, (void *)&v, sizeof(v));
      if (rc == sizeof(v)) {
        val = be32toh(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, int64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t  v;
      if (read(fd, (void *)&v, sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, uint64_t & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t  v;
      if (read(fd, (void *)&v, sizeof(v)) == sizeof(v)) {
        val = be64toh(v);
        rc = sizeof(v);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, float & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      char     buf[4];
      rc = read(fd, buf, 4);
      if (rc == 4) {
        XDRUtils::Decode(buf, val);
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, double & val)
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      char     buf[8];
      rc = read(fd, buf, 8);
      if (rc == 8) {
        XDRUtils::Decode(buf, val);
      }
    }
    return(rc);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, std::string & s)
  {
    s.clear();
    ssize_t  rc = -1;
    if (fd >= 0) {
      uint64_t  len;
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
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t DescriptorIO::Read(int fd, void *buf, size_t buflen)
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
  ssize_t DescriptorIO::Write(int fd, const void *buf, size_t buflen)
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


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
