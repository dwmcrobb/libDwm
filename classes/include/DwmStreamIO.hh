//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016, 2017, 2020, 2024
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
//!  \file DwmStreamIO.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::StreamIO class declaration
//---------------------------------------------------------------------------

#ifndef _DWMSTREAMIO_HH_
#define _DWMSTREAMIO_HH_

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "DwmPortability.hh"
#include "DwmStreamIOCapable.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class contains a collection of static functions for reading and
  //!  writing simple types, in network byte order (MSB first).  It also
  //!  contains functions to read and write strings.  It also contains
  //!  function templates to read and write arrays, deques, lists, vectors,
  //!  maps, multimaps, sets, multisets, unordered_maps, unordered_multimaps,
  //!  unordered_sets, unordered_multisets, tuples and variants.
  //!  We use our member functions to handle reading and writing simple
  //!  types in the containers, and function templates to handle reading 
  //!  and writing other class types.  For a user-defined class, the class
  //!  must implement the StreamReadable and StreamWritable interfaces, since
  //!  our function templates simply call out to them.  Since templates are
  //!  static polymorphism, you don't need to inherit from StreamReadable and
  //!  StreamWritable, but you must implement the interfaces.
  //--------------------------------------------------------------------------
  class StreamIO
  {
  public:
    //------------------------------------------------------------------------
    //!  Reads \c c from \c is.  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, char & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c os.  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, char c);

    //------------------------------------------------------------------------
    //!  Reads \c c from \c is.  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, int8_t & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c os.  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, int8_t c);

    //------------------------------------------------------------------------
    //!  Reads \c c from \c is.  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c os.  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, uint8_t c);

    //------------------------------------------------------------------------
    //!  Reads \c b from \c is.  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, bool & b);
    
    //------------------------------------------------------------------------
    //!  Writes a bool \c b to an ostream \c os.  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, bool b);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in network byte order (MSB first).
    //!  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, int16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in network byte order (MSB first).
    //!  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, int16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in network byte order (MSB first).
    //!  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, uint16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in network byte order (MSB first).
    //!  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, uint16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in network byte order (MSB first).
    //!  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, int32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in network byte order (MSB first).
    //!  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, int32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in network byte order (MSB first).
    //!  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, uint32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in network byte order (MSB first).
    //!  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, uint32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in network byte order (MSB first).
    //!  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, int64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in network byte order (MSB first).
    //!  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, const int64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in network byte order (MSB first).
    //!  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, uint64_t & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in network byte order (MSB first).
    //!  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, const uint64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, float & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, float val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c is, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c is.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, double & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c os, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c os.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, const double & val);

    //------------------------------------------------------------------------
    //!  Reads string \c s from \c is.  Since we write strings with a 
    //!  64-bit length value preceding, and always write the terminating
    //!  NULL, this function will always read at least 9 bytes on success.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, std::string & s);

    //------------------------------------------------------------------------
    //!  Writes \c s to \c os.  Note that the length of \c s is written
    //!  first, as an unsigned 64-bit number in network byte order
    //!  (MSB first).  Hence at least 9 bytes will always be written;
    //!  4 for the length and 1 for the terminating NULL character of
    //!  an empty string.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, const std::string & s);

    //------------------------------------------------------------------------
    //!  Reads \c t from \c is, where \c t is an enumerated type.  Returns
    //!  \c is.  Note that this is risky for enum types with an underlying
    //!  type whose size is not fixed.
    //------------------------------------------------------------------------
    template <typename T>
    static std::istream & Read(std::istream & is, T & t)
      requires std::is_enum_v<T>
    {
      std::underlying_type_t<T>  cp;
      if (Read(is, cp)) {
        t = static_cast<T>(cp);
      }
      return is;
    }
    
    //------------------------------------------------------------------------
    //!  Writes \c t to \c os, where \c t is an enumerated type.  Returns
    //!  \c os.  Note that this is risky for enum types with an underlying
    //!  type whose size is not fixed.
    //------------------------------------------------------------------------
    template <typename T>
    static std::ostream & Write(std::ostream & os, const T & t)
      requires std::is_enum_v<T>
    {
      return Write(os, static_cast<std::underlying_type_t<T>>(t));
    }
    
    //------------------------------------------------------------------------
    //!  Wrapper function to read a StreamReadable object from an istream.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, StreamReadable & val)
    {
      return(val.Read(is));
    }

    //------------------------------------------------------------------------
    //!  Reads an object @c t that meets the HasStreamRead requirement from
    //!  an istream.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, HasStreamRead auto & t)
    { return t.Read(is); }
     
    //------------------------------------------------------------------------
    //!  Wrapper function to write a StreamWritable object to an ostream.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, const StreamWritable & val)
    {
      return(val.Write(os));
    }

    //------------------------------------------------------------------------
    //!  Writes an object that meets the HasStreamWrite requirement to an
    //!  ostream.
    //------------------------------------------------------------------------
    static std::ostream &
    Write(std::ostream & os, const HasStreamWrite auto & t)
    { return t.Write(os); }
      
    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static std::istream & Read(std::istream & is,
                               std::pair<_firstT, _secondT> & p)
    {
      if (is) {
        if (Read(is, p.first))
          Read(is, p.second);
      }
      return(is);
    }
    
    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static std::ostream & Write(std::ostream & os,
                                const std::pair<_firstT,_secondT> & p)
    {
      if (os) {
        if (Write(os, p.first)) {
          Write(os, p.second);
        }
      }
      return(os);
    }

    //------------------------------------------------------------------------
    //!  Reads a map<_keyT,_valueT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(is, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static std::ostream & Write(std::ostream & os,
                                const std::map<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::map<_keyT,_valueT,_Compare,_Alloc> >(os, m));
    }

    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> from an istream.  Returns the 
    //!  istream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static std::istream &
    Read(std::istream & is, std::multimap<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(PairAssocContRead<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(is, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static std::ostream & 
    Write(std::ostream & os,
          const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(os, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an array<_valueT,N> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static std::istream & Read(std::istream & is,
                               std::array<_valueT, N> & a)
    {
      if (is) {
        for (size_t i = 0; i < N; ++i) {
          if (! Read(is, a[i])) {
            break;
          }
        }
      }
      return is;
    }

    //------------------------------------------------------------------------
    //!  Writes an array<_valueT,N> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static std::ostream & Write(std::ostream & os,
                                const std::array<_valueT, N> & a)
    {
      if (os) {
        for (size_t i = 0; i < N; ++i) {
          if (! Write(os, a[i])) {
            break;
          }
        }
      }
      return os;
    }
    
    //------------------------------------------------------------------------
    //!  Reads a vector<_valueT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerRead<std::vector<_valueT, _Alloc> >(is, v));
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static std::ostream & Write(std::ostream & os,
                                const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerWrite<std::vector<_valueT, _Alloc> >(os, v));
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerRead<std::deque<_valueT, _Alloc> >(is, d));
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static std::ostream & Write(std::ostream & os,
                                const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerWrite<std::deque<_valueT, _Alloc> >(os, d));
    }

    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::list<_valueT, _Alloc> & l)
    {
      return(ContainerRead<std::list<_valueT, _Alloc> >(is, l));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static std::ostream & Write(std::ostream & os,
                                const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerWrite<std::list<_valueT, _Alloc> >(os, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::set<_valueT, _Compare, _Alloc> >(is, l));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static std::ostream & Write(std::ostream & os,
                                const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::set<_valueT, _Compare, _Alloc> >(os, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::multiset<_valueT, _Compare, _Alloc> >(is, l));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static std::ostream & 
    Write(std::ostream & os, 
          const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::multiset<_valueT, _Compare, _Alloc> >(os, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a tuple from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename... Args>
    static std::istream & Read(std::istream & is, 
                               std::tuple<Args...> & t)
    {
      std::apply([&is](auto&&... args) {((Read(is,args)) && ...);}, t);
      return is;
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename... Args>
    static std::ostream & Write(std::ostream & os, 
                                const std::tuple<Args...> & t)
    {
      std::apply([&os](auto&&... args) {((Write(os,args)) && ...);}, t);
      return os;
    }
    
    //------------------------------------------------------------------------
    //!  Reads an unordered_map<_keyT,_valueT> from an istream.  Returns 
    //!  the istream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static std::istream & 
    Read(std::istream & is,
         std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContRead<std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> >(is, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_map<_keyT,_valueT> to an ostream.  Returns the
    //!  ostream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static std::ostream & 
    Write(std::ostream & os,
          const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(os, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multimap<_keyT,_valueT> from an istream.  Returns 
    //!  the istream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT,
              typename _Hash, typename _Pred, typename _Alloc>
    static std::istream & Read(std::istream & is,
                               std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContRead<std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> >(is, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_multimap<_keyT,_valueT> to an ostream.  Returns 
    //!  the ostream.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static std::ostream & 
    Write(std::ostream & os,
          const std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerWrite<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(os, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_set<_valueT> from an istream.  Returns 
    //!  the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash, 
              typename _Pred, typename _Alloc>
    static std::istream & 
    Read(std::istream & is,
         std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerRead<std::unordered_set<_valueT, _Hash, _Pred, _Alloc> >(is, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_set<_valueT> to an ostream.  Returns the
    //!  ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static std::ostream & 
    Write(std::ostream & os,
          const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(os, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multiset<_valueT> from an istream.  Returns 
    //!  the istream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash, 
              typename _Pred, typename _Alloc>
    static std::istream & 
    Read(std::istream & is,
         std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerRead<std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> >(is, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_multiset<_valueT> to an ostream.  Returns the
    //!  ostream.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static std::ostream & 
    Write(std::ostream & os,
          const std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerWrite<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(os, m));
    }
    
    //------------------------------------------------------------------------
    //!  Reads a variant from an istream.  Returns the istream.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static std::istream & Read(std::istream & is,
                               std::variant<Ts...> & v)
    {
      uint64_t  index = 0;
      if (Read(is, index)) {
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { Read(is, arg); }, v);
        }
        else {
          is.setstate(std::ios_base::failbit);
        }
      }
      return is;
    }

    //------------------------------------------------------------------------
    //!  Writes a variant to an ostream.  Returns the ostream.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static std::ostream & Write(std::ostream & os,
                                const std::variant<Ts...> & v)
    {
      uint64_t  index = v.index();
      if (Write(os, index)) {
        std::visit([&os] (const auto & arg) { Write(os, arg); }, v);
      }
      return os;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns the
    //!  given istream.
    //------------------------------------------------------------------------
    static std::istream & Read(std::istream & is, std::monostate & sm)
    {
      return is;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns the
    //!  given ostream.
    //------------------------------------------------------------------------
    static std::ostream & Write(std::ostream & os, const std::monostate & sm)
    {
      return os;
    }

    //------------------------------------------------------------------------
    //!  Reads multiple objects from an istream.  Returns the istream.  This
    //!  is just a convenience function.
    //------------------------------------------------------------------------
    template <typename... Args>
    static std::istream & ReadV(std::istream & is, Args & ...args)
    {
      (Read(is,args) &&...);
      return is;
    }

    //------------------------------------------------------------------------
    //!  Writes multiple objects to an ostream.  Returns the ostream.  This
    //!  is just a convenience function.
    //------------------------------------------------------------------------
    template <typename... Args>
    static std::ostream & WriteV(std::ostream & os, const Args & ...args)
    {
      (Write(os,args) &&...);
      return os;
    }
    
  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static std::ostream & Write(std::ostream & os,
                                _inputIteratorT f, _inputIteratorT l)
    {
      if (os) {
        for ( ; f != l; ++f) {
          if (! Write(os, *f)) {
            break;
          }
        }
      }
      return(os);
    }

    //------------------------------------------------------------------------
    //!  Reads a _containerT from an istream.  Returns the istream.
    //!  We use this for deques, lists, vectors, sets and multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static std::istream & ContainerRead(std::istream & is,
                                        _containerT & c)
    {
      if (! c.empty())
        c.clear();
      if (is) {
        uint64_t  numEntries;
        if (Read(is, numEntries)) {
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::value_type  val;
            if (! Read(is, val))
              break;
            c.insert(c.end(), std::move(val));
          }
        }
      }
      return(is);
    }

    //------------------------------------------------------------------------
    //!  Writes a container to an ostream.  Returns the ostream.
    //!  We use this for all containers.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static std::ostream & ContainerWrite(std::ostream & os, 
                                         const _containerT & c)
    {
      if (os) {
        uint64_t  numEntries = c.size();
        if (Write(os, numEntries)) {
          if (numEntries) {
            Write<typename _containerT::const_iterator>(os, 
                                                        c.begin(), 
                                                        c.end());
          }
        }
      }
      return(os);
    }

    //------------------------------------------------------------------------
    //!  Reads a PairAssociative container from an istream.  Returns the
    //!  istream.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static std::istream & 
    PairAssocContRead(std::istream & is, _containerT & m)
    {
      if (! m.empty())
        m.clear();
      if (is) {
        uint64_t  numEntries;
        if (Read(is, numEntries)) {
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::key_type  key;
            if (Read(is, key)) {
              typename _containerT::mapped_type  val;
              if (Read(is, val))
                m.insert(typename _containerT::value_type(std::move(key), std::move(val)));
              else
                break;
            }
            else
              break;
          }
        }
      }
      return(is);
    }
    
  };

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be written
  //!  to an ostream via a StreamIO::Write() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsStreamWritable = requires(const T & t, std::ostream & os) {
    { StreamIO::Write(os, t) } -> std::same_as<std::ostream &>;
  };

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be read from
  //!  an istream via a StreamIO::Read() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsStreamReadable = requires(T & t, std::istream & is) {
    { StreamIO::Read(is, t) } -> std::same_as<std::istream &>;
  };

}  // namespace Dwm

#endif  // _DWMSTREAMIO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
