//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmIpv4Utils.hh 8401 $
// @(#) $Id: DwmIpv4Utils.hh 8401 2016-04-17 06:44:31Z dwm $
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
//!  \file DwmIpv4Utils.hh
//!  \brief Miscellaneous IPv4 utilities
//---------------------------------------------------------------------------

#ifndef _DWMIPV4UTILS_HH_
#define _DWMIPV4UTILS_HH_

extern "C" {
  #include <inttypes.h>
}

namespace Dwm {
  
  namespace Ipv4Utils {

    //------------------------------------------------------------------------
    //!  Compute the IP sum (part of checksum computation) of the memory
    //!  at @c ptr of length @c len.
    //------------------------------------------------------------------------
    uint32_t Sum(const uint8_t *ptr, uint16_t len);

    //------------------------------------------------------------------------
    //!  Part of IP checksum computation... add the carries and do the 1's
    //!  complement.
    //------------------------------------------------------------------------
    void AddCarries(uint32_t & sum);

  }  // namespace Ipv4Utils

}  // namespace Dwm

#endif  // _DWMIPV4UTILS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
