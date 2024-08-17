//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file DwmException.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Exception class template
//---------------------------------------------------------------------------

#ifndef _DWMEXCEPTION_HH_
#define _DWMEXCEPTION_HH_

#include <source_location>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A basic exception template with parameterized data and automatic
  //!  source location.
  //--------------------------------------------------------------------------
  template <typename T>
  class Exception
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Exception(std::string what, T data,
              const std::source_location & loc
              = std::source_location::current())
        : _what{std::move(what)}, _data{std::move(data)}, _loc{loc}
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::string & what()                                    { return _what; }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const std::string & what() const noexcept               { return _what; }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const std::source_location & where() const noexcept      { return _loc; }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    T & data()                                              { return _data; }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const T & data() const noexcept                         { return _data; }

  private:
    std::string                 _what;
    T                           _data;
    const std::source_location  _loc;
  };
  
}  // namespace Dwm

#endif  // _DWMEXCEPTION_HH_
