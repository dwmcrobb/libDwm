%{
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
//!  \file DwmTimeUtil.ll
//!  \author Daniel W. McRobb
//!  \brief Dwm::TimeUtil class implementation
//---------------------------------------------------------------------------
  
  extern "C" {
    #include <assert.h>
    #include <inttypes.h>

    #define TOKEN_YYYYMMDD       1024
    #define TOKEN_YYYY_MM_DD     1025
    #define TOKEN_MM_DD_YYYY     1026
    #define TOKEN_MNAME_DD_YYYY  1027
  }

  #include <cstring>  
  #include <iostream>
  #include <string>

  #include "DwmTimeUtil.hh"

  static char const * const g_monthNames[] = { "Jan","Feb","Mar","Apr","May","Jun",
                                               "Jul","Aug","Sep","Oct","Nov","Dec",
                                               NULL };
  
  // We can parse dates of the following forms:
  //
  //    YYYYMMDD
  //    YYYY/MM/DD
  //    MM/DD/YYYY
  //    Jan DD, YYYY
  //    Jan DD YYYY
  
%}

%option noyywrap
%option nounput
%option prefix="timeutil"

m_month		([0]*[1-9])|10|11|12
m_monthName     Jan(uary){0,1}|Feb(ruary){0,1}|Mar(ch){0,1}|Apr(il){0,1}|May|Jun(e){0,1}|Jul(y){0,1}|Aug(ust){0,1}|Sep(temper){0,1}|Oct(ober){0,1}|Nov(ember){0,1}|Dec(ember){0,1}
m_year		[12][0-9]{3,3}
m_mday		([0]*[1-9])|[12][0-9]|3[01]
m_hour		([0]*[0-9])|[1][0-9]|[2][1-3]
m_minute	([0]*[0-9])|[1-5][0-9]
m_second        ([0][0-9])|[1-5][0-9]
m_yyyymmdd	{m_year}([0][1-9]|10|11|12)([0][1-9]|[12][0-9]|3[01])

%x x_year
%x x_month
%x x_mday
%x x_hour
%x x_minute
%x x_second
%x x_error

%%

<INITIAL>^{m_month}[/]{m_mday}[/]{m_year}	{ BEGIN(x_hour);
					          return(TOKEN_MM_DD_YYYY); }
<INITIAL>^{m_year}[/]{m_month}[/]{m_mday}	{ BEGIN(x_hour);
					          return(TOKEN_YYYY_MM_DD); }
<INITIAL>^{m_yyyymmdd}			{ BEGIN(x_hour); 
					  return(TOKEN_YYYYMMDD); } 
<INITIAL>^{m_monthName}                 { BEGIN(x_mday); return(x_month); }
<x_mday>{m_mday}			{ BEGIN(x_year); return(x_mday); }
<x_mday>[ \t]
<x_mday>[,]				{ BEGIN(x_year); }
<x_mday>.
<x_year>{m_year}			{ BEGIN(x_hour); return(x_year); }
<x_year>[ \t,]
<x_year>.				{ BEGIN(x_error); yyless(0); }
<x_hour>([2][5-9])|([3-9][0-9])|([1-9][0-9][0-9][0-9]*)  { BEGIN(x_error);
						     yyless(strlen(timeutiltext)); }
<x_hour>{m_hour}			{ return(x_hour); }
<x_hour>[:]				{ BEGIN(x_minute); }
<x_hour>[ \t]
<x_hour><<EOF>>				{ BEGIN(INITIAL); }
<x_hour>.				{ BEGIN(x_error); yyless(0); }
<x_minute>{m_minute}			{ return(x_minute); }
<x_minute>[:]				{ BEGIN(x_second); }
<x_minute><<EOF>>			{ BEGIN(INITIAL); }
<x_minute>.
<x_second>{m_second}			{ return(x_second); }
<x_second><<EOF>>			{ BEGIN(INITIAL); }
<x_second>.				{ BEGIN(x_error); yyless(0); }
<x_error>.				{ return(x_error); }
<INITIAL>[ \t]
<INITIAL>.
<<EOF>>					{ return(0); }

