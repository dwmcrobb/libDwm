//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestSvnTag.cc 9116 $
// @(#) $Id: TestSvnTag.cc 9116 2017-04-14 23:17:04Z dwm $
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
//!  \file TestSvnTag.cc
//!  \brief Unit tests for Dwm::SvnTag
//---------------------------------------------------------------------------
#include <regex>

#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag  svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestSvnTag.cc 9116 $");

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
bool FindMatch(const Dwm::SvnTag & tag)
{
  bool         rc = false;
  std::regex   tagrgx("libDwm\\-[0-9]+\\.[0-9]+\\.[0-9]+");
  std::regex   trunkrgx("trunk \\([0-9]+\\)");
  std::regex   branchrgx("[^ ]+ \\(branch\\)");
  std::smatch  tagMatch, trunkMatch, branchMatch;
  std::string  &&tagName = tag.TagName();
  regex_search(tagName, tagMatch, tagrgx);
  regex_search(tagName, trunkMatch, trunkrgx);
  regex_search(tagName, branchMatch, branchrgx);
  if ((tagMatch.size() > 0)
      || (trunkMatch.size() > 0)
      || (branchMatch.size() > 0)) {
    rc = true;
  }
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  UnitAssert(FindMatch(svntag));

  const Dwm::SvnTag  tag1("@(#) $Header: svn+ssh://www/zfs1/svn/dwm/libDwm/trunk/tests/TestSvnTag.cc 9116 2017-04-14 23:17:04Z dwm $");
  UnitAssert(FindMatch(tag1));
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    return 1;
  }
  else {
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;
    return 0;
  }
}
