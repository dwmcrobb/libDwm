//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmPassword.cc 11122 $
// @(#) $Id: DwmPassword.cc 11122 2020-09-07 23:16:38Z dwm $
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
//!  \file DwmPassword.cc
//!  \author Daniel W. McRobb
//!  \brief Password class implementation
//---------------------------------------------------------------------------

#include "DwmSvnTag.hh"
#include "DwmPassword.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmPassword.cc 11122 $");

namespace Dwm {

  std::mutex  Password::_mutex;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Password::Password(const std::string & userName)
  {
    std::lock_guard<std::mutex>  lk(_mutex);
    struct passwd  *p = getpwnam(userName.c_str());
    if (p)
      Set(*p);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Password::Password(uid_t userId)
  {
    std::lock_guard<std::mutex>  lk(_mutex);
    struct passwd  *p = getpwuid(userId);
    if (p)
      Set(*p);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Password::Set(const struct passwd & passwdStruct)
  {
    _userName          = passwdStruct.pw_name;
    _encryptedPassword = passwdStruct.pw_passwd;
    _userId            = passwdStruct.pw_uid;
    _groupId           = passwdStruct.pw_gid;
    _gecos             = passwdStruct.pw_gecos;
    _homeDirectory     = passwdStruct.pw_dir;
    _shell             = passwdStruct.pw_shell;

    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Password::UserName() const
  {
    return(_userName);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Password::EncryptedPassword() const
  {
    return(_encryptedPassword);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uid_t Password::UserId() const
  {
    return(_userId);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  gid_t Password::GroupId() const
  {
    return(_groupId);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Password::Gecos() const
  {
    return(_gecos);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Password::HomeDirectory() const
  {
    return(_homeDirectory);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Password::Shell() const
  {
    return(_shell);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const Password & password)
  {
    if (os) {
      os << password._userName << ":" << password._encryptedPassword << ":"
         << password._userId << ":" << password._groupId << ":"
         << password._gecos << ":" << password._homeDirectory << ":"
         << password._shell;
    }
    return(os);
  }
  

}  // namespace Dwm
