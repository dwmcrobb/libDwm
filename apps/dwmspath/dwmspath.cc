//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016, 2021
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
//!  \file dwmspath.cc
//!  \brief Simple application to sanitize path(s) given on command line by
//!  removing redundancy ('/./' and '//') and replacing '../'.  Will yield
//!  an absolute path.  This is much like realpath.  One significant
//!  difference: I don't require the path to exist.  A long time ago, this
//!  program had my own string manipulation code; we didn't have
//!  std::filesystem.  Now it just uses std::filesystem.
//---------------------------------------------------------------------------

#include <filesystem>
#include <iostream>

#include "DwmOptArgs.hh"

using namespace std;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  namespace fs = std::filesystem;
  
  Dwm::OptArgs  optargs;
  optargs.AddOptArg("r", "relative", false, "0",
                    "Show relative path instead of absolute path");
  optargs.AddNormalArg("path(s)", true);
  int  nextArg = optargs.Parse(argc, argv);

  if (nextArg == argc) {
    optargs.Usage(argv[0]);
    return 1;
  }

  for (int arg = nextArg; arg < argc; ++arg) {
    if (optargs.Get<bool>('r')) {
      cout << fs::relative(argv[arg]).string() << '\n';
    }
    else {
      cout << fs::weakly_canonical(argv[arg]).string() << '\n';
    }
  }
  
  return 0;
}
