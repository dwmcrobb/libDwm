//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmFileLogger.hh 10164 $
// @(#) $Id: DwmFileLogger.hh 10164 2018-07-22 23:20:15Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmFileLogger.hh
//!  \brief Dwm::FileLogger class definition
//---------------------------------------------------------------------------

#ifndef _DWMFILELOGGER_HH_
#define _DWMFILELOGGER_HH_

extern "C" {
#include <inttypes.h>
#include <stdio.h>
#include <syslog.h>
}

#include <map>
#include <mutex>
#include <string>

namespace Dwm {

  //----------------------------------------------------------------------
  //!  This class encapsulates syslog-like logging, but uses files instead
  //!  of the syslog facility.  You may set the maximum size of the log
  //!  file (in bytes) by using the RolloverSize(uint32_t) member. The
  //!  default is 102400 (100K) bytes.  You may set the number of rollover
  //!  files using the NumRolloverFiles(uint32_t) member.  The default is 1.
  //!  The logger will roll over "logfile" to "logfile.0" and "logfile.N"
  //!  to "logfile.N+1" for { 0 <= N < NumRolloverFiles() }.  If the number
  //!  of rollover files is set to 0, the log file will be truncated each
  //!  time it grows beyond RolloverSize().  Note that RolloverSize() is
  //!  only an estimate; the actual log size will be slightly larger since
  //!  we treat log messages as atomic (we don't split them across files).
  //!  You may set a priority threshold with MinimumPriority(int); any
  //!  messages below this priority will be silently discarded.  This allows
  //!  dynamic logging levels, as well as allowing syslog.conf-like priority
  //!  filtering functionality.
  //----------------------------------------------------------------------
  class FileLogger
  {
  public:
    typedef enum {
      e_notBuffered   = _IONBF,
      e_lineBuffered  = _IOLBF,
      e_blockBuffered = _IOFBF
    } BufferingType;
    
    static const uint32_t  k_defaultRolloverSize      = 100 * 1024;
    static const uint32_t  k_defaultNumRolloverFiles  = 1;

    //--------------------------------------------------------------------
    //!  Constructor.
    //--------------------------------------------------------------------
    FileLogger();

    //--------------------------------------------------------------------
    //!  Destructor.  Closes the log if it's open.
    //--------------------------------------------------------------------
    ~FileLogger();
    
    //--------------------------------------------------------------------
    //!  Opens the logger.  Returns true on success, false on failure.
    //!  \c ident is used to name entries in the log (in the same manner
    //!  as openlog()).  \c logopt is the logging options, used in the
    //!  same manner as the corresponding argument to openlog().  The
    //!  acceptable log options are \c LOG_PID, \c LOG_CONS and
    //!  \c LOG_NDELAY on all systems.  \c LOG_PERROR is accepted on
    //!  systems which support it.  \c bufferingType specifies the
    //!  desired buffering.  Specifiy \c e_notBuffered to get unbuffered,
    //!  \c e_lineBuffered to get line buffered, and \c e_blockBuffered
    //!  to get fully buffered.  
    //--------------------------------------------------------------------
    bool Open(const std::string & ident, int logopt, 
              const std::string & filename,
              BufferingType bufferingType = e_blockBuffered);

    //--------------------------------------------------------------------
    //!  Returns the log file prefix, i.e. the @c filename argument that
    //!  was given to Open().
    //--------------------------------------------------------------------
    std::string FilePrefix() const;

    //--------------------------------------------------------------------
    //!  Returns true if the logger is open, else returns false.
    //--------------------------------------------------------------------
    bool IsOpen() const;
    
    //--------------------------------------------------------------------
    //!  Closes the logger.
    //--------------------------------------------------------------------
    bool Close();
    
    //--------------------------------------------------------------------
    //!  Just like syslog(), takes a priority and a format string and
    //!  variable list of arguments.  Returns true on success, false
    //!  on failure.
    //--------------------------------------------------------------------
    bool Log(int priority, const char * message, ...);

    //--------------------------------------------------------------------
    //!  Like syslog(), but takes \c filename, \c lineno and \c function
    //!  arguments which are usually __FILE__, __LINE__ and
    //!  __PRETTY_FUNCTION__ in the caller source.
    //--------------------------------------------------------------------
    bool Log(const std::string & filename, int lineno, 
             const std::string & function, int priority, 
             const char * message, ...);
    
    //--------------------------------------------------------------------
    //!  Just like vsyslog(), takes a priority and a format string and
    //!  a va_list.  Returns true on success, false on failure.
    //--------------------------------------------------------------------
    bool VaLog(int priority, const std::string & message, 
               va_list & vaList);

    //--------------------------------------------------------------------
    //!  Just like vsyslog(), but takes \c filename, \c lineno  and
    //!  \c function arguments which are usually __FILE__, __LINE__ and
    //!  __PRETTY_FUNCTION__ in the caller source.  Returns true on 
    //!  success, false on failure.
    //--------------------------------------------------------------------
    bool VaLog(const std::string & filename, int lineno, 
               const std::string & function, int priority,
               const std::string & message, va_list & vaList);

    //--------------------------------------------------------------------
    //!  Writes buffered data to the output log (i.e. flushes the buffer).
    //!  Returns true on success, false on failure.
    //--------------------------------------------------------------------
    bool Flush();
    
