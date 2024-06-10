//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2024
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
//!  \file DwmProcessTable.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::ProcessTable class implementation
//---------------------------------------------------------------------------

#include "DwmPortability.hh"

extern "C" {
  #include <fcntl.h>
  #include <unistd.h>
#ifdef HAVE_KVM_H
    #include <kvm.h>
#endif
#ifdef __APPLE__
  #include <sys/sysctl.h>
#endif
}

#include "DwmProcessTable.hh"
#include "DwmSysLogger.hh"

namespace Dwm {

  using namespace std;

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void DebugPrintProc(ostream & os, const ProcessInfo & proc)
  {
    os << proc.Id() << ' ' << proc.Command();
    for (const auto & arg : proc.Args()) {
      os << ' ' << arg;
    }
    os << '\n';
  }
  
#if (defined __FreeBSD__)

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & processTable)
  {
    bool  rc = false;
    
    if (! processTable.empty())
      processTable.clear();

    kvm_t  *kvm = kvm_open(0, "/dev/null", 0, O_RDONLY, 0);
    if (kvm) {
      int  numProcs = 0;
      struct kinfo_proc  *procs;
      procs = kvm_getprocs(kvm, KERN_PROC_ALL, 0, &numProcs);
      if (procs && numProcs) {
        for (int i = 0; i < numProcs; ++i) {
          if (procs[i].ki_textvp) {
            ProcessInfo  proc(procs[i]);
            char **args = kvm_getargv(kvm, &procs[i], 0);
            if (args) {
              for (int j = 0; args[j]; ++j) {
                proc.AddArg(args[j]);
              }
            }
            processTable[procs[i].ki_pid] = proc;
          }
        }
        rc = true;
      }
      kvm_close(kvm);
    }
    return(rc);
  }
  
#elif (defined(__APPLE__))

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void GetProcArgs(pid_t pid, vector<string> & args)
  {
    args.clear();
    size_t  argsSize = 0;
    size_t  argSizeEstimate;
    int     tries = 3;
    string  s;
    do {
      int  mib[4] = { CTL_KERN, KERN_PROCARGS2, pid };
      if (sysctl(mib, 3, nullptr, &argSizeEstimate, nullptr, 0) != 0) {
        return;
      }
      argsSize = argSizeEstimate + 1;
      s.resize(argsSize);
      if (sysctl(mib, 3, s.data(), &argsSize, nullptr, 0) != 0) {
        return;
      }
    } while ((argsSize == argSizeEstimate) && tries--);

    if ((argsSize >= (argSizeEstimate + 1))
        || (argsSize < sizeof(int))) {
      Syslog(LOG_ERR, "bad argsSize %zu", argsSize);
      return;
    }

    int  numArgs = *(int *)(s.data());
    size_t  startPos = sizeof(numArgs);
    size_t  nextNul = s.find('\0', startPos);
    if (nextNul == string::npos) {
      return;
    }
    // skip over first argument since it's duplicated in the second
    // argument
    startPos = s.find_first_not_of('\0', nextNul + 1);
    
    while (numArgs-- && (startPos != string::npos)) {
      nextNul = s.find('\0', startPos);
      args.push_back(s.substr(startPos, nextNul - startPos));
      startPos = s.find_first_not_of('\0', nextNul + 1);
    }
    return;
  }
       
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & processTable)
  {
    bool  rc = false;
    pid_t  origeuid = geteuid();
    processTable.clear();
    int  numPids = proc_listpids(PROC_ALL_PIDS, 0, nullptr, 0);
    if (numPids > 0) {
      vector<int>  pids(numPids);
      numPids = proc_listallpids(pids.data(), pids.size() * sizeof(int));
      if (numPids > 0 && (numPids <= pids.size())) {
        seteuid(0);
        for (int i = 0; i < numPids; ++i) {
          struct proc_taskallinfo  allInfo;
          if (proc_pidinfo(pids[i], PROC_PIDTASKALLINFO, 0, &allInfo,
                           PROC_PIDTASKALLINFO_SIZE)
              == PROC_PIDTASKALLINFO_SIZE) {
            ProcessInfo  proc(allInfo);
            vector<string>  args;
            GetProcArgs(proc.Id(), args);
            if (! args.empty()) { proc.Command(args.front()); }
            for (auto & arg : args) { proc.AddArg(arg.c_str()); }
            processTable[proc.Id()] = proc;
          }
          else {
            Syslog(LOG_ERR, "proc_pidinfo(%d,PROC_PIDTASKALLINFO) failed",
                   pids[i]);
          }
        }
        seteuid(origeuid);
      }
    }
    return (! processTable.empty());
  }
  
#elif (defined __OpenBSD__)

	//------------------------------------------------------------------------
	//!  
	//------------------------------------------------------------------------
	bool GetProcessTable(ProcessTable & processTable)
  {
    bool  rc = false;
    
    if (! processTable.empty())
      processTable.clear();

    kvm_t  *kvm = kvm_open(0, 0, 0, O_RDONLY, 0);
    if (kvm) {
      int  numProcs = 0;
      struct kinfo_proc2  *procs;
      procs = kvm_getproc2(kvm, KERN_PROC_ALL, 0, sizeof(kinfo_proc2), &numProcs);
      if (procs && numProcs) {
        for (int i = 0; i < numProcs; ++i) {
          ProcessInfo  proc(procs[i]);
          char **args = kvm_getargv2(kvm, &procs[i], 0);
          if (args) {
            for (int j = 0; args[j]; ++j) {
              proc.AddArg(args[j]);
            }
          }
          processTable[procs[i].p_pid] = proc;
        }
        rc = true;
      }
      kvm_close(kvm);
    }
    return(rc);
  }

#elif (defined(__linux__))

#define PROC_DESIRED_FLAGS                                                     \
  (PROC_FILLMEM | PROC_FILLCOM | PROC_FILLARG | PROC_FILLENV |                 \
   PROC_FILLSTATUS | PROC_FILLSTAT)

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & processTable)
  {
    bool  rc = false;

    proc_t  **procs = readproctab(PROC_DESIRED_FLAGS);
    for (int i = 0; procs[i] != nullptr; ++i) {
      ProcessInfo  proc(procs[i]);
      processTable[proc.Id()] = proc;
      rc = true;
    }
    return rc;
  }

#else

	//--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & processTable)
  {
    return(false);
  }

#endif


}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
