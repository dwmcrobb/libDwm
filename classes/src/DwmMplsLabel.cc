//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2020
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
//!  \file DwmMplsLabel.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::MplsLabel class implementation
//---------------------------------------------------------------------------

#include <cassert>
#include <cstdlib>
#include <sstream>

#include "DwmBZ2IO.hh"
#include "DwmGZIO.hh"
#include "DwmDescriptorIO.hh"
#include "DwmFileIO.hh"
#include "DwmStreamIO.hh"
#include "DwmMplsLabel.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MplsLabel::MplsLabel()
      : _data(0)
  {}

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  MplsLabel::MplsLabel(const std::string & s)
      : _data(0)
  {
    std::istringstream  is(s);
    is >> *this;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t MplsLabel::Label() const
  {
    return(_data >> 12);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t MplsLabel::Label(uint32_t label)
  {
    assert(label <= 0xFFFFF);

    //  clear the label bits
    _data &= 0x00000FFF;

    //  set the label bits
    _data |= (label << 12);

    return(_data >> 12);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t MplsLabel::Exp() const
  {
    return((_data >> 9) & 0x07);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t MplsLabel::Exp(uint8_t exp)
  {
    assert(exp <= 0x07);

    //  clear the exp bits
    _data &= 0xFFFFF1FF;

    //  set the exp bits
    _data |= ((uint32_t)exp << 9);
    
    return((_data >> 9) & 0x07);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MplsLabel::S() const
  {
    return((_data >> 8) & 0x01);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MplsLabel::S(bool s)
  {
    //  clear the s bit
    _data &= 0xFFFFFEFF;
    
    //  set the s bit
    if (s)
      _data |= 0x100;
    
    return((_data >> 8) & 0x01);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t MplsLabel::Ttl() const
  {
    return(_data & 0xFF);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint8_t MplsLabel::Ttl(uint8_t ttl)
  {
    //  clear the ttl bits
    _data &= 0xFFFFFF00;
    
    //  set the ttl bits
    _data |= ttl;
    
    return(_data & 0xFF);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MplsLabel::operator == (const MplsLabel & label) const
  {
    return(_data == label._data);
  }
  
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::istream & MplsLabel::Read(std::istream & is)
  {
    return(StreamIO::Read(is, _data));
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream & MplsLabel::Write(std::ostream & os) const
  {
    return(StreamIO::Write(os, _data));
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t MplsLabel::Read(int fd)
  {
    return(DescriptorIO::Read(fd, _data));
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  ssize_t MplsLabel::Write(int fd) const
  {
    return(DescriptorIO::Write(fd, _data));
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t MplsLabel::Read(FILE *f)
  {
    return(FileIO::Read(f, _data));
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  size_t MplsLabel::Write(FILE *f) const
  {
    return(FileIO::Write(f, _data));
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabel::Read(gzFile gzf)
  {
    return(GZIO::Read(gzf, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabel::Write(gzFile gzf) const
  {
    return(GZIO::Write(gzf, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabel::BZRead(BZFILE *bzf)
  {
    return(BZ2IO::BZRead(bzf, _data));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabel::BZWrite(BZFILE *bzf) const
  {
    return(BZ2IO::BZWrite(bzf, _data));
  }

  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  uint64_t MplsLabel::StreamedLength() const
  {
    return(4);
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  std::ostream &
  operator << (std::ostream & os, const MplsLabel & addr)
  {
    if (os) {
      os << addr.Label() << ':' << (uint16_t)addr.Exp() << ':'
         << addr.S() << ':' << (uint16_t)addr.Ttl();
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream &
  operator >> (std::istream & is, MplsLabel & addr)
  {
    if (is) {
      std::string  s;
      if (getline(is, s, ':')) {
        addr.Label(strtoul(s.c_str(), 0, 10));
        if (getline(is, s, ':')) {
          addr.Exp(strtoul(s.c_str(), 0, 10));
          if (getline(is, s, ':')) {
            addr.S(s[0] == '1' ? 1 : 0);
            if (getline(is, s, ':')) {
              addr.Ttl(strtoul(s.c_str(), 0, 10));
            }
          }
        }
      }
    }
    return(is);
  }
  

}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
