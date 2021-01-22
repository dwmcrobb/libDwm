//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmDescriptor.hh 11087 $
// @(#) $Id: DwmDescriptor.hh 11087 2020-09-07 09:58:37Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 1998-2006, 2016
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
//!  \file DwmDescriptor.hh
//!  \brief Dwm::Descriptor class definition
//---------------------------------------------------------------------------

#ifndef _DWMDESCRIPTOR_HH_
#define _DWMDESCRIPTOR_HH_

extern "C" {
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
}

namespace Dwm {
  
  //--------------------------------------------------------------------------
  //!  This class encapsulates UNIX file and socket descriptors.
  //!  It only abstracts those functions which have common interfaces, so
  //!  that applications may use this interface to handle both file and
  //!  socket descriptor I/O in the same place.
  //--------------------------------------------------------------------------
  class Descriptor
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Descriptor();

    //------------------------------------------------------------------------
    //!  Copy constructor.  Will dup() the descriptor of @c d.
    //------------------------------------------------------------------------
    Descriptor(const Descriptor & d);

    Descriptor(int d);

    //------------------------------------------------------------------------
    //!  operator = (assignment operator).  Will close @c this and then
    //!  dup() the descriptor of @c d.
    //------------------------------------------------------------------------
    Descriptor & operator = (const Descriptor & d);
    
    // Descriptor & operator = (int fd);
    
    //------------------------------------------------------------------------
    //!  Destructor.  Note this will close the underlying UNIX descriptor.
    //------------------------------------------------------------------------
    virtual ~Descriptor();

    //------------------------------------------------------------------------
    //!  Closes the Descriptor.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    virtual bool Close();

    //------------------------------------------------------------------------
    //!  Associates the Descriptor with an existing UNIX descriptor
    //!  \c fd.  Note this will cause the Descriptor to automatically
    //!  close \c fd when destructed.  Returns true if \c fd is a valid
    //!  descriptor, else returns false.
    //------------------------------------------------------------------------
    virtual bool Attach(int fd);

    //------------------------------------------------------------------------
    //!  Disassociates the Descriptor from the underlying UNIX descriptor.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    virtual bool Detach();
    
    //------------------------------------------------------------------------
    //!  Sets the Descriptor to non-blocking.  Returns true on success,
    //!  false on failure.
    //------------------------------------------------------------------------
    virtual bool SetNonBlocking();

    //------------------------------------------------------------------------
    //!  Sets the soccket to blocking.  Returns true on success, false on
    //!  failure.
    //------------------------------------------------------------------------
    virtual bool SetBlocking();
    
    //------------------------------------------------------------------------
    //!  This completely blows encapsulation, but is needed so that
    //!  Descriptor may be used in place of a UNIX descriptor in
    //!  retrofitted code.
    //------------------------------------------------------------------------
    virtual operator int () const
    {
      return(this->_fd);
    }

    //------------------------------------------------------------------------
    //!  If Descriptor is valid, sets it in \c fdSet and returns true.
    //!  Else returns false.
    //------------------------------------------------------------------------
    virtual bool FdSet(fd_set & fdSet);

    //------------------------------------------------------------------------
    //!  If Descriptor is valid, clears it in \c fdSet and returns true.
    //!  Else returns false.
    //------------------------------------------------------------------------
    virtual bool FdClr(fd_set & fdSet);

    //------------------------------------------------------------------------
    //!  If Descriptor is valid and is set in \c fdSet, returns true.
    //!  Else returns false.
    //------------------------------------------------------------------------
    virtual bool FdIsSet(fd_set & fdSet);

    virtual bool ReadWouldBlock();
    
  protected:
    int            _fd;
  };
  
}  // namespace Dwm

#endif  // _DWMDESCRIPTOR_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
