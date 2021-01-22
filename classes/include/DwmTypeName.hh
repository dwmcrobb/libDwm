//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmTypeName.hh 9017 $
// @(#) $Id: DwmTypeName.hh 9017 2017-04-11 17:51:09Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007
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
//!  \file DwmTypeName.hh
//!  \brief Hackish support for getting the name of a given object's type,
//!  since std::type_info::name() returns a mangled name (or a string of
//!  length 1 for builtin types).
//---------------------------------------------------------------------------

#ifndef _DWMTYPENAME_HH_
#define _DWMTYPENAME_HH_

#include <cxxabi.h>

/*
extern "C" {
  //!  We depend on this being in libstdc++, which is true for the GNU library
  char *__cxa_demangle(const char *, char *, size_t *, int *);
}
*/

#include <cstdlib>
#include <string>
#include <typeinfo>

using namespace abi;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Returns a string holding the demangled name for the given \c typeInfo.
  //!  For builtin types, we return the literal name by doing a lookup in
  //!  a map that's set up in this function.
  //--------------------------------------------------------------------------
  std::string BuiltinTypeName(const std::type_info & typeInfo);
  
  //--------------------------------------------------------------------------
  //!  Returns the name of type @c T.
  //--------------------------------------------------------------------------
  template <typename T>
  std::string BuiltinTypeName()
  {
    return(BuiltinTypeName(typeid(T)));
  }
  
  //------------------------------------------------------------------------
  //!  Returns the name of type @c T.
  //------------------------------------------------------------------------
  template <typename T>
  std::string TypeName(const T & x)
  {
    static const std::string rcsid = "@(#) $DwmPath: dwm/libDwm/trunk/include/DwmTypeName.hh 9017 $";
    
    std::string  rc;
    int        demangleStatus = -1;
    char       *demangledName = 
      abi::__cxa_demangle(typeid(x).name(), 0, 0, &demangleStatus);
    if (demangledName) {
//    if (demangleStatus == 0) {
        rc = demangledName;
        if (rc == "unsigned") {
          rc += " int";
        }
//    }
      free(demangledName);
    }
    else {
      //  __cxa_demangle() returns NULL for built-in types, with a -2 status
      if (demangleStatus == -2) {
        rc = BuiltinTypeName<T>();
      }
    }
    
    return(rc);
  }

  //------------------------------------------------------------------------
  //!  Returns the name of type @c T, sans its namespace.
  //------------------------------------------------------------------------
  template <typename T>
  std::string TypeNameNoNamespace(const T & x)
  {
    std::string  rc;
    int        demangleStatus;
    char       *demangledName = 
      __cxa_demangle(typeid(x).name(), 0, 0, &demangleStatus);
    if (demangledName) {
      if (demangleStatus == 0) {
        rc = demangledName;
        if (rc == "unsigned") {
          rc += " int";
        }
      }
      free(demangledName);
    }
    else {
      //  __cxa_demangle() returns NULL for built-in types, with a -2 status
      if (demangleStatus == -2) {
        rc = BuiltinTypeName<T>();
      }
    }

    std::string::size_type  idx = rc.find_last_of("::");
    if (idx != rc.npos) {
      rc = rc.substr(idx + 1);
    }
    
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  Returns the name of type @c T.
  //--------------------------------------------------------------------------
  template <typename T>
  std::string TypeName()
  {
    std::string  rc;
    int        demangleStatus;
    char       *demangledName = 
      __cxa_demangle(typeid(T).name(), 0, 0, &demangleStatus);
    if (demangledName) {
      if (demangleStatus == 0) {
        rc = demangledName;
      }
      free(demangledName);
    }
    else {
      //  __cxa_demangle() returns NULL for built-in types, with a -2 status
      if (demangleStatus == -2) {
        rc = BuiltinTypeName<T>();
      }
    }
    
    return(rc);
  }

}  // namespace Dwm

#endif  // _DWMTYPENAME_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
