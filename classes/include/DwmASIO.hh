//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020, 2023-2025
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
//!  \file DwmASIO.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::ASIO class declaration
//---------------------------------------------------------------------------

#ifndef _DWMASIO_HH_
#define _DWMASIO_HH_

#include <string>

#include "DwmASIOCapable.hh"
#include "DwmIOConcepts.hh"
#include "DwmSysLogger.hh"
#include "DwmTypeName.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Concept for the types of ASIO sockets we can utilize.  Originally I
  //!  only used ASIO for TCP sockets, but later I had use cases for
  //!  UNIX domain sockets.
  //--------------------------------------------------------------------------
  template <typename S>
  concept IsSupportedASIOSocket =
    std::is_same_v<S,boost::asio::ip::tcp::socket>
    || std::is_same_v<S,boost::asio::local::stream_protocol::socket>
    || std::is_same_v<S,boost::asio::generic::stream_protocol::socket>;

  namespace asio_detail {

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be read from
    //!  a supported asio socket via an ASIO::Read() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsReadable =
    (io_detail::Readable<std::remove_reference_t<T>,HasAsioRead_t>() == true);

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be written
    //!  to a supported asio socket via an ASIO::Write() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsWritable =
    (io_detail::Readable<std::remove_cvref_t<T>,HasAsioRead_t>() == true)
      and (io_detail::Writable<std::remove_cvref_t<T>,HasAsioWrite_t>() == true);

  }  // namespace asio_detail

  //--------------------------------------------------------------------------
  //!  A collection of functions for reading from and writing to
  //!  Boost ASIO stream sockets.
  //!  TODO: I should probably redesign this.  Initially I was only using
  //!  sockets of type boost::asio::ip::tcp::socket.  Once I needed support
  //!  for boost::asio::local::stream_protocol::socket, things got messy.
  //!  boost::asio doesn't have a class heirarchy here, it's all templatized.
  //!  At the moment I don't have time to templatize or switch to only
  //!  supporting generic::stream_protocol.
  //--------------------------------------------------------------------------
  class ASIO
  {
  public:
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, char & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, char value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     char & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      char value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     char & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      char value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint8_t & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint8_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     uint8_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      uint8_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     uint8_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      uint8_t value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, bool & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, bool value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     bool & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      bool value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     bool & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      bool value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint16_t & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint16_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     uint16_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      uint16_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     uint16_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      uint16_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint32_t & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint32_t value,
                      boost::system::error_code & ec);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     uint32_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      uint32_t value, boost::system::error_code & ec);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     uint32_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      uint32_t value, boost::system::error_code & ec);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint64_t & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint64_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     uint64_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      uint64_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     uint64_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      uint64_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int8_t & value,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int8_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     int8_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      int8_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     int8_t & value, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      int8_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int16_t & value,
                     boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int16_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     int16_t & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      int16_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     int16_t & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      int16_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int32_t & value,
                     boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int32_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     int32_t & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      int32_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     int32_t & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      int32_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int64_t & value,
                     boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int64_t value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     int64_t & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      int64_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     int64_t & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      int64_t value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, std::string & value,
                     boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::string & value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     std::string & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      const std::string & value,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     std::string & value, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      const std::string & value,
                      boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call the
    //!  Write() member for a string.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(const std::string & s);

    //------------------------------------------------------------------------
    //!  Reads @c t from @c s, where @c t is an enumerated type.  Returns true
    //!  on success, false on failure.  Should only be used for enumerated
    //!  types with a fixed size underlying type.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     T & t, boost::system::error_code & ec)
      requires std::is_enum_v<T>
    {
      std::underlying_type_t<T>  cp;
      bool  rc = Read(s, cp, ec);
      if (rc) {
        t = static_cast<T>(cp);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes @c t to @c s, where @c t is an enumerated type.  Returns true
    //!  on success, false on failure.  Should only be used for enumerated
    //!  types with a fixed size underlying type.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const T & t, boost::system::error_code & ec)
      requires std::is_enum_v<T>
    {
      return Write(s, static_cast<std::underlying_type_t<T>>(t), ec);
    }

    //------------------------------------------------------------------------
    //!  Reads @c t from @c s, where @c t is an enumerated type.  Returns true
    //!  on success, false on failure.  Should only be used for enumerated
    //!  types with a fixed size underlying type.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     T & t, boost::system::error_code & ec)
      requires std::is_enum_v<T>
    {
      std::underlying_type_t<T>  cp;
      bool  rc = Read(s, cp, ec);
      if (rc) {
        t = static_cast<T>(cp);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes @c t to @c s, where @c t is an enumerated type.  Returns true
    //!  on success, false on failure.  Should only be used for enumerated
    //!  types with a fixed size underlying type.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      const T & t, boost::system::error_code & ec)
      requires std::is_enum_v<T>
    {
      return Write(s, static_cast<std::underlying_type_t<T>>(t), ec);
    }
        
    //------------------------------------------------------------------------
    //!  Reads @c t from @c s, where @c t is an enumerated type.  Returns true
    //!  on success, false on failure.  Should only be used for enumerated
    //!  types with a fixed size underlying type.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     T & t, boost::system::error_code & ec)
      requires std::is_enum_v<T>
    {
      std::underlying_type_t<T>  cp;
      bool  rc = Read(s, cp, ec);
      if (rc) {
        t = static_cast<T>(cp);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes @c t to @c s, where @c t is an enumerated type.  Returns true
    //!  on success, false on failure.  Should only be used for enumerated
    //!  types with a fixed size underlying type.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      const T & t, boost::system::error_code & ec)
      requires std::is_enum_v<T>
    {
      return Write(s, static_cast<std::underlying_type_t<T>>(t), ec);
    }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, float & val,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, float val,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     float & val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      float val, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     float & val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      float val, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, double & val,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, double val,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     double & val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      double val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::generic::stream_protocol::socket & s,
                     double & val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::generic::stream_protocol::socket & s,
                      double val, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from @c s.  Returns @c true on
    //!  success, @c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _firstT, typename _secondT>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::pair<_firstT, _secondT> & p,
                     boost::system::error_code & ec)
    {
      bool  rc = false;
      if (Read(s, p.first, ec)) {
        rc = Read(s, p.second, ec);
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to @c s.  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _firstT, typename _secondT>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::pair<_firstT,_secondT> & p,
                      boost::system::error_code & ec)
    {
      bool  rc = false;
      if (Write(s, p.first, ec)) {
        rc = Write(s, p.second, ec);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a tuple from @c s.  Returns @c true on success, @c false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename S, typename... Args>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::tuple<Args...> & t,
                     boost::system::error_code & ec)
    {
      return(std::apply([&s,&ec](auto&&...args) 
      {
        auto read_tuple_mem = [&s,&ec](auto&& x) { return Read(s, x, ec); };
        return (read_tuple_mem(args) && ...);
      }, t));
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to @c s.  Returns @c true on success, @c false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename S, typename... Args>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::tuple<Args...> & t,
                      boost::system::error_code & ec)
    {
      return(std::apply([&s,&ec](auto&&...args)
      {
        auto write_tuple_mem = [&s,&ec](auto&& x) { return Write(s, x, ec); };
        return (write_tuple_mem(args) && ...);
      }, t));
    }
    
    //------------------------------------------------------------------------
    //!  Reads a variant @c v from @c s.  Returns @c true on success, @c false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename S, typename... Ts>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::variant<Ts...> & v,
                     boost::system::error_code & ec)
    {
      bool  rc = false;
      uint64_t  index = 0;
      if (Read(s, index, ec)) {
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          rc = true;
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { rc &= Read(s, arg, ec); }, v);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a variant @c v to @c s.  Returns @c true on sucess, @c false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename S, typename... Ts>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::variant<Ts...> & v,
                      boost::system::error_code & ec)
    {
      bool  rc = false;
      uint64_t  index = v.index();
      if (Write(s, index, ec)) {
        rc = true;
        std::visit([&] (const auto & arg) { rc &= Write(s, arg, ec); }, v);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads an array<_valueT,N> @c v from @c s.  Returns @c true on
    //!  success, @c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, size_t N>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::array<_valueT, N> & a,
                     boost::system::error_code & ec)
    {
      size_t  i = 0;
      for ( ; i < N; ++i) {
        if (! Read(s, a[i], ec)) {
          break;
        }
      }
      return (N == i);
    }

    //------------------------------------------------------------------------
    //!  Writes an array<_valueT.N> @c v to @c s.  Returns @c true on success,
    //!  @c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, size_t N>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::array<_valueT, N> & a,
                      boost::system::error_code & ec)
    {
      size_t  i = 0;
      for ( ; i < N; ++i) {
        if (! Write(s, a[i], ec)) {
          break;
        }
      }
      return (N == i);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S, typename T>
    requires IsSupportedASIOSocket<S>
    and (Concepts::is_std_associative_container<T>
         or (Concepts::is_std_sequence_container<T>
             and (not Concepts::is_std_array<T>)))
    static bool Read(S & s, T & c, boost::system::error_code & ec)
    {
      bool  rc = false;
      c.clear();
      uint64_t  numEntries;
      if (Read(s, numEntries, ec)) {
        uint64_t  i = 0;
        for ( ; i < numEntries; ++i) {
          typename T::value_type  val;
          if (! Read(s, val, ec)) {
            break;
          }
          c.insert(c.end(), std::move(val));
        }
        rc = (i == numEntries);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a container @c c to a socket @c s.  Returns true on success,
    //!  false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename T>
    requires IsSupportedASIOSocket<S>
    and (Concepts::is_std_associative_container<T>
         or Concepts::is_std_pair_associative_container<T>
         or (Concepts::is_std_sequence_container<T>
             and (not Concepts::is_std_array<T>)))
    static bool Write(S & s, const T & c, boost::system::error_code & ec)
    {
      bool  rc = false;
      uint64_t  numEntries = c.size();
      if (Write(s, numEntries, ec)) {
        if (numEntries) {
          auto it = c.cbegin();
          for ( ; it != c.cend(); ++it) {
            if (! Write(s, *it, ec)) {
              break;
            }
          }
          rc = (it == c.cend());
        }
        else {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a vector<bool> @c v from socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::vector<bool, _Alloc> & v,
                     boost::system::error_code & ec) 
    {
      bool  rc = false;
      v.clear();
      uint64_t  numEntries;
      if (Read(s, numEntries, ec)) {
        uint64_t  i = 0;
        for ( ; i < numEntries; ++i) {
          bool  val;
          if (! Read(s, val, ec)) {
            break;
          }
          v.insert(v.end(), std::move(val));
        }
        rc = (i == numEntries);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<bool> @c v to socket @c s.  Returns true on success,
    //!  false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::vector<bool, _Alloc> & v,
                      boost::system::error_code & ec) 
    {
      bool  rc = false;
      uint64_t  numEntries = v.size();
      if (Write(s, numEntries, ec)) {
        rc = true;
        if (numEntries) {
          for (auto it = v.cbegin(); it != v.cend(); ++it) {
            bool  val = *it;
            if (! Write(s, val, ec)) {
              rc = false;
              break;
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a pair-associative container @c m from socket @c s.  Returns
    //!  @c true on success, @c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename T>
    requires IsSupportedASIOSocket<S>
      and Concepts::is_std_pair_associative_container<T>
    static bool Read(S & s, T & m, boost::system::error_code & ec)
    {
      return(PairAssocContRead<S,T>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c s.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, ASIOReadable & val,
                     boost::system::error_code & ec)
    { return val.Read(s, ec); }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c s.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, HasAsioRead auto & val,
                     boost::system::error_code & ec)
    { return val.Read(s, ec); }
      
    //------------------------------------------------------------------------
    //!  Writes @c val to @c s.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const ASIOWritable & val,
                      boost::system::error_code & ec)
    { return val.Write(s, ec); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c s.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const HasAsioWrite auto & val,
                      boost::system::error_code & ec)
    { return val.Write(s, ec); }
    
    //------------------------------------------------------------------------
    //!  Reads @c args from @c s.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename... Args>
    requires IsSupportedASIOSocket<S>
    static bool ReadV(S & s, boost::system::error_code & ec, Args & ...args)
    {
      return (Read(s,args,ec) && ...);
    }

    //------------------------------------------------------------------------
    //!  Writes @c args to @c s.  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename... Args>
    requires IsSupportedASIOSocket<S>
    static bool WriteV(S & s, boost::system::error_code & ec,
                       const Args & ...args)
    {
      return (Write(s,args,ec) && ...);
    }

    //------------------------------------------------------------------------
    //!  Writes a bounded array @c v to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static bool Write(IsSupportedASIOSocket auto & s, T const & v,
                      boost::system::error_code & ec)
    {
      static_assert(asio_detail::IsWritable<std::remove_all_extents_t<T>>);
      bool      rc = false;
      uint64_t  n = std::extent_v<T>;
      if (ASIO::Write(s, n, ec)) {
        size_t i = 0;
        for ( ; i < std::extent_v<T>; ++i) {
          if (! ASIO::Write(s, v[i], ec)) {
            break;
          }
        }
        rc = (std::extent_v<T> == i);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a bounded array @c v from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static bool Read(IsSupportedASIOSocket auto & s, T & v,
                      boost::system::error_code & ec)
    {
      static_assert(asio_detail::IsReadable<std::remove_all_extents_t<T>>);
      bool      rc = false;
      uint64_t  n;
      if (ASIO::Read(s, n, ec)) {
        if (std::extent_v<T> == n) {
          size_t i = 0;
          for ( ; i < std::extent_v<T>; ++i) {
            if (! ASIO::Read(s, v[i], ec)) {
              break;
            }
          }
          rc = (std::extent_v<T> == i);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, if it points to a single
    //!  object (deduced by requiring std::default_delete as its deleter).
    //------------------------------------------------------------------------
    template <typename T>
    static bool Write(IsSupportedASIOSocket auto & s,
                      const std::unique_ptr<T> & t,
                      boost::system::error_code & ec)
    {
      using deleterType = std::remove_cvref_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType,std::default_delete<T>>);
      static_assert(! std::is_unbounded_array_v<T>);
      static_assert(asio_detail::IsWritable<T>);
      bool  rc = false;
      bool  isNull = (nullptr == t);
      if (ASIO::Write(s, isNull, ec)) {
        rc = true;
        if (! isNull) {
          rc = ASIO::Write(s, *t, ec);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, if it points to a single
    //!  object (deduced by requiring std::default_delete as its deleter).
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_default_constructible_v<T>
    static bool Read(IsSupportedASIOSocket auto & s,
                     std::unique_ptr<T> & t,
                     boost::system::error_code & ec)
    {
      using deleterType = std::remove_reference_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType,std::default_delete<T>>);
      static_assert(! std::is_unbounded_array_v<T>);
      static_assert(asio_detail::IsReadable<T>);
      bool  rc = false;
      bool  isNull = true;
      if (ASIO::Read(s, isNull, ec)) {
        if (isNull) {
          t.release();
          rc = true;
        }
        else {
          if (nullptr == t) {
            try {
              t = std::make_unique<T>();
            }
            catch (std::bad_alloc & ex) {
              FSyslog(LOG_ERR, "Failed to allocate an object of type {}",
                      TypeName<decltype(t)>());
              return false;
            }
          }
          if (ASIO::Read(s, *t, ec)) {
            rc = true;
          }
          else {
            t.release();
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a std::optional<T> to @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename T>
    static bool Write(IsSupportedASIOSocket auto & s,
                      const std::optional<T> & t,
                      boost::system::error_code & ec)
    {
      static_assert(asio_detail::IsWritable<T>);
      bool  rc = false;
      bool  hasValue = t.has_value();
      if (ASIO::Write(s, hasValue, ec)) {
        rc = true;
        if (hasValue) {
          rc = ASIO::Write(s, t.value(), ec);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a std::optional<T> from @c s.  Returns true on success, false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_default_constructible_v<T>
    static bool Read(IsSupportedASIOSocket auto & s, std::optional<T> & t,
                     boost::system::error_code & ec)
    {
      static_assert(asio_detail::IsReadable<T>);
      bool  rc = false;
      bool  hasValue = false;
      if (ASIO::Read(s, hasValue, ec)) {
        if (hasValue) {
          if (! t.has_value()) {
            t = T();
          }
          rc = ASIO::Read(s, t.value(), ec);
        }
        else {
          t.reset();
          rc = true;
        }
      }
      return rc;
    }
    
#if defined(DWM_CAN_USE_REFLECTION)
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
      and (not io_detail::DirectlySupported<T>)
      and (not io_detail::SupportedContainer<T>)
      and (not io_detail::DenyType<T>)
      and (not HasAsioWrite<T>)
    static bool Write(IsSupportedASIOSocket auto & s, const T & v,
                      boost::system::error_code & ec)
    {
      using asio_detail::IsWritable;
      using Dwm::io_detail::Skip;
      using Dwm::io_detail::SkipReason;
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (Skip<decltype(v.[:mem:]),mem>()) {
          FSyslog(LOG_INFO, "{}.{} of type '{}' skipped{}",
                  TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                  std::meta::display_string_of(std::meta::type_of(mem)),
                  SkipReason<decltype(v.[:mem:]),mem>());
        }
        else {
          if constexpr (IsWritable<typename[:std::meta::type_of(mem):]>) {
            if (! ASIO::Write(s, v.[:mem:], ec)) {
              return false;
            }
          }
          else {
            FSyslog(LOG_ERR, "{}.{} of type '{}' is unwritable{}",
                    TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                    std::meta::display_string_of(std::meta::type_of(mem)),
                    DenyReason<mem>());
            return false;
          }
        }
      }
      return (! ec);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
      and (not io_detail::DirectlySupported<T>)
      and (not io_detail::SupportedContainer<T>)
      and (not io_detail::DenyType<T>)
      and (not HasAsioRead<T>)
    static bool Read(IsSupportedASIOSocket auto & s, T & v,
                     boost::system::error_code & ec)
    {
      using asio_detail::IsReadable;
      using Dwm::io_detail::Skip;
      using Dwm::io_detail::SkipReason;
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                      define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (Skip<decltype(v.[:mem:]),mem>()) {
          FSyslog(LOG_INFO, "{}.{} of type '{}' skipped{}",
                  TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                  std::meta::display_string_of(std::meta::type_of(mem)),
                  SkipReason<decltype(v.[:mem:]),mem>());
        }
        else {
          if constexpr (IsReadable<typename[:std::meta::type_of(mem):]>) {
            if (! ASIO::Read(s, v.[:mem:], ec)) {
              return false;
            }
          }
          else {
            FSyslog(LOG_ERR, "{}.{} of type '{}' is unreadable{}",
                    TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                    std::meta::display_string_of(std::meta::type_of(mem)),
                    DenyReason<mem>());
            return false;
          }
        }
      }
      return (! ec);
    }

#endif  // defined(DWM_CAN_USE_REFLECTION)
    
  private:
    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns true.
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>      
    static bool Read(S & s, std::monostate & sm,
                     boost::system::error_code & ec)
    {
      return true;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns true.
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>      
    static bool Write(S & s, const std::monostate & sm,
                      boost::system::error_code & ec)
    {
      return true;
    }

    //------------------------------------------------------------------------
    //!  Reads a PairAssociative container from an istream.  Returns the
    //!  istream.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename S, typename _containerT>
    requires IsSupportedASIOSocket<S>
    static bool PairAssocContRead(S & s, _containerT & m,
                                  boost::system::error_code & ec)
    {
      bool  rc = false;
      m.clear();
      uint64_t  numEntries;
      if (Read(s, numEntries, ec)) {
        uint64_t i = 0;
        for ( ; i < numEntries; ++i) {
          typename _containerT::key_type  key;
          if (Read(s, key, ec)) {
            typename _containerT::mapped_type  val;
            if (Read(s, val, ec)) {
              m.insert(typename _containerT::value_type(std::move(key),
                                                        std::move(val)));
            }
            else {
              break;
            }
          }
          else {
            break;
          }
        }
        rc = (i == numEntries);
      }
      return rc;
    }

  };  // class ASIO

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be written
  //!  to a supported asio socket via an ASIO::Write() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsASIOWritable =
  (asio_detail::IsWritable<T> == true)
  and requires(const T & t, boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) {
    { ASIO::Write(s, t, ec) } -> std::same_as<bool>;
  }
  and requires(const T & t, boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) {
    { ASIO::Write(s, t, ec) } -> std::same_as<bool>;
  }
  and requires(const T & t, boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) {
    { ASIO::Write(s, t, ec) } -> std::same_as<bool>;
  };
  
  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be read from
  //!  a supported asio socket via am ASIO::Read() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsASIOReadable =
  (asio_detail::IsReadable<T> == true)
  and requires(T & t, boost::asio::ip::tcp::socket & s,
               boost::system::error_code & ec) {
    { ASIO::Read(s, t, ec) } -> std::same_as<bool>;
  }
  and requires(T & t, boost::asio::generic::stream_protocol::socket & s,
               boost::system::error_code & ec) {
    { ASIO::Read(s, t, ec) } -> std::same_as<bool>;
  }
  and requires(T & t, boost::asio::local::stream_protocol::socket & s,
               boost::system::error_code & ec) {
    { ASIO::Read(s, t, ec) } -> std::same_as<bool>;
  };
  
  
}  // namespace Dwm

#endif  // _DWMASIO_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
