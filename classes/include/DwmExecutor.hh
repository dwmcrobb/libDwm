//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmExecutor.hh 8401 $
// @(#) $Id: DwmExecutor.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2010, 2016
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
//!  \file DwmExecutor.hh
//!  \brief Dwm::Executor class definition.
//---------------------------------------------------------------------------

#include <mutex>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Provides a simple interface to fork() + execvp().  I wanted a call that
  //!  looked like system() in usage, but didn't block for the exit of the
  //!  child and returned a PID.  This call is Execute(const std::string &).
  //--------------------------------------------------------------------------
  class Executor
  {
  public:
    //------------------------------------------------------------------------
    //!  Constant for the lexer.
    //------------------------------------------------------------------------
    static const int  k_COMMAND    = 260;
    //------------------------------------------------------------------------
    //!  Constant for the lexer.
    //------------------------------------------------------------------------
    static const int  k_ARGUMENT   = 261;
    //------------------------------------------------------------------------
    //!  Constant for the lexer.
    //------------------------------------------------------------------------
    static const int  k_WHITESPACE = 262;
    
    //------------------------------------------------------------------------
    //!  Executes the given @c commandString, which may contain arguments.
    //!  Returns the process ID.
    //------------------------------------------------------------------------
    static pid_t Execute(const std::string & commandString);

    //------------------------------------------------------------------------
    //!  Executes the given @c command with arguments @c argv.  Returns the
    //!  process ID.
    //------------------------------------------------------------------------
    static pid_t Execute(const std::string & command, char *const argv[]);
    
    //------------------------------------------------------------------------
    //!  Executes the command with arguments that are combined in @c argv.
    //!  Returns the process ID.
    //------------------------------------------------------------------------
    static pid_t Execute(char *const argv[]);
    
  private:
    static bool FillArgs(const std::string & commandStr, char * & command,
                         char ** & args);
    static pid_t ExecVP(const std::string & command, char *const argv[]);

    static std::mutex  _mutex;
  };
  
}  // namespace Dwm

#define  COMMAND     260
#define  ARGUMENT    261
#define  WHITESPACE  262

