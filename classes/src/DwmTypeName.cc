//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmTypeName.cc 11146 $
// @(#) $Id: DwmTypeName.cc 11146 2020-09-07 23:55:23Z dwm $
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
//!  \file DwmTypeName.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::BuiltinTypeName implementation
//---------------------------------------------------------------------------

#include <map>
#include <string>

#include "DwmTypeName.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmTypeName.cc 11146 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string BuiltinTypeName(const std::type_info & typeInfo)
  {
    std::string  rc;

    static std::map<std::string,std::string>  builtins;
    if (builtins.empty()) {
      builtins[typeid(signed char).name()]         = "signed char";
      builtins[typeid(bool).name()]                = "bool";
      builtins[typeid(char).name()]                = "char";
      builtins[typeid(double).name()]              = "double";
      builtins[typeid(long double).name()]         = "long double";
      builtins[typeid(float).name()]               = "float";
      builtins[typeid(unsigned char).name()]       = "unsigned char";
      builtins[typeid(int).name()]                 = "int";
      builtins[typeid(unsigned int).name()]        = "unsigned int";
      builtins[typeid(long).name()]                = "long";
      builtins[typeid(unsigned long).name()]       = "unsigned long";
      builtins[typeid(short).name()]               = "short";
      builtins[typeid(unsigned short).name()]      = "unsigned short";
      builtins[typeid(void).name()]                = "void";
      builtins[typeid(wchar_t).name()]             = "wchar_t";
      builtins[typeid(long long).name()]           = "long long";
      builtins[typeid(unsigned long long).name()]  = "unsigned long long";
    }
    std::map<std::string,std::string>::const_iterator  i = 
      builtins.find(typeInfo.name());
    if (i != builtins.end())
      rc = i->second;
    
    return(rc);
  }
  
  
}  // namespace Dwm
