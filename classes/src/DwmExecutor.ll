%{
  //===========================================================================
  // @(#) $Name$
  // @(#) $Id: DwmExecutor.ll 8477 2016-05-07 02:59:10Z dwm $
  //===========================================================================
  //  Copyright (c) Daniel W. McRobb 2010
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
  //  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
  //  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
  //  TRADEMARK OR OTHER RIGHTS.
  //===========================================================================

  //---------------------------------------------------------------------------
  //!  \file DwmExecutor.ll
  //!  \brief Executor class and command line lexer implementation
  //---------------------------------------------------------------------------
  
  extern "C" {
    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
  }
  #include <cstring>
  #include <iostream>
  #include <string>

  #include "DwmSignal.hh"
  #include "DwmExecutor.hh"
  #include "DwmSvnTag.hh"
  
  std::string  g_cmdArg;
%}

%option noyywrap
%option prefix="dwmexecutor"

%x x_singleQuotedArg
%x x_doubleQuotedArg

%%

<INITIAL>^[^"'\n\t ]+       { g_cmdArg = yytext; 
                              return(Dwm::Executor::k_COMMAND); }
<INITIAL>[']                { BEGIN(x_singleQuotedArg); }
<x_singleQuotedArg>[^']+    { g_cmdArg = yytext; }
<x_singleQuotedArg>[']      { BEGIN(INITIAL); 
                              return Dwm::Executor::k_ARGUMENT; }
<INITIAL>["]                { BEGIN(x_doubleQuotedArg); }
<x_doubleQuotedArg>[^"]+    { g_cmdArg = yytext; }
<x_doubleQuotedArg>["]      { BEGIN(INITIAL); 
                              return Dwm::Executor::k_ARGUMENT; }
<INITIAL>[\n\t ]+           { return Dwm::Executor::k_WHITESPACE; }
<INITIAL>[^"'\n\t ]+        { g_cmdArg = yytext;
                              return Dwm::Executor::k_ARGUMENT; }
<<EOF>>                     { BEGIN(INITIAL); return 0; }
%%

static Dwm::SvnTag  svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmExecutor.ll 8477 $");

int  g_numProcs = 0;

#if 0
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void SigChldHandler(int sig)
{
  int    exitStatus;
  pid_t  pid = wait(&exitStatus);
  std::cout << "pid " << pid << " exited.\n";
  --g_numProcs;
  return;
}
#endif

namespace Dwm {

  std::mutex  Executor::_mutex;

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Executor::FillArgs(const std::string & commandStr,
                          char * & command, char ** & args)
  {
    int  numArgs = 0;
    int  rc;

    std::lock_guard<std::mutex>  lk(_mutex);

    args = (char **)malloc(sizeof(char *));
    args[0] = 0;

    YY_BUFFER_STATE  bufferState = yy_scan_string(commandStr.c_str());

    while ((rc = dwmexecutorlex()) != 0) {
      switch (rc) {
        case k_COMMAND:
          command = strdup(g_cmdArg.c_str());
          ++numArgs;
          args = (char **)realloc(args, (numArgs+1)*sizeof(char *));
          args[numArgs-1] = strdup(g_cmdArg.c_str());
          args[numArgs] = 0;
          break;
        case k_ARGUMENT:
          ++numArgs;
          args = (char **)realloc(args, (numArgs+1)*sizeof(char *));
          args[numArgs-1] = strdup(g_cmdArg.c_str());
          args[numArgs] = 0;
          break;
        default:
          break;
      }
    }
    yy_delete_buffer(bufferState);
    return (command && (strlen(command)));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t Executor::Execute(const std::string & commandStr)
  {
    pid_t   pid = 0;
    char   *command = 0;
    char  **args = 0;

    if (FillArgs(commandStr, command, args)) {
      pid = ExecVP(command, args);
      int  argNum = 0;
      for (; args[argNum]; ++argNum) {
        free(args[argNum]);
        args[argNum] = 0;
      }
      free(args);
      free(command);
    }
    return pid;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t Executor::Execute(const std::string & command, 
                          char *const argv[])
  {
    return ExecVP(command, argv);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t Executor::Execute(char *const argv[])
  {
    pid_t  pid = 0;
    if (argv[0]) {
      pid = ExecVP(argv[0], argv);
    }
    return pid;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  pid_t Executor::ExecVP(const std::string & command,
                         char * const argv[])
  {
    pid_t  pid = 0;
    if (! command.empty()) {
      pid = fork();
      if (! pid) {
        //   disassociate from process group
        setpgid(0, getpid());
#ifdef SIGTTOU
        //  ignore terminal I/O signals
        signal(SIGTTOU, SIG_IGN);
#endif
        //  disassociate from control terminal
        int  fd = open("/dev/tty", O_RDWR);
        if (fd >= 0) {
          ioctl(fd,TIOCNOTTY, (char *)0);
          close(fd);
        }
        execvp(command.c_str(), argv);
      }
    }
    return pid;
  }

}  // namespace Dwm

#if 0       
int main(int argc, char *argv[])
{
  Dwm::Signal  SigChld(SIGCHLD);
  SigChld.PushHandler(SigChldHandler);

  if (argc >= 2) {
    pid_t  pid = Dwm::Executor::Execute(argv[1]);
    std::cout << "pid: " << pid << "\n";

    ++g_numProcs;

    if (argc >= 3) {
      pid = Dwm::Executor::Execute(argv[2], &argv[2]);
      std::cout << "pid: " << pid << "\n";

      pid = Dwm::Executor::Execute(&argv[2]);
      std::cout << "pid: " << pid << "\n";
 
      g_numProcs += 2;
    }

    for (;;) {
      if (! g_numProcs) {
        break;
      }
    }
  }
  else {
    return 1;
  }

}
#endif
