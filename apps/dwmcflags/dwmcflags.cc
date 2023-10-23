//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2023
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
//!  \file dwmflags.cc
//!  \author Daniel W. McRobb
//!  \brief echo command line arguments with duplicate include paths,
//!         library paths and libraries removed
//---------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  unordered_set<string>  libPaths, incPaths, libs;
  vector<string>         result;

  for (int i = 1; i < argc; ++i) {
    string  arg(argv[i]);
    if ("-L" == arg.substr(0,2)) {
      auto  it = libPaths.find(arg);
      if (it != libPaths.end()) {
        continue;
      }
      libPaths.insert(arg);
      result.push_back(arg);
    }
    else if ("-I" == arg.substr(0,2)) {
      auto  it = incPaths.find(arg);
      if (it != incPaths.end()) {
        continue;
      }
      incPaths.insert(arg);
      result.push_back(arg);
    }
    else if ("-l" == arg.substr(0,2)) {
      auto  it = libs.find(arg);
      if (it != libs.end()) {
        continue;
      }
      libs.insert(arg);
      result.push_back(arg);
    }
    else {
      result.push_back(arg);
    }
  }

  if (! result.empty()) {
    auto  it = result.begin();
    cout << *it;
    ++it;
    for ( ; it != result.end(); ++it) {
      cout << ' ' << *it;
    }
    cout << '\n';
  }
  
  return 0;
}
