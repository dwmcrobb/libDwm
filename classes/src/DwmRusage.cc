//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2020, 2025
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

#include "DwmEngFormat.hh"
#include "DwmDescriptorIO.hh"
#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmSysLogger.hh"
#include "DwmRusage.hh"
#include "DwmEndianness.hh"

using namespace std;

namespace Dwm {

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
  const TimeValue64 & Rusage::UserTime() const
  {
    return(_userTime);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TimeValue64 & Rusage::SystemTime() const
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
#if defined(DWM_CAN_USE_REFLECTION)
    return StreamIO::ReadNonstaticMembers(is, *this);
#else
    StreamIO::ReadV(is, _userTime, _systemTime, _maxResidentSetSize,
                    _integralSharedTextMemorySize, _integralUnsharedDataSize,
                    _integralUnsharedStackSize, _pageReclaims, _pageFaults,
                    _swaps, _blockInputOperations, _blockOutputOperations,
                    _messagesSent, _messagesReceived, _signalsReceived,
                    _voluntaryContextSwitches, _involuntaryContextSwitches);
    return(is);
#endif
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Rusage::Write(std::ostream & os) const
  {
#if defined(DWM_CAN_USE_REFLECTION)
    return StreamIO::WriteNonstaticMembers(os, *this);
#else
    StreamIO::WriteV(os, _userTime, _systemTime, _maxResidentSetSize,
                     _integralSharedTextMemorySize,
                     _integralUnsharedDataSize, _integralUnsharedStackSize,
                     _pageReclaims, _pageFaults, _swaps,
                     _blockInputOperations, _blockOutputOperations,
                     _messagesSent, _messagesReceived, _signalsReceived,
                     _voluntaryContextSwitches, _involuntaryContextSwitches);
    return(os);
#endif
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & Rusage::NRead(std::istream & is)
  {
    StreamIO::NReadV(is, _userTime, _systemTime, _maxResidentSetSize,
                     _integralSharedTextMemorySize, _integralUnsharedDataSize,
                     _integralUnsharedStackSize, _pageReclaims, _pageFaults,
                     _swaps, _blockInputOperations, _blockOutputOperations,
                     _messagesSent, _messagesReceived, _signalsReceived,
                     _voluntaryContextSwitches, _involuntaryContextSwitches);
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Rusage::NWrite(std::ostream & os) const
  {
#if defined(DWM_CAN_USE_REFLECTION)
    return StreamIO::NWriteNonstaticMembers(os, *this);
#else
    StreamIO::NWriteV(os, _userTime, _systemTime, _maxResidentSetSize,
                      _integralSharedTextMemorySize,
                      _integralUnsharedDataSize, _integralUnsharedStackSize,
                      _pageReclaims, _pageFaults, _swaps,
                      _blockInputOperations, _blockOutputOperations,
                      _messagesSent, _messagesReceived, _signalsReceived,
                      _voluntaryContextSwitches, _involuntaryContextSwitches);
    return(os);
#endif
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


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
