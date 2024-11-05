//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmFileLogger.cc 10863 $
// @(#) $Id: TestDwmFileLogger.cc 10863 2020-07-27 08:54:08Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018
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
//!  \file TestDwmFileLogger.cc
//!  \brief Dwm::FileLogger unit tests
//---------------------------------------------------------------------------

#include <filesystem>
#include <fstream>
#include <regex>
#include <set>
#include <utility>
#include <vector>

extern "C" {
  #include <dirent.h>
  #include <libgen.h>
}

#include "DwmFileLogger.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
#define LogToFile(l, ...)                                     \
  (l).Log(__FILE__,__LINE__,__PRETTY_FUNCTION__,__VA_ARGS__)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static vector<int>  g_priorities = {
  LOG_EMERG,
  LOG_ALERT,
  LOG_CRIT,
  LOG_ERR,
  LOG_WARNING,
  LOG_NOTICE,
  LOG_INFO,
  LOG_DEBUG
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static bool GetLogFiles(FileLogger & logger, set<string> & logFiles)
{
  logFiles.clear();
  std::filesystem::path  filePrefix(logger.FilePrefix());
  std::string            dirName(filePrefix.parent_path().string());
  DIR *cd = opendir(dirName.c_str());
  if (cd) {
    std::regex   logrgx(filePrefix.filename().string() + ".*",
                        regex::ECMAScript|regex::optimize);
    struct dirent  *de;
    while ((de = readdir(cd))) {
      string  fn(de->d_name);
      smatch  sm;
      regex_search(fn, sm, logrgx);
      if (sm.size() > 0) {
        logFiles.insert(fn);
      }
    }
    closedir(cd);
  }
  return (! logFiles.empty());
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void RemoveLogFiles(FileLogger & logger)
{
  string  filePrefix(logger.FilePrefix());
  const char  *dirName = dirname((char *)filePrefix.c_str());
  set<string> logFiles;
  if (GetLogFiles(logger, logFiles)) {
    for (auto logFile : logFiles) {
      string  path(string(dirName) + '/' + logFile);
      UnitAssert(0 == std::remove(path.c_str()));
    }
  }
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool TestPriorities(FileLogger & logger)
{
  bool  rc = true;
  for (auto priority : g_priorities) {
    rc &= UnitAssert(logger.MinimumPriority(priority) == priority);
    rc &= UnitAssert(logger.MinimumPriority() == priority);
  }
  rc &= UnitAssert(true == logger.ShowPriorities(true));
  rc &= UnitAssert(true == logger.ShowPriorities());
  rc &= UnitAssert(false == logger.ShowPriorities(false));
  rc &= UnitAssert(false == logger.ShowPriorities());

  rc &= UnitAssert(true == logger.ShowPriorities(true));

  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool TestRollovers(FileLogger & logger)
{
  bool rc = true;

  rc &= UnitAssert(20 == logger.RolloverSize(20));
  rc &= UnitAssert(5 == logger.NumRolloverFiles(5));
  rc &= UnitAssert(5 == logger.NumRolloverFiles());
  rc &= UnitAssert(LogToFile(logger, LOG_DEBUG,
                             "Hello more than twenty characters!!!"));
  rc &= UnitAssert(LogToFile(logger, LOG_DEBUG,
                             "Hello more than twenty characters!!!"));
  rc &= UnitAssert(LogToFile(logger, LOG_DEBUG,
                             "Hello more than twenty characters!!!"));
  set<string>  logFiles;
  rc &= UnitAssert(GetLogFiles(logger, logFiles));
  rc &= UnitAssert(logFiles.size() == 4);
  
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool TestContents(FileLogger & logger)
{
  bool  rc = true;
  
  map<string,size_t>  messages = {
    { "Zeroth message 0 zero",   0 },
    { "First message 1 one",     0 },
    { "Second message 2 two",    0 },
    { "Third message 3 three",   0 },
    { "Fourth message 4 four",   0 },
    { "Fifth message 5 five",    0 },
    { "Sixth message 6 six",     0 },
    { "Seventh message 7 seven", 0 },
    { "Eigth message 8 eight",   0 },
    { "Ninth message 9 nine",    0 }
  };

  rc &= UnitAssert(logger.RolloverSize(100000) == 100000);
  rc &= UnitAssert(logger.ShowPriorities(true));

  for (auto priority : g_priorities) {
    for (auto message : messages) {
      rc &= UnitAssert(logger.Log(priority, message.first.c_str()));
    }
  }

  rc &= UnitAssert(logger.Close());

  ifstream  is(logger.FilePrefix().c_str());
  rc &= UnitAssert(is);
  if (is) {
    vector<pair<regex,string>>  rgxs;
    for (auto m : messages) {
      regex  rgx(string(".*\\[[MACEWNID]\\] ") + m.first + string(".*"),
                 regex::ECMAScript|regex::optimize);
      rgxs.push_back(pair<regex,string>(rgx, m.first));
    }
    string  logLine;
    while (getline(is, logLine)) {
      smatch  sm;
      for (auto rgx : rgxs) {
        regex_search(logLine, sm, rgx.first);
        if (sm.size() > 0) {
          auto  mi = messages.find(rgx.second);
          if (UnitAssert(mi != messages.end())) {
            ++mi->second;
          }
          else {
            rc = false;
          }
          break;
        }
      }
    }
    is.close();
  }
  for (auto message : messages) {
    rc &= UnitAssert(message.second == g_priorities.size());
  }
                
  
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  FileLogger   logger;
  if (UnitAssert(logger.Open("TestDwmFileLogger", 0,
                             "./TestDwmFileLogger.log"))) {
    UnitAssert(TestPriorities(logger));
    UnitAssert(TestRollovers(logger));
    UnitAssert(logger.Close());
  }
  
  RemoveLogFiles(logger);

  if (UnitAssert(logger.Open("TestDwmFileLogger", 0,
                             "./TestDwmFileLogger.log"))) {
    TestContents(logger);
  }

  RemoveLogFiles(logger);
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    return 1;
  }

  cout << Assertions::Total() << " passed" << endl;
  return 0;
}
