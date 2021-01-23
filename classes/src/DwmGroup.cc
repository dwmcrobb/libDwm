//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2016, 2020
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
//!  \file DwmGroup.cc
//!  \author Daniel W. McRobb
//!  \brief Group class implementation
//---------------------------------------------------------------------------

#include "DwmSvnTag.hh"
#include "DwmGroup.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmGroup.cc 11098 $");

using namespace std;

namespace Dwm {

  std::mutex  Group::_mutex;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Group::Group(gid_t gid)
      : _id(0), _name(), _password(), _members()
  {
    std::lock_guard<std::mutex>  lk(_mutex);
    struct group  *g = getgrgid(gid);
    if (g) {
      Set(*g);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Group::Group(const string & name)
      : _id(0), _name(), _password(), _members()
  {
    std::lock_guard<std::mutex>  lk(_mutex);
    struct group  *g = getgrnam(name.c_str());
    if (g) {
      Set(*g);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void Group::Set(const struct group & group)
  {
    _id       = group.gr_gid;
    _name     = group.gr_name;
    _password = group.gr_passwd;
    
    if (! _members.empty())
      _members.clear();
    for (int i = 0; group.gr_mem[i]; ++i) {
      _members.push_back(group.gr_mem[i]);
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  gid_t Group::Id() const
  {
    return(_id);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & Group::Name() const
  {
    return(_name);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & Group::Password() const
  {
    return _password;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const vector<string> & Group::Members() const
  {
    return(_members);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ostream & operator << (ostream & os, const Group & group)
  {
    if (os) {
      os << group._name << ":" << group._password << ":" << group._id;
      if (! group._members.empty()) {
        vector<string>::const_iterator  iter;
        for (iter = group._members.begin(); 
             iter != group._members.end(); ++iter) {
          os << ":" << *iter;
        }
      }
      else {
        os << ":";
      }
    }
    return(os);
  }
  
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
