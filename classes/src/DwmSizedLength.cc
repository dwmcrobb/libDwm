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

#include <cassert>

#include "DwmSizedLength.hh"

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
  //!  Array of number of bytes we need to represent a given value.
  //--------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------
  //!  Returns the number of bytes we need to represent our _length field
  //!  on the wire.
  //--------------------------------------------------------------------------
  uint8_t SizedLength::SizeFromLength() const
  {
    uint8_t  rc = 8;
    auto it = std::find_if(sk_sizeThresholds.cbegin(),
                           sk_sizeThresholds.cend(),
                           [this]
                           (const std::pair<uint64_t,uint8_t> & thresh)
                           { return (_length <= thresh.first); });
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
  //!  Translate a size (1 to 8) to the on-the-wire representation.  Without
  //!  the sanity checks and masking, this is just s-1.
  //--------------------------------------------------------------------------
  static uint8_t SizeToWire(uint8_t s)
  {
    assert((s <= (k_sizeMask + 1)) && (s > 0));
    if ((s <= (k_sizeMask + 1)) && (s > 0)) {
      return (s - 1) & k_sizeMask;
    }
    return 0;
  }
  
  //--------------------------------------------------------------------------
  //!  Translate the on-the-wire size encoding to a size.
  //--------------------------------------------------------------------------
  static uint8_t SizeFromWire(uint8_t s)
  {
    return (s & k_sizeMask) + 1;
  };
  
  //--------------------------------------------------------------------------
  //  We write the size field and the first MSB of our data in one shot
  //  since our data must be at least one byte.  We then write the
  //  rmeaining bytes of our data (if any).
  //--------------------------------------------------------------------------
  std::ostream & SizedLength::Write(std::ostream & os) const
  {
    if (os) {
      auto  buf = MakeWriteVector();
      os.write((caddr_t)(buf.data()), buf.size());
    }
    return os;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & SizedLength::Read(std::istream & is)
  {
    _length = 0;
    uint8_t  buf[2];
    if (is.read((caddr_t)buf, sizeof(buf))) {
      uint8_t  sz = SizeFromWire(buf[0]);
      caddr_t  p = ((caddr_t)(&_length)) + sizeof(_length) - sz;
      *p = buf[1];
      if ((1 < sz) && (sizeof(_length) >= sz)) {
        is.read(++p, sz - 1);
      }
    }
    if (is) {
      _length = BE2Host(_length);
    }
    return is;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t SizedLength::Write(FILE *f) const
  {
    ssize_t  rc = 0;
    if (f) {
      auto  buf = MakeWriteVector();
      rc = fwrite(buf.data(), buf.size(), 1, f);
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t SizedLength::Read(FILE *f)
  {
    size_t  rc = 0;
    _length = 0;
    uint8_t  buf[2];
    if (fread((caddr_t)buf, sizeof(buf), 1, f)) {
      uint8_t  sz = SizeFromWire(buf[0]);
      caddr_t  p = ((caddr_t)(&_length)) + sizeof(_length) - sz;
      *p = buf[1];
      if ((1 < sz) && (sizeof(_length) >= sz)) {
        rc = fread(++p, sz - 1, 1, f);
      }
      else {
        rc = 1;
      }
    }
    if (rc) {
      _length = BE2Host(_length);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t SizedLength::Write(int fd) const
  {
    ssize_t  rc = -1;
    if (0 <= fd) {
      const uint8_t   sz = SizeFromLength();
      const uint64_t  val = Host2BE(_length);
      const caddr_t   p = ((caddr_t)(&val)) + sizeof(val) - sz;
      const uint8_t   wsz = SizeToWire(sz);

      // cheaper than building a vector: no heap allocation
      const struct iovec  iovs[2] = { { (void *)&wsz, 1 }, { p, sz } };
      if (::writev(fd, iovs, 2) == (sz + 1)) {
        rc = sz + 1;
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t SizedLength::Read(int fd)
  {
    ssize_t  rc = -1;
    _length = 0;
    if (0 <= fd) {
      uint8_t  buf[2];
      if (::read(fd, buf, sizeof(buf)) == sizeof(buf)) {
        rc = sizeof(buf);
        uint8_t  sz = SizeFromWire(buf[0]);
        caddr_t  p = ((caddr_t)(&_length)) + sizeof(_length) - sz;
        *p = buf[1];
        if ((1 < sz) && (sizeof(_length) >= sz)) {
          if (::read(fd, ++p, sz - 1) == (sz - 1)) {
            rc += (sz - 1);
          }
          else {
            rc = -1;
          }
        }
      }
    }
    if (rc > 0) {
      _length = BE2Host(_length);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::vector<uint8_t> SizedLength::MakeWriteVector() const
  {
    std::vector<uint8_t>  vec;

    //  The maximum length we need is 9 bytes (1 for size, 8 for data)
    vec.reserve(9);

    //  First, add the encoded size byte.
    uint8_t   sz = SizeFromLength();
    vec.push_back(SizeToWire(sz));
    
    //  Then our data.  Note that val is in big endian order, hence
    //  the setting of the pointer to the offset where the first
    //  MSB is located.
    uint64_t  val = Host2BE(_length);
    caddr_t   p = ((caddr_t)(&val)) + sizeof(val) - sz;                       
    for (size_t i = 0; i < sz; ++i) {
      vec.push_back(*p++);
    }                                                                         
    return vec;
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SizedLength::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      auto  buf = MakeWriteVector();
      if (BZ2_bzwrite(bzf, buf.data(), buf.size()) == buf.size()) {
        rc = buf.size();
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  int SizedLength::BZRead(BZFILE *bzf)
  {
    int  rc = -1;
    _length = 0;
    if (bzf) {
      uint8_t  buf[2];
      if (BZ2_bzread(bzf, buf, sizeof(buf)) == sizeof(buf)) {
        rc = sizeof(buf);
        uint8_t  sz = SizeFromWire(buf[0]);
        caddr_t  p = ((caddr_t)(&_length)) + sizeof(_length) - sz;
        *p = buf[1];
        if ((1 < sz) && (sizeof(_length) >= sz)) {
          if (BZ2_bzread(bzf, ++p, sz - 1) == (sz - 1)) {
            rc += (sz - 1);
          }
          else {
            rc = -1;
          }
        }
      }
    }
    if (rc > 0) {
      _length = BE2Host(_length);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  int SizedLength::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      auto  buf = MakeWriteVector();
      if (gzwrite(gzf, buf.data(), buf.size()) == buf.size()) {
        rc = buf.size();
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SizedLength::Read(gzFile gzf)
  {
    int  rc = -1;
    _length = 0;
    if (gzf) {
      uint8_t  buf[2];
      if (gzread(gzf, buf, sizeof(buf)) == sizeof(buf)) {
        rc = sizeof(buf);
        uint8_t  sz = SizeFromWire(buf[0]);
        caddr_t  p = ((caddr_t)(&_length)) + sizeof(_length) - sz;
        *p = buf[1];
        if ((1 < sz) && (sizeof(_length) >= sz)) {
          if (gzread(gzf, ++p, sz - 1) == (sz - 1)) {
            rc += (sz - 1);
          }
          else {
            rc = -1;
          }
        }
      }
    }
    if (rc > 0) {
      _length = BE2Host(_length);
    }
    return rc;
  }

}  // namespace Dwm
