//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2025
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
//!  \file DwmSizedLength.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMSIZEDLENGTH_HH_
#define _DWMSIZEDLENGTH_HH_

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A simple class to encode a length (up to 64 bits) in a variable-sized
  //!  format on the wire.  On the wire, a single 'size' byte precedes the
  //!  value, indicating the on-the-wire length of the value (1, 2, 4 or 8
  //!  bytes).
  //--------------------------------------------------------------------------
  class SizedLength
  {
  public:
    SizedLength() = default;
    SizedLength(const SizedLength &) = default;
    SizedLength & operator = (const SizedLength &) = default;
    SizedLength(SizedLength &&) = default;
    SizedLength & operator = (SizedLength &&) = default;
    ~SizedLength() = default;

    //------------------------------------------------------------------------
    //!  Construct from the given @c length.
    //------------------------------------------------------------------------
    SizedLength(uint64_t length)
        : _length(length)
    {}

    //------------------------------------------------------------------------
    //!  Assign from the given @c length.
    //------------------------------------------------------------------------
    SizedLength & operator = (uint64_t length)
    { _length = length; return *this; }
    
    //------------------------------------------------------------------------
    //!  Return the length as a uint64_t value.
    //------------------------------------------------------------------------
    operator uint64_t () const
    { return _length; }

    //------------------------------------------------------------------------
    //!  Writes the length to @c os.  Returns @c os.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;
    
    //------------------------------------------------------------------------
    //!  Reads the length from @c is.  Returns @c is.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Writes the length to @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f);

    //------------------------------------------------------------------------
    //!  Writes the length to @c fd.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c fd.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);
    
    //------------------------------------------------------------------------
    //!  Writes the length to @c bzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c bzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);
    
    //------------------------------------------------------------------------
    //!  Writes the length to @c gzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c gzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);
    
    //------------------------------------------------------------------------
    //!  Returns the size of the type we'll need to use when writing the
    //!  length, in bytes (1, 2, 4 or 8).
    //------------------------------------------------------------------------
    uint8_t SizeFromLength() const;

  private:
    uint64_t  _length;
  };
  
}  // namespace Dwm

#endif  // _DWMSIZEDLENGTH_HH_
