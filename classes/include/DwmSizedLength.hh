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
//!  \brief Dwm::SizedLength class declaration
//---------------------------------------------------------------------------

#ifndef _DWMSIZEDLENGTH_HH_
#define _DWMSIZEDLENGTH_HH_

extern "C" {
  #include <sys/types.h>
  #include <bzlib.h>
  #include <zlib.h>
}

#include <cstdint>
#include <cstdio>
#include <vector>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A simple class to encode a length (up to 64 bits) in a variable-sized
  //!  format on the wire.  On the wire, a single 'size' byte precedes the
  //!  value, indicating the on-the-wire length of the value (1 to 8 bytes).
  //!  We only use the lower 3 bits of the 'size' byte, since we can map
  //!  [0-7] -> [1-8] (there's no such thing as a length field of 0 bytes).
  //!  This leaves us 5 bits for any future usage.
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
    inline std::ostream & Write(std::ostream & os) const
    { return NWrite(os); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & NWrite(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c is.  Returns @c is.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Reads the length from @c is.  Returns @c is.
    //------------------------------------------------------------------------
    inline std::istream & NRead(std::istream & is)
    { return Read(is); }
    
    //------------------------------------------------------------------------
    //!  Writes the length to @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    inline size_t Write(FILE *f) const
    { return NWrite(f); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    size_t NWrite(FILE *f) const;
    
    //------------------------------------------------------------------------
    //!  Reads the length from @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline size_t NRead(FILE *f)
    { return Read(f); }
    
    //------------------------------------------------------------------------
    //!  Writes the length to @c fd.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline ssize_t Write(int fd) const
    { return NWrite(fd); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t NWrite(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c fd.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline ssize_t NRead(int fd)
    { return Read(fd); }
    
    //------------------------------------------------------------------------
    //!  Writes the length to @c bzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline int BZWrite(BZFILE *bzf) const
    { return NBZWrite(bzf); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int NBZWrite(BZFILE *bzf) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c bzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int NBZRead(BZFILE *bzf)
    { return BZRead(bzf); }

    //------------------------------------------------------------------------
    //!  Writes the length to @c gzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline int Write(gzFile gzf) const
    { return NWrite(gzf); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int NWrite(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads the length from @c gzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int NRead(gzFile gzf)
    { return Read(gzf); }
    
    //------------------------------------------------------------------------
    //!  Returns the size of the type we'll need to use when writing the
    //!  length, in bytes (1, 2, 4 or 8).
    //------------------------------------------------------------------------
    uint8_t SizeFromLength() const;

  private:
    uint64_t  _length;

    std::vector<uint8_t> MakeWriteVector() const;
    std::vector<uint8_t> MakeNativeEndianWriteVector() const;
    std::istream & Read(std::istream & is, bool bigEndianOnWire);
    size_t Read(FILE *f, bool bigEndianOnWire);
    ssize_t Read(int fd, bool bigEndianOnWire);
    int BZRead(BZFILE *bzf, bool bigEndianOnWire);
    int Read(gzFile gzf, bool bigEndianOnWire);

    struct TwoBytesProcessed
    {
      TwoBytesProcessed(const uint8_t buf[2], uint64_t *value);

      std::endian  encoding;
      uint8_t      sz;
      caddr_t      sp;
    };

  };
  
}  // namespace Dwm

#endif  // _DWMSIZEDLENGTH_HH_
