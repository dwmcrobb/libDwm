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
//!  \file DwmIpv4Utils.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4Utils function implementations
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <netinet/in.h>
}

#include "DwmSvnTag.hh"
#include "DwmIpv4Utils.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmIpv4Utils.cc 11115 $");

namespace Dwm {

  namespace Ipv4Utils {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint32_t Sum(const uint8_t *ptr, uint16_t len)    
    {
      uint32_t         rc = 0;
      const uint16_t  *p = (const uint16_t *)ptr;
      while (len >= 2) {
        rc += *p++;
        len -= 2;
      }
      if (len) {
        ptr = (uint8_t *)p;
        rc += htons((uint16_t)(*ptr) << 8);
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void AddCarries(uint32_t & sum)
    {
      // take only 16 bits out of the 32 bit sum and add up the carries
      while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
      // one's complement the result
      sum = ~sum;
      return;
    }
    
    
  }  // namespace Ipv4Utils

}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
