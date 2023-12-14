//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2021, 2023
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
//!  \file DwmASIOCapable.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::ASIOReadable, Dwm::ASIOWritable, Dwm::ASIOCapable interface
//!    declarations
//---------------------------------------------------------------------------

#ifndef _DWMASIOCAPABLE_HH_
#define _DWMASIOCAPABLE_HH_

#include <boost/asio.hpp>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Interface for classes that wish to be readable via Dwm::ASIO.
  //--------------------------------------------------------------------------
  class ASIOReadable
  {
  public:
    //------------------------------------------------------------------------
    //!  Should read the class from the given socket @c s and return true
    //!  on success, faluse on failure.
    //------------------------------------------------------------------------
    virtual bool Read(boost::asio::ip::tcp::socket & s) = 0;

    //------------------------------------------------------------------------
    //!  Should read the class from the given socket @c s and return true
    //!  on success, faluse on failure.
    //------------------------------------------------------------------------
    virtual bool Read(boost::asio::local::stream_protocol::socket & s) = 0;
  };

  //--------------------------------------------------------------------------
  //!  Interface for classes that wish to be writable via Dwm::ASIO.
  //--------------------------------------------------------------------------
  class ASIOWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  Should write the class to the given socket @c s and return true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    virtual bool Write(boost::asio::ip::tcp::socket & s) const = 0;

    //------------------------------------------------------------------------
    //!  Should write the class to the given socket @c s and return true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    virtual bool
    Write(boost::asio::local::stream_protocol::socket & s) const = 0;
  };
  
  //--------------------------------------------------------------------------
  //!  Interface for classes that can read/write their contents from/to
  //!  a boost::asio::ip::tcp::socket.
  //--------------------------------------------------------------------------
  class ASIOCapable
    : public ASIOReadable, public ASIOWritable
  {};
  
}  // namespace Dwm

#endif  // _DWMASIOCAPABLE_HH_