%%

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeUtil::ParseDateTime(const std::string & dateTimeString, 
                               struct tm & tm)
  {
    bool  rc = false;
    
    memset(&tm,0,sizeof(tm));
    
    YY_BUFFER_STATE  bufferState =
      timeutil_scan_string(dateTimeString.c_str());
    
    int  lexrc;
    while ((lexrc = timeutillex()) != 0) {
      switch (lexrc) {
        case TOKEN_MM_DD_YYYY:
          sscanf(timeutiltext,"%02d/%02d/%04d",
                 &(tm.tm_mon),&(tm.tm_mday),&(tm.tm_year));
          --tm.tm_mon;
          tm.tm_year -= 1900;
          break;
        case TOKEN_YYYY_MM_DD:
          sscanf(timeutiltext,"%04d/%02d/%02d",
                 &(tm.tm_year),&(tm.tm_mon),&(tm.tm_mday));
          --tm.tm_mon;
          tm.tm_year -= 1900;
          break;
        case TOKEN_YYYYMMDD:
          sscanf(timeutiltext,"%04d%02d%02d",
                 &(tm.tm_year),&(tm.tm_mon),&(tm.tm_mday));
          --tm.tm_mon;
          tm.tm_year -= 1900;
          break;
        case x_year:
          sscanf(timeutiltext,"%04d",&(tm.tm_year));
          tm.tm_year -= 1900;
          break;
        case x_month:
          for (int i = 0; i < 12; ++i) {
            if (strncmp(timeutiltext,g_monthNames[i],
                        strlen(g_monthNames[i])) == 0) {
              tm.tm_mon = i;
              break;
            }
          }
          break;
        case x_mday:
          tm.tm_mday = atoi(timeutiltext);
          break;
        case x_hour:
          tm.tm_hour = atoi(timeutiltext); 
          break;
        case x_minute:
          tm.tm_min = atoi(timeutiltext); 
          break;
        case x_second:
          tm.tm_sec = atoi(timeutiltext);
          break;
        case x_error:
          memset(&tm,0,sizeof(tm));
          break;
      }
    }
    
    yy_delete_buffer(bufferState);

    if ((tm.tm_year != 0)
        && (tm.tm_mon < 12)
        && (tm.tm_mday > 0)
        && (tm.tm_mday <= 31)) {
      rc = true;
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeUtil::ParseDateTime(const std::string & dateTimeString, time_t & t)
  {
    bool  rc = false;
    
    struct tm  tm;
    
    if (ParseDateTime(dateTimeString,tm)) {
      tm.tm_isdst = -1;
      t = mktime(&tm);
      rc = true;
    }
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //      void TimeUtil::GmTime(time_t unixsecs, struct tm & result) 
  //..........................................................................
  //  
  //--------------------------------------------------------------------------
  void TimeUtil::GmTime(time_t unixsecs, struct tm & result)
  {
    time_t  unixTime = unixsecs;
    struct tm *tmPtr = gmtime(&unixTime);
    
    assert(tmPtr);
    memcpy(&result,tmPtr,sizeof(result));
    
    return;
  }

  //--------------------------------------------------------------------------
  //       void TimeUtil::LocalTime(time_t unixsecs, struct tm & result) 
  //..........................................................................
  //  
  //--------------------------------------------------------------------------
  void TimeUtil::LocalTime(time_t unixsecs, struct tm & result)
  {
    time_t  unixTime = unixsecs;
    struct tm *tmPtr = localtime(&unixTime);
    
    assert(tmPtr);
    memcpy(&result,tmPtr,sizeof(result));
    
    return;
  }

  //--------------------------------------------------------------------------
  //       bool TimeUtil::SubtractTimeval(const struct timeval & lhs, 
  //                                      const struct timeval & rhs, 
  //                                      struct timeval & result) 
  //..........................................................................
  //  
  //--------------------------------------------------------------------------
  bool TimeUtil::SubtractTimeval(const struct timeval & lhs,
                                 const struct timeval & rhs,
                                 struct timeval & result)
  {
    bool  rc = false;
    
    uint64_t  lhVal = (lhs.tv_sec * 1000000) + lhs.tv_usec;
    uint64_t  rhVal = (rhs.tv_sec * 1000000) + rhs.tv_usec;

    if (lhVal > rhVal) {
      rc = true;
      result.tv_sec = (lhVal - rhVal) / 1000000;
      result.tv_usec = (lhVal - rhVal) % 1000000;
    }
    else {
      result.tv_sec = (rhVal - lhVal) / 1000000;
      result.tv_usec = (rhVal - lhVal) % 1000000;
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  time_t TimeUtil::MidnightDaysLater(int daysLater)
  { 
    time_t     rc = 0;
    time_t     now = time((time_t *)0);
    struct tm  now_tm;
    if (localtime_r(&now, &now_tm) == &now_tm) {
      now_tm.tm_hour = now_tm.tm_min = now_tm.tm_sec = 0;
      now_tm.tm_isdst = -1;
      time_t     midnightToday = mktime(&now_tm);
      time_t     then = midnightToday + ((int64_t)daysLater * 24 * 60 * 60);
      struct tm  then_tm;
      if (localtime_r(&then, &then_tm) == &then_tm) {
        rc = then;
        if (now_tm.tm_isdst != then_tm.tm_isdst) {
          rc += (now_tm.tm_isdst ? 3600 : -3600);
        }
      }
    }
    return rc;
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
