//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016
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
//!  \file DwmStringUtils.hh
//!  \brief Miscellaneous string utilities
//---------------------------------------------------------------------------

#ifndef _DWMSTRINGUTILS_HH_
#define _DWMSTRINGUTILS_HH_

#include <cerrno>
#include <cstdlib>
#include <climits>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include "DwmTypeName.hh"

namespace Dwm {

  namespace StringUtils {

    //------------------------------------------------------------------------
    //!  Returns true if \c str ends with \c suffix.  For example,
    //!  HasSuffix("foo.cc",".cc") would return true.
    //------------------------------------------------------------------------
    bool HasSuffix(const std::string & str, 
                   const std::string & suffix);

    //------------------------------------------------------------------------
    //!  Populates @c result with substrings from @c s, where each substring
    //!  is separated by @c sep in @c s.  If @c ignoreEmpty is true, all empty
    //!  substrings (a result of immediately adjacent instances of @c sep or
    //!  trailing instance(s) of @c sep) are excluded from @c result.
    //!  Returns true if any substrings were placed in @c result.
    //------------------------------------------------------------------------
    bool StringToVector(const std::string & s, char sep,
                        std::vector<std::string> & result,
                        bool ignoreEmpty = true);

    //------------------------------------------------------------------------
    //!  Simple exception class for StringTo().
    //------------------------------------------------------------------------
    class BadStringConversion
      : public std::runtime_error
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      BadStringConversion(const std::string & s)
          : std::runtime_error(s)
      {}
    };

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    bool StringToIntegral(const std::string & s, T & target, int base = 0)
    {
      bool  rc = false;
     
      if (std::numeric_limits<T>::is_signed) {
        // target is signed
        char  *endptr = 0;
        errno = 0;
        long long  val = strtoll(s.c_str(), &endptr, base);
        if ((! errno) && (endptr != s.c_str()) &&
            (val <= static_cast<long long>(std::numeric_limits<T>::max())) &&
            (val >= static_cast<long long>(std::numeric_limits<T>::min()))) {
          target = static_cast<T>(val);
          rc = true;
        }
      }
      else {
        //  target is unsigned
        char  *endptr = 0;
        errno = 0;
        unsigned long long  val = strtoull(s.c_str(), &endptr, base);
        if ((! errno) && (endptr != s.c_str()) &&
            (val <= static_cast<T>(std::numeric_limits<T>::max())) &&
            (val >= static_cast<T>(std::numeric_limits<T>::min()))) {
          target = static_cast<T>(val);
          rc = true;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    bool StringTo(const std::string & s, T & target)
    {
      bool  rc = false;

      if (! std::numeric_limits<T>::digits10) {
        //  target is a non-integer type
        std::istringstream  is(s);
        if ((is >> target)) {
          rc = true;
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  why anyone would every call this, I don't know.
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, std::string & target);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, char & target, int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, signed char & target, int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, unsigned char & target,
                  int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, short & target, int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, unsigned short & target,
                  int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, int & target, int base = 0);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, unsigned int & target, int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, long & target, int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, unsigned long & target,
                  int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, long long & target, int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, unsigned long long & target,
                  int base = 0);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, float & target);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, double & target);
     
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const std::string & s, bool & target);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static T StringTo(const std::string & s)
    {
      T  x;
      if (! StringTo(s, x)) {
        std::ostringstream  os;
        os << "\"" << s << "\" -> " << TypeName(x);
        throw(BadStringConversion(os.str()));
      }
      return(x);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    std::string ToString(const T & x, 
                         std::ios_base & (*iom)(std::ios_base &) = std::dec)
    {
      std::ostringstream  os;
      os << std::setprecision(500);
      if (iom)
        os << iom;
      os << x;
      return(os.str());
    }

    
  }  // namespace StringUtils
  
}  // namespace Dwm

#endif  // _DWMSTRINGUTILS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
