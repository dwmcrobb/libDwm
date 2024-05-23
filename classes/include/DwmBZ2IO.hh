//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2006, 2016, 2020, 2024
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
//!  \file DwmBZ2IO.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::BZ2IO class declaration
//---------------------------------------------------------------------------

#ifndef _DWMBZ2IO_HH_
#define _DWMBZ2IO_HH_

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
#include "DwmBZ2IOCapable.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class contains a collection of static functions for reading and
  //!  writing simple types in network byte order (MSB first) from/to bzip2
  //!  files.  It also contains functions to read and write strings from/to
  //!  bzip2 files.  It also contains function templates to read and write 
  //!  arrays, deques, lists, vectors, maps, multimaps, sets, multisets,
  //!  unordered_maps, unordered_multimaps, unordered_sets,
  //!  unordered_multisets, tuples and variants from/to bzip2 files.  We use
  //!  our member functions to handle reading and writing simple types in
  //!  these containers, and function templates to handle reading and writing
  //!  other class types.  In the latter case, the class must implement the 
  //!  BZ2Readable and BZ2Writable interfaces, since our function templates
  //!  simply call out to them.
  //--------------------------------------------------------------------------
  class BZ2IO
  {
  public:
    //------------------------------------------------------------------------
    //!  Reads \c from \c bzf.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, char & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c bzf.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, char c);

    //------------------------------------------------------------------------
    //!  Reads \c c from \c bzf.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Writes \c c to \c bzf.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, uint8_t c);

    //------------------------------------------------------------------------
    //!  Reads \c b from \c bzf.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, bool & b);

    //------------------------------------------------------------------------
    //!  Writes \c b to \c bzf.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, bool b);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, int16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c bzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, int16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, uint16_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c bzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, uint16_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, int32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c bzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, int32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, uint32_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c bzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, uint32_t val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, int64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c bzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, const int64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, uint64_t & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c bzf, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, const uint64_t & val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (4)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, float & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val tp \c bzf, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, float val);
    
    //------------------------------------------------------------------------
    //!  Reads \c val from \c bzf, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (8)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, double & val);
    
    //------------------------------------------------------------------------
    //!  Writes \c val tp \c bzf, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, const double & val);
    
    //------------------------------------------------------------------------
    //!  Reads \c s from \c bzf.  Returns the number of bytes read on success,
    //!  -1 on failure.  Since we write strings with a 64-bit unsigned length
    //!  value preceding the actual string, and always have a terminating
    //!  NULL, this always reads at least 9 bytes on success.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, std::string & s);
    
    //------------------------------------------------------------------------
    //!  Writes \c s to \c bzf.  Returns the number of bytes written on
    //!  success, -1 on failure.  Note that a 64-bit value is written first,
    //!  holding the length of the string.  The terminating NULL is also
    //!  written.  Hence, on success this will always return a value of
    //!  9 or greater.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Wrapper function to read a BZ2Readable object from a BZFILE pointer.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, BZ2Readable & val)
    {
      return(val.BZRead(bzf));
    }

    //------------------------------------------------------------------------
    //!  Wrapper function to write a BZ2Writable object to a BZFILE pointer.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, const BZ2Writable & val)
    {
      return(val.BZWrite(bzf));
    }

    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from a BZFILE pointer.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static int BZRead(BZFILE *bzf, std::pair<_firstT, _secondT> & p)
    {
      int  rc = -1;
      if (bzf) {
        int  bytesRead = BZRead(bzf, p.first);
        if (bytesRead > 0) {
          rc = bytesRead;
          bytesRead = BZRead(bzf, p.second);
          if (bytesRead > 0)
            rc += bytesRead;
          else
            rc = -1;
        }
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to a BZFILE pointer.  Returns
    //!  the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static int BZWrite(BZFILE *bzf, const std::pair<_firstT,_secondT> & p)
    {
      int  rc = -1;
      if (bzf) {
        int  bytesWritten = BZWrite(bzf, p.first);
        if (bytesWritten > 0) {
          rc = bytesWritten;
          bytesWritten = BZWrite(bzf, p.second);
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
    //!  Reads a map<_keyT,_valueT> from a BZFILE pointer.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static int BZRead(BZFILE *bzf, std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContBZRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(bzf, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> to a BZFILE pointer.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Compare, typename _Alloc>
    static int 
    BZWrite(BZFILE *bzf, const std::map<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerBZWrite<std::map<_keyT,_valueT,_Compare,_Alloc> >(bzf, m));
    }
                                
    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> from a BZFILE pointer.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static int 
    BZRead(BZFILE *bzf, std::multimap<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContBZRead<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(bzf, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> to a BZFILE pointer.  Returns 
    //!  the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static int 
    BZWrite(BZFILE *bzf, const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerBZWrite<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(bzf, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an array<_valueT,N> from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static int BZRead(BZFILE *bzf, std::array<_valueT, N> & a)
    {
      int  rc = 0;
      for (size_t i = 0; i < N; ++i) {
        int  bytesRead = BZRead(bzf, a[i]);
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
    //!  Writes an array<_valueT,N> to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static int BZWrite(BZFILE *bzf, const std::array<_valueT, N> & a)
    {
      int  rc = 0;
      for (size_t i = 0; i < N; ++i) {
        int  bytesWritten = BZWrite(bzf, a[i]);
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
    //!  Reads a vector<_valueT> from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int BZRead(BZFILE *bzf, std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerBZRead<std::vector<_valueT, _Alloc> >(bzf, v));
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int BZWrite(BZFILE *bzf, const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerBZWrite<std::vector<_valueT, _Alloc> >(bzf, v));
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int BZRead(BZFILE *bzf, std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerBZRead<std::deque<_valueT, _Alloc> >(bzf, d));
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int BZWrite(BZFILE *bzf, const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerBZWrite<std::deque<_valueT, _Alloc> >(bzf, d));
    }

    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int BZRead(BZFILE *bzf, std::list<_valueT, _Alloc> & l)
    {
      return(ContainerBZRead<std::list<_valueT, _Alloc> >(bzf, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static int BZWrite(BZFILE *bzf, const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerBZWrite<std::list<_valueT, _Alloc> >(bzf, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int BZRead(BZFILE *bzf, std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerBZRead<std::set<_valueT, _Compare, _Alloc> >(bzf, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int BZWrite(BZFILE *bzf, const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerBZWrite<std::set<_valueT, _Compare, _Alloc> >(bzf, l));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int BZRead(BZFILE *bzf, std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerBZRead<std::multiset<_valueT, _Compare, _Alloc> >(bzf, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static int
    BZWrite(BZFILE *bzf, const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerBZWrite<std::multiset<_valueT, _Compare, _Alloc> >(bzf, l));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename... Args>
    static int BZWrite(BZFILE *bzf, const std::tuple<Args...> & t)
    {
      return(std::apply([&bzf](auto&&...args) 
      {
        int  rc = 0;
        auto read_tuple_mem = [&bzf,&rc](auto&& x) {
          int  bytesWritten = BZWrite(bzf, x);
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
    //!  
    //------------------------------------------------------------------------
    template <typename... Args>
    static int BZRead(BZFILE *bzf, std::tuple<Args...> & t)
    {
      return(std::apply([&bzf](auto&&...args) 
      {
        int  rc = 0;
        auto read_tuple_mem = [&bzf,&rc](auto&& x) {
          int  bytesRead = BZRead(bzf, x);
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
    //!  Reads an unordered_map from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    BZRead(BZFILE *bzf, 
           std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(PairAssocContBZRead<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_map to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    BZWrite(BZFILE *bzf, 
            const std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerBZWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_set from a BZFILE pointer.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    BZRead(BZFILE *bzf, 
           std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerBZRead<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_set to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    BZWrite(BZFILE *bzf, 
            const std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerBZWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multimap from a BZFILE pointer.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    BZRead(BZFILE *bzf, 
           std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(PairAssocContBZRead<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multimap to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    BZWrite(BZFILE *bzf, 
            const std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerBZWrite<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_multiset from a BZFILE pointer.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int
    BZRead(BZFILE *bzf, 
           std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerBZRead<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multiset to a BZFILE pointer.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static int 
    BZWrite(BZFILE *bzf, 
            const std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & hm)
    {
      return(ContainerBZWrite<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(bzf, hm));
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static int BZRead(BZFILE *bzf, std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static int BZWrite(BZFILE *bzf, const std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  Reads a variant from a BZFILE.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static int BZRead(BZFILE *bzf, std::variant<Ts...> & v)
    {
      int       rc = -1;
      uint64_t  index = 0;
      int       bytesRead = BZRead(bzf, index);
      if (bytesRead > 0) {
        rc = bytesRead;
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { rc = BZRead(bzf, arg); }, v);
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
    //!  
    //------------------------------------------------------------------------
    template <typename... Ts>
    static int BZWrite(BZFILE *bzf, const std::variant<Ts...> & v)
    {
      int       rc = -1;
      uint64_t  index = v.index();
      int       bytesWritten = BZWrite(bzf, index);
      if (bytesWritten > 0) {
        rc = bytesWritten;
        std::visit([&] (const auto & arg)
                   { bytesWritten = BZWrite(bzf, arg); }, v);
        if (bytesWritten >= 0) {
          rc += bytesWritten;
        }
        else {
          rc = -1;
        }
      }
      return rc;
    }

  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static int BZWrite(BZFILE *bzf, _inputIteratorT f, _inputIteratorT l)
    {
      int  rc = 0;
      if (bzf) {
        for ( ; f != l; ++f) {
          int  bytesWritten = BZWrite(bzf, *f);
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
    //!  Reads a container from a BZFILE pointer.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //!  We use this for deques, lists, vectors, sets and multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static int ContainerBZRead(BZFILE *bzf, _containerT & c)
    {
      if (! c.empty())
        c.clear();
      int  rc = -1;
      if (bzf) {
        uint64_t  numEntries;
        int  bytesRead = BZRead(bzf, numEntries);
        if (bytesRead == sizeof(numEntries)) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::value_type  val;
            bytesRead = BZRead(bzf, val);
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
    //!  Writes a container to a BZFILE pointer.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //!  We use this for all containers.  
    //------------------------------------------------------------------------
    template <typename _containerT>
    static int ContainerBZWrite(BZFILE *bzf, const _containerT & c)
    {
      int  rc = -1;
      if (bzf) {
        uint64_t  numEntries = c.size();
        uint64_t  bytesWritten = BZWrite(bzf, numEntries);
        if (bytesWritten == sizeof(numEntries)) {
          rc = bytesWritten;
          if (numEntries) {
            bytesWritten = 
              BZWrite<typename _containerT::const_iterator>(bzf,
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
    //!  Reads a PairAssociative container from a BZFILE pointer.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static int PairAssocContBZRead(BZFILE *bzf, _containerT & m)
    {
      int  rc = -1;
      if (! m.empty())
        m.clear();
      if (bzf) {
        uint64_t  numEntries;
        int  bytesRead = BZRead(bzf, numEntries);
        if (bytesRead == sizeof(numEntries)) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::key_type  key;
            bytesRead = BZRead(bzf, key);
            if (bytesRead > 0) {
              rc += bytesRead;
              typename _containerT::mapped_type  val;
              bytesRead = BZRead(bzf, val);
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

#endif  // _DWMBZ2IO_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
