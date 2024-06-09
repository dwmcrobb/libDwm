//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmProcessInfo.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::ProcessInfo class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/stat.h>
  #include <sys/types.h>
#ifdef __APPLE__
  #include <sys/tty.h>
  #include <sys/proc_info.h>
#endif
  #include <pwd.h>
}

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DwmGZIO.hh"
#include "DwmIOUtils.hh"
#include "DwmStreamIO.hh"
#include "DwmPassword.hh"
#include "DwmProcessInfo.hh"
#include "DwmProcessTable.hh"

namespace Dwm {

  using namespace std;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo()
      : _id(0), _parentId(0), _groupId(0), _controllingTty(), 
        _sessionId(0), _terminalSessionId(0),
        _effectiveUserId(0), _realUserId(0), _savedEffectiveUserId(0),
        _realGroupId(0), _savedEffectiveGroupId(0), _command(), 
        _startTime(0, 0), _size(0), _residentSetSize(0), _rusage(), _args()
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo(pid_t pid)
      : _id(0), _parentId(0), _groupId(0), _controllingTty(),
        _sessionId(0), _terminalSessionId(0),
        _effectiveUserId(0), _realUserId(0), _savedEffectiveUserId(0),
        _realGroupId(0), _savedEffectiveGroupId(0), _command(),
        _startTime(0, 0), _size(0), _residentSetSize(0), _rusage(), _args()
  {
    ProcessTable  processTable;
    GetProcessTable(processTable);
    ProcessTable::const_iterator  iter = processTable.find(pid);
    if (iter != processTable.end())
      *this = iter->second;
  }
  
#if (defined(__FreeBSD__))

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo(const struct kinfo_proc & proc)
      : _controllingTty(), _args()
  {
    _id                    = proc.ki_pid;
    _parentId              = proc.ki_ppid;
    _groupId               = proc.ki_pgid;
    _sessionId             = proc.ki_sid;
    _terminalSessionId     = proc.ki_tsid;
    _effectiveUserId       = proc.ki_uid;
    _realUserId            = proc.ki_ruid;
    _savedEffectiveUserId  = proc.ki_svuid;
    _realGroupId           = proc.ki_rgid;
    _savedEffectiveGroupId = proc.ki_svgid;
    _command               = proc.ki_comm;
    _startTime             = proc.ki_start;
    _size                  = proc.ki_size;
    _residentSetSize       = proc.ki_rssize * getpagesize();
    _rusage                = proc.ki_rusage;
    
    char  *tty = devname(proc.ki_tdev, S_IFCHR);
    if (tty)
      _controllingTty = tty;
  }

#elif (defined __APPLE__)

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo(const struct kinfo_proc & proc)
      : _controllingTty(), _args()
  {
    _id                    = proc.kp_proc.p_pid;
    _parentId              = proc.kp_eproc.e_ppid;
    _groupId               = proc.kp_eproc.e_pgid;
    _sessionId = getsid(_id);
    _terminalSessionId     = proc.kp_eproc.e_tpgid;
    _effectiveUserId       = proc.kp_eproc.e_ucred.cr_uid;
    _realUserId            = proc.kp_eproc.e_pcred.p_ruid;
    _savedEffectiveUserId  = proc.kp_eproc.e_pcred.p_svuid;
    _realGroupId           = proc.kp_eproc.e_pcred.p_rgid;
    _savedEffectiveGroupId = proc.kp_eproc.e_pcred.p_svgid;
    if (proc.kp_eproc.e_paddr) {
      _command               = ((const struct proc_bsdinfo *)(proc.kp_eproc.e_paddr))->pbi_comm;
    }
    /*
    if (_id != getpid()) {
      if (proc.kp_eproc.e_paddr) {
        _command               = proc.kp_eproc.e_paddr->p_comm;
      }
    }
    */
    _startTime             = proc.kp_proc.p_starttime;
    _size                  = proc.kp_eproc.e_xsize;
    _residentSetSize       = proc.kp_eproc.e_xrssize * getpagesize();
    if (proc.kp_proc.p_ru)
      _rusage                = *proc.kp_proc.p_ru;

    /*    
    char  *tty = devname(proc.kp_eproc.e_tsess->s_ttyp->t_dev, S_IFCHR);
    if (tty)
      _controllingTty = tty;
    */
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo(const struct proc_taskallinfo & proc)
      : _controllingTty(), _args()
  {
    _id                    = proc.pbsd.pbi_pid;
    _parentId              = proc.pbsd.pbi_ppid;
    _groupId               = proc.pbsd.pbi_pgid;
    _sessionId = getsid(_id);
    _terminalSessionId     = proc.pbsd.e_tpgid;
    _effectiveUserId       = proc.pbsd.pbi_uid;
    _realUserId            = proc.pbsd.pbi_ruid;
    _savedEffectiveUserId  = proc.pbsd.pbi_svuid;
    _realGroupId           = proc.pbsd.pbi_rgid;
    _savedEffectiveGroupId = proc.pbsd.pbi_svgid;
    _command               = proc.pbsd.pbi_comm;
    _startTime             = TimeValue64(proc.pbsd.pbi_start_tvsec,
                                         proc.pbsd.pbi_start_tvusec);
    _size                  = proc.ptinfo.pti_virtual_size;
    _residentSetSize       = proc.ptinfo.pti_resident_size;
  }
  
#elif (defined __OpenBSD__)

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo(const struct kinfo_proc2 & proc)
      : _controllingTty(), _args()
  {
    _id                    = proc.p_pid;
    _parentId              = proc.p_ppid;
    _groupId               = proc.p__pgid;
    _sessionId             = proc.p_sid;
    _terminalSessionId     = proc.p_tpgid;
    _effectiveUserId       = proc.p_uid;
    _realUserId            = proc.p_ruid;
    _savedEffectiveUserId  = proc.p_svuid;
    _realGroupId           = proc.p_rgid;
    _savedEffectiveGroupId = proc.p_svgid;
    _command               = proc.p_comm;
    _startTime.Set(proc.p_ustart_sec, proc.p_ustart_usec);
    _size                  = proc.p_vm_tsize;
    _size                  += proc.p_vm_dsize;
    _size                  += proc.p_vm_ssize;
    _size                  *= getpagesize();
    _residentSetSize       = proc.p_vm_rssize * getpagesize();
    
    // ProcessInfo::_rusage not implemented for OpenBSD
    // _rusage                = ??;  //  XXX - fix!

    char  *tty = devname(proc.p_tdev, S_IFCHR);
    if (tty)
      _controllingTty = tty;
  }

#elif (defined __linux__)

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static string ReadProcCommFile(pid_t pid)
  {
    string  comm;
    ifstream  is(string("/proc/") + to_string(pid) + "/comm");
    if (is) {
      std::getline(is, comm, '\n');
    }
    comm = "[" + comm + "]";
    return comm;
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::ProcessInfo(proc_t *proc)
  {
    if (proc) {
      _id = proc->tid;
      _parentId = proc->ppid;
      _groupId = proc->pgrp;
      _sessionId = proc->session;
      _terminalSessionId = proc->tpgid;
      _effectiveUserId = proc->euid;
      _realUserId = proc->ruid;
      _savedEffectiveUserId = proc->suid;
      _realGroupId = proc->rgid;
      _savedEffectiveGroupId = proc->sgid;
      if (proc->cmdline) {
        _command = proc->cmdline[0];
        for (int arg = 0; proc->cmdline[arg]; ++arg) {
          _args.push_back(proc->cmdline[arg]);
        }
      }
      else {
        _command = ReadProcCommFile(_id);
      }
      _startTime.Set(proc->start_time, 0);
      _size = proc->size * getpagesize();
      _residentSetSize = proc->resident * getpagesize();
      // tty?
    }
  }
  
#endif

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessInfo::~ProcessInfo()
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t ProcessInfo::Id() const
  {
    return(_id);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t ProcessInfo::ParentId() const
  {
    return(_parentId);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t ProcessInfo::GroupId() const
  {
    return(_groupId);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & ProcessInfo::ControllingTty() const
  {
    return(_controllingTty);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uid_t ProcessInfo::EffectiveUserId() const
  {
    return(_effectiveUserId);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string ProcessInfo::EffectiveUserName() const
  {
    Password  passwd(_effectiveUserId);
    return(passwd.UserName());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uid_t ProcessInfo::RealUserId() const
  {
    return(_realUserId);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string ProcessInfo::RealUserName() const
  {
    Password  passwd(_realUserId);
    return(passwd.UserName());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uid_t ProcessInfo::SavedEffectiveUserId() const
  {
    return(_savedEffectiveUserId);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string ProcessInfo::SavedEffectiveUserName() const
  {
    Password  passwd(_savedEffectiveUserId);
    return(passwd.UserName());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uid_t ProcessInfo::RealGroupId() const
  {
    return(_realGroupId);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uid_t ProcessInfo::SavedEffectiveGroupId() const
  {
    return(_savedEffectiveGroupId);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & ProcessInfo::Command() const
  {
    return(_command);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & ProcessInfo::Command(const string & cmd)
  {
    return(_command = cmd);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const vector<string> & ProcessInfo::Args() const
  {
    return(_args);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void ProcessInfo::AddArg(const char *arg)
  {
    _args.push_back(arg);
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TimeValue64 & ProcessInfo::StartTime() const 
  { 
    return(_startTime);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t ProcessInfo::Size() const
  { 
    return(_size);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t ProcessInfo::ResidentSetSize() const
  { 
    return(_residentSetSize);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ProcessInfo::operator == (const ProcessInfo & process) const
  {
    return ((_id == process._id) &&
            (_parentId == process._parentId) &&
            (_groupId == process._groupId) &&
            (_effectiveUserId == process._effectiveUserId) &&
            (_realUserId == process._realUserId) &&
            (_savedEffectiveUserId == process._savedEffectiveUserId) &&
            (_realGroupId == process._realGroupId) &&
            (_savedEffectiveGroupId == process._savedEffectiveGroupId) &&
            (_command == process._command) &&
            (_startTime == process._startTime) &&
            (_size == process._size) &&
            (_residentSetSize == process._residentSetSize));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  istream & ProcessInfo::Read(istream & is)
  {
    if (is) {
      int32_t  val32;
      if (! StreamIO::Read(is, val32))
        goto readError;
      _id = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _parentId = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _groupId = val32;

      if (! StreamIO::Read(is, _controllingTty))
        goto readError;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _sessionId = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _terminalSessionId = val32;
      
      if (! StreamIO::Read(is, val32))
        goto readError;
      _effectiveUserId = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _realUserId = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _savedEffectiveUserId = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _realGroupId = val32;

      if (! StreamIO::Read(is, val32))
        goto readError;
      _savedEffectiveGroupId = val32;

      if (! StreamIO::Read(is, _command))
        goto readError;

      if (! StreamIO::Read(is, _startTime))
        goto readError;

      if (! StreamIO::Read(is, _size))
        goto readError;

      if (! StreamIO::Read(is, _residentSetSize))
        goto readError;

      if (! StreamIO::Read(is, _rusage))
        goto readError;

      if (!StreamIO::Read(is, _args))
        goto readError;
    }
    readError:
    
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & ProcessInfo::Write(ostream & os) const
  {
    if (os) {
      int32_t  val32 = _id;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _parentId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _groupId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      if (! StreamIO::Write(os, _controllingTty))
        goto writeError;
      
      val32 = _sessionId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _terminalSessionId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _effectiveUserId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _realUserId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _savedEffectiveUserId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _realGroupId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      val32 = _savedEffectiveGroupId;
      if (! StreamIO::Write(os, val32))
        goto writeError;

      if (! StreamIO::Write(os, _command))
        goto writeError;

      if (! StreamIO::Write(os, _startTime))
        goto writeError;

      if (! StreamIO::Write(os, _size))
        goto writeError;

      if (! StreamIO::Write(os, _residentSetSize))
        goto writeError;

      if (! StreamIO::Write(os, _rusage))
        goto writeError;

      if (! StreamIO::Write(os, _args))
        goto writeError;
    }
    writeError:
    
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int ProcessInfo::Read(gzFile gzf)
  {
    if (! gzf)
      return(-1);
    
    int  rc = 0;
    
    int32_t  val32;
    int  bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _id = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _parentId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _groupId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, _controllingTty);
    if (bytesRead != (int)IOUtils::StreamedLength(_controllingTty))
      goto readError;
    rc += bytesRead;

    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _sessionId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _terminalSessionId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _effectiveUserId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _realUserId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _savedEffectiveUserId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _realGroupId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, val32);
    if (bytesRead != (int)IOUtils::StreamedLength(val32))
      goto readError;
    _savedEffectiveGroupId = val32;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, _command);
    if (bytesRead != (int)IOUtils::StreamedLength(_command))
      goto readError;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, _startTime);
    if (bytesRead != (int)IOUtils::StreamedLength(_startTime))
      goto readError;
    rc += bytesRead;

    bytesRead = GZIO::Read(gzf, _size);
    if (bytesRead != (int)IOUtils::StreamedLength(_size))
      goto readError;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, _residentSetSize);
    if (bytesRead != (int)IOUtils::StreamedLength(_residentSetSize))
      goto readError;
    rc += bytesRead;
    
    bytesRead = GZIO::Read(gzf, _rusage);
    if (bytesRead != (int)IOUtils::StreamedLength(_rusage))
      goto readError;
    rc += bytesRead;

    bytesRead = GZIO::Read(gzf, _args);
    if (bytesRead != (int)IOUtils::StreamedLength(_args))
      goto readError;
    rc += bytesRead;

    return(rc);

  readError:
    cerr << "Read(gzf) failed for ProcessInfo" << endl;
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int ProcessInfo::Write(gzFile gzf) const
  {
    if (! gzf)
      return(-1);
    
    int  rc = 0;
    int32_t  val32 = _id;
    int  bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
        goto writeError;
    rc += bytesWritten;
    
    val32 = _parentId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _groupId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;
    
    bytesWritten = GZIO::Write(gzf, _controllingTty);
    if (bytesWritten != (int)IOUtils::StreamedLength(_controllingTty))
      goto writeError;
    rc += bytesWritten;
    
    val32 = _sessionId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _terminalSessionId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _effectiveUserId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _realUserId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _savedEffectiveUserId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _realGroupId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    val32 = _savedEffectiveGroupId;
    bytesWritten = GZIO::Write(gzf, val32);
    if (bytesWritten != (int)IOUtils::StreamedLength(val32))
      goto writeError;
    rc += bytesWritten;

    bytesWritten = GZIO::Write(gzf, _command);
    if (bytesWritten != (int)IOUtils::StreamedLength(_command))
      goto writeError;
    rc += bytesWritten;
    
    bytesWritten = GZIO::Write(gzf, _startTime);
    if (bytesWritten != (int)IOUtils::StreamedLength(_startTime))
      goto writeError;
    rc += bytesWritten;
    
    bytesWritten = GZIO::Write(gzf, _size);
    if (bytesWritten != (int)IOUtils::StreamedLength(_size))
      goto writeError;
    rc += bytesWritten;
    
    bytesWritten = GZIO::Write(gzf, _residentSetSize);
    if (bytesWritten != (int)IOUtils::StreamedLength(_residentSetSize))
      goto writeError;
    rc += bytesWritten;

    bytesWritten = GZIO::Write(gzf, _rusage);
    if (bytesWritten != (int)IOUtils::StreamedLength(_rusage))
      goto writeError;
    rc += bytesWritten;
    
    bytesWritten = GZIO::Write(gzf, _args);
    if (bytesWritten != (int)IOUtils::StreamedLength(_args))
      goto writeError;
    rc += bytesWritten;
    
    return(rc);
      
    writeError:
      return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t ProcessInfo::StreamedLength() const
  {
    uint64_t  rc = 8 * sizeof(uint32_t);
    rc += IOUtils::StreamedLength(_command);
    rc += IOUtils::StreamedLength(_startTime);
    rc += IOUtils::StreamedLength(_size);
    rc += IOUtils::StreamedLength(_residentSetSize);
    rc += IOUtils::StreamedLength(_rusage);
    
    return(rc);
  }
  

}  // namespace Dwm
