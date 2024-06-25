//===========================================================================
// @(#) $Name:$
// @(#) $Id:$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file TestFSyslog.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <unistd.h>
  #include <fcntl.h>
}

#include <cstdio>
#include <fstream>
#include <regex>

#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool RedirectStderrToFile(const std::string & filePath,
                                 int & fileDescriptor,
                                 int & restoreDescriptor)
{
  bool  rc = false;
  fileDescriptor = open(filePath.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (UnitAssert(fileDescriptor >= 0)) {
    restoreDescriptor = dup(STDERR_FILENO);
    dup2(fileDescriptor, STDERR_FILENO);
    rc = true;
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void RestoreStderr(int fileDescriptor, int restoreDescriptor)
{
  fflush(stderr);
  dup2(restoreDescriptor, STDERR_FILENO);
  close(fileDescriptor);
  return;
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool TestTrivialLogMsg()
{
  bool  rc = false;
  string  outFileName = "/tmp/TestFSyslog_stderr." + to_string(getpid());
  int  fd, restorefd;
  if (UnitAssert(RedirectStderrToFile(outFileName, fd, restorefd))) {
    FSyslog(LOG_INFO, "Hello user id {}", getuid());
    RestoreStderr(fd, restorefd);
    ifstream  is(outFileName);
    if (UnitAssert(is)) {
      string  line;
      if (UnitAssert(std::getline(is, line))) {
        regex  rgx(".*Hello user id [0-9]+.*",
                   regex::ECMAScript|regex::optimize);
        smatch  sm;
        rc = UnitAssert(regex_match(line, sm, rgx));
      }
      is.close();
    }
    std::remove(outFileName.c_str());
  }
  return rc;
}
      
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("TestFSyslog", LOG_PERROR, LOG_USER);

  UnitAssert(TestTrivialLogMsg());
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
  }
  
  exit(0);

}

