//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmWritable.hh 11181 $
// @(#) $Id: DwmWritable.hh 11181 2020-09-19 03:28:28Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2020
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
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmWritable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Writable pure virtual class definition
//---------------------------------------------------------------------------
#ifndef _DWMWRITABLE_HH_
#define _DWMWRITABLE_HH_

#include "DwmDescriptorWritable.hh"
#include "DwmFileWritable.hh"
#include "DwmStreamWritable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can write their
  //!  contents to an ostream, file descriptor or FILE pointer.
  //!
  //!  Note that this class is deprecated as of libDwm-0.8.0, and is now just
  //!  a grouping of DescriptorWritable, FileWritable and StreamWritable.
  //!  The older version of this class also included a StreamedLength()
  //!  pure virtual member.  To get that interface, you now need to add
  //!  StreamedLengthCapable to any derived class.
  //--------------------------------------------------------------------------
  class [[deprecated("Use DescriptorWritable, FileWritable and StreamWritable.")]] Writable
    : public DescriptorWritable, public FileWritable, public StreamWritable
  {};
    
}  // namespace Dwm

#endif  // _DWMWRITABLE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
