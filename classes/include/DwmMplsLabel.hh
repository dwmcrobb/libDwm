//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmMplsLabel.hh 11087 $
// @(#) $Id: DwmMplsLabel.hh 11087 2020-09-07 09:58:37Z dwm $
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
//!  \file DwmMplsLabel.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::MplsLabel class definition
//---------------------------------------------------------------------------
#ifndef _DWMMPLSLABEL_HH_
#define _DWMMPLSLABEL_HH_

#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmBZ2IOCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates an MPLS label.
  //--------------------------------------------------------------------------
  class MplsLabel
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Sets all label bits to 0.
    //------------------------------------------------------------------------
    MplsLabel();

    //------------------------------------------------------------------------
    //!  Construct from a string of the form 'label:exp:s:ttl'.
    //------------------------------------------------------------------------
    MplsLabel(const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Returns the 20-bit label portion of the label.
    //------------------------------------------------------------------------
    uint32_t Label() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the 20-bit label portion of the label.
    //------------------------------------------------------------------------
    uint32_t Label(uint32_t label);

    //------------------------------------------------------------------------
    //!  Returns the 3-bit exp portion of the label.
    //------------------------------------------------------------------------
    uint8_t Exp() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the 3-bit exp portion of the label.
    //------------------------------------------------------------------------
    uint8_t Exp(uint8_t exp);

    //------------------------------------------------------------------------
    //!  Returns true if the S (bottom-of-stack) bit is set.
    //------------------------------------------------------------------------
    bool S() const;

    //------------------------------------------------------------------------
    //!  Sets the S bit.
    //------------------------------------------------------------------------
    bool S(bool s);

    //------------------------------------------------------------------------
    //!  Returns the TTL portion of the label.
    //------------------------------------------------------------------------
    uint8_t Ttl() const;

    //------------------------------------------------------------------------
    //!  Sets and returns the TTL portion of the label.
    //------------------------------------------------------------------------
    uint8_t Ttl(uint8_t ttl);

    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const MplsLabel & label) const;
    
    //------------------------------------------------------------------------
    //!  Reads the MPLS label from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes the MPLS label to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the MPLS label from a file descriptor.  Returns the number
    //!  of bytes read on success (should be 4), -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Writes the MPLS label to a file descriptor.  Returns the number of
    //!  bytes written on success (should be 4), -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads the MPLS label from a FILE.  Returns 1 on success, 0 on
    //!  failure (fread() semantics).
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes the MPLS label to a FILE.  Returns 1 on success, 0 on 
    //!  failure (fwrite() semantics).
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Reads the MPLS label from the given gzFile @c gzf.  Return the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes the MPLS lable to the given gzFile @c gzf.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the MPLS label from the given BZFILE @c bzf.  Return the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;
    
    //------------------------------------------------------------------------
    //!  Writes the MPLS lable to the given BZFILE @c bzf.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call one
    //!  of the Write() members.  Should always return 4.
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override;
    
    //------------------------------------------------------------------------
    //!  Prints to an ostream in 'label:exp:s:ttl' format.
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const MplsLabel & label);

    //------------------------------------------------------------------------
    //!  Reads from an istream in 'label:exp:s:ttl' format.
    //------------------------------------------------------------------------
    friend std::istream &
    operator >> (std::istream & is, MplsLabel & label);
    
  protected:
    uint32_t  _data;
  };
  
}  // namespace Dwm

#endif  // _DWMMPLSLABEL_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
