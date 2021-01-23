//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY 
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmReadable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Readable pure virtual class definition
//---------------------------------------------------------------------------

#ifndef _DWMREADABLE_HH_
#define _DWMREADABLE_HH_

#include "DwmDescriptorReadable.hh"
#include "DwmFileReadable.hh"
#include "DwmStreamReadable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class defines an interface for classes that can read their
  //!  contents from an istream, file descriptor or FILE pointer.
  //!
  //!  Note that this class is deprecated as of libDwm-0.8.0, and is now just
  //!  a trivial grouping of DescriptorReadable, FileReadable and
  //!  StreamReadable.
  //--------------------------------------------------------------------------
  class [[deprecated("Use DescriptorReadable, FileReadable and StreamReadable.")]] Readable
    : public DescriptorReadable,
      public FileReadable,
      public StreamReadable
  { };
  
}  // namespace Dwm

#endif  // _DWMREADABLE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
