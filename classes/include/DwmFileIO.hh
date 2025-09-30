//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2004-2007, 2016-2017, 2020, 2024-2025
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

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>

#include "DwmPortability.hh"
#include "DwmIOConcepts.hh"
#include "DwmFileIOCapable.hh"
#include "DwmSysLogger.hh"
#include "DwmTypeName.hh"
#include "DwmVariantFromIndex.hh"

namespace Dwm {

  namespace fileio_detail {

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be read from
    //!  a FILE via a FileIO::Read() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsReadable =
    (io_detail::Readable<std::remove_reference_t<T>,HasFileRead_t>() == true);

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be written
    //!  to a FILE via a File::Write() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsWritable =
    (io_detail::Readable<std::remove_cvref_t<T>,HasFileRead_t>() == true)
      and (io_detail::Writable<std::remove_cvref_t<T>,HasFileWrite_t>() == true);

  }  // namespace fileio_detail
  
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
    //!  Reads @c c from @c f.  Returns the number of bytes read (1 on
    //!  success, 0 on failure).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, char & c);

    //------------------------------------------------------------------------
    //!  Writes @c c to @c f.  Returns the number of bytes written (1) on 
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, char c);

    //------------------------------------------------------------------------
    //!  Reads @c c from @c f.  Returns the number of bytes read (1 on
    //!  success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int8_t & c);

    //------------------------------------------------------------------------
    //!  Writes @c c to @c f.  Returns the number of bytes written (1) on
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, int8_t c);
 
    //------------------------------------------------------------------------
    //!  Reads @c c from @c f.  Returns the number of bytes read (1 on
    //!  success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Writes @c c to @c f.  Returns the number of bytes written (1) on
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, uint8_t c);
 
    //------------------------------------------------------------------------
    //!  Reads @c b from @c f.  Returns the number of bytes read (1 on
    //!  success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, bool & b);
    
    //------------------------------------------------------------------------
    //!  Writes @c b to @c f.  Returns the number of bytes written (1) on
    //!  success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, bool b);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (2 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int16_t & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (2) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, int16_t val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (2 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint16_t & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (2) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, uint16_t val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (4 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int32_t & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (4) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, int32_t val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (4 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint32_t & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (4) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, uint32_t val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (8 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, int64_t & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (8) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const int64_t & val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in network byte order (MSB first).
    //!  Returns the number of bytes read (8 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, uint64_t & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in network byte order (MSB first).  Returns
    //!  the number of bytes written (8) on success.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const uint64_t & val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in IEEE format (see RFC 1832 and/or
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes read
    //! (4 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, float & val);
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes 
    //!  written (4 on success).
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, float val);
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c f, in IEEE format (see RFC 1832 and/or
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes read
    //! (8 on success).
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, double & val);
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c f, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns the number of bytes 
    //!  written (8 on success).
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const double & val);
    
    //------------------------------------------------------------------------
    //!  Reads @c s from @c f.  Since we write strings with a 64-bit length
    //!  value preceding, and always write the terminating NULL, this
    //!  function will always return a value of 9 or greater on success.
    //------------------------------------------------------------------------
    static size_t Read(FILE * f, std::string & s);

    //------------------------------------------------------------------------
    //!  Writes @c s to @c f.  Returns the number of bytes written.  Note
    //!  that we first write a 64-bit length value, then the string itself
    //!  (with terminating NULL).  Hence a successful write will always
    //!  return a value of 8 + @c s.length() + 1.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const std::string & s);

    //------------------------------------------------------------------------
    //!  Reads @c t from @c f, where @c t is an enumerated type.  Returns 1
    //!  on success, 0 on failure.  Note this is risky for enumerated types
    //!  whose underlying type is not of fixed size.
    //------------------------------------------------------------------------
    template <typename T>
    static size_t Read(FILE * f,  T & t)
      requires std::is_enum_v<T>
    {
      size_t  rc = 0;
      std::underlying_type_t<T>  cp;
      if (Read(f, cp)) {
        t = static_cast<T>(cp);
        rc = 1;
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Writes @c t to @c f, where @c t is an enumerated type.  Returns 1
    //!  on success, 0 on failure.  Note this is risky for enumerated types
    //!  whose underlying type is not of fixed size.
    //------------------------------------------------------------------------
    template <typename T>
    static size_t Write(FILE * f, const T & t)
      requires std::is_enum_v<T>
    {
      return Write(f, static_cast<std::underlying_type_t<T>>(t));
    }

    //------------------------------------------------------------------------
    //!  Wrapper function to read a FileReadable object from a FILE.
    //------------------------------------------------------------------------
    static size_t Read(FILE *f, FileReadable & val)
    { return val.Read(f); }

    //------------------------------------------------------------------------
    //!  Reads @c val from FILE @c f, where @c val meets the requirements
    //!  of the HasFileRead concept.
    //------------------------------------------------------------------------
    static size_t Read(FILE *f, HasFileRead auto & val)
    { return val.Read(f); }
      
    //------------------------------------------------------------------------
    //!  Wrapper function to write a FileWritable object to a FILE.
    //------------------------------------------------------------------------
    static size_t Write(FILE * f, const FileWritable & val)
    { return(val.Write(f)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to FILE @c f, where @c val meets the requirements of
    //!  the HasFileWrite concept.
    //------------------------------------------------------------------------
    static size_t Write(FILE *f, const HasFileWrite auto & val)
    { return val.Write(f); }
    
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
    //!  Reads a pair-associative container (map, multimap, unordered_map or
    //!  unordered_multimap) from a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires Concepts::is_std_pair_associative_container<T>
    static size_t Read(FILE *f, T & c)
    {
      return PairAssocContRead<T>(f, c);
    }

    //------------------------------------------------------------------------
    //!  Writes a container (except std::array) @c c to a FILE @c f.  Returns
    //!  1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires Concepts::is_std_associative_container<T>
      or Concepts::is_std_pair_associative_container<T>
      or (Concepts::is_std_sequence_container<T>
          and (not Concepts::is_std_array<T>))
    static size_t Write(FILE *f, const T & c)
    {
      size_t  rc = 0;
      if (f) {
        uint64_t  numEntries = c.size();
        if (Write(f, numEntries)) {
          if (numEntries) {
            rc = Write<typename T::const_iterator>(f, c.cbegin(), c.cend());
          }
          else {
            rc = 1;
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a sequence container (deque, list or vector) or associative
    //!  container (set, multiset, unordered_set or unordered_multiset) from
    //!  a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires Concepts::is_std_associative_container<T>
      or (Concepts::is_std_sequence_container<T>
          and (not Concepts::is_std_array<T>))
    static size_t Read(FILE *f, T & c)
    {
      c.clear();
      size_t  rc = 0;
      if (f) {
        uint64_t  numEntries;
        if (Read(f, numEntries)) {
          uint64_t  i = 0;
          for ( ; i < numEntries; ++i) {
            typename T::value_type  val;
            if (! Read(f, val)) {
              break;
            }
            c.insert(c.end(), std::move(val));
          }
          rc = (i == numEntries) ? 1 : 0;
        }
      }
      return rc;
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
    //!  Reads a tuple from a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename... Args>
    static size_t Read(FILE *f, std::tuple<Args...> & t)
    {
      return(std::apply([&f](auto && ...args)
      {
        auto  read_tuple_mem = [&f](auto && x) { return Read(f, x); };
        return (read_tuple_mem(args) && ...);
      }, t));
    }

    //------------------------------------------------------------------------
    //!  Writes a tuple to a FILE.  Returns 1 on success, 0 on failure.
    //------------------------------------------------------------------------
    template <typename... Args>
    static size_t Write(FILE *f, const std::tuple<Args...> & t)
    {
      return(std::apply([&f](auto && ...args)
      {
        auto  write_tuple_mem = [&f](auto && x) { return Write(f, x); };
        return (write_tuple_mem(args) && ...);
      }, t));
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
    //!  Reads multiple objects from a FILE.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename... Args>
    static size_t ReadV(FILE *f, Args & ...args)
    {
      return (Read(f,args) && ...);
    }

    //------------------------------------------------------------------------
    //!  Writes multiple objects to a FILE.  Returns 1 on success, 0 on
    //!  failure.
    //------------------------------------------------------------------------
    template <typename... Args>
    static size_t WriteV(FILE *f, const Args & ...args)
    {
      return (Write(f,args) && ...);
    }

    //------------------------------------------------------------------------
    //!  Writes a bounded array @c v to FILE @c f.  Returns 1 on success, 0
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static size_t Write(FILE *f, const T & v)
    {
      static_assert(fileio_detail::IsWritable<std::remove_reference_t<decltype(v[0])>>);
      size_t  rc = 0;
      if (f) {
        const uint64_t  n = std::extent_v<T>;
        if (Write(f, n)) {
          size_t  i = 0;
          for ( ; i < std::extent_v<T>; ++i) {
            if (! Write(f, v[i])) {
              break;
            }
          }
          rc = (n == i) ? 1 : 0;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a bounded array @c v from FILE @c f.  Returns 1 on success, 0
    //!  on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static size_t Read(FILE *f, T & v)
    {
      static_assert(fileio_detail::IsReadable<std::remove_reference_t<decltype(v[0])>>);
      size_t  rc = 0;
      if (f) {
        uint64_t  n;
        if (Read(f, n)) {
          if (std::extent_v<T> == n) {
            size_t  i = 0;
            for ( ; i < std::extent_v<T>; ++i) {
              if (! Read(f, v[i])) {
                break;
              }
            }
            rc = (std::extent_v<T> == i) ? 1 : 0;
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, if it points to a single
    //!  object (deduced by requiring std::default_delete as its deleter).
    //------------------------------------------------------------------------
    template <typename T>
    static size_t Write(FILE *f, const std::unique_ptr<T> & t)
    {
      using deleterType = std::remove_cvref_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType, std::default_delete<T>>);
      static_assert(! std::is_unbounded_array_v<T>);
      static_assert(fileio_detail::IsWritable<T>);
      size_t  rc = 0;
      bool  isNull = (nullptr == t);
      if (Write(f, isNull)) {
        if (! isNull) {
          rc = Write(f, *t);
        }
        else {
          rc = 1;
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
    static size_t Read(FILE *f, std::unique_ptr<T> & t)
    {
      using deleterType = std::remove_reference_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType,std::default_delete<T>>);
      static_assert(! std::is_unbounded_array_v<T>);
      static_assert(fileio_detail::IsReadable<T>);
      size_t  rc = 0;
      if (f) {
        bool  isNull = true;
        if (Read(f, isNull)) {
          if (isNull) {
            t.release();
            rc = 1;
          }
          else {
            if (nullptr == t) {
              try {
                t = std::make_unique<T>();
              }
              catch (std::bad_alloc & ex) {
                FSyslog(LOG_ERR, "Failed to allocate an object of type {}",
                        TypeName<decltype(t)>());
                return 0;
              }
            }
            if (Read(f, *t)) {
              rc = 1;
            }
            else {
              t.release();
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes a std::optional<T> @c t to FILE @c f.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    static size_t Write(FILE *f, const std::optional<T> & t)
    {
      static_assert(fileio_detail::IsWritable<T>);
      size_t  rc = 0;
      if (f) {
        bool  hasValue = t.has_value();
        if (Write(f, hasValue)) {
          if (hasValue) { rc = Write(f, t.value()); }
          else          { rc = 1; }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads a std::optional<T> @c t from FILE @c f.  Returns 1 on success,
    //!  0 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_default_constructible_v<T>
    static size_t Read(FILE *f, std::optional<T> & t)
    {
      size_t  rc = 0;
      if (f) {
        bool  hasValue = false;
        if (Read(f, hasValue)) {
          if (hasValue) {
            if (! t.has_value()) {
              t = T();
            }
            rc = Read(f, t.value());
          }
          else {
            t.reset();
            rc = 1;
          }
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
      and (not HasFileWrite<T>)
    static size_t Write(FILE *f, const T & v)
    {
      if (f) {
        using fileio_detail::IsWritable;
        using io_detail::Skip, io_detail::SkipReason, io_detail::DenyReason;
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
            if constexpr (IsWritable<decltype(v.[:mem:])>) {
              if (! Write(f, v.[:mem:])) {
                return 0;
              }
            }
            else {
              FSyslog(LOG_ERR, "{}.{} of type '{}' is unwritable{}",
                      TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                      std::meta::display_string_of(std::meta::type_of(mem)),
                      DenyReason<mem>());
              return 0;
            }
          }
        }
        return 1;
      }
      return 0;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
      and (not io_detail::DirectlySupported<T>)
      and (not io_detail::SupportedContainer<T>)
      and (not io_detail::DenyType<T>)
      and (not HasFileRead<T>)
    static size_t Read(FILE *f, T & v)
    {
      using fileio_detail::IsReadable;
      using io_detail::Skip, io_detail::SkipReason, io_detail::DenyReason;
      if (f) {
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
            if constexpr (IsReadable<decltype(v.[:mem:])>) {
              if (! Read(f, (v.[:mem:]))) {
                return 0;
              }
            }
            else {
              FSyslog(LOG_ERR, "{}.{} of type '{}' is unreadable{}",
                      TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                      std::meta::display_string_of(std::meta::type_of(mem)),
                      DenyReason<mem>());
              return 0;
            }
          }
        }
        return 1;
      }
      return 0;
    }
    
#endif  // defined(DWM_CAN_USE_REFLECTION)
    
  private:
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

  };  // class FileIO

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be written to
  //!  a FILE via a FileIO::Write() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsFileWritable =
  (fileio_detail::IsWritable<T> == true)
  and requires(const T & t, FILE *f) {
    { FileIO::Write(f, t) } -> std::same_as<size_t>;
  };

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be read from
  //!  a FILE via a FileIO::Read() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsFileReadable =
  (fileio_detail::IsReadable<T> == true)
  and requires(T & t, FILE *f) {
    { FileIO::Read(f, t) } -> std::same_as<size_t>;
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
