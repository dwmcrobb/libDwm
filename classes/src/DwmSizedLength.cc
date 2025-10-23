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
  //!  
  //--------------------------------------------------------------------------
  static const std::vector<std::pair<uint64_t,uint8_t>>  sk_sizeThresholds
  { 
    { 0xFF,             1 },
    { 0xFFFF,           2 },
    { 0xFFFFFF,         3 },
    { 0xFFFFFFFF,       4 },
    { 0xFFFFFFFFFF,     5 },
    { 0xFFFFFFFFFFFF,   6 },
    { 0xFFFFFFFFFFFFFF, 7 }
  };

  //--------------------------------------------------------------------------
  //!  
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
  //!  
  //--------------------------------------------------------------------------
  static constexpr uint8_t  k_sizeMask = 0x07;

  //--------------------------------------------------------------------------
  //!  
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
  //!  
  //--------------------------------------------------------------------------
  static uint8_t SizeFromWire(uint8_t s)
  {
    return (s & k_sizeMask) + 1;
  };
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & SizedLength::Write(std::ostream & os) const
  {
    uint8_t   sz = SizeFromLength();
    uint64_t  val = Host2BE(_length);
    uint8_t   wsz = SizeToWire(sz);
    if (os.write((caddr_t)&wsz, sizeof(wsz))) {
      os.write(((caddr_t)&val) + (sizeof(val) - sz), sz);
    }
    return os;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & SizedLength::Read(std::istream & is)
  {
    _length = 0;
    uint8_t  sz;
    if (is.read((caddr_t)&sz, sizeof(sz))) {
      sz = SizeFromWire(sz);
      if (sz) {
        if (sizeof(_length) >= sz) {
          if (is.read(((caddr_t)&_length) + (sizeof(_length) - sz), sz)) {
            _length = BE2Host(_length);
          }
        }
      }
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
      uint8_t   sz = SizeFromLength();
      uint8_t   wsz = SizeToWire(sz);
      uint64_t  val = Host2BE(_length);
      if (fwrite((caddr_t)&wsz, sizeof(wsz), 1, f)) {
        rc = fwrite(((caddr_t)&val) + (sizeof(val) - sz), sz, 1, f);
      }
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
    uint8_t  sz;
    if (fread((caddr_t)&sz,sizeof(sz), 1, f)) {
      sz = SizeFromWire(sz);
      uint64_t  val = 0;
      if (sizeof(val) >= sz) {
        rc = fread(((caddr_t)&val) + (sizeof(val) - sz), sz, 1, f);
        if (rc) {
          _length = BE2Host(val);
        }
      }
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
      uint8_t  sz = SizeFromLength();
      uint8_t  wsz = SizeToWire(sz);
      if (::write(fd, &wsz, sizeof(wsz)) == sizeof(wsz)) {
        rc = sizeof(wsz);
        uint64_t  val = Host2BE(_length);
        if (::write(fd, ((caddr_t)&val) + (sizeof(val) - sz), sz) == sz) {
          rc += sz;
        }
        else {
          rc = -1;
        }
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
      uint8_t  sz;
      if (::read(fd, &sz, sizeof(sz)) == sizeof(sz)) {
        sz = SizeFromWire(sz);
        if (sizeof(_length) >= sz) {
          rc = sizeof(sz);
          if (::read(fd, ((caddr_t)&_length)+(sizeof(_length)-sz), sz) == sz) {
            _length = BE2Host(_length);
            rc += sz;
          }
          else {
            rc = -1;
          }
        }
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int SizedLength::BZWrite(BZFILE *bzf) const
  {
    int  rc = -1;
    if (bzf) {
      uint8_t  sz = SizeFromLength();
      uint8_t  wsz = SizeToWire(sz);
      if (BZ2_bzwrite(bzf,(caddr_t)&wsz,sizeof(wsz)) == sizeof(wsz)) {
        rc = sizeof(wsz);
        uint64_t  val = Host2BE(_length);
        if (BZ2_bzwrite(bzf,((caddr_t)&val) + (sizeof(val)-sz), sz) == sz) {
          rc += sz;
        }
        else {
          rc = -1;
        }
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
      uint8_t  sz;
      if (BZ2_bzread(bzf, (caddr_t)&sz, sizeof(sz)) == sizeof(sz)) {
        sz = SizeFromWire(sz);
        if (sizeof(_length) >= sz) {
          rc = sizeof(sz);
          if (BZ2_bzread(bzf, ((caddr_t)&_length) + (sizeof(_length)-sz), sz)
              == sz) {
            _length = BE2Host(_length);
            rc += sz;
          }
          else {
            rc = -1;
          }
        }
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  int SizedLength::Write(gzFile gzf) const
  {
    int  rc = -1;
    if (gzf) {
      uint8_t  sz = SizeFromLength();
      uint8_t  wsz = SizeToWire(sz);
      if (gzwrite(gzf, (caddr_t)&wsz, sizeof(wsz)) == sizeof(wsz)) {
        rc = sizeof(wsz);
        uint64_t  val = Host2BE(_length);
        if (gzwrite(gzf, ((caddr_t)&val) + (sizeof(val)-sz), sz) == sz) {
          rc += sz;
        }
        else {
          rc = -1;
        }
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
      uint8_t  sz;
      if (gzread(gzf, &sz, sizeof(sz)) == sizeof(sz)) {
        sz = SizeFromWire(sz);
        if (sizeof(_length) >= sz) {
          rc = sizeof(sz);
          if (gzread(gzf, ((caddr_t)&_length) + (sizeof(_length)-sz), sz)
              == sz) {
            _length = BE2Host(_length);
            rc += sz;
          }
          else {
            rc = -1;
          }
        }
      }
    }
    return rc;
  }

}  // namespace Dwm
