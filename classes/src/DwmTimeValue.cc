//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2020
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmTimeValue.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::TimeValue class implementation
//---------------------------------------------------------------------------

#include <cassert>

#include "DwmGZIO.hh"
#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmTimeValue.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue::TimeValue(bool setNow)
    : _secs(0), _usecs(0)
  {
    if (setNow)
      SetNow();
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue::TimeValue(const struct timeval & tv)
  {
    Set(tv);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue::TimeValue(const struct timespec & ts)
  {
    Set(ts.tv_sec, ts.tv_nsec / 1000);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue::TimeValue(uint32_t secs, uint32_t usecs)
  {
    Set(secs, usecs);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void TimeValue::SetNow()
  {
      struct timeval  tp;
      gettimeofday(&tp, 0);
      _secs  = tp.tv_sec;
      _usecs = tp.tv_usec;
      return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void TimeValue::Set(uint32_t secs, uint32_t usecs)
  {
    _secs  = secs;
    _usecs = usecs;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void TimeValue::Set(const struct timeval & tv)
  {
    _secs  = tv.tv_sec & 0xFFFFFFFF;
    _usecs = tv.tv_usec & 0xFFFFFFFF;
    return;
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t TimeValue::Secs() const
  {
    return(_secs);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t TimeValue::Usecs() const
  {
    return(_usecs);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  struct tm TimeValue::LocalTime() const
  {
    struct tm  rc;
    time_t  t = _secs;
    localtime_r(&t, &rc);
    return(rc);
  }
      
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeValue::operator < (const TimeValue & tv) const
  {
    return((_secs * 1000000ULL) + _usecs <
           (tv._secs * 1000000ULL) + tv._usecs);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeValue::operator <= (const TimeValue & tv) const
  {
    return((_secs * 1000000ULL) + _usecs <=
           (tv._secs * 1000000ULL) + tv._usecs);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeValue::operator > (const TimeValue & tv) const
  {
    return((double)(*this) > (double)tv);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeValue::operator >= (const TimeValue & tv) const
  {
    return((_secs * 1000000ULL) + _usecs >=
           (tv._secs * 1000000ULL) + tv._usecs);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeValue::operator == (const TimeValue & tv) const
  {
    return((_secs == tv._secs) && (_usecs == tv._usecs));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue & TimeValue::operator += (const TimeValue & tv)
  {
    uint64_t  v = (_secs * 1000000ULL) + _usecs;
    v += (tv._secs * 1000000ULL) + tv._usecs;
    _secs  = v / 1000000ULL;
    _usecs = v % 1000000ULL;
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue & TimeValue::operator -= (const TimeValue & tv)
  {
    uint64_t  v = _secs;
    v *= 1000000ULL;
    v+= _usecs;

    v -= (uint64_t)((uint64_t)tv._secs * 1000000ULL);
    v -= tv._usecs;

    _secs = v / 1000000ULL;
    _usecs = v % 1000000ULL;
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue & TimeValue::operator *= (uint32_t x)
  {
    uint64_t  v = (_secs * 1000000ULL) + _usecs;
    v *= x;
    _secs  = v / 1000000ULL;
    _usecs = v % 1000000ULL;
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue & TimeValue::operator /= (uint32_t x)
  {
    assert(x > 0);
    uint64_t  v = (_secs * 1000000ULL) + _usecs;
    v /= x;
    _secs  = v / 1000000ULL;
    _usecs = v % 1000000ULL;
    return(*this);
  } 
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t TimeValue::StreamedLength() const
  {
    return(8);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & TimeValue::Read(std::istream & is)
  {
    _secs  = 0;
    _usecs = 0;
    if (is)
      if (StreamIO::Read(is, _secs))
        StreamIO::Read(is, _usecs);
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & TimeValue::Write(std::ostream & os) const
  {
    if (os)
      if (StreamIO::Write(os, _secs))
        StreamIO::Write(os, _usecs);
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t TimeValue::Read(FILE *f)
  {
    size_t  rc = 0;
    _secs  = 0;
    _usecs = 0;
    if (f)
      if (FileIO::Read(f, _secs) > 0)
        if (FileIO::Read(f, _usecs) > 0)
          rc = 1;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t TimeValue::Write(FILE *f) const
  {
    size_t  rc = 0;
    if (f)
      if (FileIO::Write(f, _secs) > 0)
        if (FileIO::Write(f, _usecs) > 0)
          rc = 1;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t TimeValue::Read(int fd)
  {
    ssize_t  rc = -1;
    _secs  = 0;
    _usecs = 0;
    if (fd >= 0) {
      ssize_t  bytesRead = DescriptorIO::Read(fd, _secs);
      if (bytesRead == sizeof(_secs)) {
        rc = bytesRead;
        bytesRead = DescriptorIO::Read(fd, _usecs);
        if (bytesRead == sizeof(_usecs))
          rc += bytesRead;
        else
          rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t TimeValue::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (fd >= 0) {
      ssize_t  bytesWritten = DescriptorIO::Write(fd, _secs);
      if (bytesWritten == sizeof(_secs)) {
        rc = bytesWritten;
        bytesWritten = DescriptorIO::Write(fd, _usecs);
        if (bytesWritten == sizeof(_usecs))
          rc += bytesWritten;
        else
          rc = -1;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeValue::Read(gzFile gzf)
  {
    int  rc = -1;
    _secs  = 0;
    _usecs = 0;
    if (gzf) {
      int  bytesRead = GZIO::Read(gzf, _secs);
      if (bytesRead == sizeof(_secs)) {
        rc = bytesRead;
        bytesRead = GZIO::Read(gzf, _usecs);
        if (bytesRead == sizeof(_usecs))
          rc += bytesRead;
        else
          rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeValue::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      int  bytesWritten = GZIO::Write(gzf, _secs);
      if (bytesWritten == sizeof(_secs)) {
        rc = bytesWritten;
        bytesWritten = GZIO::Write(gzf, _usecs);
        if (bytesWritten == sizeof(_usecs))
          rc += bytesWritten;
        else
          rc = -1;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeValue::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    _secs  = 0;
    _usecs = 0;
    if (bzf) {
      int  bytesRead = BZ2IO::BZRead(bzf, _secs);
      if (bytesRead == sizeof(_secs)) {
        rc = bytesRead;
        bytesRead = BZ2IO::BZRead(bzf, _usecs);
        if (bytesRead == sizeof(_usecs))
          rc += bytesRead;
        else
          rc = -1;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeValue::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      int  bytesWritten = BZ2IO::BZWrite(bzf, _secs);
      if (bytesWritten == sizeof(_secs)) {
        rc = bytesWritten;
        bytesWritten = BZ2IO::BZWrite(bzf, _usecs);
        if (bytesWritten == sizeof(_usecs))
          rc += bytesWritten;
        else
          rc = -1;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue::operator double () const
  {
    double  t = Secs();
    t += (double)Usecs() / 1000000.0;
    return(t);
  }


}  // namespace Dwm

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
Dwm::TimeValue operator + (const Dwm::TimeValue & tv1,
                           const Dwm::TimeValue & tv2)
{
  Dwm::TimeValue  rc(tv1);
  rc += tv2;
  return(rc);
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
Dwm::TimeValue operator - (const Dwm::TimeValue & tv1,
                           const Dwm::TimeValue & tv2)
{
  Dwm::TimeValue  rc(tv1);
  rc -= tv2;
  return(rc);
}


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
