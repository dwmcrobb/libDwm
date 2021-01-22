//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmTimeInterval64.hh 11087 $
// @(#) $Id: DwmTimeInterval64.hh 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2016, 2017, 2020
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
//!  \file DwmTimeInterval64.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::TimeInterval64 class definition
//---------------------------------------------------------------------------

#ifndef _DWMTIMEINTERVAL64_HH_
#define _DWMTIMEINTERVAL64_HH_

extern "C" {
  #include <assert.h>
}

#include <utility>

#include "DwmTimeInterval.hh"
#include "DwmTimeValue64.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmFileIOCapable.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmBZ2IOCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates a time interval.
  //--------------------------------------------------------------------------
  class TimeInterval64
    : public DescriptorIOCapable, public FileIOCapable, public StreamIOCapable,
      public StreamedLengthCapable, public GZIOCapable, public BZ2IOCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Contructor
    //------------------------------------------------------------------------
    TimeInterval64()
        : _data()
    {}
    
    //------------------------------------------------------------------------
    //!  Construct from a start and end time.
    //------------------------------------------------------------------------
    TimeInterval64(const TimeValue64 & start, const TimeValue64 & end)
        : _data(start, end)
    {
      assert(end >= start);
    }
    
    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    TimeInterval64(const TimeInterval64 & timeInterval)
        : _data(timeInterval._data)
    {}

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    TimeInterval64(const TimeInterval & timeInterval)
    {
      _data.first = timeInterval.Start();
      _data.second = timeInterval.End();
    }

    //------------------------------------------------------------------------
    //!  Returns the start time.
    //------------------------------------------------------------------------
    const TimeValue64 & Start() const
    {
      return(_data.first);
    }

    //------------------------------------------------------------------------
    //!  Sets and returns the start time.
    //------------------------------------------------------------------------
    const TimeValue64 & Start(const TimeValue64 & start);
    
    //------------------------------------------------------------------------
    //!  Returns the end time.
    //------------------------------------------------------------------------
    const TimeValue64 & End() const
    {
      return(_data.second);
    }

    //------------------------------------------------------------------------
    //!  Sets and returns the end time.
    //------------------------------------------------------------------------
    const TimeValue64 & End(const TimeValue64 & end);
    
    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const TimeInterval64 & ti) const;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    TimeValue64 Duration() const
    {
      return (_data.second - _data.first);
    }
    
    //------------------------------------------------------------------------
    //!  Returns true if the TimeValue64 \c tv falls within the time
    //!  interval, i.e. \tv is greater than or equal to the start and
    //!  less than or equal to the end.
    //------------------------------------------------------------------------
    bool Contains(const TimeValue64 & tv) const;

    //------------------------------------------------------------------------
    //!  Returns true if the TimeInterval \c ti is completely contained
    //!  within our time interval.
    //------------------------------------------------------------------------
    bool Contains(const TimeInterval64 & ti) const;
    
    //------------------------------------------------------------------------
    //!  Returns true if \c ti overlaps the time interval.
    //------------------------------------------------------------------------
    bool Overlaps(const TimeInterval64 & ti) const;
    
    //------------------------------------------------------------------------
    //!  Returns true if \c ti is immediately adjacent our time interval,
    //!  i.e. our start is equal to \c ti.End() or our end is equal to
    //!  \c ti.Start()
    //------------------------------------------------------------------------
    bool Adjacent(const TimeInterval64 & ti) const;
    
    //------------------------------------------------------------------------
    //!  Returns the length of the time insterval, in bytes.  This is
    //!  used for some I/O functions.
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override;

    //------------------------------------------------------------------------
    //!  Reads the time interval from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override;

    //------------------------------------------------------------------------
    //!  Writes the time interval to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f) override;

    //------------------------------------------------------------------------
    //!  Writes the time interval to a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override;

    //------------------------------------------------------------------------
    //!  Wries the time interval to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf) override;

    //------------------------------------------------------------------------
    //!  Writes the time interval to a gzFile.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf) override;

    //------------------------------------------------------------------------
    //!  Writes the time interval to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
  private:
    std::pair<TimeValue64, TimeValue64>  _data;
  };
  
}  // namespace Dwm

#endif  // _DWMTIMEINTERVAL64_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
