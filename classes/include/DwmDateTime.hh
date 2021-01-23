//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmDateTime.hh
//!  \brief Dwm::DateTime class definition
//---------------------------------------------------------------------------

#ifndef _DWMDATETIME_HH_
#define _DWMDATETIME_HH_

extern "C" {
#include <inttypes.h>
#include <time.h>
}

#include <string>

#include "DwmTimeValue.hh"
#include "DwmTimeValue64.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class just encapsulates a struct tm and some of the typical
  //!  functionality for dealing with date and time.
  //!
  //!  Note that we always initialize using the local timezone.
  //--------------------------------------------------------------------------
  class DateTime
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Will initialize to the UNIX epoch if now is false,
    //!  otherwise will initialize ot the current time.
    //------------------------------------------------------------------------
    DateTime(bool now = false);

    //------------------------------------------------------------------------
    //!  Construct from a TimeValue.
    //------------------------------------------------------------------------
    DateTime(const TimeValue & tv);

    //------------------------------------------------------------------------
    //!  Construct from a TimeValue64.
    //------------------------------------------------------------------------
    DateTime(const Dwm::TimeValue64 & tv);

    //------------------------------------------------------------------------
    //!  Return the equivalent TimeValue, using the current timezone setting.
    //------------------------------------------------------------------------
    TimeValue GetTimeValue() const;

    //------------------------------------------------------------------------
    //!  Return the equivalent TimeValue64, using the current timezone setting.
    //------------------------------------------------------------------------
    TimeValue64 GetTimeValue64() const;

    //------------------------------------------------------------------------
    //!  Returns true if daylight savings is in effect.
    //------------------------------------------------------------------------
    bool IsDaylightSavings() const;

    //------------------------------------------------------------------------
    //!  Returns the seconds (0 - 59).
    //------------------------------------------------------------------------
    uint8_t Sec() const;

    //------------------------------------------------------------------------
    //!  Returns the minutes (0 - 59).
    //------------------------------------------------------------------------
    uint8_t Min() const;

    //------------------------------------------------------------------------
    //!  Returns the hour (0 - 23).
    //------------------------------------------------------------------------
    uint8_t Hour() const;

    //------------------------------------------------------------------------
    //!  Returns the day of the month (1 - 31).
    //------------------------------------------------------------------------
    uint8_t DayOfMonth() const;

    //------------------------------------------------------------------------
    //!  Returns the month of the year (1 - 12).
    //------------------------------------------------------------------------
    uint8_t Month() const;

    //------------------------------------------------------------------------
    //!  Returns the year.
    //------------------------------------------------------------------------
    uint16_t Year() const;

    //------------------------------------------------------------------------
    //!  Returns the day of the week (Sunday == 0)
    //------------------------------------------------------------------------
    uint8_t DayOfWeek() const;

    //------------------------------------------------------------------------
    //!  Returns the day of the year (0 - 365).
    //------------------------------------------------------------------------
    uint16_t DayOfYear() const;
    
    //------------------------------------------------------------------------
    //!  Returns the time zone.
    //------------------------------------------------------------------------
    std::string TimeZone() const;
    
    //------------------------------------------------------------------------
    //!  Returns a string formatted according to \c format.  This just
    //!  calls strftime() under the hood; see the strftime() manpage.
    //------------------------------------------------------------------------
    std::string Formatted(const std::string & format) const;

    //------------------------------------------------------------------------
    //!  Parses time string \c s according to the given \c format.
    //!  Returns tree on success.
    //!  @note This calls strptime() under the hood; see the strptime()
    //!  manpage.
    //------------------------------------------------------------------------
    bool Parse(const std::string & s, const std::string & format);

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const DateTime & dt) const;

    //------------------------------------------------------------------------
    //!  operator <
    //------------------------------------------------------------------------
    bool operator < (const DateTime & dt) const;
    
  private:
    struct tm  _tm;
  };
  
}  // namespace Dwm

#endif  // _DWMDATETIME_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
