//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2016, 2017, 2020
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmTimeValue64.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::TimeValue64 class definition
//---------------------------------------------------------------------------

#ifndef _DWMTIMEVALUE64_HH_
#define _DWMTIMEVALUE64_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/time.h>
#if ((! __APPLE__) && (! __linux__))
  #include <sys/timespec.h>
#endif
  #include <sys/uio.h>
  #include <unistd.h>
  #include <stdio.h>
  #include <time.h>
}

#include <iostream>

#include "DwmTimeValue.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmBZ2IOCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates a time value beyond the UNIX epoch, with
  //!  microsecond granularity.
  //--------------------------------------------------------------------------
  class TimeValue64
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Initializes to 0 (the UNIX epoch) if setNow is
    //!  false, else initializaes to the current time.
    //------------------------------------------------------------------------
    TimeValue64(bool setNow = false);

    //------------------------------------------------------------------------
    //!  Construct from a struct timeval.
    //------------------------------------------------------------------------
    TimeValue64(const struct timeval & tv);

    //------------------------------------------------------------------------
    //!  Construct from a struct timespec.
    //------------------------------------------------------------------------
    TimeValue64(const struct timespec & ts);

    //------------------------------------------------------------------------
    //!  Construct from a TimeValue.
    //------------------------------------------------------------------------
    TimeValue64(const TimeValue & tv);
    
    //------------------------------------------------------------------------
    //!  Construct from the given \c secs and \c usecs.
    //------------------------------------------------------------------------
    TimeValue64(uint64_t secs, uint32_t usecs);

    //------------------------------------------------------------------------
    //!  Sets the TimeValue64 to the current time.
    //------------------------------------------------------------------------
    void SetNow();

    //------------------------------------------------------------------------
    //!  Sets the TimeValue64 from the given struct timeval \c tv.
    //------------------------------------------------------------------------
    void Set(const struct timeval & tv);
    
    //------------------------------------------------------------------------
    //!  Sets the TimeValue64 from the given \c secs and \c usecs.
    //------------------------------------------------------------------------
    void Set(uint64_t secs, uint32_t usecs);
    
    //------------------------------------------------------------------------
    //!  Returns the seconds.
    //------------------------------------------------------------------------
    uint64_t Secs() const;

    //------------------------------------------------------------------------
    //!  Returns the residual microseconds.
    //------------------------------------------------------------------------
    uint32_t Usecs() const;
    
    struct tm LocalTime() const;

    //------------------------------------------------------------------------
    //!  Less-than operator.
    //------------------------------------------------------------------------
    bool operator < (const TimeValue64 & tv) const;

    //------------------------------------------------------------------------
    //!  operator <=
    //------------------------------------------------------------------------
    bool operator <= (const TimeValue64 & tv) const;
    
    //------------------------------------------------------------------------
    //!  Greater-than operator.
    //------------------------------------------------------------------------
    bool operator > (const TimeValue64 & tv) const;

    //------------------------------------------------------------------------
    //!  operator >=
    //------------------------------------------------------------------------
    bool operator >= (const TimeValue64 & tv) const;

    //------------------------------------------------------------------------
    //!  Equal-to operator.
    //------------------------------------------------------------------------
    bool operator == (const TimeValue64 & tv) const;

    TimeValue64 & operator += (const TimeValue64 & tv);
    TimeValue64 & operator -= (const TimeValue64 & tv);
    TimeValue64 & operator *= (uint32_t x);
    TimeValue64 & operator /= (uint32_t x);

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if one of the
    //!  Write() members were called.
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override;
    
    //------------------------------------------------------------------------
    //!  Reads the TimeValue64 from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes the TimeValue64 to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the TimeValue64 from a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes the TimeValue64 to a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Reads the TimeValue64 from a file descriptor.  Returns the number of
    //!  bytes read on success (8), -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Writes the TimeValue64 to a file descriptor.  Returns the number of
    //!  bytes written on success (8), -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the TimeValue64 from a gzFile.  Returns the number of
    //!  bytes read on success (8), -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;

    //------------------------------------------------------------------------
    //!  Writes the TimeValue64 to a gzFile.  Returns the number of
    //!  bytes written on success (8), -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the TimeValue64 from a BZFILE pointer.  Returns the number of
    //!  bytes read on success (8), -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;

    //------------------------------------------------------------------------
    //!  Writes the TimeValue64 to a BZFILE pointer.  Returns the number of
    //!  bytes written on success (8), -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;

    //--------------------------------------------------------------------
    //!  Returns value as a double (useful for printing).
    //--------------------------------------------------------------------
    operator double () const;
    
  private:
    uint64_t  _secs;
    uint32_t  _usecs;
  };
  
}  // namespace Dwm

Dwm::TimeValue64 operator + (const Dwm::TimeValue64 & tv1,
                           const Dwm::TimeValue64 & tv2);
Dwm::TimeValue64 operator - (const Dwm::TimeValue64 & tv1,
                           const Dwm::TimeValue64 & tv2);

#endif  // _DWMTIMEVALUE64_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
