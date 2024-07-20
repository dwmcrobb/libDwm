//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2016, 2024
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
//!  \file DwmRusage.hh
//!  \brief Dwm::Rusage class definition
//---------------------------------------------------------------------------

#ifndef _DWMRUSAGE_HH_
#define _DWMRUSAGE_HH_

extern "C" {
  #include <sys/resource.h>
}

#include <cstdint>

#include "DwmTimeValue.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates struct rusage and getrusage() functionality.
  //--------------------------------------------------------------------------
  class Rusage
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Rusage();

    //------------------------------------------------------------------------
    //!  Construct from a struct rusagee
    //------------------------------------------------------------------------
    Rusage(const struct rusage & rusage);
    
    //------------------------------------------------------------------------
    //!  Get the current process' rusage.  \c who may be RUSAGE_SELF or
    //!  RUSAGE_CHILDREN.  See the getrusage(2) manpage.
    //------------------------------------------------------------------------
    bool Get(int who);
  
    //------------------------------------------------------------------------
    //!  The total amount of time spent executing in user mode.
    //------------------------------------------------------------------------
    const TimeValue & UserTime() const;

    //------------------------------------------------------------------------
    //!  The total amount of time spent in the system executing on
    //!  behalf of the process(es).
    //------------------------------------------------------------------------
    const TimeValue & SystemTime() const;

    //------------------------------------------------------------------------
    //!  The maximum resident set size utilized (in bytes).
    //------------------------------------------------------------------------
    int64_t MaxResidentSetSize() const;

    //------------------------------------------------------------------------
    //!  An integral value indicating the amount of memory used
    //!  by the text segment that was also shared among other pro-
    //!  cesses.  This value is expressed in units of bytes *
    //!  ticks-of-execution.  Ticks are statistics clock ticks.  The
    //!  statistics clock has a frequency of sysconf(_SC_CLK_TCK)
    //!  ticks per second.
    //------------------------------------------------------------------------
    int64_t IntegralSharedTextMemorySize() const;

    //------------------------------------------------------------------------
    //!  An integral value of the amount of unshared memory residing
    //!  in the data segment of a process (expressed in units of
    //!  bytes * ticks-of-execution).
    //------------------------------------------------------------------------
    int64_t IntegralUnsharedDataSize() const;

    //------------------------------------------------------------------------
    //!  An integral value of the amount of unshared memory residing
    //!  in the stack segment of a process (expressed in units of
    //!  bytes * ticks-of-execution).
    //------------------------------------------------------------------------
    int64_t IntegralUnsharedStackSize() const;

    //------------------------------------------------------------------------
    //!  The number of page faults serviced without any I/O activity;
    //!  here I/O activity is avoided by reclaiming a page frame
    //!  from the list of pages awaiting reallocation.
    //------------------------------------------------------------------------
    int32_t PageReclaims() const;

    //------------------------------------------------------------------------
    //!  The number of page faults serviced that required I/O activity.
    //------------------------------------------------------------------------
    int32_t PageFaults() const;

    //------------------------------------------------------------------------
    //!  The number of times a process was ``swapped'' out of main memory.
    //------------------------------------------------------------------------
    int32_t Swaps() const;

    //------------------------------------------------------------------------
    //!  The number of times the file system had to perform input.
    //------------------------------------------------------------------------
    int32_t BlockInputOperations() const;

    //------------------------------------------------------------------------
    //!  The number of times the file system had to perform output.
    //------------------------------------------------------------------------
    int32_t BlockOutputOperations() const;

    //------------------------------------------------------------------------
    //!  The number of IPC messages sent.
    //------------------------------------------------------------------------
    int32_t MessagesSent() const;

    //------------------------------------------------------------------------
    //!  The number of IPC messages received.
    //------------------------------------------------------------------------
    int32_t MessagesReceived() const;

    //------------------------------------------------------------------------
    //!  The number of signals delivered.
    //------------------------------------------------------------------------
    int32_t SignalsReceived() const;

    //------------------------------------------------------------------------
    //!  The number of times a context switch resulted due to a process
    //!  voluntarily giving up the processor before its time slice was 
    //!  completed (usually to await availability of a resource).
    //------------------------------------------------------------------------
    int32_t VoluntaryContextSwitches() const;

    //------------------------------------------------------------------------
    //!  The number of times a context switch resulted due to a higher
    //!  priority process becoming runnable or because the current process
    //!  exceeded its time slice.
    //------------------------------------------------------------------------
    int32_t InvoluntaryContextSwitches() const;

    //------------------------------------------------------------------------
    //!  Prints to an ostream.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, const Rusage & r);

    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);
    
    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;
    
    //------------------------------------------------------------------------
    //!  Reads from a gzFile.  Returns the number of bytes read on success,
    //!  -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);
    
    //------------------------------------------------------------------------
    //!  Writes to a gzFile.  Returns the number of bytes written on 
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;
    
    //------------------------------------------------------------------------
    //!  Reads from a file descriptor.  Returns the number of bytes read 
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);
    
    //------------------------------------------------------------------------
    //!  Writes to a file descriptor.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  one of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const Rusage & rusage) const;
    
  private:
    TimeValue  _userTime;
    TimeValue  _systemTime;
    int64_t    _maxResidentSetSize;
    int64_t    _integralSharedTextMemorySize;
    int64_t    _integralUnsharedDataSize;
    int64_t    _integralUnsharedStackSize;
    int32_t    _pageReclaims;
    int32_t    _pageFaults;
    int32_t    _swaps;
    int32_t    _blockInputOperations;
    int32_t    _blockOutputOperations;
    int32_t    _messagesSent;
    int32_t    _messagesReceived;
    int32_t    _signalsReceived;
    int32_t    _voluntaryContextSwitches;
    int32_t    _involuntaryContextSwitches;
    
    void Set(const struct rusage & rusage);
  };
  
}  // namespace Dwm

#endif  // _DWMRUSAGE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
