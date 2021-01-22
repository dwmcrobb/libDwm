//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmStrptime.cc 11136 $
// @(#) $Id: DwmStrptime.cc 11136 2020-09-07 23:46:30Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file DwmStrptime.cc
//!  \author Daniel W. McRobb
//!  \brief OpenBSD strptime() doesn't accept %Z in the format string; use
//!  one mostly borrowed from FreeBSD.
//---------------------------------------------------------------------------

#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmStrptime.cc 11136 $");

#if (defined(__OpenBSD__))

extern "C" {
  #include <sys/cdefs.h>
  #include <sys/localedef.h>
  #include <time.h>
  #include <ctype.h>
  #include <locale.h>
  #include <errno.h>
  #include <stdlib.h>
  #include <string.h>
  #include <pthread.h>
}

namespace Dwm {

  #define _ctloc(x)     (_CurrentTimeLocale->x)
  #define asizeof(a)	  (sizeof (a) / sizeof ((a)[0]))

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static char *_Strptime(const char *buf, const char *fmt, 
                         struct tm *tm, int *GMTp)
  {
    char	     c;
    const char  *ptr;
    int	         i, len;
    int          Ealternative, Oalternative;
		
    ptr = fmt;
    while (*ptr != 0) {
      if (*buf == 0)
        break;
			
      c = *ptr++;
			
      if (c != '%') {
        if (isspace((unsigned char)c))
          while (*buf != 0 && isspace((unsigned char)*buf))
            buf++;
        else if (c != *buf++)
          return 0;
        continue;
      }
			
      Ealternative = 0;
      Oalternative = 0;
    label:
      c = *ptr++;
      switch (c) {
        case 0:
        case '%':
          if (*buf++ != '%')
            return 0;
          break;
				
        case '+':
          buf = _Strptime(buf, _ctloc(d_fmt), tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'C':
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          /* XXX This will break for 3-digit centuries. */
          len = 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (i < 19)
            return 0;
				
          tm->tm_year = i * 100 - 1900;
          break;
				
        case 'c':
          buf = _Strptime(buf, _ctloc(d_t_fmt), tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'D':
          buf = _Strptime(buf, "%m/%d/%y", tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'E':
          if (Ealternative || Oalternative)
            break;
          Ealternative++;
          goto label;
				
        case 'O':
          if (Ealternative || Oalternative)
            break;
          Oalternative++;
          goto label;
				
        case 'F':
          buf = _Strptime(buf, "%Y-%m-%d", tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'R':
          buf = _Strptime(buf, "%H:%M", tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'r':
          buf = _Strptime(buf, "%I:%M:%S %p", tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'T':
          buf = _Strptime(buf, "%H:%M:%S", tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'X':
          buf = _Strptime(buf, _ctloc(t_fmt), tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'x':
          buf = _Strptime(buf, _ctloc(d_fmt), tm, GMTp);
          if (buf == 0)
            return 0;
          break;
				
        case 'j':
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = 3;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (i < 1 || i > 366)
            return 0;
				
          tm->tm_yday = i - 1;
          break;
				
        case 'M':
        case 'S':
          if (*buf == 0 || isspace((unsigned char)*buf))
            break;
				
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
				
          if (c == 'M') {
            if (i > 59)
              return 0;
            tm->tm_min = i;
          } else {
            if (i > 60)
              return 0;
            tm->tm_sec = i;
          }
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 'H':
        case 'I':
        case 'k':
        case 'l':
          /*
           * Of these, %l is the only specifier explicitly
           * documented as not being zero-padded.  However,
           * there is no harm in allowing zero-padding.
           *
           * XXX The %l specifier may gobble one too many
           * digits if used incorrectly.
           */
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (c == 'H' || c == 'k') {
            if (i > 23)
              return 0;
          } else if (i > 12)
            return 0;
				
          tm->tm_hour = i;
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 'p':
          /*
           * XXX This is bogus if parsed before hour-related
           * specifiers.
           */
          len = strlen(_ctloc(am_pm[0]));
          if (strncasecmp(buf, _ctloc(am_pm[0]), len) == 0) {
            if (tm->tm_hour > 12)
              return 0;
            if (tm->tm_hour == 12)
              tm->tm_hour = 0;
            buf += len;
            break;
          }
				
          len = strlen(_ctloc(am_pm[1]));
          if (strncasecmp(buf, _ctloc(am_pm[1]), len) == 0) {
            if (tm->tm_hour > 12)
              return 0;
            if (tm->tm_hour != 12)
              tm->tm_hour += 12;
            buf += len;
            break;
          }
				
          return 0;
				
        case 'A':
        case 'a':
          for (i = 0; (unsigned int)i < asizeof(_ctloc(day)); i++) {
            len = strlen(_ctloc(day[i]));
            if (strncasecmp(buf, _ctloc(day[i]),
                            len) == 0)
              break;
            len = strlen(_ctloc(abday[i]));
            if (strncasecmp(buf, _ctloc(abday[i]),
                            len) == 0)
              break;
          }
          if (i == asizeof(_ctloc(day)))
            return 0;
				
          tm->tm_wday = i;
          buf += len;
          break;
				
        case 'U':
        case 'W':
          /*
           * XXX This is bogus, as we can not assume any valid
           * information present in the tm structure at this
           * point to calculate a real value, so just check the
           * range for now.
           */
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (i > 53)
            return 0;
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 'w':
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          i = *buf - '0';
          if (i > 6)
            return 0;
				
          tm->tm_wday = i;
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 'd':
        case 'e':
          /*
           * The %e specifier is explicitly documented as not
           * being zero-padded but there is no harm in allowing
           * such padding.
           *
           * XXX The %e specifier may gobble one too many
           * digits if used incorrectly.
           */
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (i > 31)
            return 0;
				
          tm->tm_mday = i;
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 'B':
        case 'b':
        case 'h':
          for (i = 0; (unsigned int)i < asizeof(_ctloc(mon)); i++) {
            len = strlen(_ctloc(mon[i]));
            if (strncasecmp(buf, _ctloc(mon[i]),
                            len) == 0)
              break;
            len = strlen(_ctloc(abmon[i]));
            if (strncasecmp(buf, _ctloc(abmon[i]),
                            len) == 0)
              break;
          }
          if (i == asizeof(_ctloc(mon)))
            return 0;
				
          tm->tm_mon = i;
          buf += len;
          break;
				
        case 'm':
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (i < 1 || i > 12)
            return 0;
				
          tm->tm_mon = i - 1;
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 's':
          {
            char *cp;
            int sverrno;
            long n;
            time_t t;
					
            sverrno = errno;
            errno = 0;
            n = strtol(buf, &cp, 10);
            if (errno == ERANGE || (long)(t = n) != n) {
              errno = sverrno;
              return 0;
            }
            errno = sverrno;
            buf = cp;
            gmtime_r(&t, tm);
            *GMTp = 1;
          }
          break;
				
        case 'Y':
        case 'y':
          if (*buf == 0 || isspace((unsigned char)*buf))
            break;
				
          if (!isdigit((unsigned char)*buf))
            return 0;
				
          len = (c == 'Y') ? 4 : 2;
          for (i = 0; len && *buf != 0 && isdigit((unsigned char)*buf); buf++) {
            i *= 10;
            i += *buf - '0';
            len--;
          }
          if (c == 'Y')
            i -= 1900;
          if (c == 'y' && i < 69)
            i += 100;
          if (i < 0)
            return 0;
				
          tm->tm_year = i;
				
          if (*buf != 0 && isspace((unsigned char)*buf))
            while (*ptr != 0 && !isspace((unsigned char)*ptr))
              ptr++;
          break;
				
        case 'Z':
          {
            const char *cp;
            char *zonestr;
					
            for (cp = buf; *cp && isupper((unsigned char)*cp); ++cp) {/*empty*/}
            if (cp - buf) {
              zonestr = (char *)alloca(cp - buf + 1);
              strncpy(zonestr, buf, cp - buf);
              zonestr[cp - buf] = '\0';
              tzset();
              if (0 == strcmp(zonestr, "GMT")) {
                *GMTp = 1;
              } else if (0 == strcmp(zonestr, tzname[0])) {
                tm->tm_isdst = 0;
              } else if (0 == strcmp(zonestr, tzname[1])) {
                tm->tm_isdst = 1;
              } else {
                return 0;
              }
              buf += cp - buf;
            }
          }
          break;
      }
    }
    return (char *)buf;
  }
	
	
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  char *
  Strptime(const char * __restrict buf, const char * __restrict fmt,
           struct tm * __restrict tm)
  {
    char *ret;
    int gmt;
		
    gmt = 0;
    ret = _Strptime(buf, fmt, tm, &gmt);
    if (ret && gmt) {
      time_t t = timegm(tm);
      localtime_r(&t, tm);
    }
		
    return (ret);
  }

}  // namespace Dwm

#else

#include <time.h>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  char *Strptime(const char * __restrict buf, const char * __restrict fmt,
                 struct tm * __restrict tm)
  {
    return(strptime(buf, fmt, tm));
  }
	
} // namespace Dwm

#endif


/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
