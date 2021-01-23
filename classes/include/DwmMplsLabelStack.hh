//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016, 2020
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
//!  \file DwmMplsLabelStack.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::MplsLabelStack class definition
//---------------------------------------------------------------------------

#ifndef _DWMMPLSLABELSTACK_HH_
#define _DWMMPLSLABELSTACK_HH_

#include <vector>

#include "DwmMplsLabel.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an MPLS label stack.
  //--------------------------------------------------------------------------
  class MplsLabelStack
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Returns a const reference to the contained MPLS labels.
    //------------------------------------------------------------------------
    const std::vector<MplsLabel> & Labels() const;

    //------------------------------------------------------------------------
    //!  Returns a mutable reference to the contained MPLS labels.
    //------------------------------------------------------------------------
    std::vector<MplsLabel> & Labels();

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const MplsLabelStack & labelStack) const;
    
    //------------------------------------------------------------------------
    //!  Reads the label stack from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes the label stack to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the label stack from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Writes the label stack to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads the label stack from a FILE.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes the label stack to a FILE.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Reads the label stack from the given gzFile @c gzf.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes the label stack to the given gzFile @c gzf.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;

    //------------------------------------------------------------------------
    //!  Reads the label stack from the given BZFILE @c bzf.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;

    //------------------------------------------------------------------------
    //!  Writes the label stack to the given BZFILE @c bzf.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call one
    //!  of the Write() members.
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override;

    //------------------------------------------------------------------------
    //!  Prints the label stack to an ostream in human-readable form.
    //------------------------------------------------------------------------
    friend std::ostream & 
    operator << (std::ostream & os, const MplsLabelStack & labelStack);
    
  protected:
    std::vector<MplsLabel>  _labels;
  };
  
}  // namespace Dwm

#endif  // _DWMMPLSLABELSTACK_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
