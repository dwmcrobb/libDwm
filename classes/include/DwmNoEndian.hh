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
//!  \file DwmNoEndian.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMNOENDIAN_HH_
#define _DWMNOENDIAN_HH_

#include "DwmBZ2IO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmGZIO.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  requires IsEndianSensitiveInteger<T>
  class NoEndian
  {
  public:
    NoEndian() = default;
    NoEndian(const NoEndian &) = default;
    NoEndian & operator = (const NoEndian &) = default;
    NoEndian(NoEndian &&) = default;
    NoEndian & operator = (NoEndian &&) = default;
    ~NoEndian() = default;

    //------------------------------------------------------------------------
    //!  Construct from the given @c value.
    //------------------------------------------------------------------------
    NoEndian(T value)
        : _value(value)
    {}

    //------------------------------------------------------------------------
    //!  Assign from the given @c value.
    //------------------------------------------------------------------------
    NoEndian & operator = (T value)
    { _value = value; return *this; }
    
    //------------------------------------------------------------------------
    //!  Return the value.
    //------------------------------------------------------------------------
    operator T () const
    { return _value; }

    //------------------------------------------------------------------------
    //!  Writes the value to @c os.  Returns @c os.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const
    { return StreamIO::NWrite(os, _value); }
    
    //------------------------------------------------------------------------
    //!  Reads the value from @c is.  Returns @c is.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is)
    { return StreamIO::NRead(is, _value); }

    //------------------------------------------------------------------------
    //!  Writes the value to @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const
    {
      size_t  rc = 0;
      if (f) {
        if (fwrite((const void *)_value, sizeof(_value), 1, f)) {
          rc = 1;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads the value from @c f.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f)
    {
      size_t  rc = 0;
      if (f) {
        if (fread((void *)_value, sizeof(_value), 1, f)) {
          rc = 1;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes the value to @c fd.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const
    { return DescriptorIO::NWrite(fd, _value); }

    //------------------------------------------------------------------------
    //!  Reads the value from @c fd.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd)
    { return DescriptorIO::NRead(fd, _value); }
    
    //------------------------------------------------------------------------
    //!  Writes the value to @c bzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const
    {
      int  rc = -1;
      if (bzf) {
        if (BZ2_bzwrite(bzf, (void *)&_value, sizeof(_value))
            == sizeof(_value)) {
          rc = sizeof(_value);
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
      int  rc = -1;
      if (bzf) {
        if (BZ2_bzread(bzf, (void *)&_value, sizeof(_value))
            == sizeof(_value)) {
          rc = sizeof(_value);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes the value to @c gzf.  Returns the number of bytes written
    //!  on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const
    {
      int  rc = -1;
      if (gzf) {
        if (gzwrite(gzf, (const void *)&_value, sizeof(_value)
                    == sizeof(_value))) {
          rc = sizeof(_value);
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
      int  rc = -1;
      if (gzf) {
        if (gzread(gzf, (void *)&_value, sizeof(_value) == sizeof(_value))) {
          rc = sizeof(_value);
        }
      }
      return rc;
    }
    
  private:
    T  _value;
  };
  
}  // namespace Dwm

#endif  // _DWMNOENDIAN_HH_
