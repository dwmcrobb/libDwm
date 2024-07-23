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
  //!  T has a
  //!  Read(boost::asio::ip::tcp::socket &, boost::system::error_code &)
  //!  method that returns bool.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioTcpRead = requires(T & t, boost::asio::ip::tcp::socket & s,
                                    boost::system::error_code & ec) {
    { t.Read(s, ec) } -> std::same_as<bool>;
  };

  //--------------------------------------------------------------------------
  //!  T has a
  //!  Write(boost::asio::ip::tcp::socket &, boost::system::error_code &)
  //!  method that returns bool.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioTcpWrite = requires(const T & t,
                                     boost::asio::ip::tcp::socket & s,
                                     boost::system::error_code & ec) {
    { t.Write(s, ec) } -> std::same_as<bool>;
  };

  //--------------------------------------------------------------------------
  //!  T has a
  //!  Read(boost::asio::local::stream_protocol::socket &,
  //!       boost::system::error_code &)  method that returns bool.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioLocalRead =
  requires(T & t, boost::asio::local::stream_protocol::socket & s,
           boost::system::error_code & ec) {
    { t.Read(s, ec) } -> std::same_as<bool>;
  };

  //--------------------------------------------------------------------------
  //!  T has a
  //!  Write(boost::asio::local::stream_protocol::socket &,
  //!        boost::system::error_code &) const method that returns bool.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioLocalWrite =
  requires(const T & t, boost::asio::local::stream_protocol::socket & s,
           boost::system::error_code & ec) {
    { t.Write(s, ec) } -> std::same_as<bool>;
  };

  //--------------------------------------------------------------------------
  //!  T has a
  //!  Read(boost::asio::generic::stream_protocol::socket &,
  //!       boost::system::error_code &)  method that returns bool.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioGenericStreamRead =
  requires(T & t, boost::asio::generic::stream_protocol::socket & s,
           boost::system::error_code & ec) {
    { t.Read(s, ec) } -> std::same_as<bool>;
  };

  //--------------------------------------------------------------------------
  //!  T has a
  //!  Write(boost::asio::generic::stream_protocol::socket &,
  //!        boost::system::error_code &) const method that returns bool.
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioGenericStreamWrite =
  requires(const T & t, boost::asio::generic::stream_protocol::socket & s,
           boost::system::error_code & ec) {
    { t.Write(s, ec) } -> std::same_as<bool>;
  };

  //--------------------------------------------------------------------------
  //!  T has asio read members
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioRead = (HasAsioTcpRead<T>
                         && HasAsioLocalRead<T>
                         && HasAsioGenericStreamRead<T>);

  //--------------------------------------------------------------------------
  //!  T has asio write members
  //--------------------------------------------------------------------------
  template <typename T>
  concept HasAsioWrite = (HasAsioTcpWrite<T>
                         && HasAsioLocalWrite<T>
                         && HasAsioGenericStreamWrite<T>);
  
  //--------------------------------------------------------------------------
  //!  Interface for classes that wish to be readable via Dwm::ASIO.  Pure
  //!  virtual.
  //--------------------------------------------------------------------------
  class ASIOReadable
  {
  public:
    //------------------------------------------------------------------------
    //!  Should read the class from the given socket @c s and return true
    //!  on success, false on failure.  Should set @c ec to the correct
    //!  error code on failure.
    //------------------------------------------------------------------------
    virtual bool Read(boost::asio::ip::tcp::socket & s,
                      boost::system::error_code & ec) = 0;

    //------------------------------------------------------------------------
    //!  Should read the class from the given socket @c s and return true
    //!  on success, false on failure.  Should set @c ec to the correct
    //!  error code on failure.
    //------------------------------------------------------------------------
    virtual bool Read(boost::asio::local::stream_protocol::socket & s,
                      boost::system::error_code & ec) = 0;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    virtual bool Read(boost::asio::generic::stream_protocol::socket & s,
                      boost::system::error_code & ec) = 0;
  };

  //--------------------------------------------------------------------------
  //!  Interface for classes that wish to be writable via Dwm::ASIO.
  //--------------------------------------------------------------------------
  class ASIOWritable
  {
  public:
    //------------------------------------------------------------------------
    //!  Should write the class to the given socket @c s and return true on
    //!  success, false on failure.  Should set @c ec to the correct
    //!  error code on failure.
    //------------------------------------------------------------------------
    virtual bool Write(boost::asio::ip::tcp::socket & s,
                       boost::system::error_code & ec) const = 0;

    //------------------------------------------------------------------------
    //!  Should write the class to the given socket @c s and return true on
    //!  success, false on failure.  Should set @c ec to the correct
    //!  error code on failure.
    //------------------------------------------------------------------------
    virtual bool Write(boost::asio::local::stream_protocol::socket & s,
                       boost::system::error_code & ec) const = 0;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    virtual bool Write(boost::asio::generic::stream_protocol::socket & s,
                       boost::system::error_code & ec) const = 0;
        
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
