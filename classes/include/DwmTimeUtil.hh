//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1999-2007
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
//!  \file DwmTimeUtil.hh
//!  \brief Dwm::TimeUtil class definition
//---------------------------------------------------------------------------

#ifndef _DWMTIMEUTIL_HH_
#define _DWMTIMEUTIL_HH_

extern "C" {
#include <time.h>
#include <sys/time.h>
}

#include <string>

namespace Dwm {
  
  //--------------------------------------------------------------------------
  //      class TimeUtil 
  //--------------------------------------------------------------------------
  //!  This class is just a collection of time utility functions.
  //--------------------------------------------------------------------------
  class TimeUtil
  {
  public:
    //------------------------------------------------------------------------
    //!  This is just a convenient wrapper for gmtime(time_t *) that
    //!  stores the result in \c result.
    //------------------------------------------------------------------------
    static void GmTime(time_t unixsecs, struct tm & result);
    
    //------------------------------------------------------------------------
    //!  This is just a convenient wrapper for localtime(time_t *) that
    //!  stores the result in \c result.
    //------------------------------------------------------------------------
    static void LocalTime(time_t unixsecs, struct tm & result);

    //------------------------------------------------------------------------
    //!  Subtracts \c rhs from \c lhs and places the result in \c result.
    //!  Returns true if the result is positive (\c lhs greater than \c rhs),
    //!  else returns false.
    //------------------------------------------------------------------------
    static bool SubtractTimeval(const struct timeval & lhs,
                                const struct timeval & rhs,
                                struct timeval & result);

    //------------------------------------------------------------------------
    //!  Parses \c dateTimeString and stores the resulting time in \c tm.
    //!  Returns true on success, false on failure.  \c dateTimeString must
    //!  contain a date of one of the following forms:
    //!
    //!    YYYYMMDD
    //!    YYYY/MM/DD
    //!    MM/DD/YYYY
    //!    Jan DD, YYYY
    //!    Jan DD YYYY
    //!
    //!  \c dataTimeString may have a space and a time after the date, where
    //!  the time must be of the form HH:MM:SS (minutes and seconds are 
    //!  optional, and will default to 0 if not specified).
    //------------------------------------------------------------------------
    static bool ParseDateTime(const std::string & dateTimeString, 
                              struct tm & tm);

    //------------------------------------------------------------------------
    //!  Parses \c dateTimeString and stores the resulting time in \c t.
    //!  Returns true on success, false on failure.  \c dateTimeString must
    //!  contain a date of one of the following forms:
    //!
    //!    YYYYMMDD
    //!    YYYY/MM/DD
    //!    MM/DD/YYYY
    //!    Jan DD, YYYY
    //!    Jan DD YYYY
    //!
    //!  \c dataTimeString may have a space and a time after the date, where
    //!  the time must be of the form HH:MM:SS (minutes and seconds are 
    //!  optional, and will default to 0 if not specified).
    //------------------------------------------------------------------------
    static bool ParseDateTime(const std::string & dateTimeString, time_t & t);

    //------------------------------------------------------------------------
    //!  Returns the time for midnight (00:00) at the given @c daysLater.
    //!  For example, if @c daysLater is 1, we'll return the upcoming
    //!  midnight time (useful when rolling over log files, etc.).
    //------------------------------------------------------------------------
    static time_t MidnightDaysLater(int daysLater);
  };
  
  
}  // namespace Dwm

#endif  // _DWMTIMEUTIL_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
