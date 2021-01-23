//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2013, 2016, 2019
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
//!  \file DwmStringUtils.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::StringUtils class implementation
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/stat.h>
}

#include "DwmStringUtils.hh"

using namespace std;

namespace Dwm {

  namespace StringUtils {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool HasSuffix(const string & str, const string & suffix)
    {
      bool  rc = false;
      if (str.length() >= suffix.length()) {
        string::size_type  suffixPos = str.rfind(suffix);
        if (suffixPos != string::npos) {
          if (suffixPos == (str.length() - suffix.length())) {
            rc = true;
          }
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringToVector(const string & s, char sep, vector<string> & result,
                        bool ignoreEmpty)
    {
      result.clear();
      string::size_type  idx = 0;
      string::size_type  nextIdx;
      do {
        nextIdx = s.find_first_of(sep, idx);
        if (nextIdx != s.npos) {
          if (ignoreEmpty) {
            if (nextIdx > idx) {
              result.push_back(s.substr(idx, nextIdx - idx));
            }
          }
          else {
            result.push_back(s.substr(idx, nextIdx - idx));
          }
          idx = nextIdx + 1;
        }
      } while (nextIdx != s.npos);
      
      if (s.length() > idx) {
        result.push_back(s.substr(idx, s.length() - idx));
      }
      else if ((! ignoreEmpty) && (s.length() == idx)) {
        result.push_back("");
      }
      return (! result.empty());
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, string & target)
    {
      target = s;
      return(true);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, char & target, int base)
    {
      return(StringToIntegral<char>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, signed char & target, int base)
    {
      return(StringToIntegral<signed char>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, unsigned char & target, int base)
    {
      return(StringToIntegral<unsigned char>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, short & target, int base)
    {
      return(StringToIntegral<short>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, unsigned short & target, int base)
    {
      return(StringToIntegral<unsigned short>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, int & target, int base)
    {
      return(StringToIntegral<int>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, unsigned int & target, int base)
    {
      return(StringToIntegral<unsigned int>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, long & target, int base)
    {
      return(StringToIntegral<long>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, unsigned long & target, int base)
    {
      return(StringToIntegral<unsigned long>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, long long & target, int base)
    {
      return(StringToIntegral<long long>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, unsigned long long & target, int base)
    {
      return(StringToIntegral<unsigned long long>(s, target, base));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, float & target)
    {
      bool  rc = false;
      if (! s.empty()) {
#ifdef HAVE_STRTOF
        char  *endptr = 0;
        errno = 0;
        target = strtof(s.c_str(), &endptr);
        if ((! errno) && (endptr == s.c_str() + s.length())) {
          rc = true;
        }
#else
		istringstream  is;
		is.str(s);
		if (is >> target)
			rc = true;
#endif
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, double & target)
    {
      bool  rc = false;

      if (! s.empty()) {
        char  *endptr = 0;
        errno = 0;
        target = strtod(s.c_str(), &endptr);
        if ((! errno) && (endptr == s.c_str() + s.length())) {
          rc = true;
        }
      }
      return(rc);
    }
      
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool StringTo(const string & s, bool & target)
    {
      bool  rc = false;
      
      if (s == "1" || s == "true") {
        target = true;
        rc = true;
      }
      else if (s == "0" || s == "false") {
        target = false;
        rc = true;
      }

      return(rc);
    }
    
  }  // namespace StringUtils
  
    
}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
