//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2018, 2020
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

#include <boost/asio.hpp>

#include "DwmASIOReadable.hh"
#include "DwmASIOWritable.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

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
    static bool Read(boost::asio::ip::tcp::socket & s, uint8_t & value);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint8_t value);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint16_t & value);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint16_t value);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint32_t & value);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint32_t value);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, uint64_t & value);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, uint64_t value);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int8_t & value);

    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int8_t value);
        
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int16_t & value);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int16_t value);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int32_t & value);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int32_t value);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, int64_t & value);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, int64_t value);
    
    //------------------------------------------------------------------------
    //!  Reads @c value from the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, std::string & value);
    
    //------------------------------------------------------------------------
    //!  Write @c value to the given socket @c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::string & value);

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
    static bool Read(boost::asio::ip::tcp::socket & s, float & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, float val);

    //------------------------------------------------------------------------
    //!  Reads \c val from \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, double & val);

    //------------------------------------------------------------------------
    //!  Writes \c val to \c s, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s, double val);

    //------------------------------------------------------------------------
    //!  Reads a pair<_firstT,_secondT> from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::pair<_firstT, _secondT> & p)
    {
      bool  rc = false;
      if (Read(s, p.first)) {
        rc = Read(s, p.second);
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Writes a pair<_firstT,_secondT> to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::pair<_firstT,_secondT> & p)
    {
      bool  rc = false;
      if (Write(s, p.first)) {
        rc = Write(s, p.second);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a tuple from \c s.  Returns \c true on success, \c false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename T, typename... Args>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::tuple<T, Args...> & t)
    {
      return ReadTuple<std::tuple<T, Args...> >(s, t);
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to \c s.  Returns \c true on success, \c false on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename T, typename... Args>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::tuple<T, Args...> & t)
    {
      return WriteTuple<std::tuple<T, Args...> >(s, t);
    }

    //------------------------------------------------------------------------
    //!  Reads a variant \C V from \c s.  Returns \c true on success, \c false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::variant<Ts...> & v)
    {
      bool  rc = false;
      uint64_t  index = 0;
      if (Read(s, index)) {
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          rc = true;
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { rc &= Read(s, arg); }, v);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a variant \c v to \c s.  Returns \c true on sucess, \c false
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::variant<Ts...> & v)
    {
      bool  rc = false;
      uint64_t  index = v.index();
      if (Write(s, index)) {
        rc = true;
        std::visit([&] (const auto & arg) { rc &= Write(s, arg); }, v);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a vector<_valueT> \c v from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::vector<_valueT, _Alloc> & v)
    {
      return ContainerRead<std::vector<_valueT, _Alloc> >(s, v);
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> \c v to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::vector<_valueT, _Alloc> & v)
    {
      return ContainerWrite<std::vector<_valueT, _Alloc> >(s, v);
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> \c d from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::deque<_valueT, _Alloc> & d)
    {
      return ContainerRead<std::deque<_valueT, _Alloc> >(s, d);
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> \c d to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::deque<_valueT, _Alloc> & d)
    {
      return ContainerWrite<std::deque<_valueT, _Alloc> >(s, d);
    }
    
    //------------------------------------------------------------------------
    //!  Reads a list<_valueT> \c l from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::list<_valueT, _Alloc> & l)
    {
      return ContainerRead<std::list<_valueT, _Alloc> >(s, l);
    }

    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> \c l to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::list<_valueT, _Alloc> & l)
    {
      return ContainerWrite<std::list<_valueT, _Alloc> >(s, l);
    }

    //------------------------------------------------------------------------
    //!  Reads a map<_keyT,_valueT> \c m from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(s, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> \c m to \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::map<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::map<_keyT,_valueT,_Compare,_Alloc> >(s, m));
    }

    //------------------------------------------------------------------------
    //!  Reads a multimap<_keyT,_valueT> \c m from \c s.  Returns true on
    //!  success, false on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static bool
    Read(boost::asio::ip::tcp::socket & s,
         std::multimap<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(PairAssocContRead<std::multimap<_keyT,_valueT,_Compare,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multimap<_keyT,_valueT> \c m to \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static bool
    Write(boost::asio::ip::tcp::socket & s,
          const std::multimap<_keyT,_valueT, _Compare, _Alloc> & m)
    {
      return(ContainerWrite<std::multimap<_keyT,_valueT,_Compare,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_map<_keyT,_valueT> \c m from \c s.  Returns 
    //!  \c true on success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static bool
    Read(boost::asio::ip::tcp::socket & s,
         std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContRead<std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> >(s, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_map<_keyT,_valueT> \c m to \c s.  Returns \c true
    //!  on success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static bool
    Write(boost::asio::ip::tcp::socket & s,
          const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(s, m));
    }

    //------------------------------------------------------------------------
    //!  Reads a set<_valueT> \c m from \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::set<_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerRead<std::set<_valueT,_Compare,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> \c m to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::set<_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerWrite<std::set<_valueT,_Compare,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Reads a multiset<_valueT> \c m from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
                     std::multiset<_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerRead<std::multiset<_valueT,_Compare,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> \c m to \c s.  Returns \c true on success,
    //!  \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::multiset<_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerWrite<std::multiset<_valueT,_Compare,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Reads an unordered_set<_valueT> \c m from \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash, 
              typename _Pred, typename _Alloc>
    static bool Read(boost::asio::ip::tcp::socket & s,
         std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerRead<std::unordered_set<_valueT,_Hash,_Pred,_Alloc>>(s, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a unordered_set<_valueT> \c m to \c s.  Returns \c true on
    //!  success, \c false on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static bool
    Write(boost::asio::ip::tcp::socket & s,
          const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc>>(s, m));
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, ASIOReadable & val);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const ASIOWritable & val);
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(const ASIOWritable & val);

  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static bool ReadTuple(boost::asio::ip::tcp::socket & s, T & t)
    {
      return TupleIOHelper<T,std::tuple_size<T>::value-1>::Read(s, t);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static bool WriteTuple(boost::asio::ip::tcp::socket & s, const T & t)
    {
      return TupleIOHelper<T,std::tuple_size<T>::value-1>::Write(s, t);
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
      static bool Read(boost::asio::ip::tcp::socket & s, T & t)
      {
        return(ASIO::Read(s, std::get<0>(t)));
      }

      //----------------------------------------------------------------------
      //!  Write a tuple \c t to \c s.  Returns \c true on success, \c false
      //!  on failure.
      //----------------------------------------------------------------------
      static bool Write(boost::asio::ip::tcp::socket & s, const T & t)
      {
        return(ASIO::Write(s, std::get<0>(t)));
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
      static bool Read(boost::asio::ip::tcp::socket & s, T & t)
      {
        bool  rc = false;
        if (TupleIOHelper<T,elt-1>::Read(s, t)) {
          rc = ASIO::Read(s, std::get<elt>(t));
        }
        return rc;
      }
      
      //----------------------------------------------------------------------
      //!  Write a tuple \c t to an ostream \c os.
      //----------------------------------------------------------------------
      static bool Write(boost::asio::ip::tcp::socket & s, const T & t)
      {
        bool  rc = false;
        if (TupleIOHelper<T,elt-1>::Write(s, t)) {
          rc = ASIO::Write(s, std::get<elt>(t));
        }
        return rc;
      }
    };

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns true.
    //------------------------------------------------------------------------
    static bool Read(boost::asio::ip::tcp::socket & s, std::monostate & sm)
    {
      return true;
    }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns true.
    //------------------------------------------------------------------------
    static bool Write(boost::asio::ip::tcp::socket & s,
                      const std::monostate & sm)
    {
      return true;
    }

    //------------------------------------------------------------------------
    //!  Reads a _containerT from \c s.  Returns \c true on success, \c false
    //!  on failure.  We use this for deques, lists, vectors, sets and
    //!  multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static bool ContainerRead(boost::asio::ip::tcp::socket & s,
                              _containerT & c)
    {
      bool  rc = false;
      c.clear();
      uint32_t  numEntries;
      if (Read(s, numEntries)) {
        uint32_t  i = 0;
        for ( ; i < numEntries; ++i) {
          typename _containerT::value_type  val;
          if (! Read(s, val)) {
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
    template <typename _containerT>
    static bool ContainerWrite(boost::asio::ip::tcp::socket & s,
                               const _containerT & c)
    {
      bool  rc = false;
      uint32_t  numEntries = c.size();
      if (Write(s, numEntries)) {
        if (numEntries) {
          rc = Write<typename _containerT::const_iterator>(s, 
                                                           c.begin(), 
                                                           c.end());
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
    template <typename _inputIteratorT>
    static bool Write(boost::asio::ip::tcp::socket & s,
                      _inputIteratorT f, _inputIteratorT l)
    {
      for ( ; f != l; ++f) {
        if (! Write(s, *f)) {
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
    template <typename _containerT>
    static bool PairAssocContRead(boost::asio::ip::tcp::socket & s,
                                  _containerT & m)
    {
      bool  rc = false;
      m.clear();
      uint32_t  numEntries;
      if (Read(s, numEntries)) {
        uint32_t i = 0;
        for ( ; i < numEntries; ++i) {
          typename _containerT::key_type  key;
          if (Read(s, key)) {
            typename _containerT::mapped_type  val;
            if (Read(s, val)) {
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
