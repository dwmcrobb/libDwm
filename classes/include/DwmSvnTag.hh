//===========================================================================
// @(#) $DwmPath$
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
//!  \file DwmSvnTag.hh
//!  \brief Dwm::SvnTag class declaration
//---------------------------------------------------------------------------

#ifndef _DWMSVNTAG_HH_
#define _DWMSVNTAG_HH_

#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Trivial class used to fetch an assumed SVN tag name from a
  //!  repository path (in a Subversion keyword).
  //--------------------------------------------------------------------------
  class SvnTag
  {
  public:
    //------------------------------------------------------------------------
    //!  Construct from a Header keyword substitution or any custom keyword
    //!  substition that includes %P or %u.
    //------------------------------------------------------------------------
    SvnTag(const std::string & keyword);

    //------------------------------------------------------------------------
    //!  Fetches the SVN tag name using a regular expression @c tagRegex.
    //!  The default regular expression covers the typical case where
    //!  a tagged version of software is under a 'tags' directory in the
    //!  repository.  Ditto for fetching a branch name using the regular
    //!  expression @c branchRegex or the trunk using @c trunkRegex.
    //!  When the working copy is on a branch, we will add " (branch)"
    //!  to the end of the name to distinguish it from a tag with the same
    //!  name.  When the working copy is the trunk, we will add " (revnum)"
    //!  to the end of the name, where revnum is the revision number of the
    //!  file's last modification.  We can only add the revnum if the keyword
    //!  in the constructor included Header, Id, Revision or a custom keyword
    //!  substitution whose 3rd word was the revision.  For example,
    //!  "$DwmPath: dwm/libDwm/trunk/include/DwmSvnTag.hh 11210 $".
    //------------------------------------------------------------------------
    std::string
    TagName(const std::string & tagRegex = R"(\/tags\/([^\/]+))",
            const std::string & branchRegex = R"(\/branches\/([^\/]+))",
            const std::string & trunkRegex = R"(\/(trunk)\/)") const;

    //------------------------------------------------------------------------
    //!  Returns the file name.  This is shortened a bit to try to remove
    //!  some of the path to the repository.
    //------------------------------------------------------------------------
    std::string FileName() const;
    
  private:
    const std::string  _keyword;

    bool IsRemoteRepo(const std::string & url) const;
    void RemoveRepoStart(std::string & path) const;
  };
  
}  // namespace Dwm

#endif  // _DWMSVNTAG_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
