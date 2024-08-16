//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004, 2016, 2020, 2024
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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmGZIO.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::GZIO class definition
//---------------------------------------------------------------------------

#ifndef _DWMGZIO_HH_
#define _DWMGZIO_HH_

extern "C" {
  #include <inttypes.h>
  #include <sys/types.h>
}

#include <array>
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
#include "DwmGZIOCapable.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class contains a collection of static functions for reading and
  //!  writing simple types in network byte order (MSB first) from/to gzip
  //!  files.  It also contains functions to read and write strings from/to
  //!  gzip files.  It also contains function templates to read and write 
  //!  arrays, deques, lists, vectors, maps, multimaps, sets, multisets,
  //!  unordered_maps, unordered_multimaps, unordered_sets,
  //!  unordered_multisets, tuples and variants from/to gzip files.  We use
  //!  our member functions to handle reading and writing simple types in
  //!  these containers, and function templates to handle reading and writing
  //!  other class types.  In the latter case, the class must implement the
  //!  GZReadable and GZWritable interfaces, since our function templates
  //!  simply call out to them.
  //--------------------------------------------------------------------------
  class GZIO
  {
  public:
    //------------------------------------------------------------------------
    //!  Reads \c from \c gzf.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, char & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c gzf.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, char c);

    //------------------------------------------------------------------------
    //!  Reads \c c from \c gzf.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c gzf.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, uint8_t c);

    //------------------------------------------------------------------------
    //!  Reads \c b from \c gzf.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, bool & b);
    
    //------------------------------------------------------------------------
    //!  Writes \c b to \c gzf.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, bool b);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, int16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c gzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, int16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, uint16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c gzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, uint16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, int32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c gzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, int32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, uint32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c gzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, uint32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, int64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c gzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const int64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, uint64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c gzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const uint64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (4)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, float & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val tp \c gzf, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, float val);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c gzf, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (8)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, double & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val tp \c gzf, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const double & val);
    
    //------------------------------------------------------------------------
    //!  Reads \c s from \c gzf.  Returns the number of bytes read on success,
    //!  -1 on failure.  Since we write strings with a 64-bit unsigned length
    //!  value preceding the actual string, and always have a terminating
    //!  NULL, this always reads at least 9 bytes on success.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, std::string & s);
    
    //------------------------------------------------------------------------
    //!  Writes \c s to \c gzf.  Returns the number of bytes written on
    //!  success, -1 on failure.  Note that a 64-bit value is written first,
    //!  holding the length of the string.  The terminating NULL is also
    //!  written.  Hence, on success this will always return a value of
    //!  9 or greater.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Wrapper function to read a GZReadable object from a gzFile.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, GZReadable & val)
    { return(val.Read(gzf)); }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c gzf, where @c val meets the requirements of
    //!  the HasGZRead concept.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, HasGZRead auto & val)
    { return val.Read(gzf); }
      
    //------------------------------------------------------------------------
    //!  Wrapper function to write a GZWritable object to a gzFile.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const GZWritable & val)
    { return(val.Write(gzf)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c gzf, where @c val meets the requirements of the
    //!  HasGZWrite concept.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const HasGZWrite auto & val)
    { return val.Write(gzf); }
    
    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from a gzFile.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static int Read(gzFile gzf, std::pair<_firstT, _secondT> & p)
    {
      int  rc = -1;
      if (gzf) {
        int  bytesRead = Read(gzf, p.first);
        if (bytesRead > 0) {
          rc = bytesRead;
          bytesRead = Read(gzf, p.second);
          if (bytesRead > 0)
            rc += bytesRead;
          else
            rc = -1;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to a gzFile.  Returns
    //!  the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static int Write(gzFile gzf, const std::pair<_firstT,_secondT> & p)
    {
      int  rc = -1;
      if (gzf) {
        int  bytesWritten = Write(gzf, p.first);
        if (bytesWritten > 0) {
          rc = bytesWritten;
          bytesWritten = Write(gzf, p.second);
          if (bytesWritten > 0) {
            rc += bytesWritten;
          }
          else {
            rc = -1;
          }
        }
      }
      return(rc);
    }
                                
    //------------------------------------------------------------------------
    //!  Reads a map<_keyT,_valueT> from a gzFile.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static int Read(gzFile gzf, std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(gzf, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> to a gzFile.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Compare, typename _Alloc>
    static int 
    Write(gzFile gzf, const std::map<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerWrite<std::map<_keyT,_valueT,_Compare,_Alloc> >(gzf, m));
    }
                                
    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> from a gzFile.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static int 
    Read(gzFile gzf, std::multimap<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(gzf, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> to a gzFile.  Returns 
    //!  the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static int 
    Write(gzFile gzf, const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(gzf, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an array<_valueT,N> from a gzFile.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static int Read(gzFile gzf, std::array<_valueT, N> & a)
    {
      int  rc = 0;
      for (size_t i = 0; i < N; ++i) {
        int  bytesRead = Read(gzf, a[i]);
        if (bytesRead > 0) {
          rc += bytesRead;
        }
        else {
          rc = -1;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes an array<_valueT,N> to a gzFile.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static int Write(gzFile gzf, const std::array<_valueT, N> & a)
    {
      int  rc = 0;
      for (size_t i = 0; i < N; ++i) {
        int  bytesWritten = Write(gzf, a[i]);
        if (bytesWritten > 0) {
          rc += bytesWritten;
        }
        else {
          rc = -1;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a vector<_valueT> from a gzFile.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int Read(gzFile gzf, std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerRead<std::vector<_valueT, _Alloc> >(gzf, v));
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> to a gzFile.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int Write(gzFile gzf, const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerWrite<std::vector<_valueT, _Alloc> >(gzf, v));
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from a gzFile.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int Read(gzFile gzf, std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerRead<std::deque<_valueT, _Alloc> >(gzf, d));
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to a gzFile.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int Write(gzFile gzf, const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerWrite<std::deque<_valueT, _Alloc> >(gzf, d));
    }

    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> from a gzFile.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int Read(gzFile gzf, std::list<_valueT, _Alloc> & l)
    {
      return(ContainerRead<std::list<_valueT, _Alloc> >(gzf, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> to a gzFile.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int Write(gzFile gzf, const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerWrite<std::list<_valueT, _Alloc> >(gzf, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> from a gzFile.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int Read(gzFile gzf, std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::set<_valueT, _Compare, _Alloc> >(gzf, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> to a gzFile.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int Write(gzFile gzf, const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::set<_valueT, _Compare, _Alloc> >(gzf, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> from a gzFile.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int Read(gzFile gzf, std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::multiset<_valueT, _Compare, _Alloc> >(gzf, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> to a gzFile.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int
    Write(gzFile gzf, const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::multiset<_valueT, _Compare, _Alloc> >(gzf, l));
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static int Read(gzFile gzf, std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static int Write(gzFile gzf, const std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  Reads a variant from a gzFile.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static int Read(gzFile gzf, std::variant<Ts...> & v)
    {
      int       rc = -1;
      uint64_t  index = 0;
      int       bytesRead = Read(gzf, index);
      if (bytesRead > 0) {
        rc = bytesRead;
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { rc = Read(gzf, arg); }, v);
          if (bytesRead >= 0) {
            rc += bytesRead;
          }
          else {
            rc = -1;
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a variant to a gzFile.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static int Write(gzFile gzf, const std::variant<Ts...> & v)
    {
      int       rc = -1;
      uint64_t  index = v.index();
      int       bytesWritten = Write(gzf, index);
      if (bytesWritten > 0) {
        rc = bytesWritten;
        std::visit([&] (const auto & arg)
                   { bytesWritten = Write(gzf, arg); }, v);
        if (bytesWritten >= 0) {
          rc += bytesWritten;
        }
        else {
          rc = -1;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a tuple from a gzFile.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Args>
    static int Read(gzFile gzf, std::tuple<Args...> & t)
    {
      return(std::apply([&gzf](auto&&...args) 
      {
        int  rc = 0;
        auto read_tuple_mem = [&gzf,&rc](auto&& x) {
          int  bytesRead = Read(gzf, x);
          if (bytesRead > 0) { rc += bytesRead; return true; }
          else               { return false; }
        };
        if ((read_tuple_mem(args) && ...)) {
          return rc;
        }
        else {
          return -1;
        }
      }, t));
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to a gzFile.  Returns the number of bytes written on
    //!  success, -1 on faiulure.
    //------------------------------------------------------------------------
    template <typename... Args>
    static int Write(gzFile gzf, const std::tuple<Args...> & t)
    {
      return(std::apply([&gzf](auto&&...args) 
      {
        int  rc = 0;
        auto read_tuple_mem = [&gzf,&rc](auto&& x) {
          int  bytesWritten = Write(gzf, x);
          if (bytesWritten > 0) { rc += bytesWritten; return true; }
          else                  { return false; }
        };
        if ((read_tuple_mem(args) && ...)) {
          return rc;
        }
        else {
          return -1;
        }
      }, t));
    }
    
    //------------------------------------------------------------------------
    //!  Reads an unordered_map from a gzFile.  Returns the number of bytes
    //!  read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    Read(gzFile gzf, std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(PairAssocContRead<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_map to a gzFile.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    Write(gzFile gzf, 
          const std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_set from a gzFile.  Returns the number of bytes
    //!  read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    Read(gzFile gzf, std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerRead<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_set to a gzFile.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    Write(gzFile gzf, 
          const std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multimap from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    Read(gzFile gzf, std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(PairAssocContRead<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multimap to a gzFile.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    Write(gzFile gzf, 
          const std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multiset from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    Read(gzFile gzf, std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerRead<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multiset to a gzFile.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    Write(gzFile gzf, 
          const std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(gzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads @c args from @c gzf.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename ...Args>
    static int ReadV(gzFile gzf, Args & ...args)
    {
      int  rv = 0;
      auto  readOne = [&] (auto & arg) {
        bool  rc = true;
        int bytesRead = Read(gzf, arg);
        if (bytesRead > 0) {
          rv += bytesRead;
        }
        else {
          rv = -1;
          rc = false;
        }
        return rc;
      };
      (readOne(args) && ...);
      return rv;
    }
    
    //------------------------------------------------------------------------
    //!  Writes @c args to @c gzf.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename ...Args>
    static int WriteV(gzFile gzf, const Args & ...args)
    {
      ssize_t  rv = 0;
      auto  writeOne = [&] (auto & arg) {
        bool  rc = true;
        int bytesWritten = Write(gzf, arg);
        if (bytesWritten > 0) {
          rv += bytesWritten;
        }
        else {
          rv = -1;
          rc = false;
        }
        return rc;
      };
      (writeOne(args) && ...);
      return rv;
    }
    
  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static int Write(gzFile gzf, _inputIteratorT f, _inputIteratorT l)
    {
      int  rc = 0;
      if (gzf) {
        for ( ; f != l; ++f) {
          int  bytesWritten = Write(gzf, *f);
          if (bytesWritten > 0) {
            rc += bytesWritten;
          }
          else {
            rc = -1;
            break;
          }
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Reads a container from a gzFile.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //!  We use this for deques, lists, vectors, sets and multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static int ContainerRead(gzFile gzf, _containerT & c)
    {
      if (! c.empty())
        c.clear();
      int  rc = -1;
      if (gzf) {
        uint64_t  numEntries;
        int  bytesRead = Read(gzf, numEntries);
        if (bytesRead == sizeof(numEntries)) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::value_type  val;
            bytesRead = Read(gzf, val);
            if (bytesRead > 0) {
              rc += bytesRead;
              c.insert(c.end(), std::move(val));
            }
            else {
              rc = -1;
              break;
            }
          }
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Writes a container to a gzFile.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //!  We use this for all containers.  
    //------------------------------------------------------------------------
    template <typename _containerT>
    static int ContainerWrite(gzFile gzf, const _containerT & c)
    {
      int  rc = -1;
      if (gzf) {
        uint64_t  numEntries = c.size();
        uint64_t  bytesWritten = Write(gzf, numEntries);
        if (bytesWritten == sizeof(numEntries)) {
          rc = bytesWritten;
          if (numEntries) {
            bytesWritten = 
              Write<typename _containerT::const_iterator>(gzf,
                                                          c.begin(), 
                                                          c.end());
            if (bytesWritten > 0) {
              rc += bytesWritten;
            }
            else {
              rc = -1;
            }
          }
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Reads a PairAssociative container from a gzFile.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static int PairAssocContRead(gzFile gzf, _containerT & m)
    {
      int  rc = -1;
      if (! m.empty())
        m.clear();
      if (gzf) {
        uint64_t  numEntries;
        int  bytesRead = Read(gzf, numEntries);
        if (bytesRead == sizeof(numEntries)) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::key_type  key;
            bytesRead = Read(gzf, key);
            if (bytesRead > 0) {
              rc += bytesRead;
              typename _containerT::mapped_type  val;
              bytesRead = Read(gzf, val);
              if (bytesRead > 0) {
                rc += bytesRead;
                m.insert(typename _containerT::value_type(std::move(key),
                                                          std::move(val)));
              }
              else {
                rc = -1;
                break;
              }
            }
            else {
              rc = -1;
              break;
            }
          }
        }
      }
      return(rc);
    }

  };

  // GZWritable

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be written to
  //!  a gzFile via a GZIO::Write() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsGZWritable = requires(const T & t, gzFile gzf) {
    { GZIO::Write(gzf, t) } -> std::same_as<int>;
  };

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be read from
  //!  a gzFile via a GZIO::Read() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsGZReadable = requires(T & t, gzFile gzf) {
    { GZIO::Read(gzf, t) } -> std::same_as<int>;
  };
  
}  // namespace Dwm

#endif  // _DWMGZIO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
