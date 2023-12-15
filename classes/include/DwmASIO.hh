//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020, 2023
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

#include <array>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "DwmASIOCapable.hh"
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
    || std::is_same_v<S,boost::asio::local::stream_protocol::socket>;

  //--------------------------------------------------------------------------
  //!  A collection of functions for reading from and writing to
  //!  boost::asio::ip::tcp::socket.
  //--------------------------------------------------------------------------
  class ASIO
  {
  public:
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
    //!  Returns the number of bytes that should be written if we call the
    //!  Write() member for a string.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(const std::string & s);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, float & val,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, float val,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     float & val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      float val, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, double & val,
                     boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, double val,
                      boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::local::stream_protocol::socket & s,
                     double & val, boost::system::error_code & ec);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::local::stream_protocol::socket & s,
                      double val, boost::system::error_code & ec);
    
    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from \c s.  Returns \c true on
    //!  success, \c false on failure.
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
    //!  Writes a pair<_firstT,_secondT> to \c s.  Returns \c true on success,
    //!  \c false on failure.
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
    //!  Reads a tuple from \c s.  Returns \c true on success, \c false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename S, typename T, typename... Args>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::tuple<T, Args...> & t,
                     boost::system::error_code & ec)
    {
      return ReadTuple<S,std::tuple<T, Args...> >(s, t, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to \c s.  Returns \c true on success, \c false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename S, typename T, typename... Args>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::tuple<T, Args...> & t,
                      boost::system::error_code & ec)
    {
      return WriteTuple<S,std::tuple<T, Args...> >(s, t, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads a variant \C V from \c s.  Returns \c true on success, \c false
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
    //!  Writes a variant \c v to \c s.  Returns \c true on sucess, \c false
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
    //!  Reads an array<_valueT,N> \c v from \c s.  Returns \c true on
    //!  success, \c false on failure.
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
    //!  Reads a vector<_valueT> \c v from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::vector<_valueT, _Alloc> & v,
                     boost::system::error_code & ec)
    {
      return ContainerRead<S,std::vector<_valueT, _Alloc> >(s, v, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> \c v to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::vector<_valueT, _Alloc> & v,
                      boost::system::error_code & ec)
    {
      return ContainerWrite<S,std::vector<_valueT, _Alloc> >(s, v, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> \c d from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::deque<_valueT, _Alloc> & d,
                     boost::system::error_code & ec)
    {
      return ContainerRead<S,std::deque<_valueT, _Alloc> >(s, d, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> \c d to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::deque<_valueT, _Alloc> & d,
                      boost::system::error_code & ec)
    {
      return ContainerWrite<S,std::deque<_valueT, _Alloc> >(s, d, ec);
    }
    
    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> \c l from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::list<_valueT, _Alloc> & l,
                     boost::system::error_code & ec)
    {
      return ContainerRead<S,std::list<_valueT, _Alloc> >(s, l, ec);
    }

    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> \c l to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::list<_valueT, _Alloc> & l,
                      boost::system::error_code & ec)
    {
      return ContainerWrite<S,std::list<_valueT, _Alloc> >(s, l, ec);
    }

    //------------------------------------------------------------------------
    //!  Reads a map<_keyT,_valueT> \c m from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::map<_keyT, _valueT, _Compare, _Alloc> & m,
                     boost::system::error_code & ec)
    {
      return(PairAssocContRead<S,std::map<_keyT, _valueT, _Compare, _Alloc> >(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> \c m to \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s,
                      const std::map<_keyT,_valueT, _Compare, _Alloc> & m,
                      boost::system::error_code & ec)
    {
      return(ContainerWrite<S,std::map<_keyT,_valueT,_Compare,_Alloc> >(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> \c m from \c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::multimap<_keyT,_valueT,_Compare,_Alloc> & m,
                     boost::system::error_code & ec)
    {
      return(PairAssocContRead<S,std::multimap<_keyT,_valueT,_Compare,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> \c m to \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool
    Write(S & s,
          const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m,
          boost::system::error_code & ec)
    {
      return(ContainerWrite<S,std::multimap<_keyT,_valueT,_Compare,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_map<_keyT,_valueT> \c m from \c s.  Returns 
    //!  \c true on success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool
    Read(S & s, std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m,
         boost::system::error_code & ec)
    {
      return(PairAssocContRead<S,std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> >(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_map<_keyT,_valueT> \c m to \c s.  Returns \c true
    //!  on success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool
    Write(S & s,
          const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m,
          boost::system::error_code & ec)
    {
      return(ContainerWrite<S,std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> \c m from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s,
                     std::set<_valueT,_Compare,_Alloc> & m,
                     boost::system::error_code & ec)
    {
      return(ContainerRead<S,std::set<_valueT,_Compare,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> \c m to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const std::set<_valueT,_Compare,_Alloc> & m,
                      boost::system::error_code & ec)
    {
      return(ContainerWrite<S,std::set<_valueT,_Compare,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> \c m from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, std::multiset<_valueT,_Compare,_Alloc> & m,
                     boost::system::error_code & ec)
    {
      return(ContainerRead<S,std::multiset<_valueT,_Compare,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> \c m to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Compare, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s,
                      const std::multiset<_valueT,_Compare,_Alloc> & m,
                      boost::system::error_code & ec)
    {
      return(ContainerWrite<S,std::multiset<_valueT,_Compare,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_set<_valueT> \c m from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Hash, 
              typename _Pred, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool
    Read(S & s, std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m,
         boost::system::error_code & ec)
    {
      return(ContainerRead<S,std::unordered_set<_valueT,_Hash,_Pred,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_set<_valueT> \c m to \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename S, typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    requires IsSupportedASIOSocket<S>
    static bool
    Write(S & s, const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m,
          boost::system::error_code & ec)
    {
      return(ContainerWrite<S,std::unordered_set<_valueT,_Hash,_Pred,_Alloc>>(s, m, ec));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    static bool Read(S & s, ASIOReadable & val,
                     boost::system::error_code & ec)
    { return val.Read(s, ec); }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, const ASIOWritable & val,
                      boost::system::error_code & ec)
    { return val.Write(s, ec); }
    
  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S, typename T>
    requires IsSupportedASIOSocket<S>
    static bool ReadTuple(S & s, T & t, boost::system::error_code & ec)
    {
      return TupleIOHelper<T,std::tuple_size<T>::value-1>::Read(s, t, ec);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S, typename T>
    requires IsSupportedASIOSocket<S>
    static bool WriteTuple(S & s, const T & t,
                           boost::system::error_code & ec)
    {
      return TupleIOHelper<T,std::tuple_size<T>::value-1>::Write(s, t, ec);
    }
    
    //------------------------------------------------------------------------
    //!  Declare tuple IO helper class template.  elt is the last element
    //!  index (size of the tuple minus 1).
    //------------------------------------------------------------------------
    template <typename T, size_t elt>
    class TupleIOHelper;

    //------------------------------------------------------------------------
    //!  Specialization for a tuple with one element.
    //------------------------------------------------------------------------
    template <typename T>
    class TupleIOHelper<T, 0>
    {
    public:
      //----------------------------------------------------------------------
      //!  Read a tuple \c t from \c s.  Returns \c true on success, \c false
      //!  on failure.
      //----------------------------------------------------------------------
      template <typename S>
      requires IsSupportedASIOSocket<S>
      static bool Read(S & s, T & t, boost::system::error_code & ec)
      {
        return(ASIO::Read(s, std::get<0>(t), ec));
      }

      //----------------------------------------------------------------------
      //!  Write a tuple \c t to \c s.  Returns \c true on success, \c false
      //!  on failure.
      //----------------------------------------------------------------------
      template <typename S>
      requires IsSupportedASIOSocket<S>
      static bool Write(S & s, const T & t,
                        boost::system::error_code & ec)
      {
        return(ASIO::Write(s, std::get<0>(t), ec));
      }
    };

    //------------------------------------------------------------------------
    //!  The recursive tuple ASIO helper template.
    //------------------------------------------------------------------------
    template <typename T, size_t elt>
    class TupleIOHelper
    {
    public:
      //----------------------------------------------------------------------
      //!  Read a tuple \c t from \c s.  Returns true on success, \c false on
      //!  failure.
      //----------------------------------------------------------------------
      template <typename S>
      requires IsSupportedASIOSocket<S>      
      static bool Read(S & s, T & t, boost::system::error_code & ec)
      {
        bool  rc = false;
        if (TupleIOHelper<T,elt-1>::Read(s, t, ec)) {
          rc = ASIO::Read(s, std::get<elt>(t), ec);
        }
        return rc;
      }
      
      //----------------------------------------------------------------------
      //!  Write a tuple \c t to an ostream \c os.
      //----------------------------------------------------------------------
      template <typename S>
      requires IsSupportedASIOSocket<S>      
      static bool Write(S & s, const T & t,
                        boost::system::error_code & ec)
      {
        bool  rc = false;
        if (TupleIOHelper<T,elt-1>::Write(s, t, ec)) {
          rc = ASIO::Write(s, std::get<elt>(t), ec);
        }
        return rc;
      }
    };

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
    //!  Reads a _containerT from \c s.  Returns \c true on success, \c false
    //!  on failure.  We use this for deques, lists, vectors, sets and
    //!  multisets.
    //------------------------------------------------------------------------
    template <typename S, typename _containerT>
    requires IsSupportedASIOSocket<S>
    static bool ContainerRead(S & s, _containerT & c,
                              boost::system::error_code & ec)
    {
      bool  rc = false;
      c.clear();
      uint64_t  numEntries;
      if (Read(s, numEntries, ec)) {
        uint64_t  i = 0;
        for ( ; i < numEntries; ++i) {
          typename _containerT::value_type  val;
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
    //!  Writes a container \c c to \c s.  Returns true on success, false on
    //!  failure.  We use this for all containers.
    //------------------------------------------------------------------------
    template <typename S, typename _containerT>
    requires IsSupportedASIOSocket<S>
    static bool ContainerWrite(S & s, const _containerT & c,
                               boost::system::error_code & ec)
    {
      bool  rc = false;
      uint64_t  numEntries = c.size();
      if (Write(s, numEntries, ec)) {
        if (numEntries) {
          rc = Write<S,typename _containerT::const_iterator>(s, 
                                                             c.begin(), 
                                                             c.end(),
                                                             ec);
        }
        else {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename S, typename _inputIteratorT>
    requires IsSupportedASIOSocket<S>
    static bool Write(S & s, _inputIteratorT f, _inputIteratorT l,
                      boost::system::error_code & ec)
    {
      for ( ; f != l; ++f) {
        if (! Write(s, *f, ec)) {
          break;
        }
      }
      return (f == l);
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
