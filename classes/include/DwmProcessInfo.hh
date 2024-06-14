//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2016, 2020, 2024
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
//!  \file DwmProcessInfo.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::ProcessInfo class declaration
//---------------------------------------------------------------------------

#ifndef _DWMPROCESSINFO_HH_
#define _DWMPROCESSINFO_HH_

extern "C" {
  #include <unistd.h>
}

#include <cstdint>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

#include "DwmStreamIOCapable.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmGZIOCapable.hh"
#include "DwmBZ2IOCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  class ProcessInfo
    : public StreamIOCapable, public GZIOCapable, public BZ2IOCapable,
      public StreamedLengthCapable
  {
  public:
    std::string  user {""};
    std::string  ruser {""};
    uint64_t     pid {0};
    uint64_t     ppid {0};
    uint64_t     gid {0};
    uint64_t     uid {0};
    uint64_t     ruid {0};
    uint64_t     svuid {0};
    uint64_t     rgid {0};
    uint64_t     svgid {0};
    uint64_t     rss {0};
    uint64_t     vsz {0};
    float        pcpu {0.0};
    float        pmem {0.0};
    int64_t      pri {0};
    int64_t      lstart {0};
    std::string  tty {""};
    std::string  args {""};

    std::istream & Read(std::istream & is) override;
    std::ostream & Write(std::ostream & os) const override;
    int Read(gzFile gzf) override;
    int Write(gzFile gzf) const override;
    int BZRead(BZFILE *bzf) override;
    int BZWrite(BZFILE *bzf) const override;
    uint64_t StreamedLength() const override;
    
    //------------------------------------------------------------------------
    //!  Returns a JSON representation of the ProcessInfo.
    //------------------------------------------------------------------------
    nlohmann::json ToJson() const;
    
    bool operator == (const ProcessInfo &) const;
  };
  
}  // namespace Dwm

std::ostream & operator << (std::ostream & os, const Dwm::ProcessInfo & pi);

#endif  // _DWMPROCESSINFO_HH_
