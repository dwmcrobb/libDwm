//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2020, 2024
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
//!  \file DwmTimeInterval.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::TimeInterval class definition
//---------------------------------------------------------------------------

#ifndef _DWMTIMEINTERVAL_HH_
#define _DWMTIMEINTERVAL_HH_

#include <cassert>
#include <utility>

#include "DwmTimeValue.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class encapsulates a time interval.
  //--------------------------------------------------------------------------
  class TimeInterval
  {
  public:
    //------------------------------------------------------------------------
    //!  Contructor
    //------------------------------------------------------------------------
    TimeInterval()
        : _data()
    {}
    
    //------------------------------------------------------------------------
    //!  Construct from a start and end time.
    //------------------------------------------------------------------------
    TimeInterval(const TimeValue & start, const TimeValue & end)
        : _data(start, end)
    {
      assert(end >= start);
    }
    
    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    TimeInterval(const TimeInterval & timeInterval)
        : _data(timeInterval._data)
    {}

    //------------------------------------------------------------------------
    //!  Returns the start time.
    //------------------------------------------------------------------------
    const TimeValue & Start() const
    {
      return(_data.first);
    }

    //------------------------------------------------------------------------
    //!  Sets and returns the start time.
    //------------------------------------------------------------------------
    const TimeValue & Start(const TimeValue & start);
    
    //------------------------------------------------------------------------
    //!  Returns the end time.
    //------------------------------------------------------------------------
    const TimeValue & End() const
    {
      return(_data.second);
    }

    //------------------------------------------------------------------------
    //!  Sets and returns the end time.
    //------------------------------------------------------------------------
    const TimeValue & End(const TimeValue & end);
    
    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const TimeInterval & ti) const;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    TimeValue Duration() const
    {
      return (_data.second - _data.first);
    }
    
    //------------------------------------------------------------------------
    //!  Returns true if the TimeValue \c tv falls within the time
    //!  interval, i.e. \tv is greater than or equal to the start and
    //!  less than or equal to the end.
    //------------------------------------------------------------------------
    bool Contains(const TimeValue & tv) const;

    //------------------------------------------------------------------------
    //!  Returns true if the TimeInterval \c ti is completely contained
    //!  within our time interval.
    //------------------------------------------------------------------------
    bool Contains(const TimeInterval & ti) const;
    
    //------------------------------------------------------------------------
    //!  Returns true if \c ti overlaps the time interval.
    //------------------------------------------------------------------------
    bool Overlaps(const TimeInterval & ti) const;
    
    //------------------------------------------------------------------------
    //!  Returns true if \c ti is immediately adjacent our time interval,
    //!  i.e. our start is equal to \c ti.End() or our end is equal to
    //!  \c ti.Start()
    //------------------------------------------------------------------------
    bool Adjacent(const TimeInterval & ti) const;
    
    //------------------------------------------------------------------------
    //!  Returns the length of the time insterval, in bytes.  This is
    //!  used for some I/O functions.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;

    //------------------------------------------------------------------------
    //!  Reads the time interval from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Writes the time interval to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f);

    //------------------------------------------------------------------------
    //!  Writes the time interval to a FILE pointer.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);

    //------------------------------------------------------------------------
    //!  Wries the time interval to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);

    //------------------------------------------------------------------------
    //!  Writes the time interval to a gzFile.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads the time interval from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);

    //------------------------------------------------------------------------
    //!  Writes the time interval to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;
    
  private:
    std::pair<TimeValue, TimeValue>  _data;
  };
  
}  // namespace Dwm

#endif  // _DWMTIMEINTERVAL_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
