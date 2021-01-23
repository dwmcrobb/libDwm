//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016
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
//!  \file DwmSunriseSunset.cc
//!  \author Daniel W. McRobb
//!  \brief Sunrise, Sunset and IsDaytime implementations
//---------------------------------------------------------------------------

//  Deal with linux silliness with respect to isnan()
#ifdef __linux__
  #define _DEFAULT_SOURCE
  #define _BSD_SOURCE
  extern "C" {
    #include <math.h>
  }
#else
  #include <cmath>
#endif

#include "DwmSunriseSunset.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double RadiansToDegrees(double radians)
  {
    return (radians * 180) / M_PI;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double DegreesToRadians(double degrees)
  {
    return (degrees * M_PI) / 180;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double Gamma(double julianDate)
  {
    return (2.0 * M_PI / 365.0) * (julianDate - 1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double Gamma2(double julianDate, double hour)
  {
    return (2 * M_PI / 365) * (julianDate - 1 + (hour/24));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double EquationOfTime(double gamma)
  {
    return (229.18 * (0.000075 + 0.001868 * cos(gamma)
                      - 0.032077 * sin(gamma)
                      - 0.014615 * cos(2 * gamma)
                      - 0.040849 * sin(2 * gamma)));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double SolarDeclination(double gamma)
  {
    return (0.006918 - 0.399912 * cos(gamma)
            + 0.070257 * sin(gamma)
            - 0.006758 * cos(2 * gamma)
            + 0.000907 * sin(2 * gamma));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double HourAngle(double latitude, double solarDeclination,
                          short calcSunrise)
  {
    double latRad = DegreesToRadians(latitude);
    if (calcSunrise) {
      return (acos(cos(DegreesToRadians(90.833))
                   / (cos(latRad) * cos(solarDeclination))
                   - tan(latRad) * tan(solarDeclination)));
    }
    else {
      return -(acos(cos(DegreesToRadians(90.833)) /
                    (cos(latRad) * cos(solarDeclination)) -
                    tan(latRad) * tan(solarDeclination)));
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double SunriseGMT(double julianDay, double latitude, double longitude)
  {
    double gamma = Gamma(julianDay);
    double eqTime = EquationOfTime(gamma);
    double solarDec = SolarDeclination(gamma);
    double hourAngle = HourAngle(latitude, solarDec, 1);
    double delta = longitude - RadiansToDegrees(hourAngle);
    double timeDiff = 4 * delta;
    double timeGMT = 720 + timeDiff - eqTime;
    double gammaSunrise = Gamma2(julianDay, timeGMT/60);

    eqTime = EquationOfTime(gammaSunrise);
    solarDec = SolarDeclination(gammaSunrise);
    hourAngle = HourAngle(latitude, solarDec, 1);
    delta = longitude - RadiansToDegrees(hourAngle);
    timeDiff = 4 * delta;
    timeGMT = 720 + timeDiff - eqTime;
    return timeGMT;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double SunsetGMT(double julianDay, double latitude, double longitude)
  {
    double gamma = Gamma(julianDay + 1);
    double eqTime = EquationOfTime(gamma);
    double solarDec = SolarDeclination(gamma);
    double hourAngle = HourAngle(latitude, solarDec, 0);
    double delta = longitude - RadiansToDegrees(hourAngle);
    double timeDiff = 4 * delta;
    double setTimeGMT = 720 + timeDiff - eqTime;
    double gamma_sunset = Gamma2(julianDay, setTimeGMT/60);
    eqTime = EquationOfTime(gamma_sunset);
    solarDec = SolarDeclination(gamma_sunset);
    hourAngle = HourAngle(latitude, solarDec, 0);
    delta = longitude - RadiansToDegrees(hourAngle);
    timeDiff = 4 * delta;
    setTimeGMT = 720 + timeDiff - eqTime;
    return setTimeGMT;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double RecentSunrise(double julianDay, double latitude,
                              double longitude)
  {
    double jDay = julianDay;
    double t = SunriseGMT(jDay, latitude, longitude);
    while (isnan(t)) {
      jDay--;
      if (jDay < 1) {
        jDay = 365;
      }
      t = SunriseGMT(jDay, latitude, longitude);
    }
    return jDay;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double RecentSunset(double julianDay, double latitude,
                             double longitude)
  {
    double jDay = julianDay;
    double t = SunsetGMT(jDay, latitude, longitude);
    while (isnan(t)) {
      jDay--;
      if (jDay < 1) {
        jDay = 365;
      }
      t = SunsetGMT(jDay, latitude, longitude);
    }
    return jDay;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double NextSunrise(double julianDay, double latitude,
                            double longitude)
  {
    
    double jDay = julianDay;
    double t = SunriseGMT(jDay, latitude, longitude);
    while (isnan(t)) {
      jDay++;
      if (jDay > 366) {
        jDay = 1;
      }
      t = SunriseGMT(jDay, latitude, longitude);
    }
    return jDay;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static double NextSunset(double julianDay, double latitude,
                           double longitude)
  {
    double jDay = julianDay;
    double t = SunsetGMT(jDay, latitude, longitude);
    while (isnan(t)) {
      jDay++;
      if (jDay > 366) {
        jDay = 1;
      }
      t = SunsetGMT(jDay, latitude, longitude);
    }
    return jDay;
  }

  //--------------------------------------------------------------------------
  //!  Return a time_t value of the sunrise expressed as GMT.
  //!  It is assumed that you'll use the normal ctime(3) type functions
  //!  to extract the values.
  //--------------------------------------------------------------------------
  static time_t GetSunrise(double latitude, double longitude, double julianDay)
  {
    time_t t = time(0);
    struct tm *now = localtime(&t);
    
    double  ssGMT = SunriseGMT(julianDay, latitude, longitude);
    if (isnan(ssGMT)) {
      if (latitude > 66.4 && (julianDay > 79 && julianDay < 267)) {
        // Above the Arctic Circle, during the daylight season, find
        // the last date that the sun rose (it's been up ever since
        // then).
        julianDay = RecentSunrise(julianDay, latitude, longitude);
      }
      else if (latitude > 66.4 && (julianDay < 83 || julianDay > 263)) {
        // Above the Arctic Circle, during the nighttime season, find
        // the date that the sun will come up again and report that.
        julianDay = NextSunrise(julianDay, latitude, longitude);
      }
      else if (latitude < -66.4 && (julianDay < 83 || julianDay > 263)) {
        // Do the same things as above, only for the southern
        // hemisphere
        julianDay = RecentSunrise(julianDay, latitude, longitude);
      }
      else if (latitude < -66.4 && (julianDay > 79 && julianDay < 267)) {
        julianDay = NextSunrise(julianDay, latitude, longitude);
      }
      ssGMT = SunriseGMT(julianDay, latitude, longitude);
    }
    
    double  floatHour = ssGMT / 60;
    double  hour = floor(floatHour);
    double  floatMinute = 60 * (floatHour - floor(floatHour));
    double  minute = floor(floatMinute);
    double  floatSec = 60 * (floatMinute - floor(floatMinute));
    double  second = floor(floatSec);

    struct tm  tm;
    tm.tm_year = now->tm_year;
    tm.tm_mon = 0;
    tm.tm_mday = julianDay;
    tm.tm_hour = (int)hour;
    tm.tm_min = (int)minute;
    tm.tm_sec = (int)second;

    return timegm(&tm);
  }

  //--------------------------------------------------------------------------
  //!  Return a time_t value of the sunset expressed as GMT.  It is
  //!  assumed that you'll use the normal ctime(3) type functions to
  //!  extract the values.
  //--------------------------------------------------------------------------
  time_t GetSunset(double latitude, double longitude, double julianDay)
  {
    time_t t = time(0);
    struct tm *now = localtime(&t);
    
    double  ssGMT = SunsetGMT(julianDay, latitude, longitude);
    if (isnan(ssGMT)) {
      if (latitude > 66.4 && julianDay > 79 && julianDay < 267) {
        julianDay = NextSunset(julianDay, latitude, longitude);
      }
      else if (latitude > 66.4 && (julianDay < 83 || julianDay > 263)) {
        julianDay = RecentSunset(julianDay, latitude, longitude);
      }
      else if (latitude < -66.4 && (julianDay < 83 || julianDay > 263)) {
        julianDay = NextSunset(julianDay, latitude, longitude);
      }
      else if (latitude < -66.4 && (julianDay > 79 && julianDay < 267)) {
        julianDay = RecentSunset(julianDay, latitude, longitude);
      }
      ssGMT = SunsetGMT(julianDay, latitude, longitude);
    }

    double  floatHour = ssGMT / 60;
    double  hour = floor(floatHour);
    double  floatMinute = 60 * (floatHour - floor(floatHour));
    double  minute = floor(floatMinute);
    double  floatSec = 60 * (floatMinute - floor(floatMinute));
    double  second = floor(floatSec);

    struct tm tm;
    tm.tm_year = now->tm_year;
    tm.tm_mon = 0;
    tm.tm_mday = julianDay;
    tm.tm_hour = (int)hour;
    tm.tm_min = (int)minute;
    tm.tm_sec = (int)second;
    
    return timegm(&tm);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue64 Sunrise(double latitude, double longitude, time_t t)
  {
    time_t     lt = t;
    struct tm  tm = *localtime(&lt);
    double     j = tm.tm_yday + 1;
    if (latitude >= -90 && latitude < -89.8) {
      latitude = -89.8;
    }
    if (latitude <= 90 && latitude > 89.8) {
      latitude = 89.8;
    }
    TimeValue64  rc(0, 0);
    rc.Set(GetSunrise(latitude, longitude, j), 0);
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  TimeValue64 Sunset(double latitude, double longitude, time_t t)
  {
    time_t     lt = t;
    struct tm  tm = *localtime(&lt);
    double     j = tm.tm_yday + 1;
    if (latitude >= -90 && latitude < -89.8) {
      latitude = -89.8;
    }
    if (latitude <= 90 && latitude > 89.8) {
      latitude = 89.8;
    }
    TimeValue64  rc(0, 0);
    rc.Set(GetSunset(latitude, longitude, j), 0);
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool IsDaytime(double latitude, double longitude, time_t t)
  {
    TimeValue64  sunrise = Sunrise(latitude, longitude, t);
    TimeValue64  sunset = Sunset(latitude, longitude, t);
    return ((t > sunrise.Secs()) && (t < sunset.Secs()));
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
