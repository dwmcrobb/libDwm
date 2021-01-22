//===========================================================================
// @(#) $Name$
// @(#) $Id: DwmEngFormat.cc 8388 2016-04-17 03:23:45Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007
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

#include <iomanip>
#include <sstream>
#include <vector>

#include "DwmSvnTag.hh"
#include "DwmEngFormat.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmEngFormat.cc 8388 $");

using namespace std;

namespace Dwm {

  static std::vector<std::pair<char,double> > k_multipliers;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static void InitMultipliers()
  {
    if (k_multipliers.empty()) {
      pair<char,double>  m;
      m.first = 'Y';  m.second = 1000000000000000000000000.0;  // yotta
      k_multipliers.push_back(m);
      m.first = 'Z';  m.second = 1000000000000000000000.0;     // zetta
      k_multipliers.push_back(m);
      m.first = 'E';  m.second = 1000000000000000000.0;        // exa
      k_multipliers.push_back(m);
      m.first = 'P';  m.second = 1000000000000000.0;           // peta
      k_multipliers.push_back(m);
      m.first = 'T';  m.second = 1000000000000.0;              // tera
      k_multipliers.push_back(m);
      m.first = 'G';  m.second = 1000000000.0;                 // giga
      k_multipliers.push_back(m);
      m.first = 'M';  m.second = 1000000.0;                    // mega
      k_multipliers.push_back(m);
      m.first = 'K';  m.second = 1000.0;                       // kilo
      k_multipliers.push_back(m);
      m.first = ' ';  m.second = 1.0;
      k_multipliers.push_back(m);
      m.first = 'm';  m.second = .001;                         // milli
      k_multipliers.push_back(m);
      m.first = 'u';  m.second = .000001;                      // micro
      k_multipliers.push_back(m);
      m.first = 'n';  m.second = .000000001;                   // nano
      k_multipliers.push_back(m);
      m.first = 'p';  m.second = .000000000001;                // pico
      k_multipliers.push_back(m);
      m.first = 'f';  m.second = .000000000000001;             // femto
      k_multipliers.push_back(m);
      m.first = 'a';  m.second = .000000000000000001;          // atto
      k_multipliers.push_back(m);
      m.first = 'z';  m.second = .000000000000000000001;       // zepto
      k_multipliers.push_back(m);
      m.first = 'y';  m.second = .000000000000000000000001;    // yocto
      k_multipliers.push_back(m);
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename _valueT>
  static std::string EngPrintTemplate(const _valueT & val,
                                      uint8_t precision, uint8_t width)
  {
    InitMultipliers();
    
    std::ostringstream  os;
    os.precision(precision);

    std::vector<std::pair<char,double> >::const_iterator  iter;
    for (iter = k_multipliers.begin(); iter != k_multipliers.end(); ++iter) {
      double    v = val / iter->second;
      if ((v >= 1) || (v <= -1)) {
        os << setw(width) << v << iter->first;
        break;
      }
    }
    if (iter == k_multipliers.end()) {
      os << setw(width) << val;
      if (val > 1 || val < 1)
        os << " ";
    }
    
    return(os.str());
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const uint64_t & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const uint32_t & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const uint16_t & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const int64_t & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const int32_t & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const int16_t & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const float & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  EngFormat::EngFormat(const double & val, uint8_t precision, uint8_t width)
      : _val(EngPrintTemplate(val, precision, width))
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const EngFormat & ef)
  {
    os << ef._val;
    return(os);
  }

}  // namespace Dwm
