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
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmSizedLength.hh"
#include "DwmIOUtils.hh"

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
  template <typename T>
  class EncodedLength
  {
  public:
    EncodedLength(const SizedLength & sl)
        : _data(sizeof(T),(T)((uint64_t)sl))
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    EncodedLength()
        : _data(sizeof(T),0)
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const
    { return StreamIO::Write(os, _data); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::istream & ReadValue(std::istream & is, SizedLength & sl)
    {
      if (StreamIO::Read(is, _data.second)) {
        sl = (uint64_t)_data.second;
      }
      return is;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const
    { return FileIO::Write(f, _data); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    size_t ReadValue(FILE *f, SizedLength & sl)
    {
      size_t rc = FileIO::Read(f, _data.second);
      if (rc) {
        sl = (uint64_t)_data.second;
      }
      return rc;
    }
      
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const
    {
      return (DescriptorIO::Write(fd, _data) == _expectedBytes) ?
        _expectedBytes : -1;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t ReadValue(int fd, SizedLength & sl)
    {
      ssize_t  rc = DescriptorIO::Read(fd, _data.second);
      if (rc == sizeof(_data.second)) {
        sl = (uint64_t)_data.second;
        rc += sizeof(_data.first);
      }
      else {
        rc = -1;
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const
    {
      return (BZ2IO::BZWrite(bzf, _data) == _expectedBytes) ?
        _expectedBytes : -1;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int BZReadValue(BZFILE *bzf, SizedLength & sl)
    {
      int rc = BZ2IO::BZRead(bzf, _data.second);
      if (rc == sizeof(_data.second)) {
        sl = (uint64_t)_data.second;
        rc += sizeof(_data.first);
      }
      else {
        rc = -1;
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const
    {
      return (GZIO::Write(gzf, _data) == _expectedBytes) ?
        _expectedBytes : -1;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int ReadValue(gzFile gzf, SizedLength & sl)
    {
      int rc = GZIO::Read(gzf, _data.second);
      if (rc == sizeof(_data.second)) {
        sl = (uint64_t)_data.second;
        rc += sizeof(_data.first);
      }
      else {
        rc = -1;
      }
      return rc;
    }
    
  private:
    std::pair<uint8_t,T>  _data;
    static constexpr int  _expectedBytes = sizeof(T) + 1;
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static const  std::vector<std::pair<uint64_t,uint8_t>>  sk_sizeThresholds
  { 
    { 0xFFFFFFFF,  8 },
    { 0xFFFF,      4 },
    { 0xFF,        2 }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t SizedLength::SizeFromLength() const
  {
    uint8_t  rc = 1;
    auto it = std::find_if(sk_sizeThresholds.cbegin(),
                           sk_sizeThresholds.cend(),
                           [this]
                           (const std::pair<uint64_t,uint8_t> & thresh)
                           { return (_length > thresh.first); });
    if (it != sk_sizeThresholds.cend()) {
      rc = it->second;
    }
    return rc;
  }
      
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & SizedLength::Write(std::ostream & os) const
  {
    switch (SizeFromLength()) {
      case 1:   EncodedLength<uint8_t>(*this).Write(os);              break;
      case 2:   EncodedLength<uint16_t>(*this).Write(os);             break;
      case 4:   EncodedLength<uint32_t>(*this).Write(os);             break;
      case 8:   EncodedLength<uint64_t>(*this).Write(os);             break;
      default:  os.setstate(std::ios_base::failbit);  break;
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
    if (StreamIO::Read(is, sz)) {
      switch (sz) {
        case 1:   EncodedLength<uint8_t>().ReadValue(is, *this);      break;
        case 2:   EncodedLength<uint16_t>().ReadValue(is, *this);     break;
        case 4:   EncodedLength<uint32_t>().ReadValue(is, *this);     break;
        case 8:   EncodedLength<uint64_t>().ReadValue(is, *this);     break;
        default:  is.setstate(std::ios_base::failbit);  break;
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
      uint8_t  sz = SizeFromLength();
      switch (sz) {
        case 1:  rc = EncodedLength<uint8_t>(*this).Write(f);         break;
        case 2:  rc = EncodedLength<uint16_t>(*this).Write(f);        break;
        case 4:  rc = EncodedLength<uint32_t>(*this).Write(f);        break;
        case 8:  rc = EncodedLength<uint64_t>(*this).Write(f);        break;
        default:                                                      break;
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
    if (FileIO::Read(f, sz)) {
      switch (sz) {
        case 1:  rc = EncodedLength<uint8_t>().ReadValue(f, *this);   break;
        case 2:  rc = EncodedLength<uint16_t>().ReadValue(f, *this);  break;
        case 4:  rc = EncodedLength<uint32_t>().ReadValue(f, *this);  break;
        case 8:  rc = EncodedLength<uint64_t>().ReadValue(f, *this);  break;
        default:                                                      break;
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
      switch (sz) {
        case 1: rc = EncodedLength<uint8_t>(*this).Write(fd);         break;
        case 2: rc = EncodedLength<uint16_t>(*this).Write(fd);        break;
        case 4: rc = EncodedLength<uint32_t>(*this).Write(fd);        break;
        case 8: rc = EncodedLength<uint64_t>(*this).Write(fd);        break;
        default:                                                      break;
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
      if (DescriptorIO::Read(fd, sz) == sizeof(sz)) {
        rc = sizeof(sz);
        switch (sz) {
          case 1:  rc = EncodedLength<uint8_t>().ReadValue(fd, *this);  break;
          case 2:  rc = EncodedLength<uint16_t>().ReadValue(fd, *this); break;
          case 4:  rc = EncodedLength<uint32_t>().ReadValue(fd, *this); break;
          case 8:  rc = EncodedLength<uint64_t>().ReadValue(fd, *this); break;
          default: rc = -1;                                             break;
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
      switch (sz) {
        case 1: rc = EncodedLength<uint8_t>(*this).BZWrite(bzf);      break;
        case 2: rc = EncodedLength<uint16_t>(*this).BZWrite(bzf);     break;
        case 4: rc = EncodedLength<uint32_t>(*this).BZWrite(bzf);     break;
        case 8: rc = EncodedLength<uint64_t>(*this).BZWrite(bzf);     break;
        default:                                                      break;
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
      if (BZ2IO::BZRead(bzf, sz) == sizeof(sz)) {
        switch (sz) {
          case 1:
            rc = EncodedLength<uint8_t>(*this).BZReadValue(bzf, *this);
            break;
          case 2:
            rc = EncodedLength<uint16_t>(*this).BZReadValue(bzf, *this);
            break;
          case 4:
            rc = EncodedLength<uint32_t>(*this).BZReadValue(bzf, *this);
            break;
          case 8:
            rc = EncodedLength<uint64_t>(*this).BZReadValue(bzf, *this);
            break;
          default:
            rc = -1;
            break;
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
      switch (sz) {
        case 1: rc = EncodedLength<uint8_t>(*this).Write(gzf);        break;
        case 2: rc = EncodedLength<uint16_t>(*this).Write(gzf);       break;
        case 4: rc = EncodedLength<uint32_t>(*this).Write(gzf);       break;
        case 8: rc = EncodedLength<uint64_t>(*this).Write(gzf);       break;
        default:                                                      break;
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
      if (GZIO::Read(gzf, sz) == sizeof(sz)) {
        switch (sz) {
          case 1: rc = EncodedLength<uint8_t>().ReadValue(gzf, *this);  break;
          case 2: rc = EncodedLength<uint16_t>().ReadValue(gzf, *this); break;
          case 4: rc = EncodedLength<uint32_t>().ReadValue(gzf, *this); break;
          case 8: rc = EncodedLength<uint64_t>().ReadValue(gzf, *this); break;
          default: rc = -1;                                             break;
        }
      }
    }
    return rc;
  }

}  // namespace Dwm
