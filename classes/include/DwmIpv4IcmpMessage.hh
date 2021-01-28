//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//    endorse or promote products derived from this software without
//    specific prior written permission.
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
//!  \file DwmIpv4IcmpMessage.hh
//!  \brief Dwm::Ipv4IcmpMessage class definition
//---------------------------------------------------------------------------

#ifndef _DWMIPV4ICMPMESSAGE_HH_
#define _DWMIPV4ICMPMESSAGE_HH_

extern "C" {
#include <inttypes.h>
}

#include <utility>

#include "DwmBZ2Writable.hh"
#include "DwmGZWritable.hh"
#include "DwmDescriptorWritable.hh"
#include "DwmFileWritable.hh"
#include "DwmStreamWritable.hh"
#include "DwmStreamedLengthCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Base class for ICMP messages.
  //--------------------------------------------------------------------------
  class Ipv4IcmpMessage
    : public DescriptorWritable, public FileWritable, public StreamWritable,
      public StreamedLengthCapable, public GZWritable, public BZ2Writable
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor.
    //------------------------------------------------------------------------
    Ipv4IcmpMessage();
    
    //------------------------------------------------------------------------
    //!  Destructor.
    //------------------------------------------------------------------------
    ~Ipv4IcmpMessage();
    
    //------------------------------------------------------------------------
    //!  Returns the ICMP message type.
    //------------------------------------------------------------------------
    uint8_t Type() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the ICMP message type.
    //------------------------------------------------------------------------
    uint8_t Type(uint8_t type);
    
    //------------------------------------------------------------------------
    //!  Returns the ICMP message code.
    //------------------------------------------------------------------------
    uint8_t Code() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the ICMP message code.
    //------------------------------------------------------------------------
    uint8_t Code(uint8_t code);

    //------------------------------------------------------------------------
    //!  Returns the ICMP checksum.
    //------------------------------------------------------------------------
    uint16_t Checksum();

    //------------------------------------------------------------------------
    //!  Computes and sets the checksum.
    //------------------------------------------------------------------------
    void SetChecksum();
    
    //------------------------------------------------------------------------
    //!  Reads the message of length @c len from an istream.  Returns the
    //!  istream.
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is, uint16_t len);

    //------------------------------------------------------------------------
    //!  Writes the message to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override;

    //------------------------------------------------------------------------
    //!  Reads the message of length @c len from a file descriptor.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Read(int fd, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the message to a file descriptor.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the message of length @c len from a FILE.  Returns 1 on
    //!  success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Read(FILE *f, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the message to a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    size_t Write(FILE *f) const override;

    //------------------------------------------------------------------------
    //!  Reads the message of length @c len from a gzFile.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int Read(gzFile gzf, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the message to a gzFile.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int Write(gzFile gzf) const override;
    
    //------------------------------------------------------------------------
    //!  Reads the message of length @c len from a BZFILE.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZRead(BZFILE *bzf, uint16_t len);
    
    //------------------------------------------------------------------------
    //!  Writes the message to a BZFILE.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    int BZWrite(BZFILE *bzf) const override;
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called one
    //!  of the Write() members.
    //------------------------------------------------------------------------
    uint64_t StreamedLength() const override;
    
    //------------------------------------------------------------------------
    //!  operator ==
    //------------------------------------------------------------------------
    bool operator == (const Ipv4IcmpMessage & msg) const;
    
  protected:
    std::pair<uint16_t, uint8_t *>  _data;
    bool                            _ownData;

    void Free();
    void Allocate(uint16_t len);
    void Reallocate(uint16_t len);
  };
  
}  // namespace Dwm

#endif  // _DWMIPV4ICMPMESSAGE_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
