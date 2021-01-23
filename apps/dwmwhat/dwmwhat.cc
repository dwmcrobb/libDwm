//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/apps/dwmwhat/dwmwhat.cc 8496 $
// @(#) $Id: dwmwhat.cc 8496 2016-05-08 04:55:39Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016
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
//!  \file svnwhat.cc
//!  \brief Specialized version of 'what' command
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/mman.h>  // for mmap()
  #include <sys/stat.h>  // for stat()
  #include <fcntl.h>     // for open()
  #include <libgen.h>    // for basename()
  #include <unistd.h>
}

#include <fstream>
#include <iostream>
#include <regex>
#include <vector>
#include <utility>  // for std::pair

#include "DwmOptArgs.hh"
#include "DwmPackageInfo.hh"
#include "DwmSvnTag.hh"

using namespace std;

static bool g_shortOption = false;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static string RootRelative(const string & filename)
{
  static regex  rgx(".+\\/(branches\\/[^/]+|tags\\/[^/]+|trunk)\\/(.+)");
  string        rc;
  smatch        m;
  if (regex_search(filename, m, rgx)) {
    if (m.size() == 3) {
      rc = m[2];
    }
  }
  return rc;
}
  
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void LineMatch(const string & line, const string & tagName)
{
  static regex   lookrgx("^\\@\\(#\\).+", regex::ECMAScript|regex::optimize);
  regex   hdrrgx("(\\$Header|\\$" + tagName + ")\\:[ ]+[^\\$]+\\$",
                 regex::ECMAScript|regex::optimize);
  smatch  lookMatch, hdrMatch;
  if (regex_match(line, lookMatch, lookrgx)) {
    regex_search(line, hdrMatch, hdrrgx);
    if (hdrMatch.size() > 0) {
      Dwm::SvnTag svntag(hdrMatch[0]);
      if (! g_shortOption) {
        cout << svntag.TagName() << ' ' << svntag.FileName() << '\n';
      }
      else {
        string  shortName = RootRelative(svntag.FileName());
        if (! shortName.empty()) {
          cout << svntag.TagName() << ' '
               << RootRelative(svntag.FileName()) << '\n';
        }
        else {
          cout << svntag.TagName() << ' '
               << basename((char *)(svntag.FileName().c_str())) << '\n';
        }
      }
    }
    else {
      cout << lookMatch[0] << '\n';
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static pair<char *,size_t> MapFile(const string & filename)
{
  pair<char *,size_t> rc(nullptr,0);
  int  fd = open(filename.c_str(), O_RDONLY);
  if (fd >= 0) {
    struct stat  statbuf;
    if (stat(filename.c_str(), &statbuf) == 0) {
      rc.second = statbuf.st_size;
      rc.first = (char *)mmap(0, statbuf.st_size, PROT_READ, MAP_FILE|MAP_SHARED, fd, 0);
      if (rc.first == MAP_FAILED) {
        rc.first = 0;
        rc.second = 0;
      }
    }
    close(fd);
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static vector<string> FindSccsStrings(const char * map, size_t size)
{
  vector<string>     rc;
  size_t             i = 0;
  while (i < (size - 5)) {
    if ((map[i] == '@') && (map[i+1] == '(') && (map[i+2] == '#')
        && (map[i+3] == ')')) {
      string::size_type  startidx = i;
      i += 4;
      while ((map[i] != '\0') && (map[i] != '\n') && (i < size)) {
        ++i;
      }
      if (i < size) {
        rc.push_back(string(&map[startidx], &map[i]));
      }
    }
    else {
      ++i;
    }
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::OptArgs  optArgs;
  optArgs.AddOptArg("c:", "customtag", false, "DwmPath", "custom tagname");
  optArgs.AddOptArg("v", "version", false, "false", "show version");
  optArgs.AddOptArg("s", "short", false, "false", "remove directories from paths");
  optArgs.AddNormalArg("file(s)", true);
  int  nextArg = optArgs.Parse(argc, argv);

  if (optArgs.Get<bool>('v')) {
    cout << Dwm::PackageInfo::Name() << '-'
         << Dwm::PackageInfo::Version() << '\n';
    return 0;
  }

  if (nextArg == argc) {
    optArgs.Usage(argv[0]);
    return 1;
  }

  g_shortOption = optArgs.Get<bool>('s');
  
  int  rc = 0;
  for (int arg = nextArg; arg < argc; ++arg) {
    pair<char *,size_t>  mf = MapFile(argv[arg]);
    if (mf.first) {
      vector<string>  sccsStrings = FindSccsStrings(mf.first, mf.second);
      for (auto sit : sccsStrings) {
        LineMatch(sit, optArgs.Get<string>('c'));
      }
      munmap(mf.first, mf.second);
    }
  }
  return rc;
}
