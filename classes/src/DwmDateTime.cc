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
//!  \file DwmDateTime.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::DateTime class implementation
//---------------------------------------------------------------------------

#include <cstring>
#include <iostream>

#include "DwmDateTime.hh"
#include "DwmStrptime.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DateTime::DateTime(bool now)
  {
    time_t  t = 0;
    if (now)
      t = time((time_t *)0);
    localtime_r(&t, &_tm);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DateTime::DateTime(const Dwm::TimeValue & tv)
  {
    time_t  t = tv.Secs();
    memset(&_tm, 0, sizeof(_tm));
    localtime_r(&t, &_tm);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DateTime::DateTime(const Dwm::TimeValue64 & tv)
  {
    time_t  t = tv.Secs();
    memset(&_tm, 0, sizeof(_tm));
    localtime_r(&t, &_tm);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DateTime::IsDaylightSavings() const
  {
    return(_tm.tm_isdst);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t DateTime::Sec() const
  {
    return(_tm.tm_sec);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t DateTime::Min() const
  {
    return(_tm.tm_min);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t DateTime::Hour() const
  {
    return(_tm.tm_hour);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t DateTime::DayOfMonth() const
  {
    return(_tm.tm_mday);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t DateTime::Month() const
  {
    return(_tm.tm_mon + 1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t DateTime::Year() const
  {
    return(_tm.tm_year + 1900);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t DateTime::DayOfWeek() const
  {
    return(_tm.tm_wday);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DateTime::Formatted(const std::string & format) const
  {
    char  buf[4096];
    strftime(buf,4096,format.c_str(),&_tm);
    return(buf);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DateTime::Parse(const std::string & s, const std::string & format)
  {
    bool  rc = false;
    memset(&_tm, 0, sizeof(_tm));
    if ((! s.empty()) && (! format.empty())) {
      char *e = Strptime(s.c_str(), format.c_str(), &_tm);
      if (e) {
        rc = true;
#ifdef __linux__
        struct tm tms = _tm;
        char *tz = strdup("UTC");
        tms.tm_zone = tz;
        time_t  lt = -tms.tm_gmtoff;
        lt += timegm(&tms);
        localtime_r(&lt, &_tm);
        free(tz);
#endif
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint16_t DateTime::DayOfYear() const
  {
    return(_tm.tm_yday);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DateTime::TimeZone() const
  {
    return(_tm.tm_zone);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DateTime::operator == (const DateTime & dt) const
  {
    struct tm  lhsTm = _tm;
    struct tm  rhsTm = dt._tm;
    return(timegm(&lhsTm) == timegm(&rhsTm));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue DateTime::GetTimeValue() const
  {
    struct tm  tmCopy = _tm;
    time_t  t = mktime(&tmCopy);
    return Dwm::TimeValue((uint32_t)t, 0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue64 DateTime::GetTimeValue64() const
  {
    struct tm  tmCopy = _tm;
    time_t  t = mktime(&tmCopy);
    return Dwm::TimeValue64(t, 0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DateTime::operator < (const DateTime & dt) const
  {
#ifndef __linux__
    struct tm  myTm = _tm;
    time_t     myT = timegm(&myTm);
    struct tm  dtTm = dt._tm;
    time_t     dtT = timegm(&dtTm);
    return (myT < dtT);
#else
    struct tm tmlhs = _tm;
    char *tz = strdup("UTC");
    tmlhs.tm_zone = tz;
    time_t  lt = -tmlhs.tm_gmtoff;
    lt += timegm(&tmlhs);
    struct tm tmrhs = dt._tm;
    tmrhs.tm_zone = tz;
    time_t  rt = -tmrhs.tm_gmtoff;
    rt += timegm(&tmrhs);
    free(tz);
    return (lt < rt);
#endif
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
