//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005, 2016
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
//!  \file DwmEngFormat.hh
//!  \brief Dwm::EngFormat class definition
//---------------------------------------------------------------------------

#ifndef _DWMENGFORMAT_HH_
#define _DWMENGFORMAT_HH_

extern "C" {
#include <inttypes.h>
}

#include <iostream>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Class to format numbers in an engineering-type form.  Instead of
  //!  printing numerical powers of 10, we print 'K' (kilo), 'M' (mega),
  //!  'm' (milli), 'u' (micro), et. al.
  //!
  //!  Example:
  //!
  //!    uint32_t  v = 123456;
  //!    std::cout << Dwm::EngFormat(v) << std::endl;
  //!
  //!  would print:
  //!
  //!    123.5K
  //!
  //!  \c precision is the maximum precision (places including those to the
  //!  left of the decimal point) to print.  \c width is the minimum field 
  //!  width to print; we right-justify and fill with spaces.
  //--------------------------------------------------------------------------
  class EngFormat
  {
  public:
    EngFormat(const uint64_t & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const uint32_t & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const uint16_t & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const int64_t & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const int32_t & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const int16_t & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const float & val, uint8_t precision = 4, uint8_t width = 5);
    EngFormat(const double & val, uint8_t precision = 4, uint8_t width = 5);
    
    friend std::ostream & 
    operator << (std::ostream & os, const EngFormat & ef);
    
  private:
    std::string  _val;

    EngFormat();
  };
  
}  // namespace Dwm

#endif  // _DWMENGFORMAT_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
