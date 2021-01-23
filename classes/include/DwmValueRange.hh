//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file DwmValueRange.hh
//!  \brief Dwm::ValueRange class template definition
//---------------------------------------------------------------------------

#ifndef _DWMVALUERANGE_HH_
#define _DWMVALUERANGE_HH_

#include <cassert>
#include <utility>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This template encapsulates a range of values (low, high).
  //--------------------------------------------------------------------------
  template <typename T>
  class ValueRange
    : public std::pair<T,T>
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    ValueRange()
        : std::pair<T,T>()
    { }

    //------------------------------------------------------------------------
    //!  Construct from given @c low and @ high values.
    //------------------------------------------------------------------------
    ValueRange(const T & low, const T & high)
        : std::pair<T,T>(low, high)
    {
      assert(high >= low);
    }

    //------------------------------------------------------------------------
    //!  Returns true if the range contains @c value.
    //------------------------------------------------------------------------
    bool Contains(const T & value) const
    {
      return((value >= this->first) && (value <= this->second));
    }
    
    //------------------------------------------------------------------------
    //!  Returns true if the range contains the given @c range.
    //------------------------------------------------------------------------
    bool Contains(const ValueRange<T> & range) const
    {
      return(Contains(range.first) && Contains(range.second));
    }

    //------------------------------------------------------------------------
    //!  Returns true if the range overlaps the given @c range.
    //------------------------------------------------------------------------
    bool Overlaps(const ValueRange<T> & range) const
    {
      return(this->Contains(range.first) 
             || this->Contains(range.second) 
             || range.Contains(this->first) 
             || range.Contains(this->second));
    }
    
  };


}  // namespace Dwm

#endif  // _DWMVALUERANGE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
