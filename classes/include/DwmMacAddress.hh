//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2020, 2024
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
//!  \file DwmMacAddress.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::MacAddress class definition
//---------------------------------------------------------------------------

#ifndef _DWMMACADDRESS_HH_
#define _DWMMACADDRESS_HH_

extern "C" {
  #include <bzlib.h>
  #include <zlib.h>
}

#include <cstdio>
#include <iostream>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a 6-byte MAC address.
  //--------------------------------------------------------------------------
  class MacAddress
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.  Sets the address to 00:00:00:00:00:00
    //------------------------------------------------------------------------
    MacAddress();

    //------------------------------------------------------------------------
    //!  Construct from a string of the form xx:xx:xx:xx:xx:xx
    //------------------------------------------------------------------------
    MacAddress(const std::string & s);

    //------------------------------------------------------------------------
    //!  Copy constructor.
    //------------------------------------------------------------------------
    MacAddress(const MacAddress & addr);

    //------------------------------------------------------------------------
    //!  operator =
    //------------------------------------------------------------------------
    MacAddress & operator = (const MacAddress & addr);

    //------------------------------------------------------------------------
    //!  Returns a string representation of the MAC address.
    //------------------------------------------------------------------------
    operator std::string () const;
    
    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const MacAddress & addr) const;

    //------------------------------------------------------------------------
    //!  operator <
    //------------------------------------------------------------------------
    bool operator < (const MacAddress & addr) const;
    
    //------------------------------------------------------------------------
    //!  Reads the MAC address from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is);

    //------------------------------------------------------------------------
    //!  Writes the MAC address to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const;

    //------------------------------------------------------------------------
    //!  Reads the MAC address from a file descriptor.  Returns the number
    //!  of bytes read on success (should be 6), -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd);

    //------------------------------------------------------------------------
    //!  Writes the MAC address to a file descriptor.  Returns the number of
    //!  bytes written on success (should be 6), -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const;

    //------------------------------------------------------------------------
    //!  Reads the MAC address from a FILE.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f);

    //------------------------------------------------------------------------
    //!  Writes the MAC address to a FILE.  Returns 1 on success, 0 on 
    //!  failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const;

    //------------------------------------------------------------------------
    //!  Reads the MAC address from a gzFile.  Returns the number
    //!  of bytes read on success (should be 6), -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf);

    //------------------------------------------------------------------------
    //!  Writes the MAC address to a gzFile.  Returns the number of
    //!  bytes written on success (should be 6), -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const;

    //------------------------------------------------------------------------
    //!  Reads the MAC address from a BZFILE.  Returns the number
    //!  of bytes read on success (should be 6), -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf);

    //------------------------------------------------------------------------
    //!  Writes the MAC address to a BZFILE.  Returns the number of
    //!  bytes written on success (should be 6), -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call one
    //!  of the Write() members.  Should always return 6.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const;
    
    //------------------------------------------------------------------------
    //!  Prints to an ostream in 'xx:xx:xx:xx:xx:xx' format.
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const MacAddress & addr);
    
    //------------------------------------------------------------------------
    //!  Reads from an istream in 'xx:xx:xx:xx:xx:xx' format.
    //------------------------------------------------------------------------
    friend std::istream &
    operator >> (std::istream & is, MacAddress & addr);
    
  protected:
    uint8_t  _addr[6];
  };
  
}  // namespace Dwm

#endif  // _DWMMACADDRESS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
