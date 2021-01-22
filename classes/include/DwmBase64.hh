//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmBase64.hh 9942 $
// @(#) $Id: DwmBase64.hh 9942 2018-01-06 07:40:26Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018
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
//!  \file DwmBase64.hh
//!  \brief base64 and base64url encoding/decoding function definitions
//---------------------------------------------------------------------------

#ifndef _DWMBASE64_HH_
#define _DWMBASE64_HH_

#include <iostream>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  RFC 4648 base64 encoding and decoding functions.  Uses the alphabet
  //!  from Table 1 of RFC 4648.
  //--------------------------------------------------------------------------
  class Base64
  {
  public:
    //------------------------------------------------------------------------
    //!  Returns a base64 encoded version of @c s.
    //------------------------------------------------------------------------
    static std::string Encode(const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Returns a decoded string for the base64-encoded string @c s on
    //!  success, an empty string on failure.
    //------------------------------------------------------------------------
    static std::string Decode(const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Base64 encodes the contents of istream @c is into the ostream @ c os.
    //!  Returns the ostream @c os.
    //------------------------------------------------------------------------
    static std::ostream & Encode(std::istream & is, std::ostream & os);
    
    //------------------------------------------------------------------------
    //!  Decodes the base64 encoded contents of istream @c is into the
    //!  ostream @c os.  Returns the ostream @c os.  Note that the failbit
    //!  will be set on both streams on failure.  A typical cause of failure
    //!  would be an invalid character in the input stream @c is.
    //------------------------------------------------------------------------
    static std::ostream & Decode(std::istream & is, std::ostream & os);
  };

  //--------------------------------------------------------------------------
  //!  RFC 4648 base64 enconding and decoding functions for URLs.  Uses the
  //!  alphabet from Table 2 of RFC 4648.
  //--------------------------------------------------------------------------
  class Base64Url
  {
  public:
    //------------------------------------------------------------------------
    //!  Returns a base64url encoded version of @c s.
    //------------------------------------------------------------------------
    static std::string Encode(const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Returns a decoded string for the base64url-encoded string @c s on
    //!  success, an empty string on failure.
    //------------------------------------------------------------------------
    static std::string Decode(const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Base64url encodes the contents of istream @c is into the ostream
    //!  @ c os.  Returns the ostream @c os.
    //------------------------------------------------------------------------
    static std::ostream & Encode(std::istream & is, std::ostream & os);
    
    //------------------------------------------------------------------------
    //!  Decodes the base64url encoded contents of istream @c is into the
    //!  ostream @c os.  Returns the ostream @c os.  Note that the failbit
    //!  will be set on both streams on failure.  A typical cause of failure
    //!  would be an invalid character in the input stream @c is.
    //------------------------------------------------------------------------
    static std::ostream & Decode(std::istream & is, std::ostream & os);
  };
    
}  // namespace Dwm

#endif  // _DWMBASE64_HH_
