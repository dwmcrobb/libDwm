//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2016, 2020
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
//!  \file DwmProcessInfo.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::ProcessInfo class definition
//---------------------------------------------------------------------------

#ifndef _DWMPROCESSINFO_HH_
#define _DWMPROCESSINFO_HH_

extern "C" {
#ifdef HAVE_KVM_H
    #include <kvm.h>
#endif
  #include <sys/param.h>
#ifndef __linux__
  #include <sys/sysctl.h>
#else
  #include <linux/sysctl.h>
#endif
  #include <sys/user.h>
  #include <inttypes.h>
#ifdef __linux__
  #include <proc/readproc.h>
#endif
#ifdef __APPLE__
  #include <libproc.h>
#endif
}

#include <string>
#include <vector>

#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmRusage.hh"
#include "DwmTimeValue64.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates information about a single process.
  //--------------------------------------------------------------------------
  class ProcessInfo
    : public StreamIOCapable, public StreamedLengthCapable, public GZIOCapable
  {
  public:
    ProcessInfo();

    //------------------------------------------------------------------------
    //!  Construct from a process ID.  This will fetch values from the
    //!  current process table.
    //------------------------------------------------------------------------
    ProcessInfo(pid_t pid);
    
#if (defined(__FreeBSD__) || defined(__APPLE__))
    ProcessInfo(const struct kinfo_proc & proc);
  #if defined(__APPLE__)
    ProcessInfo(const struct proc_taskallinfo & proc);
  #endif
#elif (defined __OpenBSD__)
    ProcessInfo(const struct kinfo_proc2 & proc);
#elif (defined __linux__)
    ProcessInfo(proc_t *proc);
#endif

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    virtual ~ProcessInfo();
    
    //------------------------------------------------------------------------
    //!  Returns the process ID.
    //------------------------------------------------------------------------
    pid_t Id() const;

    //------------------------------------------------------------------------
    //!  Returns the parent process ID.
    //------------------------------------------------------------------------
    pid_t ParentId() const;

    //------------------------------------------------------------------------
    //!  Returns the process group ID.
    //------------------------------------------------------------------------
    pid_t GroupId() const;

    //------------------------------------------------------------------------
    //!  Returns the controlling tty name.
    //------------------------------------------------------------------------
    const std::string & ControllingTty() const;

    pid_t SessionId() const;
    
    //------------------------------------------------------------------------
    //!  Returns the effective user ID.
    //------------------------------------------------------------------------
    uid_t EffectiveUserId() const;

    //------------------------------------------------------------------------
    //!  Returns the username of the effective user ID.
    //------------------------------------------------------------------------
    std::string EffectiveUserName() const;

    //------------------------------------------------------------------------
    //!  Returns the real user ID.
    //------------------------------------------------------------------------
    uid_t RealUserId() const;

    //------------------------------------------------------------------------
    //!  Returns the username of the real user ID.
    //------------------------------------------------------------------------
    std::string RealUserName() const;

    //------------------------------------------------------------------------
    //!  Returns the saved effective user ID.
    //------------------------------------------------------------------------
    uid_t SavedEffectiveUserId() const;

    //------------------------------------------------------------------------
    //!  Returns the username of the saved effective user ID.
    //------------------------------------------------------------------------
    std::string SavedEffectiveUserName() const;

    //------------------------------------------------------------------------
    //!  Returns the real process group ID.
    //------------------------------------------------------------------------
    uid_t RealGroupId() const;

    //------------------------------------------------------------------------
    //!  Returns the saved effective group ID.
    //------------------------------------------------------------------------
    uid_t SavedEffectiveGroupId() const;

    //------------------------------------------------------------------------
    //!  Returns the command.
    //------------------------------------------------------------------------
    const std::string & Command() const;

    //------------------------------------------------------------------------
    //!  Returns the argument list.
    //------------------------------------------------------------------------
    const std::vector<std::string> & Args() const;
    
    //------------------------------------------------------------------------
    //!  Returns the proces start time.
    //------------------------------------------------------------------------
    const TimeValue64 & StartTime() const;

    //------------------------------------------------------------------------
    //!  Returns the virtual size of the process (in bytes).
    //------------------------------------------------------------------------
    uint32_t Size() const;

    //------------------------------------------------------------------------
    //!  Returns the resident set size of the process (in bytes).
    //------------------------------------------------------------------------
    uint32_t ResidentSetSize() const;

    //------------------------------------------------------------------------
    //!  Returns the process rusage.
    //------------------------------------------------------------------------
    const Dwm::Rusage & Rusage() const;

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const ProcessInfo & process) const;
    
    //------------------------------------------------------------------------
    //!  Reads from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads from a gzFile.  Returns the number of bytes read on success,
    //!  -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes to a gzFile.  Returns the number of bytes written on success,
    //!  -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  one of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override;
    
    //------------------------------------------------------------------------
    //!  Adds an argument to the list of args.  This shouldn't be used
    //!  by anything but GetProcessTable()
    //------------------------------------------------------------------------
    void AddArg(const char *arg);
    
  private:
    pid_t                     _id;
    pid_t                     _parentId;
    pid_t                     _groupId;
    std::string               _controllingTty;
    pid_t                     _sessionId;
    pid_t                     _terminalSessionId;
    uid_t                     _effectiveUserId;
    uid_t                     _realUserId;
    uid_t                     _savedEffectiveUserId;
    uid_t                     _realGroupId;
    uid_t                     _savedEffectiveGroupId;
    std::string               _command;
    TimeValue64               _startTime;
    uint32_t                  _size;
    uint32_t                  _residentSetSize;
    Dwm::Rusage               _rusage;
    std::vector<std::string>  _args;

  };
  
}  // namespace Dwm

#endif  // _DWMPROCESSINFO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
