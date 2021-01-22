//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmBZ2Readable.hh 8401 $
// @(#) $Id: DwmBZ2Readable.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2006, 2016
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY 
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmBZ2Readable.hh
//!  \brief Dwm::BZ2Readable pure virtual class definition
//---------------------------------------------------------------------------

#ifndef _DWMBZ2READABLE_HH_
#define _DWMBZ2READABLE_HH_

extern "C" {
  #include <sys/types.h>
  #include <stdio.h>
  #include <bzlib.h>
}

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class is a pure virtual class, defining an interface for
  //!  classes that can read their contents from a BZFILE pointer.
  //--------------------------------------------------------------------------
  class BZ2Readable
  {
  public:
    virtual ~BZ2Readable() { }

    //------------------------------------------------------------------------
    //!  Read from a BZFILE pointer.  Should return the number of bytes read
    //!  on success (-1 on failure?)
    //------------------------------------------------------------------------
    virtual int BZRead(BZFILE *bzf) = 0;
  };
  
}  // namespace Dwm

#endif  // _DWMBZ2READABLE_HH_
