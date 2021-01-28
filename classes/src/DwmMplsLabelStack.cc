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
//!  \file DwmMplsLabelStack.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::MplsLabelStack class implementation
//---------------------------------------------------------------------------

#include <cassert>

#include "DwmMplsLabelStack.hh"
#include "DwmSysLogger.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::vector<MplsLabel> & MplsLabelStack::Labels() const
  {
    return(_labels);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::vector<MplsLabel> & MplsLabelStack::Labels()
  {
    return(_labels);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool MplsLabelStack::operator == (const MplsLabelStack & labelStack) const
  {
    return(_labels == labelStack._labels);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::istream & MplsLabelStack::Read(std::istream & is)
  {
    _labels.clear();
    
    if (is) {
      MplsLabel  label;
      while (! label.S()) {
        if (! label.Read(is))
          break;
        _labels.push_back(label);
      }
    }
    return(is);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & MplsLabelStack::Write(std::ostream & os) const
  {
    assert(! _labels.empty());
    assert(_labels.rbegin()->S());
    
    if (os) {
      std::vector<MplsLabel>::const_iterator  i;
      for (i = _labels.begin(); i != _labels.end(); ++i) {
        if (! i->Write(os))
          break;
      }
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t MplsLabelStack::Read(int fd)
  {
    ssize_t  rc = 0;
    
    _labels.clear();
    if (fd >= 0) {
      MplsLabel  label;
      while (! label.S()) {
        ssize_t  bytesRead = label.Read(fd);
        if (bytesRead != label.StreamedLength())
          goto err;
        rc += bytesRead;
        _labels.push_back(label);
      }
    }
    else {
      goto err;
    }
    
    return(rc);

  err:
    Syslog(LOG_ERR, "MplsLabelStack::Read(%d) failed", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ssize_t MplsLabelStack::Write(int fd) const
  {
    assert(! _labels.empty());
    assert(_labels.rbegin()->S());

    ssize_t  rc = 0;
    if (fd >= 0) {
      std::vector<MplsLabel>::const_iterator  i;
      for (i = _labels.begin(); i != _labels.end(); ++i) {
        ssize_t  bytesWritten = i->Write(fd);
        if (bytesWritten != i->StreamedLength())
          goto err;
        rc += bytesWritten;
      }
    }
    else {
      goto err;
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "MplsLabelStack::Write(%d) failed", fd);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t MplsLabelStack::Read(FILE *f)
  {
    _labels.clear();
    if (f) {
      MplsLabel  label;
      while (! label.S()) {
        if (! label.Read(f))
          goto err;
        _labels.push_back(label);
      }
    }
    else {
      goto err;
    }
    return(1);

  err:
    Syslog(LOG_ERR, "MplsLabelStack::Read(%p) failed", f);
    return(0);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  size_t MplsLabelStack::Write(FILE *f) const
  {
    assert(! _labels.empty());
    assert(_labels.rbegin()->S());
    
    ssize_t  rc = 0;
    if (f) {
      std::vector<MplsLabel>::const_iterator  i;
      for (i = _labels.begin(); i != _labels.end(); ++i) {
        if (! i->Write(f))
          goto err;
      }
    }
    else {
      goto err;
    }
    return(1);
    
  err:
    Syslog(LOG_ERR, "MplsLabelStack::Write(%p) failed", f);
    return(0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabelStack::Read(gzFile gzf)
  {
    int  rc = 0;
    _labels.clear();
    if (gzf) {
      MplsLabel  label;
      while (! label.S()) {
        int  bytesRead = label.Read(gzf);
        if (bytesRead != label.StreamedLength())
          goto err;
        _labels.push_back(label);
        rc += bytesRead;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "MplsLabelStack::Read(gzFile %p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabelStack::Write(gzFile gzf) const
  {
    assert(! _labels.empty());
    assert(_labels.rbegin()->S());

    ssize_t  rc = 0;
    if (gzf) {
      std::vector<MplsLabel>::const_iterator  i;
      for (i = _labels.begin(); i != _labels.end(); ++i) {
        ssize_t  bytesWritten = i->Write(gzf);
        if (bytesWritten != i->StreamedLength())
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "MplsLabelStack::Write(gzFile %p) failed", gzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabelStack::BZRead(BZFILE *bzf)
  {
    int  rc = 0;
    _labels.clear();
    if (bzf) {
      MplsLabel  label;
      while (! label.S()) {
        int  bytesRead = label.BZRead(bzf);
        if (bytesRead != label.StreamedLength())
          goto err;
        _labels.push_back(label);
        rc += bytesRead;
      }
    }
    return(rc);

  err:
    Syslog(LOG_ERR, "MplsLabelStack::BZRead(BZFILE %p) failed", bzf);
    return(-1);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int MplsLabelStack::BZWrite(BZFILE *bzf) const
  {
    assert(! _labels.empty());
    assert(_labels.rbegin()->S());

    ssize_t  rc = 0;
    if (bzf) {
      std::vector<MplsLabel>::const_iterator  i;
      for (i = _labels.begin(); i != _labels.end(); ++i) {
        ssize_t  bytesWritten = i->BZWrite(bzf);
        if (bytesWritten != i->StreamedLength())
          goto err;
        rc += bytesWritten;
      }
    }
    return(rc);
    
  err:
    Syslog(LOG_ERR, "MplsLabelStack::BZWrite(BZFILE %p) failed", bzf);
    return(-1);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t MplsLabelStack::StreamedLength() const
  {
    uint64_t  rc = 0;
    std::vector<MplsLabel>::const_iterator  i;
    for (i = _labels.begin(); i != _labels.end(); ++i) {
      rc += i->StreamedLength();
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & 
  operator << (std::ostream & os, const MplsLabelStack & labelStack)
  {
    if (os) {
      if (! labelStack._labels.empty()) {
        std::vector<MplsLabel>::const_iterator  i = labelStack._labels.begin();
        os << *i;
        ++i;
        for ( ; i != labelStack._labels.end(); ++i) {
          os << ' ' << *i;
        }
      }
    }
    return(os);
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
