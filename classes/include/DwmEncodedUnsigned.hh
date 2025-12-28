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
//!  \file DwmEncodedUnsigned.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMENCODEDUNSIGNED_HH_
#define _DWMENCODEDUNSIGNED_HH_

extern "C" {
  #include <sys/types.h>
  #include <unistd.h>
  #include <bzlib.h>
  #include <zlib.h>
}

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>

#include "DwmConcepts.hh"
#include "DwmEndianness.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A class template for a uint16_t, uint32_t or uint64_t value that will
  //!  be written in an encoded form that is portable (endianness handled
  //!  automatically) and more space efficient than transmitting the value
  //!  type whenever the value is at least 8 bits smaller than what can be
  //!  held by the value type.  Particularly useful when dealing with
  //!  std::size_t values (8 bytes on 64-bit systems) used by various
  //!  size() and other members of templates in the standard library.  This
  //!  is especially true for transmitting the number of elements in a
  //!  container.  For example, it's rare to create a std::vector of more
  //!  than 2^24 elements, so I shouldn't always have to send an 8-byte
  //!  value as its size.
  //!  For uint16_t, there isn't much advantage here other than the
  //!  automatic endian conversion which only happens on the reader side
  //!  and only when necessary.
  //--------------------------------------------------------------------------
  template <typename T>
  requires IsEndianSensitiveInteger<T> and std::is_unsigned_v<T>
  class EncodedUnsigned
  {
  public:
    EncodedUnsigned() = default;
    EncodedUnsigned(const EncodedUnsigned &) = default;
    EncodedUnsigned & operator = (const EncodedUnsigned &) = default;
    EncodedUnsigned(EncodedUnsigned &&) = default;
    EncodedUnsigned & operator = (EncodedUnsigned &&) = default;
    ~EncodedUnsigned() = default;

    //------------------------------------------------------------------------
    //!  Construct from the given @c value.
    //------------------------------------------------------------------------
    EncodedUnsigned(T value)
        : _value(value), _chunkedData(false)
    {}

    //------------------------------------------------------------------------
    //!  Assign from the given @c value.
    //------------------------------------------------------------------------
    EncodedUnsigned & operator = (T value)
    { _value = value; return *this; }
    
    //------------------------------------------------------------------------
    //!  Return the value as a T value.
    //------------------------------------------------------------------------
    operator T () const
    { return _value; }

    //------------------------------------------------------------------------
    //!  Writes the value to @c os.  Returns @c os.
    //------------------------------------------------------------------------
    inline std::ostream & Write(std::ostream & os) const
    { return NWrite(os); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & NWrite(std::ostream & os) const
    {
      auto  buf = MakeNativeEndianWriteVector();
      os.write((caddr_t)(buf.data()), buf.size());
      return os;
    }

    //------------------------------------------------------------------------
    //!  Reads the value from @c is.  Returns @c is.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is)
    {
      _value = 0;
      uint8_t  buf[2];
      if (is.read((caddr_t)buf, sizeof(buf))) {
        TwoBytesProcessed  twoBP(buf, &_value);
        if (1 < twoBP.sz) {
          is.read(twoBP.sp, twoBP.sz - 1);
        }
        if (is) {
          if (std::endian::native != twoBP.encoding) {
            _value = Bswap<T>(_value);
          }
        }
      }
      return is;
    }

    //------------------------------------------------------------------------
    //!  Reads the value from @c is.  Returns @c is.
    //------------------------------------------------------------------------
    inline std::istream & NRead(std::istream & is)
    { return Read(is); }
    
    //------------------------------------------------------------------------
    //!  Writes the value to @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    inline size_t Write(FILE *f) const
    { return NWrite(f); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    size_t NWrite(FILE *f) const
    {
      size_t  rc = 0;
      if (f) {
        auto  buf = MakeNativeEndianWriteVector();
        rc = fwrite((caddr_t)(buf.data()), buf.size(), 1, f);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Reads the value from @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f)
    {
      _value = 0;
      size_t       rc = 0;
      uint8_t      buf[2];
      if (fread((caddr_t)buf, sizeof(buf), 1, f)) {
        TwoBytesProcessed  twoBP(buf, &_value);
        if (1 < twoBP.sz) {
          rc = fread(twoBP.sp, twoBP.sz - 1, 1, f);
        }
        else {
          rc = 1;
        }
        if (rc) {
          if (std::endian::native != twoBP.encoding) {
            _value = Bswap<T>(_value);
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline size_t NRead(FILE *f)
    { return Read(f); }
    
    //------------------------------------------------------------------------
    //!  Writes the value to @c fd.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline ssize_t Write(int fd) const
    { return NWrite(fd); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t NWrite(int fd) const
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

    //------------------------------------------------------------------------
    //!  Reads the value from @c fd.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd)
    {
      _value = 0;
      ssize_t  rc = -1;
      if (0 <= fd) {
        uint8_t  buf[2];
        if (::read(fd, buf, sizeof(buf)) == sizeof(buf)) {
          rc = sizeof(buf);
          TwoBytesProcessed  twoBP(buf, &_value);
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
              _value = Bswap<T>(_value);
            }
          }
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline ssize_t NRead(int fd)
    { return Read(fd); }
    
    //------------------------------------------------------------------------
    //!  Writes the value to @c bzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline int BZWrite(BZFILE *bzf) const
    { return NBZWrite(bzf); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int NBZWrite(BZFILE *bzf) const
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

    //------------------------------------------------------------------------
    //!  Reads the value from @c bzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf)
    {
      _value = 0;
      int   rc = -1;
      if (bzf) {
        uint8_t  buf[2];
        if (BZ2_bzread(bzf, buf, sizeof(buf)) == sizeof(buf)) {
          rc = sizeof(buf);
          TwoBytesProcessed  twoBP(buf, &_value);
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
              _value = Bswap<T>(_value);
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int NBZRead(BZFILE *bzf)
    { return BZRead(bzf); }

    //------------------------------------------------------------------------
    //!  Writes the value to @c gzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    inline int Write(gzFile gzf) const
    { return NWrite(gzf); }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int NWrite(gzFile gzf) const
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

    //------------------------------------------------------------------------
    //!  Reads the value from @c gzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf)
    {
      _value = 0;
      int          rc = -1;
      if (gzf) {
        uint8_t  buf[2];
        if (gzread(gzf, buf, sizeof(buf)) == sizeof(buf)) {
          rc = sizeof(buf);
          TwoBytesProcessed  twoBP(buf, &_value);
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
              _value = Bswap<T>(_value);
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int NRead(gzFile gzf)
    { return Read(gzf); }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes of value we'll write when writing the
    //!  current value.
    //------------------------------------------------------------------------
    uint8_t SizeFromValue() const
    {
      //----------------------------------------------------------------------
      //!  Array of number of bytes we need to represent a given value.
      //----------------------------------------------------------------------
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
      
      uint8_t  rc = sizeof(_value);
      auto it = std::find_if(sk_sizeThresholds.cbegin(),
                             sk_sizeThresholds.cend(),
                             [this]
                             (const auto & thresh)
                             { return (this->_value <= thresh.first); });
      if (it != sk_sizeThresholds.cend()) {
        if (sizeof(_value) >= it->second) {   // I shouldn't need this check.
          rc = it->second;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const
    {
      return (1 + SizeFromValue());  // encoding byte plus value data
    }
    
  private:
    T     _value;
    bool  _chunkedData;
    
    //------------------------------------------------------------------------
    //!  We only use the lower bits of the first byte of our encoding to
    //!  represent our 'size' field.
    //------------------------------------------------------------------------
    static constexpr uint8_t  k_sizeMask = sizeof(T) - 1;
    
    //------------------------------------------------------------------------
    //!  Bit used to indicate endianness.
    //------------------------------------------------------------------------
    static constexpr uint8_t  k_bigEndianMask = 0x80;

    static constexpr uint8_t  k_chunkedDataMask = 0x40;
    
    //------------------------------------------------------------------------
    //!  Unused bits mask.
    //------------------------------------------------------------------------
    static constexpr uint8_t  k_unusedMask =
      ~(k_sizeMask|k_bigEndianMask|k_chunkedDataMask);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    struct TwoBytesProcessed
    {
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      TwoBytesProcessed(const uint8_t buf[2], T *value)
      {
        static constexpr std::endian  encodings[2] = {
          std::endian::little,
          std::endian::big
        };
        encoding = encodings[buf[0] >> 7];
        chunkedData = (buf[0] & k_chunkedDataMask) ? true : false;
        sz       = (buf[0] & k_sizeMask) + 1;
        sp       = (caddr_t)value;
        if (std::endian::big == encoding) {
          sp += sizeof(*value) - sz;
        }
        *sp++    = buf[1];
      }
      
      caddr_t      sp;
      std::endian  encoding;
      bool         chunkedData;
      uint8_t      sz;
    };

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::vector<uint8_t> MakeNativeEndianWriteVector() const
    {
      std::vector<uint8_t>  vec;
      
      //  The maximum length we need: 1 byte for size, 1 for data
      vec.reserve(1 + sizeof(_value));
      
      //  First, add the encoded size and endian byte.
      uint8_t   sz = SizeFromValue();
      vec.push_back(EncodedStartByte(sz));
      
      //  Then our data.
      caddr_t  p = (caddr_t)(&_value);
      if constexpr (std::endian::native == std::endian::big) {
        p += sizeof(_value) - sz;
      }
      for (size_t i = 0; i < sz; ++i) {
        vec.push_back(*p++);
      }
      return vec;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static inline uint8_t EncodedStartByte(uint8_t sz)
    {
      sz = SizeToWire(sz);
      if constexpr (std::endian::native == std::endian::big) {
        sz |= k_bigEndianMask;
      }
      return sz;
    }

    //------------------------------------------------------------------------
    //!  Translate a size (1 to sizeof(T)) to the on-the-wire representation.
    //!  Without the sanity checks and masking, this is just s-1.
    //------------------------------------------------------------------------
    static inline uint8_t SizeToWire(uint8_t s)
    {
      assert((s <= (k_sizeMask + 1)) && (s > 0));
      if ((s <= (k_sizeMask + 1)) && (s > 0)) {
        return (s - 1) & k_sizeMask;
      }
      return 0;
    }
    
  };

  using EncodedU64 = EncodedUnsigned<uint64_t>;
  using EncodedU32 = EncodedUnsigned<uint32_t>;
  using EncodedU16 = EncodedUnsigned<uint16_t>;
  
}  // namespace Dwm

#endif  // _DWMENCODEDUNSIGNED_HH_
