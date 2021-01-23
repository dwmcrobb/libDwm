//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmGroup.hh
//!  \brief Dwm::Group class definition
//---------------------------------------------------------------------------

#ifndef _DWMGROUP_HH_
#define _DWMGROUP_HH_

extern "C" {
#include <grp.h>
}

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an /etc/group entry.  The main reason for this class:
  //!  some sub-par operating systems don't provide threadsafe versions
  //!  of getgrgid() and getgrnam().
  //--------------------------------------------------------------------------
  class Group
  {
  public:
    //------------------------------------------------------------------------
    //!  Construct from a group ID.
    //------------------------------------------------------------------------
    Group(gid_t gid);

    //------------------------------------------------------------------------
    //!  Construct from a group name.
    //------------------------------------------------------------------------
    Group(const std::string & name);

    //------------------------------------------------------------------------
    //!  Use the contents of \c group to fill all of our members.
    //------------------------------------------------------------------------
    void Set(const struct group & group);

    //------------------------------------------------------------------------
    //!  Returns the group ID.
    //------------------------------------------------------------------------
    gid_t Id() const;

    //------------------------------------------------------------------------
    //!  Returns the group name.
    //------------------------------------------------------------------------
    const std::string & Name() const;

    //------------------------------------------------------------------------
    //!  Returns the group password.
    //------------------------------------------------------------------------
    const std::string & Password() const;

    //------------------------------------------------------------------------
    //!  Returns the group members.
    //------------------------------------------------------------------------
    const std::vector<std::string> & Members() const;

    //------------------------------------------------------------------------
    //!  Prints to an ostream in /etc/group format.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, const Group & group);
    
  private:
    gid_t                     _id;
    std::string               _name;
    std::string               _password;
    std::vector<std::string>  _members;
    
    static std::mutex         _mutex;
  };
  
}  // namespace Dwm

#endif  // _DWMGROUP_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
