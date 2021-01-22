//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmSvnTag.cc 11138 $
// @(#) $Id: DwmSvnTag.cc 11138 2020-09-07 23:47:43Z dwm $
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
//!  \file DwmSvnTag.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::SvnTag class implementation
//---------------------------------------------------------------------------

#include <iostream>
#include <regex>

#include "DwmSvnTag.hh"

static const Dwm::SvnTag  svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmSvnTag.cc 11138 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  SvnTag::SvnTag(const string & keyword)
      : _keyword(keyword)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string SvnTag::TagName(const string & tagRegex, const string & branchRegex,
                         const string & trunkRegex) const
  {
    string  rc;
    smatch  match;
    regex   rgx(tagRegex, regex::ECMAScript|regex::optimize);
    regex_search(_keyword, match, rgx);
    if (match.size() > 1) {
      rc = match[1];
    }
    else {
      regex  brgx(branchRegex, regex::ECMAScript|regex::optimize);
      regex_search(_keyword, match, brgx);
      if (match.size() > 1) {
        rc = match[1];
        rc += "(branch)";
      }
      else {
        regex  trgx(trunkRegex, regex::ECMAScript|regex::optimize);
        regex_search(_keyword, match, trgx);
        if (match.size() > 1) {
          rc = match[1];
          regex  revrgx("(\\$DwmPath\\:[ ]+[^ ]+[ ]+"
                        "|\\$Id\\:[ ]+[^ ]+[ ]+"
                        "|\\$Header\\:[ ]+[^ ]+[ ]+"
                        "|\\$Revision\\:[ ])"
                        "([0-9]+)", regex::ECMAScript|regex::optimize);
          regex_search(_keyword, match, revrgx);
          if (match.size() > 2) {
            rc += " (";
            rc += match[2];
            rc += ")";
          }
        }
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool SvnTag::IsRemoteRepo(const string & url) const
  {
    bool    rc = false;
    regex   rgx("^(svn|http)", regex::ECMAScript|regex::optimize);
    smatch  match;
    if (regex_search(url, match, rgx)) {
      rc = true;
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void SvnTag::RemoveRepoStart(string & path) const
  {
    string  rgxstr("(svn\\+ssh|file|http|https|svn)\\:\\/\\/[^\\/]+\\/");
    if (IsRemoteRepo(path)) {
      rgxstr += "[^\\/]+\\/";
    }
    regex  rgx(rgxstr, regex::ECMAScript|regex::optimize);
    path = regex_replace(path, rgx, "");
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  string SvnTag::FileName() const
  {
    string  rc;
    smatch  match;
    regex   dwmrgx("\\$DwmPath\\:[ ]+([^ ]+)",
                   regex::ECMAScript|regex::optimize);
    if (regex_search(_keyword, match, dwmrgx)) {
      if (match.size() > 1) {
        rc = match[1];
      }
    }
    else {
      regex   rgx("\\$Header\\:[ ]+([^ ]+)",
                  regex::ECMAScript|regex::optimize);
      if (regex_search(_keyword, match, rgx)) {
        if (match.size() > 1) {
          rc = match[1];
          RemoveRepoStart(rc);
        }
      }
    }
    return rc;
  }
  
}  // namespace Dwm
