//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2007
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
//!  \file DwmDaemonUtils.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::DaemonUtils class implementation
//---------------------------------------------------------------------------

extern "C" {
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <unistd.h>
}

#include "DwmSvnTag.hh"
#include "DwmDaemonUtils.hh"
#include "DwmSysLogger.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmDaemonUtils.cc 11091 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void DaemonUtils::Daemonize()
  {
    pid_t  childPid = fork();
    
    if (childPid > 0) {
      //  parent process; exit
      exit(0);
    }
    
    if (childPid < 0) {
      Syslog(LOG_ERR, "fork() failed: %m");
      Syslog(LOG_ERR, "Exiting!");
      exit(1);
    }
    
    //  disassociate from process group
    setpgid(0, getpid());
    
#ifdef SIGTTOU
    //  ignore terminal I/O signals
    signal(SIGTTOU, SIG_IGN);
    #endif

    //  disassociate from control terminal
    int  fd = open("/dev/tty", O_RDWR);
    if (fd >= 0) {
      ioctl(fd, TIOCNOTTY, (char *)0);
      close(fd);
    }
    
    //  close stdin and stdout
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    
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
