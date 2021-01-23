//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020
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
//!  \file DwmXDRUtils.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::XDRUtils class declaration
//---------------------------------------------------------------------------

#ifndef _DWMXDRUTILS_HH_
#define _DWMXDRUTILS_HH_

#include <array>
#include <cstdint>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Wrapper for XDR encode/decode functions.
  //--------------------------------------------------------------------------
  class XDRUtils
  {
  public:
    //------------------------------------------------------------------------
    //!  Encode the given float @c val into the given buffer @c buf.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    static bool Encode(float val, std::array<char,4> & buf);
    
    //------------------------------------------------------------------------
    //!  Decodes the given buffer @c buf into the given value @c val.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    static bool Decode(const std::array<char,4> & buf, float & val);
    
    //------------------------------------------------------------------------
    //!  Encode the given double @c val into the given buffer @c buf.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    static bool Encode(double val, std::array<char,8> & buf);
    
    //------------------------------------------------------------------------
    //!  Decodes the given buffer @c buf into the given double value @c val.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    static bool Decode(const std::array<char,8> & buf, double & val);

    //------------------------------------------------------------------------
    //!  Encodes the given float @c val into @c *buf.  If @c *buf is non-null
    //!  on entry, it will be freed.  @c *buf will be allocated to hold the
    //!  encoded value.  Returns the size of *buf.
    //------------------------------------------------------------------------
    static uint32_t Encode(float val, char **buf);
    
    //------------------------------------------------------------------------
    //!  Decodes the given @c buf into the given float value @c val.
    //------------------------------------------------------------------------
    static void Decode(char *buf, float & val);
    
    //------------------------------------------------------------------------
    //!  Encodes the given double @c val into @c *buf.  If @c *buf is non-null
    //!  on entry, it will be freed.  @c *buf will be allocated to hold the
    //!  encoded value.  Returns the size of *buf.
    //------------------------------------------------------------------------
    static uint32_t Encode(double val, char **buf);
    
    //------------------------------------------------------------------------
    //!  Decodes the given @c buf into the given double value @c val.
    //------------------------------------------------------------------------
    static void Decode(char *buf, double & val);
  };
  
}  // namespace Dwm

#endif  // _DWMXDRUTILS_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
