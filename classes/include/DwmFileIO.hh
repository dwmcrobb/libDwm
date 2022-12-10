//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016, 2017, 2020
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
//!  \file DwmFileIO.hh
//!  \brief Dwm::FileIO class declaration
//---------------------------------------------------------------------------

#ifndef _DWMFILEIO_HH_
#define _DWMFILEIO_HH_

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
#include "DwmFileIOCapable.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class contains a collection of static functions for reading and
  //!  writing simple types, in network byte order (MSB first).  It also
  //!  contains functions to read and write strings.  It also contains
  //!  function templates to read and write STL deques, lists, vectors, maps, 
  //!  multimaps, sets, multisets, unordered_maps, unordered_multimaps,
  //!  unordered_sets, unordered_multisets, tuples and variants.
  //!  We use our member functions to handle reading and writing simple
  //!  types in the containers, and function templates to handle reading 
  //!  and writing other class types.  For a user-defined class, the class
  //!  must implement the FileReadable and FileWritable interfaces, since our 
  //!  function templates simply call out to them.  Since templates are
  //!  static polymorphism, you don't need to inherit from FileReadable and
  //!  FileWritable, but you must implement the interfaces.
  //--------------------------------------------------------------------------
  class FileIO
  {
  public:
    //------------------------------------------------------------------------
    //!  Reads \c c from \c f.  Returns the number of bytes read (1 on
    //!  success, 0 on failure).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, char & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c f.  Returns the number of bytes written (1) on 
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, char c);

    //------------------------------------------------------------------------
    //!  Reads \c c from \c f.  Returns the number of bytes read (1 on
    //!  success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c f.  Returns the number of bytes written (1) on
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, uint8_t c);
 
    //------------------------------------------------------------------------
    //!  Reads \c b from \c f.  Returns the number of bytes read (1 on
    //!  success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, bool & b);
    
    //------------------------------------------------------------------------
    //!  Writes \c b to \c f.  Returns the number of bytes written (1) on
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, bool b);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (2 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (2) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, int16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (2 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (2) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, uint16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (4 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (4) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, int32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (4 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (4) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, uint32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (8 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (8) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const int64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (8 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (8) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const uint64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in IEEE format (see RFC 1832 and/or
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes read
    //! (4 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, float & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes 
    //!  written (4 on success).
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, float val);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c f, in IEEE format (see RFC 1832 and/or
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes read
    //! (8 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, double & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c f, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes 
    //!  written (8 on success).
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const double & val);
    
    //------------------------------------------------------------------------
    //!  Reads \c s from \c f.  Since we write strings with a 64-bit length
    //!  value preceding, and always write the terminating NULL, this
    //!  function will always return a value of 9 or greater on success.
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, std::string & s);

    //------------------------------------------------------------------------
    //!  Writes \c s to \c f.  Returns the number of bytes written.  Note
    //!  that we first write a 64-bit length value, then the string itself
    //!  (with terminating NULL).  Hence a successful write will always
    //!  return a value of 8 + \c s.length() + 1.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Wrapper function to read a FileReadable object from a FILE.
    //------------------------------------------------------------------------
    static size_t Read(FILE *f, FileReadable & val)
    {
      return(val.Read(f));
    }
    
    //------------------------------------------------------------------------
    //!  Wrapper function to write a FileWritable object to a FILE.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const FileWritable & val)
    {
      return(val.Write(f));
    }

    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static size_t Read(FILE *f, std::pair<_firstT,_secondT> & p)
    {
      size_t  rc = 0;
      if (f)
        if (Read(f, p.first) > 0)
          if (Read(f, p.second) > 0)
            rc = 1;
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static size_t Write(FILE *f, const std::pair<_firstT,_secondT> & p)
    {
      size_t  rc = 0;
      if (f)
        if (Write(f, p.first) > 0)
          if (Write(f, p.second) > 0)
            rc = 1;
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Reads a map<_keyT,_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static size_t Read(FILE *f, std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(f, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static size_t
    Write(FILE *f, const std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::map<_keyT,_valueT> >(f, m));
    }

    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static size_t
    Read(FILE *f, std::multimap<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(f, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static size_t 
    Write(FILE *f, const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(f, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an array<_valueT,N> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static size_t Read(FILE *f, std::array<_valueT, N> & a)
    {
      size_t  rc = 1;
      for (size_t i = 0; i < N; ++i) {
        if (! Read(f, a[i])) {
          rc = 0;
          break;
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Writes an array<_valueT,N> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static size_t Write(FILE *f, const std::array<_valueT, N> & a)
    {
      size_t  rc = 1;
      for (size_t i = 0; i < N; ++i) {
        if (! Write(f, a[i])) {
          rc = 0;
          break;
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Reads a vector<_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static size_t Read(FILE *f, std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerRead<std::vector<_valueT, _Alloc> >(f, v));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static size_t Write(FILE *f, const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerWrite<std::vector<_valueT, _Alloc> >(f, v));
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static size_t Read(FILE *f, std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerRead<std::deque<_valueT, _Alloc> >(f, d));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static size_t Write(FILE *f, const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerWrite<std::deque<_valueT, _Alloc> >(f, d));
    }

    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static size_t Read(FILE *f, std::list<_valueT, _Alloc> & l)
    {
      return(ContainerRead<std::list<_valueT, _Alloc> >(f, l));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static size_t Write(FILE *f, const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerWrite<std::list<_valueT, _Alloc> >(f, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static size_t Read(FILE *f, std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::set<_valueT, _Compare, _Alloc> >(f, l));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static size_t Write(FILE *f, const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::set<_valueT, _Compare, _Alloc> >(f, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static size_t Read(FILE *f, std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::multiset<_valueT, _Compare, _Alloc> >(f, l));
    }
    
    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static size_t
    Write(FILE *f, const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::multiset<_valueT, _Compare, _Alloc> >(f, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a tuple from a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename T, typename... Args>
    static size_t Read(FILE *f, std::tuple<T, Args...> & t)
    {
      return(ReadTuple<std::tuple<T, Args...> >(f, t));
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename T, typename... Args>
    static size_t Write(FILE *f, const std::tuple<T, Args...> & t)
    {
      return(WriteTuple<std::tuple<T, Args...> >(f, t));
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 1.
    //------------------------------------------------------------------------
    static size_t Read(FILE *fp, std::monostate & sm)
    {
      return 1;
    }
    
    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 1.
    //------------------------------------------------------------------------
    static size_t Write(FILE *fp, const std::monostate & sm)
    {
      return 1;
    }

    //------------------------------------------------------------------------
    //!  Reads a variant from a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static size_t Read(FILE *fp, std::variant<Ts...> & v)
    {
      size_t    rc = 0;
      uint64_t  index = 0;
      if (Read(fp, index)) {
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { rc = Read(fp, arg); }, v);
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Writes a variant to a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static size_t Write(FILE *fp, const std::variant<Ts...> & v)
    {
      size_t    rc = 0;
      uint64_t  index = v.index();
      if (Write(fp, index)) {
        std::visit([&] (const auto & arg) { rc = Write(fp, arg); }, v);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_map from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t
    Read(FILE *f, std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(PairAssocContRead<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_map to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t  
    Write(FILE *f, const std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multimap from a FILE.  Returns 1 on
    //!  success, 0 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t
    Read(FILE *f, std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(PairAssocContRead<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multimap to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t  
    Write(FILE *f, const std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_set from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t
    Read(FILE *f, std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerRead<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_set to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t  
    Write(FILE *f, const std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multiset from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t
    Read(FILE *f, std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerRead<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multiset to a FILE.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static size_t  
    Write(FILE *f, const std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerWrite<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(f, hm));
    }

  private:
    //------------------------------------------------------------------------
    //!  T must be a tuple.
    //------------------------------------------------------------------------
    template <typename T>
    static size_t ReadTuple(FILE *f, T & t)
    {
      if (TupleIOHelper<T,std::tuple_size<T>::value-1>::Read(f, t) > 0)
        return(1);
      return(0);
    }

    //------------------------------------------------------------------------
    //!  T must be a tuple.
    //------------------------------------------------------------------------
    template <typename T>
    static size_t WriteTuple(FILE *f, const T & t)
    {
      if (TupleIOHelper<T,std::tuple_size<T>::value-1>::Write(f, t) > 0)
        return(1);
      return(0);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static size_t Write(FILE *file, _inputIteratorT f, _inputIteratorT l)
    {
      size_t  rc = 0;
      if (file) {
        for ( ; f != l; ++f) {
          if (Write(file, *f) <= 0) {
            break;
          }
        }
        if (f == l)
          rc = 1;
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Reads a container from a FILE.  Returns 1 on success, 0 on failure.
    //!  We use this for deques, lists, vectors, sets and multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static size_t ContainerRead(FILE *f, _containerT & c)
    {
      size_t  rc = 0;
      if (! c.empty())
        c.clear();
      if (f) {
        uint64_t  numEntries;
        if (Read(f, numEntries)) {
          uint64_t  i = 0;
          for ( ; i < numEntries; ++i) {
            typename _containerT::value_type  val;
            if (Read(f, val) > 0)
              c.insert(c.end(), std::move(val));
            else
              break;
          }
          if (i == numEntries)
            rc = 1;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Writes a container to a FILE.  Returns 1 on success, 0 on failure.
    //!  We use this for all containers.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static size_t ContainerWrite(FILE *f, const _containerT & c)
    {
      size_t  rc = 0;
      if (f) {
        uint64_t  numEntries = c.size();
        if (Write(f, numEntries)) {
          if (numEntries) {
            rc = Write<typename _containerT::const_iterator>(f, 
                                                             c.begin(), c.end());
          }
          else {
            rc = 1;
          }
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Reads a PairAssociative container from a FILE.  Returns 1 on success,
    //!  0 on failure.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static size_t PairAssocContRead(FILE *f, _containerT & m)
    {
      ssize_t  rc = 0;
      if (! m.empty())
        m.clear();
      if (f) {
        uint64_t  numEntries;
        if (Read(f, numEntries)) {
          uint64_t i = 0;
          for ( ; i < numEntries; ++i) {
            typename _containerT::key_type  key;
            if (Read(f, key) > 0) {
              typename _containerT::mapped_type  val;
              if (Read(f, val) > 0) {
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
          if (i == numEntries)
            rc = 1;
        }
      }
      return(rc);
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
      //!  Read a tuple \c t from a FILE \c f.  Returns 1 on success, 
      //!  0 on failure.
      //----------------------------------------------------------------------
      static size_t Read(FILE *f, T & t)
      {
        return(FileIO::Read(f, std::get<0>(t)));
      }
      
      //----------------------------------------------------------------------
      //!  Write a tuple \c t to a FILE \c f.  Returns 1 on success,
      //!  0 on failure.
      //----------------------------------------------------------------------
      static size_t Write(FILE *f, const T & t)
      {
        return(FileIO::Write(f, std::get<0>(t)));
      }
    };
    
    //------------------------------------------------------------------------
    //!  The recursive tuple IO helper template.
    //------------------------------------------------------------------------
    template <typename T, size_t elt>
    class TupleIOHelper
    {
    public:
      //----------------------------------------------------------------------
      //!  Read a tuple \c t from a FILE \c f.
      //----------------------------------------------------------------------
      static size_t Read(FILE *f, T & t)
      {
        size_t  rc = TupleIOHelper<T,elt-1>::Read(f, t);
        if (rc > 0)
          rc = FileIO::Read(f, std::get<elt>(t));
        return(rc);
      }
      
      //----------------------------------------------------------------------
      //!  Write a tuple \c t to a FILE \c f.
      //----------------------------------------------------------------------
      static size_t Write(FILE *f, const T & t)
      {
        size_t  rc = TupleIOHelper<T,elt-1>::Write(f, t);
        if (rc > 0) {
          rc = FileIO::Write(f, std::get<elt>(t));
        }
        
        return(rc);
      }
    };

  };


}  // namespace Dwm

#endif  // _DWMFILEIO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
