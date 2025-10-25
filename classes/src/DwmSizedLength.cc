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
//!  \file DwmSizedLength.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::SizedLength class implementation
//---------------------------------------------------------------------------

#include <bit>
#include <cassert>

#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmBZ2IO.hh"
#include "DwmGZIO.hh"

namespace Dwm {

  static_assert(IsStreamReadable<SizedLength>);
  static_assert(IsStreamWritable<SizedLength>);
  static_assert(IsFileReadable<SizedLength>);
  static_assert(IsFileWritable<SizedLength>);
  static_assert(IsDescriptorReadable<SizedLength>);
  static_assert(IsDescriptorWritable<SizedLength>);
  static_assert(IsGZReadable<SizedLength>);
  static_assert(IsGZWritable<SizedLength>);
  static_assert(IsBZ2Readable<SizedLength>);
  static_assert(IsBZ2Writable<SizedLength>);

  //--------------------------------------------------------------------------
  //!  Returns the number of bytes we need to represent our _length field
  //!  on the wire.
  //--------------------------------------------------------------------------
  uint8_t SizedLength::SizeFromLength() const
  {
    //------------------------------------------------------------------------
    //!  Array of number of bytes we need to represent a given value.
    //------------------------------------------------------------------------
    static constexpr std::array<std::pair<uint64_t,uint8_t>,7>
      sk_sizeThresholds {{
        { 0xFF,             1 },
        { 0xFFFF,           2 },
        { 0xFFFFFF,         3 },
        { 0xFFFFFFFF,       4 },
        { 0xFFFFFFFFFF,     5 },
        { 0xFFFFFFFFFFFF,   6 },
        { 0xFFFFFFFFFFFFFF, 7 }
      }};
    
    uint8_t  rc = 8;
    auto it = std::find_if(sk_sizeThresholds.cbegin(),
                           sk_sizeThresholds.cend(),
                           [this]
                           (const std::pair<uint64_t,uint8_t> & thresh)
                           { return (this->_length <= thresh.first); });
    if (it != sk_sizeThresholds.cend()) {
      rc = it->second;
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  We only use the lower 3 bits of the first byte of our encoding to
  //!  represent our 'size' field.
  //--------------------------------------------------------------------------
  static constexpr uint8_t  k_sizeMask = 0x07;

  //--------------------------------------------------------------------------
  //!  Bit used to indicate endianness.
  //--------------------------------------------------------------------------
  static constexpr uint8_t  k_bigEndianMask = 0x80;
  
  //--------------------------------------------------------------------------
  //!  Translate a size (1 to 8) to the on-the-wire representation.  Without
  //!  the sanity checks and masking, this is just s-1.
  //--------------------------------------------------------------------------
  static inline uint8_t SizeToWire(uint8_t s)
  {
    assert((s <= (k_sizeMask + 1)) && (s > 0));
    if ((s <= (k_sizeMask + 1)) && (s > 0)) {
      return (s - 1) & k_sizeMask;
    }
    return 0;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static inline uint8_t EncodedStartByte(uint8_t sz)
  {
    sz = SizeToWire(sz);
    if constexpr (std::endian::native == std::endian::big) {
      sz |= k_bigEndianMask;
    }
    return sz;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::vector<uint8_t> SizedLength::MakeNativeEndianWriteVector() const
  {
    std::vector<uint8_t>  vec;

    //  The maximum length we need is 9 bytes (1 for size, 8 for data)
    vec.reserve(9);

    //  First, add the encoded size and endian byte.
    uint8_t   sz = SizeFromLength();
    vec.push_back(EncodedStartByte(sz));
    
    //  Then our data.
    caddr_t  p = (caddr_t)(&_length);
    if constexpr (std::endian::native == std::endian::big) {
      p += sizeof(_length) - sz;
    }
    for (size_t i = 0; i < sz; ++i) {
      vec.push_back(*p++);
    }
    return vec;
  }
  
  //--------------------------------------------------------------------------
  std::ostream & SizedLength::NWrite(std::ostream & os) const
  {
    auto  buf = MakeNativeEndianWriteVector();
    os.write((caddr_t)(buf.data()), buf.size());
    return os;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  SizedLength::TwoBytesProcessed::TwoBytesProcessed(const uint8_t buf[2],
                                                    uint64_t *value)
  {
    static constexpr std::endian  encodings[2] = {
      std::endian::little,
      std::endian::big
    };
    encoding = encodings[buf[0] >> 7];
    sz       = (buf[0] & k_sizeMask) + 1;
    sp       = (caddr_t)value;
    if (std::endian::big == encoding) {
      sp += sizeof(*value) - sz;
    }
    *sp++    = buf[1];
  }

  //--------------------------------------------------------------------------
  std::istream & SizedLength::Read(std::istream & is)
  {
    _length = 0;
    uint8_t  buf[2];
    if (is.read((caddr_t)buf, sizeof(buf))) {
      TwoBytesProcessed  twoBP(buf, &_length);
      if (1 < twoBP.sz) {
        is.read(twoBP.sp, twoBP.sz - 1);
      }
      if (is) {
        if (std::endian::native != twoBP.encoding) {
          _length = bswap64(_length);
        }
      }
    }
    return is;
  }

  //--------------------------------------------------------------------------
  size_t SizedLength::NWrite(FILE *f) const
  {
    size_t  rc = 0;
    if (f) {
      auto  buf = MakeNativeEndianWriteVector();
      rc = fwrite((caddr_t)(buf.data()), buf.size(), 1, f);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  size_t SizedLength::Read(FILE *f)
  {
    _length = 0;
    
    size_t       rc = 0;
    uint8_t      buf[2];
    if (fread((caddr_t)buf, sizeof(buf), 1, f)) {
      TwoBytesProcessed  twoBP(buf, &_length);
      if (1 < twoBP.sz) {
        rc = fread(twoBP.sp, twoBP.sz - 1, 1, f);
      }
      else {
        rc = 1;
      }
      if (rc) {
        if (std::endian::native != twoBP.encoding) {
          _length = bswap64(_length);
        }
      }
    }
    
    return rc;
  }
  
  //--------------------------------------------------------------------------
  ssize_t SizedLength::NWrite(int fd) const
  {
    ssize_t  rc = -1;
    if (0 <= fd) {
      auto  buf = MakeNativeEndianWriteVector();
      if (::write(fd, buf.data(), buf.size()) == buf.size()) {
        rc = buf.size();
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  ssize_t SizedLength::Read(int fd)
  {
    _length = 0;
    ssize_t  rc = -1;
    if (0 <= fd) {
      uint8_t  buf[2];
      if (::read(fd, buf, sizeof(buf)) == sizeof(buf)) {
        rc = sizeof(buf);
        TwoBytesProcessed  twoBP(buf, &_length);
        if (1 < twoBP.sz) {
          if (::read(fd, twoBP.sp, twoBP.sz - 1) == (twoBP.sz - 1)) {
            rc += twoBP.sz - 1;
          }
          else {
            rc = -1;
          }
        }
        if (rc > 0) {
          if (std::endian::native != twoBP.encoding) {
            _length = bswap64(_length);
          }
        }
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  int SizedLength::NBZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      auto  buf = MakeNativeEndianWriteVector();
      if (BZ2_bzwrite(bzf, buf.data(), buf.size()) == buf.size()) {
        rc = buf.size();
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SizedLength::BZRead(BZFILE *bzf)
  {
    _length = 0;
    int   rc = -1;
    if (bzf) {
      uint8_t  buf[2];
      if (BZ2_bzread(bzf, buf, sizeof(buf)) == sizeof(buf)) {
        rc = sizeof(buf);
        TwoBytesProcessed  twoBP(buf, &_length);
        if (1 < twoBP.sz) {
          if (BZ2_bzread(bzf, twoBP.sp, twoBP.sz - 1) == (twoBP.sz - 1)) {
            rc += (twoBP.sz - 1);
          }
          else {
            rc = -1;
          }
        }
        if (rc > 0) {
          if (std::endian::native != twoBP.encoding) {
            _length = BE2Host(_length);
          }
        }
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  int SizedLength::NWrite(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      auto  buf = MakeNativeEndianWriteVector();
      if (gzwrite(gzf, buf.data(), buf.size()) == buf.size()) {
        rc = buf.size();
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  int SizedLength::Read(gzFile gzf)
  {
    _length = 0;
    int          rc = -1;
    if (gzf) {
      uint8_t  buf[2];
      if (gzread(gzf, buf, sizeof(buf)) == sizeof(buf)) {
        rc = sizeof(buf);
        TwoBytesProcessed  twoBP(buf, &_length);
        if (1 < twoBP.sz) {
          if (gzread(gzf, twoBP.sp, twoBP.sz - 1) == (twoBP.sz - 1)) {
            rc += (twoBP.sz - 1);
          }
          else {
            rc = -1;
          }
        }
        if (rc > 0) {
          if (std::endian::native != twoBP.encoding) {
            _length = BE2Host(_length);
          }
        }
      }
    }
    return rc;
  }
  
}  // namespace Dwm
