//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmBase64.cc 11089 $
// @(#) $Id: DwmBase64.cc 11089 2020-09-07 15:58:08Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020
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
//!  \file DwmBase64.cc
//!  \author Daniel W. McRobb
//!  \brief base64 and base64url encoding/decoding function implementations
//---------------------------------------------------------------------------

#include <cstdint>
#include <iomanip>

#include "DwmSvnTag.hh"
#include "DwmBase64.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmBase64.cc 11089 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static const char  g_encodeArr[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '+', '/'
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static const char  g_encodeUrlArr[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '-', '_'
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static const uint8_t  g_decodeArr[128] = {
    // unused (0-42)
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255,
    //  plus character
    62,
    //  unused (44-46)
    255, 255, 255,
    //  slash character
    63,
    //  numbers
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61,
    //  unused (58-64)
    255, 255, 255, 255, 255, 255, 255,
    //  uppercase letters
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,
    13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
    //  unused (91-96)
    255, 255, 255, 255, 255, 255,
    //  lowercase letters
    26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
    39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,
    //  unused (123-127)
    255, 255, 255, 255, 255
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static const uint8_t  g_decodeUrlArr[128] = {
    // unused (0-42)
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255,
    //  hyphen
    62,
    //  unused (46-47)
    255, 255,
    //  numbers
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61,
    //  unused (58-64)
    255, 255, 255, 255, 255, 255, 255,
    //  uppercase letters
    0,    1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,
    13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
    //  unused (91-94)
    255, 255, 255, 255,
    63,
    //  unused (96)
    255,
    //  lowercase letters
    26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
    39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,
    //  unused (123-127)
    255, 255, 255, 255, 255
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static std::string Base64Decode(const std::string & s,
                                  const uint8_t *decodeArr)
  {
    string  rc;
    bool    err = false;
    string::size_type  numChars = s.size();
    rc.reserve(((numChars * 3) / 4) + 3);
    if (numChars && ((numChars & 0x3) == 0)) {
      for (string::size_type i = 0; i < numChars; i += 4) {
        int32_t  v = 0;
        uint8_t  j, shift;
        for (j = 0, shift = 18; j < 4; ++j, shift -= 6) {
          uint8_t  c = s[i+j];
          if (c < 128) {
            uint8_t  d = decodeArr[c];
            if (d < 64)          { v |= ((uint32_t)d << shift); }
            else if (c != '=')   { err = true; break; }
            else                 { break; }
          }
          else { err = true; break; }
        }
        if (err) { break; }
        for (j = 0, shift = 16; j < 3; ++j, shift -= 8) {
          if (s[i+j+1] != '=') {
            rc += (uint8_t)((v >> shift) & 0xFF);
          }
        }
      }
    }
    if (err) {
      rc = "";
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static std::string Base64Encode(const std::string & s,
                                  const char *encodeArr)
  {
    string             rc;
    string::size_type  slen = s.size();
    string::size_type  numGroups = slen / 3;
    string::size_type  numGrouped = numGroups * 3;
    uint32_t           v;
    int8_t             shift, j;
    string::size_type  i = 0;

    rc.reserve(((s.size() * 4) / 3) + 4);
    for ( ; i < numGrouped; i += 3) {
      v = 0;
      for (j = 0, shift = 16; j < 3; ++j, shift -= 8) {
        v |= (uint32_t)((uint8_t)s[i+j]) << shift;
      }
      for (shift = 18; shift >= 0; shift -= 6) {
        rc.push_back(encodeArr[(v >> shift) & 0x3F]);
      }
    }
    if (i < slen) {
      string::size_type  n = rc.size();
      int                bitsLeft = (slen - i) * 8;
      for (j = 0; j < 4; ++j) {
        rc.push_back('=');
      }
      for (v = 0, shift = 16; shift > 0; ++i, shift -= 8) {
        v |= (uint32_t)((uint8_t)s[i]) << shift;
      }
      for (shift = 18; bitsLeft > 0; shift -= 6, bitsLeft -= 6, ++n) {
        rc[n] = encodeArr[(v >> shift) & 0x3F];
      }
    }
    
    return rc;
  }
                                    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Base64::Decode(const std::string & s)
  {
    return Base64Decode(s, g_decodeArr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Base64::Encode(const std::string & s)
  {
    return Base64Encode(s, g_encodeArr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Base64Url::Decode(const std::string & s)
  {
    return Base64Decode(s, g_decodeUrlArr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string Base64Url::Encode(const std::string & s)
  {
    return Base64Encode(s, g_encodeUrlArr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static std::ostream & Base64Decode(std::istream & is, std::ostream & os,
                                     const uint8_t *decodeArr)
  {
    bool    err = false;
    if (is && os) {
      while (is) {
        int32_t  v = 0;
        uint8_t  j, shift;
        for (j = 0, shift = 18; j < 4; ++j, shift -= 6) {
          uint8_t  c;
          if (! is.read((char *)&c, 1)) {
            if (j != 0) {
              err = true;
            }
            break;
          }
          if (c < 128) {
            uint8_t  d = decodeArr[c];
            if (d < 64)          { v |= ((uint32_t)d << shift); }
            else if (c != '=')   { err = true; break; }
            else                 { break; }
          }
          else { err = true; break; }
        }
        if (err) { break; }
        int8_t  bitsLeft = j * 6;
        for (shift = 16; bitsLeft > 5; bitsLeft -= 8, shift -= 8) {
          os << (uint8_t)((v >> shift) & 0xFF);
        }
      }
    }
    if (err) {
      os.setstate(ios_base::failbit);
      is.setstate(ios_base::failbit);
    }
    return os;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static std::ostream & Base64Encode(std::istream & is, std::ostream & os,
                                     const char *encodeArr)
  {
    if (is && os) {
      uint32_t           v;
      int8_t             shift, i, j;
      uint8_t            c;
      while (is) {
        v = 0;
        shift = 16;
        for (i = 0; i < 3; ++i, shift -= 8) {
          if (is.read((char *)&c, 1)) { v |= (uint32_t)c << shift; }
          else { break; }
        }
        int8_t  numBits = i * 8;
        for (shift = 18, j = 0; numBits > 0; ++j, numBits -= 6, shift -= 6) {
          os << encodeArr[(v >> shift) & 0x3F];
        }
        if ((i > 0) && (i < 3)) {
          for ( ; j < 4; ++j) {
            os << '=';
          }
        }
      }
    }
    return os;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Base64::Encode(std::istream & is, std::ostream & os)
  {
    return Base64Encode(is, os, g_encodeArr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Base64::Decode(std::istream & is, std::ostream & os)
  {
    return Base64Decode(is, os, g_decodeArr);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Base64Url::Encode(std::istream & is, std::ostream & os)
  {
    return Base64Encode(is, os, g_encodeUrlArr);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Base64Url::Decode(std::istream & is, std::ostream & os)
  {
    return Base64Decode(is, os, g_decodeUrlArr);
  }
  
}  // namespace Dwm
