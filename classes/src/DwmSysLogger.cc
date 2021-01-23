//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmSysLogger.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::SysLogger class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <syslog.h>
  #include <unistd.h>
}

#include <cassert>
#include <iostream>
#include <mutex>
#include <sstream>

#include "DwmSysLogger.hh"

using namespace std;

namespace Dwm {

  mutex               SysLogger::_mutex;
  bool                SysLogger::_isOpen = false;
  int                 SysLogger::_logOptions = 0;
  string              SysLogger::_ident;
  int                 SysLogger::_logFacility = LOG_USER;
  bool                SysLogger::_showPriorities = true;
  bool                SysLogger::_showFunction = false;
  bool                SysLogger::_showFileLocation = true;
  int                 SysLogger::_minimumPriority = LOG_DEBUG;
  map<int,string>     SysLogger::_priorityTags = {
    {LOG_EMERG,   "[M]"},
    {LOG_ALERT,   "[A]"},
    {LOG_CRIT,    "[C]"},
    {LOG_ERR,     "[E]"},
    {LOG_WARNING, "[W]"},
    {LOG_NOTICE,  "[N]"},
    {LOG_INFO,    "[I]"},
    {LOG_DEBUG,   "[D]"}
  };
  map<string,int>     SysLogger::_priorityNames = {
    {"debug",     LOG_DEBUG},
    {"info",      LOG_INFO},
    {"notice",    LOG_NOTICE},
    {"warning",   LOG_WARNING},
    {"error",     LOG_ERR},
    {"critical",  LOG_CRIT},
    {"alert",     LOG_ALERT},
    {"emergency", LOG_EMERG}
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static string Basename(const string & path)
  {
    string rc = path;
    string::size_type  idx = path.find_last_of('/');
    if (idx != rc.npos)
      rc = rc.substr(idx+1,rc.length() - 1);
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::Open(const char *ident, int logopt, int facility)
  {
    bool  rc = false;
    _mutex.lock();
    if (! _isOpen) {
      switch (facility) {
        case LOG_USER:
        case LOG_DAEMON:
        case LOG_AUTH:
        case LOG_LOCAL0:
        case LOG_LOCAL1:
        case LOG_LOCAL2:
        case LOG_LOCAL3:
        case LOG_LOCAL4:
        case LOG_LOCAL5:
        case LOG_LOCAL6:
        case LOG_LOCAL7:
          rc = true;
          break;
        default:
          break;
      }
      if (rc) {
        //  We can't portably check all useful options in one shot;
        //  FreeBSD and Linux have LOG_PERROR, and Solaris has LOG_NOWAIT.
        int  knownLogOpts = LOG_PID | LOG_CONS | LOG_NDELAY;
        
#ifdef LOG_NOWAIT
        knownLogOpts |= LOG_NOWAIT;
#endif
#ifdef LOG_PERROR
        knownLogOpts |= LOG_PERROR;
#endif
        if (logopt & (~knownLogOpts)) {
          //  unknown option
          rc = false;
        }
      }
    }
    if (rc) {
      openlog(ident, logopt, facility);
      _isOpen = true;
      _logOptions = logopt;
      _logFacility = facility;
      if (ident)
        _ident = ident;
    }
    _mutex.unlock();
    return(rc);
  }

  //--------------------------------------------------------------------------
  //
  //--------------------------------------------------------------------------
  bool SysLogger::Open(const char *ident, int logopt, const string & facility)
  {
    bool  rc = false;
    const map<string,int>  facilities = {
      { "user", LOG_USER },
      { "auth", LOG_AUTH },
      { "daemon", LOG_DAEMON },
      { "local0", LOG_LOCAL0 },
      { "local1", LOG_LOCAL1 },
      { "local2", LOG_LOCAL2 },
      { "local3", LOG_LOCAL3 },
      { "local4", LOG_LOCAL4 },
      { "local5", LOG_LOCAL5 },
      { "local6", LOG_LOCAL6 },
      { "local7", LOG_LOCAL7 }
    };
    auto  it = facilities.find(facility);
    if (it != facilities.end()) {
      rc = Open(ident, logopt, it->second);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::Close()
  {
    //  We don't want to closelog() here, because it'll close
    //  syslogging in global context.  Even if we were to reference
    //  count the number of loggers we have open with syslog, we
    //  couldn't account for openlog() calls outside of our class.
    _mutex.lock();
    _isOpen = false;
    _mutex.unlock();
    return(true);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::Log(int priority, const char *message, ...)
  {
    va_list     vaList;
    va_start(vaList,message);
    bool rc = VaLog(priority, message, vaList);
    va_end(vaList);
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::Log(const std::string & filename, int lineno,
                      const std::string & function, int priority,
                      const char *message, ...)
  {
    va_list     vaList;
    va_start(vaList,message);
    bool rc = VaLog(filename, lineno, function, priority, message, vaList);
    va_end(vaList);
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::VaLog(int priority, const std::string & message, 
                        va_list & vaList)
  {
    bool  rc = false;
    _mutex.lock();
    if (_isOpen) {
      if (priority > _minimumPriority) {
        _mutex.unlock();
        return(true);
      }
      string      fmtString("");
      if (_showPriorities) {
        fmtString += PriorityTag(priority) + " ";
      }
      fmtString += message;
      vsyslog(priority, fmtString.c_str(), vaList);
      rc = true;
    }
    _mutex.unlock();
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::VaLog(const std::string & filename, int lineno,
                        const std::string & function, int priority,
                        const std::string & message, va_list & vaList)
  {
    ostringstream  fmt;
    fmt << message;
    if (_showFunction)
      fmt << " (" << function << ")";
    if (_showFileLocation)
      fmt << " {" << Basename(filename) << ":" << lineno << "}";
    return(VaLog(priority, fmt.str(), vaList));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::ShowPriorities()
  {
    return(_showPriorities);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::ShowPriorities(bool showPriorities)
  {
    return(_showPriorities = showPriorities);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::ShowFileLocation()
  {
    return(_showFileLocation);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::ShowFileLocation(bool showFileLocation)
  {
    return(_showFileLocation = showFileLocation);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::ShowFunction()
  {
    return(_showFunction);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SysLogger::ShowFunction(bool showFunction)
  {
    return(_showFunction = showFunction);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SysLogger::MinimumPriority()
  {
    return(_minimumPriority);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SysLogger::MinimumPriority(int minimumPriority)
  {
    return(_minimumPriority = minimumPriority);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SysLogger::MinimumPriority(const std::string & minimumPriority)
  {
    int  rc = MinimumPriority();
    map<string,int,less<string> >::const_iterator  iter = 
      _priorityNames.find(minimumPriority);
    if (iter != _priorityNames.end()) {
      rc = MinimumPriority(iter->second);
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string SysLogger::PriorityTag(int priority)
  {
    string  rc("");
    map<int,string>::const_iterator  i = _priorityTags.find(priority);
    if (i != _priorityTags.end()) {
      rc = i->second;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SysLogger::PriorityValue(const std::string & name)
  {
    int  rc = LOG_DEBUG;
    map<string,int>::const_iterator i = _priorityNames.find(name);
    if (i != _priorityNames.end()) {
      rc = i->second;
    }
    return(rc);
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
