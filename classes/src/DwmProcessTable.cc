//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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

  #ifdef HAVE_KVM_H
    #include <kvm.h>
  #endif
}

#include "DwmProcessTable.hh"

namespace Dwm {

#if (defined __FreeBSD__) || (defined __APPLE__)

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & processTable)
  {
    bool  rc = false;
    
    if (! processTable.empty())
      processTable.clear();

#if (defined __APPLE__)
    kvm_t  *kvm = kvm_open(0, 0, 0, O_RDONLY, 0);
#else
    kvm_t  *kvm = kvm_open(0, "/dev/null", 0, O_RDONLY, 0);
#endif
    if (kvm) {
      int  numProcs = 0;
      struct kinfo_proc  *procs;
      procs = kvm_getprocs(kvm, KERN_PROC_ALL, 0, &numProcs);
      if (procs && numProcs) {
        for (int i = 0; i < numProcs; ++i) {
#if defined(__APPLE__)
          ProcessInfo  proc(procs[i]);
          char **args = kvm_getargv(kvm, &procs[i], 0);
          if (args) {
            for (int j = 0; args[j]; ++j) {
              proc.AddArg(args[j]);
            }
          }
          processTable[procs[i].kp_proc.p_pid] = proc;
#else
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
#endif
        }
        rc = true;
      }
      kvm_close(kvm);
    }
    return(rc);
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

#define PROC_DESIRED_FLAGS (PROC_FILLMEM|PROC_FILLCOM|PROC_FILLENV|     \
                            PROC_FILLSTATUS|PROC_FILLSTAT|PROC_FILLARG)
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool GetProcessTable(ProcessTable & processTable)
  {
    bool  rc = false;

    proc_t  **procs = readproctab(PROC_DESIRED_FLAGS);
    for (int i = 0; procs[i] != 0; ++i) {
      ProcessInfo  proc(procs[i]);
      if (procs[i]->cmdline) {
        for (int arg = 0; procs[i]->cmdline[arg]; ++arg) {
          proc.AddArg(procs[i]->cmdline[arg]);
        }
      }
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
