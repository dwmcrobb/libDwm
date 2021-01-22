//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmRusage.cc 11132 $
// @(#) $Id: DwmRusage.cc 11132 2020-09-07 23:34:09Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2020
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
//!  \file DwmRusage.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Rusage class implementation
//---------------------------------------------------------------------------

#include <cassert>
#include <cstring>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmEngFormat.hh"
#include "DwmDescriptorIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmSysLogger.hh"
#include "DwmRusage.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmRusage.cc 11132 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Rusage::Rusage()
      : _userTime(0, 0), _systemTime(0, 0), _maxResidentSetSize(0),
        _integralSharedTextMemorySize(0), _integralUnsharedDataSize(0),
        _integralUnsharedStackSize(0), _pageReclaims(0), _pageFaults(0),
        _swaps(0), _blockInputOperations(0), _blockOutputOperations(0),
        _messagesSent(0), _messagesReceived(0), _signalsReceived(0),
        _voluntaryContextSwitches(0), _involuntaryContextSwitches(0)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Rusage::Rusage(const struct rusage & rusage)
  {
    Set(rusage);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Rusage::Set(const struct rusage & rusage)
  {
    _userTime                      = rusage.ru_utime;
    _systemTime                    = rusage.ru_stime;
    _maxResidentSetSize            = rusage.ru_maxrss;
    _maxResidentSetSize           *= 1024;
    _integralSharedTextMemorySize  = rusage.ru_ixrss;
    _integralSharedTextMemorySize *= 1024;
    _integralUnsharedDataSize      = rusage.ru_idrss;
    _integralUnsharedDataSize     *= 1024;
    _integralUnsharedStackSize     = rusage.ru_isrss;
    _integralUnsharedStackSize    *= 1024;
    _pageReclaims                  = rusage.ru_minflt;
    _pageFaults                    = rusage.ru_majflt;
    _swaps                         = rusage.ru_nswap;
    _blockInputOperations          = rusage.ru_inblock;
    _blockOutputOperations         = rusage.ru_oublock;
    _messagesSent                  = rusage.ru_msgsnd;
    _messagesReceived              = rusage.ru_msgrcv;
    _signalsReceived               = rusage.ru_nsignals;
    _voluntaryContextSwitches      = rusage.ru_nvcsw;
    _involuntaryContextSwitches    = rusage.ru_nivcsw;
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Rusage::Get(int who)
  {
    bool  rc = false;
    struct rusage  r;
    if (getrusage(who, &r) == 0) {
      Set(r);
      rc = true;
    }
    return(rc);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TimeValue & Rusage::UserTime() const
  {
    return(_userTime);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TimeValue & Rusage::SystemTime() const
  {
    return(_systemTime);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int64_t Rusage::MaxResidentSetSize() const
  {
    return(_maxResidentSetSize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int64_t Rusage::IntegralSharedTextMemorySize() const
  {
    return(_integralSharedTextMemorySize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int64_t Rusage::IntegralUnsharedDataSize() const
  {
    return(_integralUnsharedDataSize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int64_t Rusage::IntegralUnsharedStackSize() const
  {
    return(_integralUnsharedStackSize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::PageReclaims() const
  {
    return(_pageReclaims);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::PageFaults() const
  {
    return(_pageFaults);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::Swaps() const
  {
    return(_swaps);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::BlockInputOperations() const
  {
    return(_blockInputOperations);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::BlockOutputOperations() const
  {
    return(_blockOutputOperations);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::MessagesSent() const
  {
    return(_messagesSent);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::MessagesReceived() const
  {
    return(_messagesReceived);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::SignalsReceived() const
  {
    return(_signalsReceived);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::VoluntaryContextSwitches() const
  {
    return(_voluntaryContextSwitches);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int32_t Rusage::InvoluntaryContextSwitches() const
  {
    return(_involuntaryContextSwitches);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & operator << (ostream & os, const Rusage & r)
  {
    if (os) {
      os << "Rusage: "
         << endl
         << "  UserTime:                     "
         << EngFormat((double)r._userTime) << " secs"
         << endl
         << "  SystemTime:                   "
         << EngFormat((double)r._systemTime) << " secs"
         << endl
         << "  MaxResidentSetSize:           " 
         << EngFormat(r._maxResidentSetSize)
         << endl
         << "  IntegralSharedTextMemorySize: "
         << EngFormat(r._integralSharedTextMemorySize)
         << endl
         << "  IntegralUnsharedDataSize:     "
         << EngFormat(r._integralUnsharedDataSize)
         << endl
         << "  IntegralUnsharedStackSize:    "
         << EngFormat(r._integralUnsharedStackSize)
         << endl
         << "  PageReclaims:                 " << r._pageReclaims
         << endl
         << "  PageFaults:                   " << r._pageFaults
         << endl
         << "  Swaps:                        " << r._swaps
         << endl
         << "  BlockInputOperations:         " << r._blockInputOperations
         << endl
         << "  BlockOutputOperations:        " << r._blockOutputOperations
         << endl
         << "  MessagesSent:                 " << r._messagesSent
         << endl
         << "  MessagesReceived:             " << r._messagesReceived
         << endl
         << "  SignalsReceived:              " << r._signalsReceived
         << endl
         << "  VoluntaryContextSwitches:     " << r._voluntaryContextSwitches
         << endl
         << "  InvoluntaryContextSwitches:   " 
         << r._involuntaryContextSwitches
         << endl;
      
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Rusage::Read(std::istream & is)
  {
    if (is) {
      if (! StreamIO::Read(is, _userTime))                       goto readError;
      if (! StreamIO::Read(is, _systemTime))                     goto readError;
      if (! StreamIO::Read(is, _maxResidentSetSize))             goto readError;
      if (! StreamIO::Read(is, _integralSharedTextMemorySize))   goto readError;
      if (! StreamIO::Read(is, _integralUnsharedDataSize))       goto readError;
      if (! StreamIO::Read(is, _integralUnsharedStackSize))      goto readError;
      if (! StreamIO::Read(is, _pageReclaims))                   goto readError;
      if (! StreamIO::Read(is, _pageFaults))                     goto readError;
      if (! StreamIO::Read(is, _swaps))                          goto readError;
      if (! StreamIO::Read(is, _blockInputOperations))           goto readError;
      if (! StreamIO::Read(is, _blockOutputOperations))          goto readError;
      if (! StreamIO::Read(is, _messagesSent))                   goto readError;
      if (! StreamIO::Read(is, _messagesReceived))               goto readError;
      if (! StreamIO::Read(is, _signalsReceived))                goto readError;
      if (! StreamIO::Read(is, _voluntaryContextSwitches))       goto readError;
      StreamIO::Read(is, _involuntaryContextSwitches);
    }
  readError:
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Rusage::Write(std::ostream & os) const
  {
    if (os) {
      if (! StreamIO::Write(os, _userTime))                        goto writeError;
      if (! StreamIO::Write(os, _systemTime))                      goto writeError;
      if (! StreamIO::Write(os, _maxResidentSetSize))              goto writeError;
      if (! StreamIO::Write(os, _integralSharedTextMemorySize))    goto writeError;
      if (! StreamIO::Write(os, _integralUnsharedDataSize))        goto writeError;
      if (! StreamIO::Write(os, _integralUnsharedStackSize))       goto writeError;
      if (! StreamIO::Write(os, _pageReclaims))                    goto writeError;
      if (! StreamIO::Write(os, _pageFaults))                      goto writeError;
      if (! StreamIO::Write(os, _swaps))                           goto writeError;
      if (! StreamIO::Write(os, _blockInputOperations))            goto writeError;
      if (! StreamIO::Write(os, _blockOutputOperations))           goto writeError;
      if (! StreamIO::Write(os, _messagesSent))                    goto writeError;
      if (! StreamIO::Write(os, _messagesReceived))                goto writeError;
      if (! StreamIO::Write(os, _signalsReceived))                 goto writeError;
      if (! StreamIO::Write(os, _voluntaryContextSwitches))        goto writeError;
      StreamIO::Write(os, _involuntaryContextSwitches);
    }
  writeError:
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  static bool MyGZIORead(gzFile gzf, T & val, int & totalBytes)
  {
    bool  rc = false;
    if (gzf) {
      int  bytesRead = GZIO::Read(gzf, val);
      if (bytesRead == (int)IOUtils::StreamedLength(val)) {
        rc = true;
        totalBytes += bytesRead;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Rusage::Read(gzFile gzf)
  {
    if (! gzf)
      return(-1);
    
    int  rc = 0;

    if (! MyGZIORead(gzf, _userTime, rc))                      goto readError;
    if (! MyGZIORead(gzf, _systemTime, rc))                    goto readError;
    if (! MyGZIORead(gzf, _maxResidentSetSize, rc))            goto readError;
    if (! MyGZIORead(gzf, _integralSharedTextMemorySize, rc))  goto readError;
    if (! MyGZIORead(gzf, _integralUnsharedDataSize, rc))      goto readError;
    if (! MyGZIORead(gzf, _integralUnsharedStackSize, rc))     goto readError;
    if (! MyGZIORead(gzf, _pageReclaims, rc))                  goto readError;
    if (! MyGZIORead(gzf, _pageFaults, rc))                    goto readError;
    if (! MyGZIORead(gzf, _swaps, rc))                         goto readError;
    if (! MyGZIORead(gzf, _blockInputOperations, rc))          goto readError;
    if (! MyGZIORead(gzf, _blockOutputOperations, rc))         goto readError;
    if (! MyGZIORead(gzf, _messagesSent, rc))                  goto readError;
    if (! MyGZIORead(gzf, _messagesReceived, rc))              goto readError;
    if (! MyGZIORead(gzf, _signalsReceived, rc))               goto readError;
    if (! MyGZIORead(gzf, _voluntaryContextSwitches, rc))      goto readError;
    if (! MyGZIORead(gzf, _involuntaryContextSwitches, rc))    goto readError;
    
    return(rc);
    
  readError:
    Syslog(LOG_ERR, "Read(gzf) failed for Rusage");
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  static bool MyGZIOWrite(gzFile gzf, const T & val, int & totalBytes)
  {
    bool  rc = false;
    if (gzf) {
      int  bytesWritten = GZIO::Write(gzf, val);
      if (bytesWritten == (int)IOUtils::StreamedLength(val)) {
        rc = true;
        totalBytes += bytesWritten;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Rusage::Write(gzFile gzf) const
  {
    if (! gzf)
      return(-1);

    int  rc = 0;

    if (! MyGZIOWrite(gzf, _userTime, rc))                     goto writeError;
    if (! MyGZIOWrite(gzf, _systemTime, rc))                   goto writeError;
    if (! MyGZIOWrite(gzf, _maxResidentSetSize, rc))           goto writeError;
    if (! MyGZIOWrite(gzf, _integralSharedTextMemorySize, rc)) goto writeError;
    if (! MyGZIOWrite(gzf, _integralUnsharedDataSize, rc))     goto writeError;
    if (! MyGZIOWrite(gzf, _integralUnsharedStackSize, rc))    goto writeError;
    if (! MyGZIOWrite(gzf, _pageReclaims, rc))                 goto writeError;
    if (! MyGZIOWrite(gzf, _pageFaults, rc))                   goto writeError;
    if (! MyGZIOWrite(gzf, _swaps, rc))                        goto writeError;
    if (! MyGZIOWrite(gzf, _blockInputOperations, rc))         goto writeError;
    if (! MyGZIOWrite(gzf, _blockOutputOperations, rc))        goto writeError;
    if (! MyGZIOWrite(gzf, _messagesSent, rc))                 goto writeError;
    if (! MyGZIOWrite(gzf, _messagesReceived, rc))             goto writeError;
    if (! MyGZIOWrite(gzf, _signalsReceived, rc))              goto writeError;
    if (! MyGZIOWrite(gzf, _voluntaryContextSwitches, rc))     goto writeError;
    if (! MyGZIOWrite(gzf, _involuntaryContextSwitches, rc))   goto writeError;
      
    return(rc);

  writeError:
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  static bool MyIORead(int fd, T & val, ssize_t & totalBytes)
  {
    bool  rc = false;
    if (fd >= 0) {
      ssize_t  bytesRead = DescriptorIO::Read(fd, val);
      if (bytesRead == (int)IOUtils::StreamedLength(val)) {
        rc = true;
        totalBytes += bytesRead;
      }
    }
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Rusage::Read(int fd)
  {
    if (fd < 0)
      return(-1);
    
    ssize_t  rc = 0;

    if (! MyIORead(fd, _userTime, rc))                      goto readError;
    if (! MyIORead(fd, _systemTime, rc))                    goto readError;
    if (! MyIORead(fd, _maxResidentSetSize, rc))            goto readError;
    if (! MyIORead(fd, _integralSharedTextMemorySize, rc))  goto readError;
    if (! MyIORead(fd, _integralUnsharedDataSize, rc))      goto readError;
    if (! MyIORead(fd, _integralUnsharedStackSize, rc))     goto readError;
    if (! MyIORead(fd, _pageReclaims, rc))                  goto readError;
    if (! MyIORead(fd, _pageFaults, rc))                    goto readError;
    if (! MyIORead(fd, _swaps, rc))                         goto readError;
    if (! MyIORead(fd, _blockInputOperations, rc))          goto readError;
    if (! MyIORead(fd, _blockOutputOperations, rc))         goto readError;
    if (! MyIORead(fd, _messagesSent, rc))                  goto readError;
    if (! MyIORead(fd, _messagesReceived, rc))              goto readError;
    if (! MyIORead(fd, _signalsReceived, rc))               goto readError;
    if (! MyIORead(fd, _voluntaryContextSwitches, rc))      goto readError;
    if (! MyIORead(fd, _involuntaryContextSwitches, rc))    goto readError;
    
    return(rc);
    
  readError:
    Syslog(LOG_ERR, "Read(%d) failed for Rusage", fd);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  static bool MyIOWrite(int fd, const T & val, ssize_t & totalBytes)
  {
    bool  rc = false;
    if (fd >= 0) {
      ssize_t  bytesWritten = DescriptorIO::Write(fd, val);
      if (bytesWritten == (int)IOUtils::StreamedLength(val)) {
        rc = true;
        totalBytes += bytesWritten;
      }
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t Rusage::Write(int fd) const
  {
    if (fd < 0)
      return(-1);

    ssize_t  rc = 0;

    if (! MyIOWrite(fd, _userTime, rc))                     goto writeError;
    if (! MyIOWrite(fd, _systemTime, rc))                   goto writeError;
    if (! MyIOWrite(fd, _maxResidentSetSize, rc))           goto writeError;
    if (! MyIOWrite(fd, _integralSharedTextMemorySize, rc)) goto writeError;
    if (! MyIOWrite(fd, _integralUnsharedDataSize, rc))     goto writeError;
    if (! MyIOWrite(fd, _integralUnsharedStackSize, rc))    goto writeError;
    if (! MyIOWrite(fd, _pageReclaims, rc))                 goto writeError;
    if (! MyIOWrite(fd, _pageFaults, rc))                   goto writeError;
    if (! MyIOWrite(fd, _swaps, rc))                        goto writeError;
    if (! MyIOWrite(fd, _blockInputOperations, rc))         goto writeError;
    if (! MyIOWrite(fd, _blockOutputOperations, rc))        goto writeError;
    if (! MyIOWrite(fd, _messagesSent, rc))                 goto writeError;
    if (! MyIOWrite(fd, _messagesReceived, rc))             goto writeError;
    if (! MyIOWrite(fd, _signalsReceived, rc))              goto writeError;
    if (! MyIOWrite(fd, _voluntaryContextSwitches, rc))     goto writeError;
    if (! MyIOWrite(fd, _involuntaryContextSwitches, rc))   goto writeError;
      
    return(rc);

  writeError:
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t Rusage::StreamedLength() const
  {
    uint32_t  rc = IOUtils::StreamedLength(_userTime);
    rc += IOUtils::StreamedLength(_systemTime);
    rc += IOUtils::StreamedLength(_maxResidentSetSize);
    rc += IOUtils::StreamedLength(_integralSharedTextMemorySize);
    rc += IOUtils::StreamedLength(_integralUnsharedDataSize);
    rc += IOUtils::StreamedLength(_integralUnsharedStackSize);
    rc += IOUtils::StreamedLength(_pageReclaims);
    rc += IOUtils::StreamedLength(_pageFaults);
    rc += IOUtils::StreamedLength(_swaps);
    rc += IOUtils::StreamedLength(_blockInputOperations);
    rc += IOUtils::StreamedLength(_blockOutputOperations);
    rc += IOUtils::StreamedLength(_messagesSent);
    rc += IOUtils::StreamedLength(_messagesReceived);
    rc += IOUtils::StreamedLength(_signalsReceived);
    rc += IOUtils::StreamedLength(_voluntaryContextSwitches);
    rc += IOUtils::StreamedLength(_involuntaryContextSwitches);
    return(rc);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Rusage::operator == (const Rusage & r) const
  {
    bool  rc = (
      (_userTime                        == r._userTime)
      && (_systemTime                   == r._systemTime)
      && (_maxResidentSetSize           == r._maxResidentSetSize)
      && (_integralSharedTextMemorySize == r._integralSharedTextMemorySize)
      && (_integralUnsharedDataSize     == r._integralUnsharedDataSize)
      && (_integralUnsharedStackSize    == r._integralUnsharedStackSize)
      && (_pageReclaims                 == r._pageReclaims)
      && (_pageFaults                   == r._pageFaults)
      && (_swaps                        == r._swaps)
      && (_blockInputOperations         == r._blockInputOperations)
      && (_blockOutputOperations        == r._blockOutputOperations)
      && (_messagesSent                 == r._messagesSent)
      && (_messagesReceived             == r._messagesReceived)
      && (_signalsReceived              == r._signalsReceived)
      && (_voluntaryContextSwitches     == r._voluntaryContextSwitches)
      && (_involuntaryContextSwitches   == r._involuntaryContextSwitches)
      );
    return(rc);
  }
  
  
}  // namespace Dwm