    //--------------------------------------------------------------------
    //!  Returns the lowest message priority that the logger will log.
    //--------------------------------------------------------------------
    int MinimumPriority() const;

    //--------------------------------------------------------------------
    //!  Sets and returns the lowest message priority that the logger
    //!  will log.  By default, all message priorities will be logged
    //!  (minimum priority == LOG_DEBUG).
    //--------------------------------------------------------------------
    int MinimumPriority(int minimumPriority);

    //--------------------------------------------------------------------
    //!  If the logger is set to show priorities in each log message,
    //!  returns true.  Else returns false.
    //--------------------------------------------------------------------
    bool ShowPriorities() const;
    
    //--------------------------------------------------------------------
    //!  If \c showPriorities is true, the logger will tag each log
    //!  message with an indication of the message's priority.  The
    //!  tags are of the form "[x]" where x is a single letter.
    //!  Hence we have "[M]" for LOG_EMERG, "[A]" for LOG_ALERT, "[C]"
    //!  for LOG_CRIT, "[E]" for LOG_ERR, "[W]" for LOG_WARN, "[N]" for
    //!  LOG_NOTICE, "[I]" for LOG_INFO and "[D]" for LOG_DEBUG.
    //!  By default, message priorities are not shown.
    //--------------------------------------------------------------------
    bool ShowPriorities(bool showPriorities);

    //--------------------------------------------------------------------
    //!  Returns the current rollover size used by the logger.
    //--------------------------------------------------------------------
    uint32_t RolloverSize() const;

    //--------------------------------------------------------------------
    //!  If the logger is set to show function information in
    //!  Log(const string &, int, const string &, int, const char *, ...),
    //!  returns true.
    //--------------------------------------------------------------------
    bool ShowFunction() const;

    //--------------------------------------------------------------------
    //!  If \c showFunction is true, the logger will show function
    //!  information in each log message logged with
    //!  Log(const string &, int, const string &, int, const char *, ...).
    //!  If \c showFunction is false, the logger will not show function
    //!  information in log messages.
    //--------------------------------------------------------------------
    bool ShowFunction(bool showFunction);
       
    //--------------------------------------------------------------------
    //!  If the logger is set to show file location information,
    //!  returns true.
    //--------------------------------------------------------------------
    bool ShowFileLocation() const;

    //--------------------------------------------------------------------
    //!  If \c showFileLocation is true, the logger will show the
    //!  file and line number information in each log message logged with
    //!  Log(const string &, int, const string &, int, const char * , ...).
    //!  If  \c showFileLocation is false, the logger will not show file
    //!  and line number information in log messages.
    //--------------------------------------------------------------------
    bool ShowFileLocation(bool showFileLocation);
       
    //--------------------------------------------------------------------
    //!  Sets and returns the rollover size used by the logger.  This is
    //!  the number of bytes at which the logger will roll over the log
    //!  files.
    //--------------------------------------------------------------------
    uint32_t RolloverSize(uint32_t rolloverSize);
    
    //--------------------------------------------------------------------
    //!  Returns the number of rollover files used by the logger.
    //--------------------------------------------------------------------
    uint32_t NumRolloverFiles() const;
    
    //--------------------------------------------------------------------
    //!  Sets and returns the number of rollover files used by the logger.
    //--------------------------------------------------------------------
    uint32_t NumRolloverFiles(uint32_t numRolloverFiles);

    //--------------------------------------------------------------------
    //!  Overrides the standard priority tags and names.  The key in
    //!  @c priorities is the log priority (e.g. @c LOG_ERR), and each
    //!  value is a (tag,name) pair, e.g. ("[E]", "error").
    //--------------------------------------------------------------------
    bool SetPriorities(const std::map<int, std::pair<std::string,std::string> > & priorities);

    //------------------------------------------------------------------------
    //!  Returns the priority tag ("[F]", "[W]", et. al.) for a given
    //!  \c priority.
    //------------------------------------------------------------------------
    std::string PriorityTag(int priority);
    
  private:
    int                        _logOptions;
    int                        _minimumPriority;
    std::string                _ident;
    bool                       _showPriorities;
    bool                       _showFunction;
    bool                       _showFileLocation;
    std::string                _hostName;
    std::string                _filePrefix;
    FILE                      *_file;
    off_t                      _rolloverSize;
    uint32_t                   _numRolloverFiles;
    off_t                      _currentFilePosition;
    BufferingType              _bufferingType;
    
    mutable std::mutex         _mutex;
    std::map<std::string,int>  _priorityNames;
    std::map<int,std::string>  _priorityTags;

    static const std::map<std::string,int>  _defaultPriorityNames;
    static const std::map<int,std::string>  _defaultPriorityTags;

    bool Lock() const;
    bool Unlock() const;
    bool CloseNoLock();
    bool OpenNoLock(const std::string & ident, int logopt,
                    const std::string & filename,
                    BufferingType bufferingType = e_blockBuffered);
    bool OpenFile(const std::string & filename);
    bool RollFilesOver();
    void InitializePriorities();
  };

}  // namespace Dwm

#endif  // _DWMFILELOGGER_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
