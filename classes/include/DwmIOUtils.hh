//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004, 2006, 2007, 2016, 2020
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
//!  \file DwmIOUtils.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::IOUtils class declaration and implementation
//---------------------------------------------------------------------------

#ifndef _DWMIOUTILS_HH_
#define _DWMIOUTILS_HH_

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
#include "DwmStreamedLengthCapable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  This class contains a collection of static functions for calculating
  //!  the streamed length of simple types when using DescriptorIO, FileIO
  //!  and StreamedIO.  It also contains functions for calculating the
  //!  streamed length of strings, deques, lists, vectors, maps, 
  //!  multimaps, sets, multisets, unordered_maps, unordered_multimaps,
  //!  unordered_sets, unordered_multisets, tuples and variants.
  //!  We use our member functions to handle simple types in the containers,
  //!  and function templates to handle other class types. User-defined
  //!  classes must implement the StreamedLengthCapable interface, since the
  //!  function templates simply call out to it.
  //--------------------------------------------------------------------------
  class IOUtils
  {
  public:
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for a char.  Should always be 1.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(char c)
    {
      return(sizeof(c));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for a uint8_t.  Should always be 1.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(uint8_t c)
    {
      return(sizeof(c));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() member for a bool.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(bool b)
    {
      return(1);
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for an \c int16_t.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(int16_t val)
    {
      return(sizeof(val));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for a \c uint16_t.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(uint16_t val)
    {
      return(sizeof(val));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for an \c int32_t.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(int32_t val)
    {
      return(sizeof(val));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for a \c uint32_t.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(uint32_t val)
    {
      return(sizeof(val));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for an \c int64_t.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(int64_t val)
    {
      return(sizeof(val));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that would be written if we called
    //!  Write() for a \c uint64_t.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(uint64_t val)
    {
      return(sizeof(val));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a float.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(float val)
    {
      return(4);
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a double.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(double val)
    {
      return(8);
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a string.
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(const std::string & s)
    {
      return(sizeof(uint64_t) + s.size());
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a pair<_firstT, _secondT>
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static uint64_t StreamedLength(const std::pair<_firstT, _secondT> & p)
    {
      return(StreamedLength(p.first) + StreamedLength(p.second));
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a map<_keyT, _valueT>
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static uint64_t
    StreamedLength(const std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(ContainerStreamedLength<std::map<_keyT, _valueT, _Compare, _Alloc> >(m));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a multimap<_keyT, _valueT>
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static uint64_t
    StreamedLength(const std::multimap<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerStreamedLength<std::multimap<_keyT,_valueT,_Compare,_Alloc> >(m));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for an array<_valueT,N>
    //------------------------------------------------------------------------
    template <typename _valueT, size_t N>
    static uint64_t StreamedLength(const std::array<_valueT, N> & a)
    {
      uint64_t  rc = 0;
      for (size_t i = 0; i < N; ++i) {
        rc += StreamedLength(a[i]);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a vector<_valueT>
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static uint64_t StreamedLength(const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerStreamedLength<std::vector<_valueT, _Alloc> >(v));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a deque<_valueT>
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static uint64_t StreamedLength(const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerStreamedLength<std::deque<_valueT, _Alloc> >(d));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a list<_valueT>
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static uint64_t StreamedLength(const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerStreamedLength<std::list<_valueT, _Alloc> >(l));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a set<_valueT>
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static uint64_t 
    StreamedLength(const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerStreamedLength<std::set<_valueT, _Compare, _Alloc> >(l));
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a multiset<_valueT>
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static uint64_t
    StreamedLength(const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerStreamedLength<std::multiset<_valueT, _Compare, _Alloc> >(l));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, typename... Args>
    static uint64_t VarStreamedLength(const T & t, Args... args)
    {
      uint64_t rc = StreamedLength(t);
      if (sizeof...(Args) > 0)
        rc += VarStreamedLength(args...);
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Returns the number of bytes that should be written if we call Write()
    //!  for a tuple.
    //------------------------------------------------------------------------
    template <typename T, typename... Args>
    static uint64_t StreamedLength(const std::tuple<T, Args...> & t)
    {
      return(TupleStreamedLength<std::tuple<T, Args...> >(t));
    }
    
    //------------------------------------------------------------------------
    //!  T must be a tuple.
    //------------------------------------------------------------------------
    template <typename T>
    static uint64_t TupleStreamedLength(const T & t)
    {
      return(TupleIOHelper<T,std::tuple_size<T>::value-1>::StreamedLength(t));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(const std::monostate & sm)
    {
      return 0;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename... Ts>
    static uint64_t StreamedLength(const std::variant<Ts...> & v)
    {
      uint64_t  rc = sizeof(uint64_t);
      std::visit([&] (const auto & arg)
                   { rc += StreamedLength(arg); }, v);
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static uint64_t
    StreamedLength(const std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      using  myType = std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc>;
      
      return(ContainerStreamedLength<myType>(m));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static uint64_t
    StreamedLength(const std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      using  myType = std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc>;
      return(ContainerStreamedLength<myType>(m));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash, 
              typename _Pred, typename _Alloc>
    static uint64_t
    StreamedLength(const std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      using  myType = std::unordered_set<_valueT, _Hash, _Pred, _Alloc>;
      return(ContainerStreamedLength<myType>(m));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash, 
              typename _Pred, typename _Alloc>
    static uint64_t
    StreamedLength(const std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      using  myType = std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc>;
      return(ContainerStreamedLength<myType>(m));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static uint64_t StreamedLength(const StreamedLengthCapable & sl)
    {
      return sl.StreamedLength();
    }
    
  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static uint64_t StreamedLength(_inputIteratorT f, _inputIteratorT l)
    {
      uint64_t  rc = 0;
      for ( ; f != l; ++f)
        rc += StreamedLength(*f);
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename _containerT>
    static uint64_t ContainerStreamedLength(const _containerT & c)
    {
      uint64_t  rc = sizeof(uint64_t);  // for size()

      rc += StreamedLength<typename _containerT::const_iterator>(c.begin(), 
                                                                 c.end());
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
      //!  Return the number of bytes that will be written if we write the
      //!  tuple \c t.
      //----------------------------------------------------------------------
      static uint64_t StreamedLength(const T & t)
      {
        return(IOUtils::StreamedLength(std::get<0>(t)));
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
      //!  Return the number of bytes that will be written if we write the
      //!  tuple \c t.
      //----------------------------------------------------------------------
      static uint64_t StreamedLength(const T & t)
      {
        uint64_t  rc = TupleIOHelper<T,elt-1>::StreamedLength(t);
        rc += IOUtils::StreamedLength(std::get<elt>(t));
        return(rc);
      }
    };

  };
  
}  // namespace Dwm

#endif  // _DWMIOUTILS_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
