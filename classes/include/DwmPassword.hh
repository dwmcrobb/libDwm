//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2016
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
//!  \file DwmPassword.hh
//!  \brief Dwm::Password class definition
//---------------------------------------------------------------------------

#ifndef _DWMPASSWORD_HH_
#define _DWMPASSWORD_HH_

extern "C" {
#include <sys/types.h>
#include <pwd.h>
}

#include <iostream>
#include <mutex>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an /etc/passwd entry.  The main reason for this class:
  //!  some operating systems don't provide threadsafe versions of getpwuid()
  //!  and getpwnam().
  //--------------------------------------------------------------------------
  class Password
  {
  public:
    //------------------------------------------------------------------------
    //!  Construct from a user name.
    //------------------------------------------------------------------------
    Password(const std::string & userName);

    //------------------------------------------------------------------------
    //!  Construct from a user ID.
    //------------------------------------------------------------------------
    Password(uid_t userId);

    //------------------------------------------------------------------------
    //!  Use the contents of \c passwdStruct to fill all of our members.
    //------------------------------------------------------------------------
    void Set(const struct passwd & passwdStruct);

    //------------------------------------------------------------------------
    //!  Returns the user name.
    //------------------------------------------------------------------------
    const std::string & UserName() const;

    //------------------------------------------------------------------------
    //!  Returns the user password.
    //------------------------------------------------------------------------
    const std::string & EncryptedPassword() const;

    //------------------------------------------------------------------------
    //!  Returns the user ID.
    //------------------------------------------------------------------------
    uid_t UserId() const;

    //------------------------------------------------------------------------
    //!  Returns the group ID.
    //------------------------------------------------------------------------
    gid_t GroupId() const;

    //------------------------------------------------------------------------
    //!  Returns the gecos.
    //------------------------------------------------------------------------
    const std::string & Gecos() const;

    //------------------------------------------------------------------------
    //!  Returns the home directory.
    //------------------------------------------------------------------------
    const std::string & HomeDirectory() const;

    //------------------------------------------------------------------------
    //!  Returns the shell.
    //------------------------------------------------------------------------
    const std::string & Shell() const;

    //------------------------------------------------------------------------
    //!  Prints to an ostream in /etc/passwd format.
    //------------------------------------------------------------------------
    friend std::ostream & operator << (std::ostream & os, 
                                       const Password & password);
    
  private:
    std::string  _userName;
    std::string  _encryptedPassword;
    uid_t        _userId;
    gid_t        _groupId;
    std::string  _gecos;
    std::string  _homeDirectory;
    std::string  _shell;

    static std::mutex  _mutex;
  };
  
}  // namespace Dwm

#endif  // _DWMPASSWORD_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
