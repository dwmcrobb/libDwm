//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007,2015,2016,2018
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
//!  \file DwmFileLogger.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/param.h>
  #include <sys/stat.h>
  #include <stdarg.h>
  #include <unistd.h>
}

#include <cstdlib>
#include <cstring>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmFileLogger.hh"
#include "DwmPathUtils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmFileLogger.cc 10180 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const map<int,string>  FileLogger::_defaultPriorityTags = {
    { LOG_EMERG,   "[M]" },
    { LOG_ALERT,   "[A]" },
    { LOG_CRIT,    "[C]" },
    { LOG_ERR,     "[E]" },
    { LOG_WARNING, "[W]" },
    { LOG_NOTICE,  "[N]" },
    { LOG_INFO,    "[I]" },
    { LOG_DEBUG,   "[D]" }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const map<string,int>  FileLogger::_defaultPriorityNames = {
    { "debug",     LOG_DEBUG   },
    { "info",      LOG_INFO    },
    { "notice",    LOG_NOTICE  },
    { "warning",   LOG_WARNING },
    { "error",     LOG_ERR     },
    { "critical",  LOG_CRIT    },
    { "alert",     LOG_ALERT   },
    { "emergency", LOG_EMERG   }
  };
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  FileLogger::FileLogger()
      : _logOptions(0), _minimumPriority(LOG_DEBUG), _ident(),
        _showPriorities(false), _showFunction(false),
        _showFileLocation(false), _hostName(), _filePrefix(), _file(0),
        _rolloverSize(k_defaultRolloverSize), 
        _numRolloverFiles(k_defaultNumRolloverFiles),
        _currentFilePosition(0), _bufferingType(e_notBuffered), _mutex()
  {
    _priorityTags = _defaultPriorityTags;
    _priorityNames = _defaultPriorityNames;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  FileLogger::~FileLogger()
  {
    this->Close();
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Lock() const
  {
    _mutex.lock();
    return(true);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Unlock() const
  {
    _mutex.unlock();
    return(true);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Open(const string & ident, int logopt,
                        const string & filename,
                        BufferingType bufferingType)
  {
    lock_guard<mutex>  lock(_mutex);
    InitializePriorities();
    return this->OpenNoLock(ident, logopt, filename, bufferingType);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::OpenNoLock(const string & ident, int logopt,
                              const string & filename,
                              BufferingType bufferingType)
  {
    bool  rc = false;

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
      return(rc);
    }

    if (_file) {
      //  we're already open; close first
      if (! CloseNoLock()) {
        //  failed to close
        return(rc);
      }
    }

    //  open the current logfile.
    rc = OpenFile(filename);

    if (rc) {
      _ident = ident;
      _logOptions = logopt;
      _filePrefix = filename;
      _bufferingType = bufferingType;
      setvbuf(_file, NULL, bufferingType, 0);
      char  buf[MAXHOSTNAMELEN];
      memset(buf,0,MAXHOSTNAMELEN);
      if (gethostname(buf, MAXHOSTNAMELEN-1) == 0) {
        _hostName = buf;
        string::size_type idx = _hostName.find_first_of(".");
        if (idx != _hostName.npos) {
          _hostName.erase(idx);
        }
      }
      else {
        _hostName = "";
      }
    }

    return(rc);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::OpenFile(const string & filename)
  {
    bool  rc = false;
    _file = fopen(filename.c_str(), "a+");
    if (_file) {
      rc = true;
      _currentFilePosition = ftell(_file);
    }

    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string FileLogger::FilePrefix() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _filePrefix;
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::IsOpen() const
  {
    lock_guard<mutex>  lock(_mutex);
    return(0 != _file);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Close()
  {
    lock_guard<mutex>  lock(_mutex);
    return CloseNoLock();
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::CloseNoLock()
  {
    bool  rc = false;

    if (! _file)
      return(rc);
    
    if (fclose(_file) == 0) {
      rc = true;
      _file = (FILE *)0;
      _ident = "";
      _logOptions = 0;
    }
    
    return(rc);
  }
      
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Log(int priority, const char * message, ...)
  {
    va_list     vaList;
    va_start(vaList,message);
    bool  rc = FileLogger::VaLog(priority,message,vaList);
    va_end(vaList);
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Log(const string & filename, int lineno, 
                       const string & function, int priority,
                       const char * message, ...)
  {
    va_list     vaList;
    va_start(vaList,message);
    bool  rc = FileLogger::VaLog(filename, lineno, function, 
                                 priority, message, vaList);
    va_end(vaList);
    return(rc);
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::VaLog(const string & filename, int lineno, 
                         const string & function, int priority,
                         const string & message, va_list & vaList)
  {
    ostringstream  fmt;
    fmt << message;
    if (FileLogger::_showFunction) {
      fmt << " (" << function << ")";
    }
    if (FileLogger::_showFileLocation) {
      fmt << " {" << PathUtils::Basename(filename) << ":" << lineno << "}";
    }
    return(FileLogger::VaLog(priority, fmt.str(), vaList));
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::VaLog(int priority, const string & message, 
                         va_list & vaList)
  {
    bool  rc = false;

    lock_guard<mutex>  lock(_mutex);
    
    if (! _file) {
      return rc;
    }

    if (priority > _minimumPriority) {
      return true;
    }

    static const char *monthNames[] = { "Jan", "Feb", "Mar", "Apr",
                                        "May", "Jun", "Jul", "Aug",
                                        "Sep", "Oct", "Nov", "Dec" };
    time_t      ts;
    struct tm  *tmPtr;
    string      fmtString("");

    time(&ts);
    tmPtr = localtime(&ts);
    fprintf(_file,"%s %d %02d:%02d:%02d %s %s",
            monthNames[tmPtr->tm_mon],tmPtr->tm_mday,tmPtr->tm_hour,
            tmPtr->tm_min,tmPtr->tm_sec,_hostName.c_str(),
            _ident.c_str());
    if (_logOptions & LOG_PID) {
      fprintf(_file,"[%u]",(uint32_t)getpid());
    }
    
    fmtString = string(": ");
    if (_showPriorities) {
      fmtString += PriorityTag(priority) + " ";
    }
    fmtString += message;
    vfprintf(_file,fmtString.c_str(),vaList);
    if (fprintf(_file,"\n") > 0) {
      rc = true;
    }
    
    //  special code for LOG_PERROR
#ifdef LOG_PERROR
    if (_logOptions & LOG_PERROR) {
      fprintf(stderr,"%s %d %02d:%02d:%02d ",
              monthNames[tmPtr->tm_mon],tmPtr->tm_mday,tmPtr->tm_hour,
              tmPtr->tm_min,tmPtr->tm_sec);
      vfprintf(stderr,fmtString.c_str(),vaList);
      fprintf(stderr,"\n");
    }
#endif
    
    _currentFilePosition = ftell(_file);
    if (_rolloverSize) {
      if (_currentFilePosition > _rolloverSize) {
        rc = RollFilesOver();
      }
    }

    return rc;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::Flush()
  {
    bool  rc = false;

    lock_guard<mutex>  lock(_mutex);
    if (_file) {
      if (fflush(_file) == 0) {
        rc = true;
      }
    }
    
    return rc;
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int FileLogger::MinimumPriority() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _minimumPriority;
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int FileLogger::MinimumPriority(int minimumPriority)
  {
    lock_guard<mutex>  lock(_mutex);
    _minimumPriority = minimumPriority;
    return _minimumPriority;
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::ShowPriorities() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _showPriorities;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::ShowPriorities(bool showPriorities)
  {
    lock_guard<mutex>  lock(_mutex);
    _showPriorities = showPriorities;
    return _showPriorities;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::ShowFunction() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _showFunction;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::ShowFunction(bool showFunction)
  {
    lock_guard<mutex>  lock(_mutex);
    _showFunction = showFunction;
    return _showFunction;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::ShowFileLocation() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _showFileLocation;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::ShowFileLocation(bool showFileLocation)
  {
    lock_guard<mutex>  lock(_mutex);
    _showFileLocation = showFileLocation;
    return _showFileLocation;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  uint32_t FileLogger::RolloverSize() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _rolloverSize;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  uint32_t FileLogger::RolloverSize(uint32_t rolloverSize)
  {
    lock_guard<mutex>  lock(_mutex);
    _rolloverSize = rolloverSize;
    return _rolloverSize;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  uint32_t FileLogger::NumRolloverFiles() const
  {
    lock_guard<mutex>  lock(_mutex);
    return _numRolloverFiles;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  uint32_t FileLogger::NumRolloverFiles(uint32_t numRolloverFiles)
  {
    lock_guard<mutex>  lock(_mutex);
    _numRolloverFiles = numRolloverFiles;
    return _numRolloverFiles;
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  bool FileLogger::RollFilesOver()
  {
    int      logopt = _logOptions;
    string   ident = _ident;
    string   filePrefix = _filePrefix;
    BufferingType  bufferingType = _bufferingType;
    
    if (_file) {
      if (! CloseNoLock())
        return(false);
    }

    bool         rc = true;
    char         buf[16];
    string       srcFile, dstFile;
    struct stat  statbuf;
    
    for (uint32_t i = _numRolloverFiles; i >= 1; --i) {
      memset(buf,0,16);
      snprintf(buf, sizeof(buf), "%u", i - 1);
      srcFile = filePrefix + "." + buf;
      if (stat(srcFile.c_str(),&statbuf) == 0) {
        //  srcFile exists.
        memset(buf,0,16);
        snprintf(buf, sizeof(buf), "%u", i);
        dstFile = filePrefix + "." + buf;
        if (rename(srcFile.c_str(),dstFile.c_str()) != 0) {
          rc = false;
          break;
        }
      }
    }

    if (_numRolloverFiles > 0) {
      dstFile = filePrefix + ".0";
      if (stat(filePrefix.c_str(),&statbuf) == 0) {
        if (rename(filePrefix.c_str(),dstFile.c_str()) != 0) {
          rc = false;
        }
      }
    }
    
    if (! rc)
      return(rc);

    rc = OpenNoLock(ident,logopt,filePrefix,bufferingType);
    
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool FileLogger::
  SetPriorities(const map<int,pair<string,string> > & priorities)
  {
    bool  rc = false;
    lock_guard<mutex>  lock(_mutex);
    _priorityTags.clear();
    _priorityNames.clear();
    map<int, pair<string,string> >::const_iterator  i;
    for (auto & i : priorities) {
      _priorityTags[i.first] = i.second.first;
      _priorityNames[i.second.second] = i.first;
    }
    rc = true;
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string FileLogger::PriorityTag(int priority)
  {
    string  rc;
    auto  i = _priorityTags.find(priority);
    if (i != _priorityTags.end()) {
      rc = i->second;
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void FileLogger::InitializePriorities()
  {
    return;
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
