//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file DwmProcessInfo.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <ctime>

#include "DwmProcessInfo.hh"
#include "DwmBZ2IO.hh"
#include "DwmGZIO.hh"
#include "DwmStreamIO.hh"
#include "DwmIOUtils.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  nlohmann::json ProcessInfo::ToJson() const
  {
    nlohmann::json  j;
    j["user"] = user;
    j["ruser"] = ruser;
    j["pid"] = pid;
    j["ppid"] = ppid;
    j["gid"] = gid;
    j["uid"] = uid;
    j["ruid"] = ruid;
    j["svuid"] = svuid;
    j["rgid"] = rgid;
    j["svgid"] = svgid;
    j["rss"] = rss;
    j["vsz"] = vsz;
    j["pcpu"] = pcpu;
    j["pmem"] = pmem;
    j["lstart"] = lstart;
    j["tty"] = tty;
    j["args"] = args;
    return j;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & ProcessInfo::Read(std::istream & is)
  {
    if (is) {
      StreamIO::ReadV(is, user, ruser, pid, ppid, gid, uid, ruid, svuid,
                      rgid, svgid, rss, vsz, pcpu, pmem, pri, lstart, tty,
                      args);
    }
    return is;
  }
 
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & ProcessInfo::Write(std::ostream & os) const
  {
    if (os) {
      StreamIO::WriteV(os, user, ruser, pid, ppid, gid, uid, ruid, svuid,
                       rgid, svgid, rss, vsz, pcpu, pmem, pri, lstart, tty,
                       args);
    }
    return os;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int ProcessInfo::Read(gzFile gzf)
  {
    if (gzf) {
      return GZIO::ReadV(gzf, user, ruser, pid, ppid, gid, uid, ruid, svuid,
                         rgid, svgid, rss, vsz, pcpu, pmem, pri, lstart, tty,
                         args);
    }
    return -1;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int ProcessInfo::Write(gzFile gzf) const
  {
    if (gzf) {
      return GZIO::WriteV(gzf, user, ruser, pid, ppid, gid, uid, ruid, svuid,
                          rgid, svgid, rss, vsz, pcpu, pmem, pri, lstart, tty,
                          args);
    }
    return -1;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int ProcessInfo::BZRead(BZFILE *bzf)
  {
    if (bzf) {
      return BZ2IO::BZReadV(bzf, user, ruser, pid, ppid, gid, uid, ruid,
                            svuid, rgid, svgid, rss, vsz, pcpu, pmem, pri,
                            lstart, tty, args);
    }
    return -1;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int ProcessInfo::BZWrite(BZFILE *bzf) const
  {
    if (bzf) {
      return BZ2IO::BZWriteV(bzf, user, ruser, pid, ppid, gid, uid, ruid,
                             svuid, rgid, svgid, rss, vsz, pcpu, pmem, pri,
                             lstart, tty, args);
    }
    return -1;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t ProcessInfo::StreamedLength() const
  {
    return IOUtils::StreamedLengthV(user, ruser, pid, ppid, gid, uid, ruid,
                                    svuid, rgid, svgid, rss, vsz, pcpu, pmem,
                                    pri, lstart, tty, args);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ProcessInfo::operator == (const ProcessInfo & pi) const
  {
    return ((user == pi.user)
            && (ruser == pi.ruser)
            && (pid == pi.pid)
            && (ppid == pi.ppid)
            && (gid == pi.gid)
            && (uid == pi.uid)
            && (ruid == pi.ruid)
            && (svuid == pi.svuid)
            && (rgid == pi.rgid)
            && (svgid == pi.svgid)
            && (rss == pi.rss)
            && (vsz == pi.vsz)
            && (pcpu == pi.pcpu)
            && (pmem == pi.pmem)
            && (pri == pi.pri)
            && (lstart == pi.lstart)
            && (tty == pi.tty)
            && (args == pi.args));
  }
  
}  // namespace Dwm

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const Dwm::ProcessInfo & pi)
{
  os << pi.user << ' ' << pi.ruser << ' ' << pi.pid << ' ' << pi.ppid << ' '
     << pi.gid << ' ' << pi.uid << ' ' << pi.ruid << ' ' << pi.svuid << ' '
     << pi.rgid << ' ' << pi.svgid << ' ' << pi.rss << ' ' << pi.vsz << ' '
     << pi.pcpu << ' ' << pi.pmem << ' ';
  time_t  ls = (time_t)pi.lstart;
  std::string  startTime = ctime(&ls);
  startTime.pop_back();
  os << startTime << ' ' << pi.tty << ' ' << pi.args;
  return os;
}
