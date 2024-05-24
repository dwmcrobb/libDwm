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
//!  \file DwmDescriptorIO.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::DescriptorIO class declaration
//---------------------------------------------------------------------------

#ifndef _DWMDESCRIPTORIO_HH_
#define _DWMDESCRIPTORIO_HH_

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
#include "DwmDescriptorIOCapable.hh"
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
  //!  must implement the DescriptorReadable and DescriptorWritable
  //!  interfaces, since our function templates simply call out to them.
  //!  Since templates are static polymorphism, you don't need to inherit
  //!  from DescriptorReadable and DescriptorWritable, but you must implement
  //!  the interfaces.
  //--------------------------------------------------------------------------
  class DescriptorIO
  {
  public:
    //------------------------------------------------------------------------
    //!  Reads \c c from \c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, char & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, char c);

    //------------------------------------------------------------------------
    //!  Reads \c c from \c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, uint8_t c);

    //------------------------------------------------------------------------
    //!  Reads \c b from \c fd.  Returns the number of bytes read (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, bool & b);

    //------------------------------------------------------------------------
    //!  Writes \c b to \c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, bool b);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, int16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, int16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, uint16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, uint16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, int32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, int32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, uint32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, uint32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, int64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const int64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, uint64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const uint64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (4)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, float & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, float val);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c fd, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (8)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, double & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val to \c fd, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const double & val);
    
    //------------------------------------------------------------------------
    //!  Reads \c s from \c fd.  Returns the number of bytes read on success,
    //!  -1 on failure.  Since we write strings with a 64-bit unsigned length
    //!  value preceding the actual string, and always have a terminating
    //!  NULL, this always reads at least 9 bytes on success.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, std::string & s);
    
    //------------------------------------------------------------------------
    //!  Writes \c s to \c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.  Note that a 64-bit value is written first,
    //!  holding the length of the string.  The terminating NULL is also
    //!  written.  Hence, on success this will always return a value of
    //!  9 or greater.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Wrapper function to read a DescriptorReadable object from a
    //!  descriptor.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, DescriptorReadable & val)
    {
      return(val.Read(fd));
    }
    
    //------------------------------------------------------------------------
    //!  Wrapper function to write a DescriptorWritable object to a
    //!  descriptor.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const DescriptorWritable & val)
    {
      return(val.Write(fd));
    }

    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from a file descriptor.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static ssize_t Read(int fd, std::pair<_firstT, _secondT> & p)
    {
      ssize_t  rc = -1;
      if (fd >= 0) {
        ssize_t  bytesRead = Read(fd, p.first);
        if (bytesRead > 0) {
          rc = bytesRead;
          bytesRead = Read(fd, p.second);
          if (bytesRead > 0)
            rc += bytesRead;
          else
            rc = -1;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to a file descriptor.  Returns
    //!  the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static ssize_t Write(int fd, const std::pair<_firstT,_secondT> & p)
    {
      ssize_t  rc = -1;
      if (fd >= 0) {
        ssize_t  bytesWritten = Write(fd, p.first);
        if (bytesWritten > 0) {
          rc = bytesWritten;
          bytesWritten = Write(fd, p.second);
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
    //!  Reads a map<_keyT,_valueT> from a file descriptor.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static ssize_t Read(int fd, std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> to a file descriptor.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Compare, typename _Alloc>
    static ssize_t 
    Write(int fd, const std::map<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerWrite<std::map<_keyT,_valueT,_Compare,_Alloc> >(fd, m));
    }
         
    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> from a file descriptor.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static ssize_t 
    Read(int fd, std::multimap<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> to a file descriptor.  Returns 
    //!  the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static ssize_t 
    Write(int fd, const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Read an array<_valueT,N> from a file descriptor.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, std::size_t N>
    static ssize_t Read(int fd, std::array<_valueT, N> & a)
    {
      ssize_t  rc = 0;
      for (std::size_t i = 0; i < N; ++i) {
        ssize_t  bytesRead = Read(fd, a[i]);
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
    //!  Writes an array<_valueT,N> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, std::size_t N>
    static ssize_t Write(int fd, const std::array<_valueT, N> & a)
    {
      ssize_t  rc = 0;
      for (std::size_t i = 0; i < N; ++i) {
        ssize_t  bytesWritten = Write(fd, a[i]);
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
    //!  Reads a vector<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Read(int fd, std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerRead<std::vector<_valueT, _Alloc> >(fd, v));
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Write(int fd, const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerWrite<std::vector<_valueT, _Alloc> >(fd, v));
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Read(int fd, std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerRead<std::deque<_valueT, _Alloc> >(fd, d));
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Write(int fd, const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerWrite<std::deque<_valueT, _Alloc> >(fd, d));
    }

    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Read(int fd, std::list<_valueT, _Alloc> & l)
    {
      return(ContainerRead<std::list<_valueT, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Write(int fd, const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerWrite<std::list<_valueT, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t Read(int fd, std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::set<_valueT, _Compare, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t Write(int fd, const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::set<_valueT, _Compare, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t Read(int fd, std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerRead<std::multiset<_valueT, _Compare, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t
    Write(int fd, const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerWrite<std::multiset<_valueT, _Compare, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a tuple from a file descriptor.  Returns the number of
    //!  bytes read.
    //------------------------------------------------------------------------
    template <typename... Args>
    static ssize_t Read(int fd, std::tuple<Args...> & t)
    {
      return(std::apply([&fd](auto&&...args) 
      {
        ssize_t  rc = 0;
        auto read_tuple_mem = [&fd,&rc](auto&& x) {
          int  bytesRead = Read(fd, x);
          if (bytesRead > 0) { rc += bytesRead; return true; }
          else               { return false; }
        };
        if ((read_tuple_mem(args) && ...)) {
          return rc;
        }
        else {
          return (ssize_t)(-1);
        }
      }, t));
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to a file descriptor.  Returns the number of
    //!  bytes written.
    //------------------------------------------------------------------------
    template <typename... Args>
    static ssize_t Write(int fd, const std::tuple<Args...> & t)
    {
      return(std::apply([&fd](auto&&...args) 
      {
        ssize_t  rc = 0;
        auto read_tuple_mem = [&fd,&rc](auto&& x) {
          int  bytesWritten = Write(fd, x);
          if (bytesWritten > 0) { rc += bytesWritten; return true; }
          else                  { return false; }
        };
        if ((read_tuple_mem(args) && ...)) {
          return rc;
        }
        else {
          return (ssize_t)(-1);
        }
      }, t));
    }
    
    //------------------------------------------------------------------------
    //!  Reads an unordered_map<_keyT,_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    Read(int fd, std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContRead<std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_map<_keyT,_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    Write(int fd, 
          const std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(fd, m));
    }
    
    //------------------------------------------------------------------------
    //!  Reads an unordered_multimap<_keyT,_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    Read(int fd, std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContRead<std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multimap<_keyT,_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    Write(int fd, const std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerWrite<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(fd, m));
    }
    
    //------------------------------------------------------------------------
    //!  Reads an unordered_set<_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static ssize_t 
    Read(int fd, std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerRead<std::unordered_set<_valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_set<_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static ssize_t 
    Write(int fd, const std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(fd, m));
    }
    
    //------------------------------------------------------------------------
    //!  Reads an unordered_multiset<_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static ssize_t 
    Read(int fd, std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerRead<std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multiset<_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static ssize_t 
    Write(int fd, const std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerWrite<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(fd, m));
    }
    
    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  Reads a variant from a descriptor.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static ssize_t Read(int fd, std::variant<Ts...> & v)
    {
      ssize_t  rc = -1;
      uint64_t index = 0;
      ssize_t  bytesRead = Read(fd, index);
      if (bytesRead > 0) {
        rc = bytesRead;
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { bytesRead = Read(fd, arg); }, v);
          if (bytesRead >= 0) {
            rc += bytesRead;
          }
          else {
            rc = -1;
          }
        }
        else {
          rc = -1;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a variant to a descriptor.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static ssize_t Write(int fd, const std::variant<Ts...> & v)
    {
      ssize_t   rc = -1;
      uint64_t  index = v.index();
      ssize_t   bytesWritten = Write(fd, index);
      if (bytesWritten > 0) {
        rc = bytesWritten;
        std::visit([&fd,&bytesWritten] (const auto & arg)
                   { bytesWritten = Write(fd, arg); }, v);
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
    //!  
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, void *buf, size_t buflen);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const void *buf, size_t buflen);
    
  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static ssize_t Write(int fd, _inputIteratorT f, _inputIteratorT l)
    {
      ssize_t  rc = 0;
      if (fd >= 0) {
        for ( ; f != l; ++f) {
          ssize_t  bytesWritten = Write(fd, *f);
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
    //!  Reads a container from a file descriptor.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //!  We use this for deques, lists, vectors, sets and multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static ssize_t ContainerRead(int fd, _containerT & c)
    {
      if (! c.empty())
        c.clear();
      ssize_t  rc = -1;
      if (fd >= 0) {
        uint64_t  numEntries;
        ssize_t   bytesRead = Read(fd, numEntries);
        if (bytesRead == sizeof(numEntries)) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::value_type  val;
            bytesRead = Read(fd, val);
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
    //!  Writes a container to a file descriptor.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //!  We use this for all containers.  
    //------------------------------------------------------------------------
    template <typename _containerT>
    static ssize_t ContainerWrite(int fd, const _containerT & c)
    {
      ssize_t  rc = -1;
      if (fd >= 0) {
        uint64_t  numEntries = c.size();
        uint64_t  bytesWritten = Write(fd, numEntries);
        if (bytesWritten == sizeof(numEntries)) {
          rc = bytesWritten;
          if (numEntries) {
            bytesWritten = 
              Write<typename _containerT::const_iterator>(fd,
                                                          c.begin(), 
                                                          c.end());
            if (bytesWritten > 0)
              rc += bytesWritten;
            else
              rc = -1;
          }
        }
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Reads a PairAssociative container from a file descriptor.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static ssize_t PairAssocContRead(int fd, _containerT & m)
    {
      ssize_t  rc = -1;
      if (! m.empty())
        m.clear();
      if (fd >= 0) {
        uint64_t  numEntries;
        ssize_t   bytesRead = Read(fd, numEntries);
        if (bytesRead == sizeof(numEntries)) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::key_type  key;
            bytesRead = Read(fd, key);
            if (bytesRead > 0) {
              rc += bytesRead;
              typename _containerT::mapped_type  val;
              bytesRead = Read(fd, val);
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


}  // namespace Dwm

#endif  // _DWMDESCRIPTORIO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
