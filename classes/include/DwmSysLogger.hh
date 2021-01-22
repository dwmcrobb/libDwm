//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmSysLogger.hh 8401 $
// @(#) $Id: DwmSysLogger.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
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
//!  \file DwmSysLogger.hh
//!  \brief Dwm::SysLogger class definition and Syslog() macro
//---------------------------------------------------------------------------

#ifndef _DWMSYSLOGGER_HH_
#define _DWMSYSLOGGER_HH_

extern "C" {
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>
}

#include <cstdio>
#include <map>
#include <mutex>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates syslog logging in global scope (all members
  //!  are static).
  //--------------------------------------------------------------------------
  class SysLogger
  {
  public:
    //--------------------------------------------------------------------
    //!  Opens the logger for syslog logging.  \c ident, \c logopt and
    //!  \c facility are used directly to call openlog() (see the 
    //!  syslog(3) manpage).  Returns true on success, false on failure.
    //!  Note that openlog() gives no indication of failure, hence we 
    //!  only return false if we get an unacceptable \c logopt or 
    //!  \c facility.
    //!  The acceptable facilities are \c LOG_USER, \c LOG_DAEMON,
    //!  \c LOG_AUTH, \c LOG_LOCAL0, \c LOG_LOCAL1, \c LOG_LOCAL2,
    //!  \c LOG_LOCAL3, \c LOG_LOCAL4, \c LOG_LOCAL5, \c LOG_LOCAL6,
    //!  and \c LOG_LOCAL7.  The acceptable log options are \c LOG_PID,
    //!  \c LOG_CONS and \c LOG_NDELAY on all systems.  \c LOG_PERROR is
    //!  accepted on systems which support it.  \c LOG_NOWAIT is
    //!  accepted on systems which support it (Solaris, for example).
    //!  Note this member can cause adverse affects on external syslog()
    //!  calls because syslog facilities are global.  That's why all of
    //!  our members are static.
    //--------------------------------------------------------------------
    static bool Open(const char *ident, int logopt, int facility);

    //------------------------------------------------------------------------
    //!  The same as Open() above except the log facility is passed as a
    //!  const reference to a string which must be one of "user", "daemon",
    //!  "auth", "local0", "local1", "local2", "local3", "local4", "local5",
    //!  "local6", or "local7".
    //------------------------------------------------------------------------
    static bool Open(const char *ident, int logopt,
                     const std::string & facility);
    
    //--------------------------------------------------------------------
    //!  Closes the logger.  Returns true on success, false on failure.
    //!  Note that this is essentially a no-op.  We don't want to call
    //!  closelog() because we don't want to prevent external syslog()
    //!  calls.
    //--------------------------------------------------------------------
    static bool Close();
    
    //--------------------------------------------------------------------
    //!  Just like syslog(), takes a priority and a format string and
    //!  variable list of arguments.  Returns true on success, false
    //!  on failure.
    //--------------------------------------------------------------------
    static bool Log(int priority, const char *message, ...);

    //--------------------------------------------------------------------
    //!  Like syslog(), but takes \c filename, \c lineno and
    //!  \c function arguments which are usually __FILE__, __LINE__ and
    //!  __PRETTY_FUNCTION__ in the caller source.
    //--------------------------------------------------------------------
    static bool Log(const std::string & filename, int lineno,
                    const std::string & function, int priority,
                    const char *message, ...);

    //--------------------------------------------------------------------
    //!  Just like vsyslog(), takes a priority and a format string and
    //!  a va_list.  Returns true on success, false on failure.
    //--------------------------------------------------------------------
    static bool VaLog(int priority, const std::string & message, 
                      va_list & vaList);

    //--------------------------------------------------------------------
    //!  Just like vsyslog(), but takes \c filename, \c lineno 
    //!  and \c function arguments which are usually __FILE__, __LINE__
    //!  and __PRETTY_FUNCTION__ in the caller source.
    //!  Returns true on success, false on failure.
    //--------------------------------------------------------------------
    static bool VaLog(const std::string & filename, int lineno,
                      const std::string & function, int priority,
                      const std::string & message, va_list & vaList);

    //--------------------------------------------------------------------
    //!  If the logger is set to show priorities in each log message,
    //!  returns true.  Else returns false.
    //--------------------------------------------------------------------
    static bool ShowPriorities();

    //--------------------------------------------------------------------
    //!  If \c showPriorities is true, the logger will tag each log
    //!  message with an indication of the message's priority.  The
    //!  tags are of the form "[x]" where x is a single letter.
    //!  Hence we have "[M]" for LOG_EMERG, "[A]" for LOG_ALERT, "[C]"
    //!  for LOG_CRIT, "[E]" for LOG_ERR, "[W]" for LOG_WARN, "[N]" for
    //!  LOG_NOTICE, "[I]" for LOG_INFO and "[D]" for LOG_DEBUG.
    //!  By default, message priorities are not shown.
    //--------------------------------------------------------------------
    static bool ShowPriorities(bool showPriorities);

    //--------------------------------------------------------------------
    //!  If the logger is set to show file location information in
    //!  Log(const std::string &, int, int, const char *, ...), returns
    //!  true.
    //--------------------------------------------------------------------
    static bool ShowFileLocation();

    //--------------------------------------------------------------------
    //!  If \c showFileLocation is true, the logger will show the
    //!  file and line number information in each log message logged with
    //!  Log(const std::string &, int, int, const char *, ...).  If
    //1  \c showFileLocation is false, the logger will not show file
    //!  and line number information in log messages.
    //--------------------------------------------------------------------
    static bool ShowFileLocation(bool showFileLocation);

    //--------------------------------------------------------------------
    //!  If the logger is set to show function information in
    //!  Log(const string &, int, const string &, int, const char *, ...),
    //!  returns true.
    //--------------------------------------------------------------------
    static bool ShowFunction();

    //--------------------------------------------------------------------
    //!  If \c showFunction is true, the logger will show function
    //!  information in each log message logged with
    //!  Log(const string &, int, const string &, int, const char *, ...).
    //!  If \c showFunction is false, the logger will not show function
    //!  information in log messages.
    //--------------------------------------------------------------------
    static bool ShowFunction(bool showFunction);

    //--------------------------------------------------------------------
    //!  Returns the lowest message priority that the logger will log.
    //--------------------------------------------------------------------
    static int MinimumPriority();

    //--------------------------------------------------------------------
    //!  Sets and returns the lowest message priority that the logger
    //!  will log.  By default, all message priorities will be logged
    //!  (minimum priority == LOG_DEBUG).
    //--------------------------------------------------------------------
    static int MinimumPriority(int minimumPriority);

    //--------------------------------------------------------------------
    //!  Sets and returns the lowest message priority that the logger
    //!  will log.  \c minimumPriority may be "debug", "info", "notice",
    //!  "warning", "error", "critical", "alert" or "emergency".
    //--------------------------------------------------------------------
    static int MinimumPriority(const std::string & minimumPriority);
    
    //------------------------------------------------------------------------
    //!  Returns the priority tag ("[F]", "[W]", et. al.) for a given
    //!  priority.
    //------------------------------------------------------------------------
    static std::string PriorityTag(int priority);

    //------------------------------------------------------------------------
    //!  Returns the priority value for the given priority @c name.
    //------------------------------------------------------------------------
    static int PriorityValue(const std::string & name);
    
  private:
    static std::mutex                    _mutex;
    static bool                          _isOpen;
    static int                           _logOptions;
    static std::string                   _ident;
    static int                           _logFacility;
    static bool                          _showPriorities;
    static bool                          _showFunction;
    static bool                          _showFileLocation;
    static int                           _minimumPriority;
    static std::map<int,std::string>     _priorityTags;
    static std::map<std::string,int>     _priorityNames;
  };

}  // namespace Dwm

//----------------------------------------------------------------------------
//!  Used just like syslog(), but calls Dwm::SysLogger::Log() to allow the
//!  filename, line number and function name to be logged automatically and
//!  the priority tag text provided by Dwm::SysLogger.  Note this requires
//!  variadic macros, but g++ and clang++ support them.
//----------------------------------------------------------------------------
#define Syslog(...)                                                     \
  Dwm::SysLogger::Log(__FILE__,__LINE__,__PRETTY_FUNCTION__,__VA_ARGS__)
  

#endif  // _DWMSYSLOGGER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
