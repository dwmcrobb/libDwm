//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmTimeInterval.cc 11141 $
// @(#) $Id: DwmTimeInterval.cc 11141 2020-09-07 23:50:25Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2005-2007, 2020
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
//!  \file DwmTimeInterval.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::TimeInterval class implementation
//---------------------------------------------------------------------------

#include "DwmSvnTag.hh"
#include "DwmTimeInterval.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmBZ2IO.hh"
#include "DwmGZIO.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmTimeInterval.cc 11141 $");

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TimeValue & TimeInterval::Start(const TimeValue & start)
  {
    _data.first = start;
    return(_data.first);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const TimeValue & TimeInterval::End(const TimeValue & end)
  {
    _data.second = end;
    return(_data.second);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeInterval::operator == (const TimeInterval & ti) const
  {
    return((Start() == ti.Start()) && (End() == ti.End()));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeInterval::Contains(const TimeValue & tv) const
  {
    return (tv >= Start() && tv <= End());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeInterval::Contains(const TimeInterval & ti) const
  {
    return(Contains(ti.Start()) && Contains(ti.End()));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeInterval::Overlaps(const TimeInterval & ti) const
  {
    //  We have to do both comparisons here, to cover the case
    //  where one interval is wholly contained in the other interval.
    return (Contains(ti.Start()) || Contains(ti.End()) ||
            ti.Contains(Start()) || ti.Contains(End()));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool TimeInterval::Adjacent(const TimeInterval & ti) const
  {
    return((ti.End() == Start()) || (ti.Start() == End()));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t TimeInterval::StreamedLength() const
  {
    return(_data.first.StreamedLength() + _data.second.StreamedLength());
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & TimeInterval::Read(std::istream & is)
  {
    return(StreamIO::Read(is, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & TimeInterval::Write(std::ostream & os) const
  {
    return(StreamIO::Write(os, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t TimeInterval::Read(FILE *f)
  {
    return(FileIO::Read(f, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t TimeInterval::Write(FILE *f) const
  {
    return(FileIO::Write(f, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t TimeInterval::Read(int fd)
  {
    return(DescriptorIO::Read(fd, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t TimeInterval::Write(int fd) const
  {
    return(DescriptorIO::Write(fd, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeInterval::Read(gzFile gzf)
  {
    return(GZIO::Read(gzf, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeInterval::Write(gzFile gzf) const
  {
    return(GZIO::Write(gzf, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeInterval::BZRead(BZFILE *bzf)
  {
    return(BZ2IO::BZRead(bzf, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int TimeInterval::BZWrite(BZFILE *bzf) const
  {
    return(BZ2IO::BZWrite(bzf, _data));
  }
  

}  // namespace Dwm
