//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2008, 2016
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
//!  \file DwmDataLog.hh
//!  \brief Dwm::DataLog and Dwm::DataLog::Entry class templates
//---------------------------------------------------------------------------

#ifndef _DWMDATALOG_HH_
#define _DWMDATALOG_HH_

#include <cassert>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmSysLogger.hh"
#include "DwmTimeValue.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Simple class template for data logging.  Each log entry is
  //!  represented by the DataLog::Entry class, which holds a timestamp
  //!  and and instance of the parameterized type.
  //--------------------------------------------------------------------------
  template <typename T>
  class DataLog
  {
  public:
    //------------------------------------------------------------------------
    //!  Encapsulates a single data log entry.
    //------------------------------------------------------------------------
    class Entry
    {
    public:
      //----------------------------------------------------------------------
      //!  Constructor
      //----------------------------------------------------------------------
      Entry()
          : _timestamp(0,0), _value()
      {}
      
      //----------------------------------------------------------------------
      //!  Construct from the given @c value.
      //----------------------------------------------------------------------
      Entry(const T & value)
          : _timestamp(true), _value(value)
      {}

      //----------------------------------------------------------------------
      //!  Returns the timestamp of the Entry.
      //----------------------------------------------------------------------
      const TimeValue & Timestamp() const
      {
        return(_timestamp);
      }
      
      //----------------------------------------------------------------------
      //!  Returns the value of the entry.
      //----------------------------------------------------------------------
      const T & Value() const
      {
        return(_value);
      }
    
      //----------------------------------------------------------------------
      //!  Reads the Entry from the given istream @c is.
      //----------------------------------------------------------------------
      std::istream & Read(std::istream & is)
      {
        if (StreamIO::Read(is, _timestamp))
          StreamIO::Read(is, _value);
        return(is);
      }
      
      //----------------------------------------------------------------------
      //!  Writes the Entry to the given ostream @c os.
      //----------------------------------------------------------------------
      std::ostream & Write(std::ostream & os) const
      {
        if (StreamIO::Write(os, _timestamp))
          StreamIO::Write(os, _value);
        return(os);
      }
    
      //----------------------------------------------------------------------
      //!  Returns the number of bytes that would be written if the Write()
      //!  member were called.
      //----------------------------------------------------------------------
      size_t StreamedLength() const
      {
        return(IOUtils::StreamedLength(_timestamp) + IOUtils::StreamedLength(_value));
      }

      //----------------------------------------------------------------------
      //!  Prints the Entry to an ostream in human-readable form.  Note that
      //!  this depends on the paramaterized type having a similar operator.
      //----------------------------------------------------------------------
      friend std::ostream &
      operator << (std::ostream & os, const Entry & entry)
      {
        struct tm  localTime = entry._timestamp.LocalTime();
        char       buf[32];
        size_t     n = strftime(buf, 32, "%m/%d/%Y %H:%M:%S", &localTime);
        std::ostringstream  ostr;
        ostr << buf << '.' 
             << std::setfill('0') 
             << std::setw(6) << entry._timestamp.Usecs();
        os << ostr.str() << " " << entry._value;
        return(os);
      }
      
    private:
      TimeValue  _timestamp;
      T          _value;
    };

    //------------------------------------------------------------------------
    //!  Constructor.  \c filename is the name of the file to use.
    //------------------------------------------------------------------------
    DataLog(const std::string & filename)
        : _filename(filename), _s(0), _mode(), _mtx()
    {
      assert(! _filename.empty());
    }
    
    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~DataLog()
    {
      Close();
    }
    
    //------------------------------------------------------------------------
    //!  Opens the log with the given \c mode.  Returns true on success.
    //------------------------------------------------------------------------
    bool Open(std::ios_base::openmode mode)
    {
      _mtx.lock();
      bool  rc = OpenNoLock(mode);
      _mtx.unlock();
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Rewinds the log to the beginning.  Returns true on success.
    //------------------------------------------------------------------------
    bool Rewind()
    {
      bool  rc = false;
      _mtx.lock();
      if (_s) {
        if ((_mode & std::ios_base::in) == std::ios_base::in) {
          _s->seekg(0);
        }
        if ((_mode & std::ios_base::out) == std::ios_base::out) {
          _s->seekp(0);
        }
        rc = true;
      }
      _mtx.unlock();
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Closes the log.  Returns true on success.
    //------------------------------------------------------------------------
    bool Close()
    {
      _mtx.lock();
      bool  rc = CloseNoLock();
      _mtx.unlock();
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Fetches the next entry from the log.  Returns true on succes.
    //------------------------------------------------------------------------
    bool GetNext(Entry & entry)
    {
      bool  rc = false;
      _mtx.lock();
      if (_s) {
        rc = entry.Read(*_s) ? true : false;
      }
      _mtx.unlock();
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Writes an entry to the log.  Returns true on success.
    //------------------------------------------------------------------------
    bool Record(const T & value)
    {
      bool  rc = false;
      _mtx.lock();
      if (_s) {
        Entry  entry(value);
        rc = entry.Write(*_s) ? true : false;
      }
      _mtx.unlock();
      return(rc);
    }

  private:
    std::string               _filename;
    std::fstream             *_s;
    std::ios_base::openmode   _mode;
    std::mutex                _mtx;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool OpenNoLock(std::ios_base::openmode mode)
    {
      assert(! _s);
      _s = new std::fstream(_filename.c_str(), mode);
      if (_s && *_s) {
        _mode = mode;
        Syslog(LOG_INFO, "DataLog %s opened", _filename.c_str());
      }
      else {
        Syslog(LOG_ERR, "Failed to open DataLog %s: %m", _filename.c_str());
      }
      return(*_s ? true : false);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool CloseNoLock()
    {
      bool  rc = false;
      if (_s) {
        _s->close();
        delete(_s);
        _s = 0;
        rc = true;
        Syslog(LOG_INFO, "DataLog %s closed", _filename.c_str());
      }
      return(rc);
    }
    
  };
  
}  // namespace Dwm

#endif  // _DWMDATALOG_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
