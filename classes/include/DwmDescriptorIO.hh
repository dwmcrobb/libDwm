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
//!  \file DwmDescriptorIO.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::DescriptorIO class declaration
//---------------------------------------------------------------------------

#ifndef _DWMDESCRIPTORIO_HH_
#define _DWMDESCRIPTORIO_HH_

extern "C" {
  #include <unistd.h>
  #include <sys/uio.h>
}

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>

#include "DwmPortability.hh"
#include "DwmDescriptorIOCapable.hh"
#include "DwmEndianness.hh"
#include "DwmIOConcepts.hh"
#include "DwmStreamIO.hh"
#include "DwmStreamedLengthCapable.hh"
#include "DwmVariantFromIndex.hh"
#include "DwmSysLogger.hh"
#include "DwmTypeName.hh"

namespace Dwm {

    namespace descio_detail {

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be read from
    //!  a descriptor via a DescriptorIO::Read() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsReadable =
    (io_detail::Readable<std::remove_reference_t<T>,HasDescriptorRead_t>() == true);

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be written
    //!  to a descriptor via a DescriptorIO::Write() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsWritable =
    (io_detail::Readable<std::remove_cvref_t<T>,HasDescriptorRead_t>() == true)
      and (io_detail::Writable<std::remove_cvref_t<T>,HasDescriptorWrite_t>() == true);

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be read from
    //!  a descriptor via a DescriptorIO::NRead() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsNReadable =
    (io_detail::Readable<std::remove_reference_t<T>,HasDescriptorNRead_t>() == true);

    //------------------------------------------------------------------------
    //!  Simple concept expressing that an instance of type T can be written
    //!  to a descriptor via a DescriptorIO::NWrite() member.
    //------------------------------------------------------------------------
    template <typename T>
    concept IsNWritable =
    (io_detail::Readable<std::remove_cvref_t<T>,HasDescriptorNRead_t>() == true)
      and (io_detail::Writable<std::remove_cvref_t<T>,HasDescriptorNWrite_t>() == true);
      
  }  // namespace descio_detail

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
    //!  Reads @c c from @c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, char & c);

    //------------------------------------------------------------------------
    //!  Reads @c c from @c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, char & c)
    { return Read(fd, c); }
    
    //------------------------------------------------------------------------
    //!  Writes @c c to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, char c);

    //------------------------------------------------------------------------
    //!  Writes @c c to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, char c)
    { return Write(fd, c); }

    //------------------------------------------------------------------------
    //!  Reads @c c from @c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, int8_t & c);

    //------------------------------------------------------------------------
    //!  Reads @c c from @c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, int8_t & c)
    { return Read(fd, c); }
    
    //------------------------------------------------------------------------
    //!  Writes @c c to @c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, int8_t c);

    //------------------------------------------------------------------------
    //!  Writes @c c to @c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, int8_t c)
    { return Write(fd, c); }
    
    //------------------------------------------------------------------------
    //!  Reads @c c from @c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, uint8_t & c);

    //------------------------------------------------------------------------
    //!  Reads @c c from @c fd.  Returns the number of bytes read (1) on 
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, uint8_t & c)
    { return Read(fd, c); }
    
    //------------------------------------------------------------------------
    //!  Writes @c c to @c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, uint8_t c);

    //------------------------------------------------------------------------
    //!  Writes @c c to @c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, uint8_t c)
    { return Write(fd, c); }
    
    //------------------------------------------------------------------------
    //!  Reads @c b from @c fd.  Returns the number of bytes read (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, bool & b);

    //------------------------------------------------------------------------
    //!  Reads @c b from @c fd.  Returns the number of bytes read (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, bool & b)
    { return Read(fd, b); }
    
    //------------------------------------------------------------------------
    //!  Writes @c b to @c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, bool b);

    //------------------------------------------------------------------------
    //!  Writes @c b to @c fd.  Returns the number of bytes written (1) on
    //!  success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, bool b)
    { return Write(fd, b); }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t Read(int fd, int16_t & val)
    { 
      if (NRead(fd, val) == sizeof(val)) {
        val = BE2Host(val);
        return sizeof(val);
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native byte order.  Returns the number
    //!  of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, int16_t & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static inline ssize_t Write(int fd, int16_t val)
    { return NWrite(fd, Host2BE(val)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native byte order.  Returns the number of
    //!  bytes written (2) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, int16_t val)
    { return (fd >= 0) ? ::write(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t Read(int fd, uint16_t & val)
    {
      if (NRead(fd, val) == sizeof(val)) {
        val = BE2Host(val);
        return sizeof(val);
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native byte order.  Returns the number of
    //!  bytes read (2) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, uint16_t & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
      
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (2) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static inline ssize_t Write(int fd, uint16_t val)
    { return NWrite(fd, Host2BE(val)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native byte order.  Returns the number of
    //!  bytes written (2) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, uint16_t val)
    { return (fd >= 0) ? ::write(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t Read(int fd, int32_t & val)
    {
      if (NRead(fd, val) == sizeof(val)) {
        val = BE2Host(val);
        return sizeof(val);
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native byte order.  Returns the number of
    //!  bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, int32_t & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static inline ssize_t Write(int fd, int32_t val)
    { return NWrite(fd, Host2BE(val)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native byte order.  Returns the number of
    //!  bytes written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, int32_t val)
    { return (fd >= 0) ? ::write(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t Read(int fd, uint32_t & val)
    {
      if (NRead(fd, val) == sizeof(val)) {
        val = BE2Host(val);
        return sizeof(val);
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native byte order.  Returns the number of
    //!  bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, uint32_t & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (4) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static inline ssize_t Write(int fd, uint32_t val)
    { return NWrite(fd, Host2BE(val)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native byte order.  Returns the number of
    //!  bytes written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, uint32_t val)
    { return (fd >= 0) ? ::write(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t Read(int fd, int64_t & val)
    {
      if (NRead(fd, val) == sizeof(val)) {
        val = BE2Host(val);
        return sizeof(val);
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native byte order.  Returns the number
    //!  of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, int64_t & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static inline ssize_t Write(int fd, const int64_t & val)
    { return NWrite(fd, Host2BE(val)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native byte order.  Returns the number of
    //!  bytes written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, const int64_t & val)
    { return (fd >= 0) ? ::write(fd, (const void *)&val, sizeof(val)) : -1; }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in network byte order (MSB first). 
    //!  Returns number of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t Read(int fd, uint64_t & val)
    {
      if (NRead(fd, val) == sizeof(val)) {
        val = BE2Host(val);
        return sizeof(val);
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native byte order.  Returns the number
    //!  of bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, uint64_t & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in network byte order (MSB first).
    //!  Returns the number of bytes written (8) on success, less on
    //!  failure.
    //------------------------------------------------------------------------
    static inline ssize_t Write(int fd, const uint64_t & val)
    { return NWrite(fd, Host2BE(val)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native byte order.  Returns the number of
    //!  bytes written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, const uint64_t & val)
    { return (fd >= 0) ? ::write(fd, (const void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (4)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, float & val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native format.  Returns the number of
    //!  bytes read (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, float & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
      
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, float val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native format.  Returns the number of
    //!  bytes written (4) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, float val)
    { return (fd >= 0) ? ::write(fd, (const void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in IEEE format (see RFC 1832 and/or 
    //!  ANSI/IEEE Standard 754-1985).  Returns number of bytes read (8)
    //!  on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, double & val);

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, in native format.  Returns the number of
    //!  bytes read (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NRead(int fd, double & val)
    { return (fd >= 0) ? ::read(fd, (void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in IEEE format (see RFC 1832
    //!  and/or ANSI/IEEE Standard 754-1985).  Returns the number of bytes
    //!  written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const double & val);

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, in native format.  Returns the number of
    //!  bytes written (8) on success, less on failure.
    //------------------------------------------------------------------------
    static inline ssize_t NWrite(int fd, const double & val)
    { return (fd >= 0) ? ::write(fd, (const void *)&val, sizeof(val)) : -1; }
    
    //------------------------------------------------------------------------
    //!  Reads @c s from @c fd.  Returns the number of bytes read on success,
    //!  -1 on failure.  Write(int, const std::string &) writes strings with
    //!  a 64-bit unsigned length value (in network byte order, MSB first)
    //!  preceding the string contents, hence this always reads at least 8
    //!  bytes on success.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, std::string & s);

    //------------------------------------------------------------------------
    //!  Reads @c s from @c fd.  Returns the number of bytes read on success,
    //!  -1 on failure.  NWrite(int,const std::string &) writes strings with
    //!  a 64-bit unsigned length value (in native byte order) preceding the
    //!  string contents, hence this always reads at least 8 bytes on success.
    //------------------------------------------------------------------------
    static ssize_t NRead(int fd, std::string & s);
    
    //------------------------------------------------------------------------
    //!  Writes @c s to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.  Note that a 64-bit value (in network byte
    //!  order, MSB first) is written first, holding the length of the string.
    //!  Hence, on success this will always return a value of 8 or greater.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const std::string & s);

    //------------------------------------------------------------------------
    //!  Writes @c s to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.  Note that a 64-bit value (in native byte
    //!  order) is written first, holding the length of the string.  Hence,
    //!  on success this will always return a value of 8 or greater.
    //------------------------------------------------------------------------
    static ssize_t NWrite(int fd, const std::string & s);
    
    //------------------------------------------------------------------------
    //!  Writes @c v to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.  Note that an EncodedU64 is written first,
    //!  holding the size of the string_view.  Hence, on success this will
    //!  always return a value of 2 or greater.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, std::string_view v);
    
    //------------------------------------------------------------------------
    //!  Reads @c t from @c fd, where @c t is an enumerated type.  Returns
    //!  the number of bytes read on success, -1 on failure.  Note that this
    //!  is risky for enumerated types whose underlying type is not of fixed
    //!  size.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Read(int fd, T & t)
      requires std::is_enum_v<T>
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        std::underlying_type_t<T>  cp;
        rc = Read(fd, cp);
        if (0 < rc) {
          t = static_cast<T>(cp);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Reads @c t from @c fd in native byte order, where @c t is an
    //!  enumerated type.  Returns the number of bytes read on success, -1 on
    //!  failure.  Note that this is risky for enumerated types whose
    //!  underlying type is not of fixed size.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NRead(int fd, T & t)
      requires std::is_enum_v<T>
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        std::underlying_type_t<T>  cp;
        rc = NRead(fd, cp);
        if (0 < rc) {
          t = static_cast<T>(cp);
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  Writes @c t to @c fd in network byte order, where @c t is an
    //!  enumerated types.  Returns the number of bytes written on success,
    //!  -1 on failure.  Note that this is risky for enumerated types whose
    //!  underlying type is not of fixed size.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Write(int fd, const T & t)
      requires std::is_enum_v<T>
    {
      return Write(fd, static_cast<std::underlying_type_t<T>>(t));
    }

    //------------------------------------------------------------------------
    //!  Writes @c t to @c fd in native byte order, where @c t is an
    //!  enumerated types.  Returns the number of bytes written on success,
    //!  -1 on failure.  Note that this is risky for enumerated types whose
    //!  underlying type is not of fixed size.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NWrite(int fd, const T & t)
      requires std::is_enum_v<T>
    {
      return NWrite(fd, static_cast<std::underlying_type_t<T>>(t));
    }
    
    //------------------------------------------------------------------------
    //!  Wrapper function to read a DescriptorReadable object from a
    //!  descriptor.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, DescriptorReadable & val)
    { return(val.Read(fd)); }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, where @c val meets the requirements of the
    //!  HasDescriptorRead concept.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, HasDescriptorRead auto & val)
    { return val.Read(fd); }

    //------------------------------------------------------------------------
    //!  Reads @c val from @c fd, where @c val meets the requirements of the
    //!  HasDescriptorNRead concept.
    //------------------------------------------------------------------------
    static ssize_t NRead(int fd, HasDescriptorNRead auto & val)
    { return val.NRead(fd); }
    
    //------------------------------------------------------------------------
    //!  Wrapper function to write a DescriptorWritable object to a
    //!  descriptor.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const DescriptorWritable & val)
    { return(val.Write(fd)); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, where @c val meets the requirements of the
    //!  HasDescriptorWrite concept.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const HasDescriptorWrite auto & val)
    { return val.Write(fd); }

    //------------------------------------------------------------------------
    //!  Writes @c val to @c fd, where @c val meets the requirements of the
    //!  HasDescriptorNWrite concept.
    //------------------------------------------------------------------------
    static ssize_t NWrite(int fd, const HasDescriptorNWrite auto & val)
    { return val.NWrite(fd); }
    
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
    //!  Reads a pair<_firstT,_secondT> from a file descriptor.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _firstT, typename _secondT>
    static ssize_t NRead(int fd, std::pair<_firstT, _secondT> & p)
    {
      ssize_t  rc = -1;
      if (fd >= 0) {
        ssize_t  bytesRead = NRead(fd, p.first);
        if (bytesRead > 0) {
          rc = bytesRead;
          bytesRead = NRead(fd, p.second);
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
    static ssize_t NWrite(int fd, const std::pair<_firstT,_secondT> & p)
    {
      ssize_t  rc = -1;
      if (fd >= 0) {
        ssize_t  bytesWritten = NWrite(fd, p.first);
        if (bytesWritten > 0) {
          rc = bytesWritten;
          bytesWritten = NWrite(fd, p.second);
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
    //!  Reads a map<_keyT,_valueT> from a file descriptor.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Compare, typename _Alloc>
    static ssize_t NRead(int fd, std::map<_keyT, _valueT, _Compare, _Alloc> & m)
    {
      return(PairAssocContNRead<std::map<_keyT, _valueT, _Compare, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes a map<_keyT,_valueT> to a file descriptor.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Compare, typename _Alloc>
    static ssize_t 
    NWrite(int fd, const std::map<_keyT,_valueT,_Compare,_Alloc> & m)
    {
      return(ContainerNWrite<std::map<_keyT,_valueT,_Compare,_Alloc> >(fd, m));
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
    //!  Read an array<_valueT,N> from a file descriptor.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, std::size_t N>
    static ssize_t NRead(int fd, std::array<_valueT, N> & a)
    {
      ssize_t  rc = 0;
      for (std::size_t i = 0; i < N; ++i) {
        ssize_t  bytesRead = NRead(fd, a[i]);
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
    static ssize_t NWrite(int fd, const std::array<_valueT, N> & a)
    {
      ssize_t  rc = 0;
      for (std::size_t i = 0; i < N; ++i) {
        ssize_t  bytesWritten = NWrite(fd, a[i]);
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
    //!  Reads a vector<_valueT> from a file descriptor, in native byte order.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t NRead(int fd, std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerNRead<std::vector<_valueT, _Alloc> >(fd, v));
    }

    //------------------------------------------------------------------------
    //!  Writes a vector<_valueT> to a file descriptor, in native byte order.
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t NWrite(int fd, const std::vector<_valueT, _Alloc> & v)
    {
      return(ContainerNWrite<std::vector<_valueT, _Alloc> >(fd, v));
    }
    
    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from a file descriptor, in network byte
    //!  order.  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Read(int fd, std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerRead<std::deque<_valueT, _Alloc> >(fd, d));
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to a file descriptor, in network byte order.
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t Write(int fd, const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerWrite<std::deque<_valueT, _Alloc> >(fd, d));
    }

    //------------------------------------------------------------------------
    //!  Reads a deque<_valueT> from a file descriptor, in native byte order.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t NRead(int fd, std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerNRead<std::deque<_valueT, _Alloc> >(fd, d));
    }

    //------------------------------------------------------------------------
    //!  Writes a deque<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t NWrite(int fd, const std::deque<_valueT, _Alloc> & d)
    {
      return(ContainerNWrite<std::deque<_valueT, _Alloc> >(fd, d));
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
    //!  Reads a list<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t NRead(int fd, std::list<_valueT, _Alloc> & l)
    {
      return(ContainerNRead<std::list<_valueT, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a list<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Alloc>
    static ssize_t NWrite(int fd, const std::list<_valueT, _Alloc> & l)
    {
      return(ContainerNWrite<std::list<_valueT, _Alloc> >(fd, l));
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
    //!  Reads a set<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t NRead(int fd, std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerNRead<std::set<_valueT, _Compare, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a set<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t
    NWrite(int fd, const std::set<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerNWrite<std::set<_valueT, _Compare, _Alloc> >(fd, l));
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
    //!  Reads a multiset<_valueT> from a file descriptor.  Returns the number
    //!  of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t NRead(int fd, std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerNRead<std::multiset<_valueT, _Compare, _Alloc> >(fd, l));
    }

    //------------------------------------------------------------------------
    //!  Writes a multiset<_valueT> to a file descriptor.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Compare, typename _Alloc>
    static ssize_t
    NWrite(int fd, const std::multiset<_valueT, _Compare, _Alloc> & l)
    {
      return(ContainerNWrite<std::multiset<_valueT, _Compare, _Alloc> >(fd, l));
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
    //!  Reads a tuple from a file descriptor.  Returns the number of
    //!  bytes read.
    //------------------------------------------------------------------------
    template <typename... Args>
    static ssize_t NRead(int fd, std::tuple<Args...> & t)
    {
      return(std::apply([&fd](auto&&...args) 
      {
        ssize_t  rc = 0;
        auto read_tuple_mem = [&fd,&rc](auto&& x) {
          int  bytesRead = NRead(fd, x);
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
    static ssize_t NWrite(int fd, const std::tuple<Args...> & t)
    {
      return(std::apply([&fd](auto&&...args) 
      {
        ssize_t  rc = 0;
        auto read_tuple_mem = [&fd,&rc](auto&& x) {
          int  bytesWritten = NWrite(fd, x);
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
    //!  Reads an unordered_map<_keyT,_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    NRead(int fd, std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContNRead<std::unordered_map<_keyT, _valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_map<_keyT,_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    NWrite(int fd, 
           const std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerNWrite<std::unordered_map<_keyT,_valueT,_Hash,_Pred,_Alloc> >(fd, m));
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
    //!  Reads an unordered_multimap<_keyT,_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _keyT, typename _valueT, 
              typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    NRead(int fd, std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(PairAssocContNRead<std::unordered_multimap<_keyT, _valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multimap<_keyT,_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _keyT, typename _valueT, 
             typename _Hash, typename _Pred, typename _Alloc>
    static ssize_t 
    NWrite(int fd,
           const std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerNWrite<std::unordered_multimap<_keyT,_valueT,_Hash,_Pred,_Alloc> >(fd, m));
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
    //!  Reads an unordered_set<_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static ssize_t 
    NRead(int fd, std::unordered_set<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerNRead<std::unordered_set<_valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_set<_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static ssize_t 
    NWrite(int fd, const std::unordered_set<_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerNWrite<std::unordered_set<_valueT,_Hash,_Pred,_Alloc> >(fd, m));
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
    //!  Reads an unordered_multiset<_valueT> from a file descriptor.
    //!  Returns the number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename _valueT, typename _Hash,
              typename _Pred, typename _Alloc>
    static ssize_t 
    NRead(int fd, std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> & m)
    {
      return(ContainerNRead<std::unordered_multiset<_valueT, _Hash, _Pred, _Alloc> >(fd, m));
    }

    //------------------------------------------------------------------------
    //!  Writes an unordered_multiset<_valueT> to a file descriptor. 
    //!  Returns the number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template<typename _valueT, typename _Hash,
             typename _Pred, typename _Alloc>
    static ssize_t 
    NWrite(int fd, const std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> & m)
    {
      return(ContainerNWrite<std::unordered_multiset<_valueT,_Hash,_Pred,_Alloc> >(fd, m));
    }
    
    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Read() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, std::monostate & sm)
    { return 0; }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our Write() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const std::monostate & sm)
    { return 0; }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our NRead() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static ssize_t NRead(int fd, std::monostate & sm)
    { return 0; }

    //------------------------------------------------------------------------
    //!  Just a dummy helper function for std::variant instances that hold
    //!  a std::monostate.  This should only be called from our NWrite() for
    //!  std::variant (via std::visit()), and does nothing.  Returns 0.
    //------------------------------------------------------------------------
    static ssize_t NWrite(int fd, const std::monostate & sm)
    { return 0; }
    
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
    //!  Reads a variant from a descriptor.  Returns the number of bytes
    //!  written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename... Ts>
    static ssize_t NRead(int fd, std::variant<Ts...> & v)
    {
      ssize_t  rc = -1;
      uint64_t index = 0;
      ssize_t  bytesRead = NRead(fd, index);
      if (bytesRead > 0) {
        rc = bytesRead;
        if (index < std::variant_size_v<std::variant<Ts...>>) {
          v = VariantFromIndex<Ts...>(index);
          std::visit([&] (auto && arg) { bytesRead = NRead(fd, arg); }, v);
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
    static ssize_t NWrite(int fd, const std::variant<Ts...> & v)
    {
      ssize_t   rc = -1;
      uint64_t  index = v.index();
      ssize_t   bytesWritten = NWrite(fd, index);
      if (bytesWritten > 0) {
        rc = bytesWritten;
        std::visit([&fd,&bytesWritten] (const auto & arg)
                   { bytesWritten = NWrite(fd, arg); }, v);
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
    //!  Reads @c args from @c fd.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename ...Args>
    static ssize_t ReadV(int fd, Args & ...args)
    {
      ssize_t  rv = 0;
      auto  readOne = [&] (auto & arg) {
        bool  rc = true;
        int bytesRead = Read(fd,arg);
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
    //!  Writes @c args to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename ...Args>
    static ssize_t WriteV(int fd, const Args & ...args)
    {
      ssize_t  rv = 0;
      auto  writeOne = [&] (auto & arg) {
        bool  rc = true;
        int bytesWritten = Write(fd, arg);
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

    //------------------------------------------------------------------------
    //!  Reads @c args from @c fd.  Returns the number of bytes read on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename ...Args>
    static ssize_t NReadV(int fd, Args & ...args)
    {
      ssize_t  rv = 0;
      auto  readOne = [&] (auto & arg) {
        bool  rc = true;
        int bytesRead = NRead(fd,arg);
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
    //!  Writes @c args to @c fd.  Returns the number of bytes written on
    //!  success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename ...Args>
    static ssize_t NWriteV(int fd, const Args & ...args)
    {
      ssize_t  rv = 0;
      auto  writeOne = [&] (auto & arg) {
        bool  rc = true;
        int bytesWritten = NWrite(fd, arg);
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

    //------------------------------------------------------------------------
    //!  Reads a bounded array @c v from a descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static ssize_t Read(int fd, T & v)
    {
      static_assert(descio_detail::IsReadable<std::remove_all_extents_t<T>>);
      if (0 <= fd) {
        ssize_t   rc = 0;
        uint64_t  n;
        ssize_t   bytesRead = Read(fd, n);
        if ((sizeof(n) == bytesRead) && (std::extent_v<T> == n)) {
          rc += bytesRead;
          size_t  i = 0;
          for ( ; i < n; ++i) {
            bytesRead = Read(fd, v[i]);
            if (bytesRead > 0) {
              rc += bytesRead;
            }
            else {
              break;
            }
          }
          if (i != n) {
            rc = -1;
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Writes a bounded array @c v to descriptor @c fd.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static ssize_t Write(int fd, T const & v)
    {
      static_assert(descio_detail::IsWritable<std::remove_all_extents_t<T>>);
      if (0 <= fd) {
        ssize_t   rc = 0;
        uint64_t  n = std::extent_v<T>;
        ssize_t   bytesWritten = Write(fd, n);
        if (sizeof(n) == bytesWritten) {
          rc += bytesWritten;
          size_t i = 0;
          for ( ; i < std::extent_v<T>; ++i) {
            bytesWritten = Write(fd, v[i]);
            if (bytesWritten > 0) {
              rc += bytesWritten;
            }
            else {
              rc = -1;
              break;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }


    //------------------------------------------------------------------------
    //!  Reads a bounded array @c v from a descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static ssize_t NRead(int fd, T & v)
    {
      static_assert(descio_detail::IsNReadable<std::remove_all_extents_t<T>>);
      if (0 <= fd) {
        ssize_t   rc = 0;
        uint64_t  n;
        ssize_t   bytesRead = NRead(fd, n);
        if ((sizeof(n) == bytesRead) && (std::extent_v<T> == n)) {
          rc += bytesRead;
          size_t  i = 0;
          for ( ; i < n; ++i) {
            bytesRead = NRead(fd, v[i]);
            if (bytesRead > 0) {
              rc += bytesRead;
            }
            else {
              break;
            }
          }
          if (i != n) {
            rc = -1;
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Writes a bounded array @c v to descriptor @c fd.  Returns the number
    //!  of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_bounded_array_v<T> and (std::rank_v<T> >= 1)
    static ssize_t NWrite(int fd, T const & v)
    {
      static_assert(descio_detail::IsNWritable<std::remove_all_extents_t<T>>);
      if (0 <= fd) {
        ssize_t   rc = 0;
        uint64_t  n = std::extent_v<T>;
        ssize_t   bytesWritten = NWrite(fd, n);
        if (sizeof(n) == bytesWritten) {
          rc += bytesWritten;
          size_t i = 0;
          for ( ; i < std::extent_v<T>; ++i) {
            bytesWritten = NWrite(fd, v[i]);
            if (bytesWritten > 0) {
              rc += bytesWritten;
            }
            else {
              rc = -1;
              break;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, iff it points to a single
    //!  object.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_default_constructible_v<T>
    static ssize_t Read(int fd, std::unique_ptr<T> & t)
    {
      using deleterType = std::remove_reference_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType,std::default_delete<T>>);
      static_assert(descio_detail::IsReadable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     isNull = true;
        ssize_t  bytesRead = Read(fd, isNull);
        if (bytesRead > 0) {
          rc += bytesRead;
          if (isNull) {
            t.release();
            return rc;
          }
          else {
            if (nullptr == t) {
              try {
                t = std::make_unique<T>();
              }
              catch (std::bad_alloc & ex) {
                FSyslog(LOG_ERR, "Failed to allocate an object of type {}",
                        TypeName<decltype(t)>());
                return -1;
              }
            }
            bytesRead = Read(fd, *t);
            if (bytesRead > 0) {
              rc += bytesRead;
            }
            else {
              t.release();
              rc = -1;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, iff it points to a single
    //!  object.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Write(int fd, const std::unique_ptr<T> & t)
    {
      using deleterType = std::remove_cvref_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType, std::default_delete<T>>);
      static_assert(descio_detail::IsWritable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     isNull = (nullptr == t);
        ssize_t  bytesWritten = Write(fd, isNull);
        if (bytesWritten > 0) {
          rc += bytesWritten;
          if (! isNull) {
            bytesWritten = Write(fd, *t);
            if (bytesWritten > 0) { rc += bytesWritten; }
            else                  { rc = -1; }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, iff it points to a single
    //!  object.
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_default_constructible_v<T>
    static ssize_t NRead(int fd, std::unique_ptr<T> & t)
    {
      using deleterType = std::remove_reference_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType,std::default_delete<T>>);
      static_assert(descio_detail::IsNReadable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     isNull = true;
        ssize_t  bytesRead = NRead(fd, isNull);
        if (bytesRead > 0) {
          rc += bytesRead;
          if (isNull) {
            t.release();
            return rc;
          }
          else {
            if (nullptr == t) {
              try {
                t = std::make_unique<T>();
              }
              catch (std::bad_alloc & ex) {
                FSyslog(LOG_ERR, "Failed to allocate an object of type {}",
                        TypeName<decltype(t)>());
                return -1;
              }
            }
            bytesRead = NRead(fd, *t);
            if (bytesRead > 0) {
              rc += bytesRead;
            }
            else {
              t.release();
              rc = -1;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Experimental support for std::unique_ptr, iff it points to a single
    //!  object.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NWrite(int fd, const std::unique_ptr<T> & t)
    {
      using deleterType = std::remove_cvref_t<decltype(t)>::deleter_type;
      static_assert(std::is_same_v<deleterType, std::default_delete<T>>);
      static_assert(descio_detail::IsNWritable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     isNull = (nullptr == t);
        ssize_t  bytesWritten = NWrite(fd, isNull);
        if (bytesWritten > 0) {
          rc += bytesWritten;
          if (! isNull) {
            bytesWritten = NWrite(fd, *t);
            if (bytesWritten > 0) { rc += bytesWritten; }
            else                  { rc = -1; }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }
    
    //------------------------------------------------------------------------
    //!  Reads a std::optional<T> @c t from descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Read(int fd, std::optional<T> & t)
    {
      static_assert(descio_detail::IsReadable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     hasValue = false;
        ssize_t  bytesRead = Read(fd, hasValue);
        if (bytesRead > 0) {
          rc += bytesRead;
          if (hasValue) {
            if (! t.has_value()) {
              t = T();
            }
            bytesRead = Read(fd, t.value());
            if (bytesRead > 0) {
              rc += bytesRead;
            }
            else {
              rc = -1;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Writes a std::optional<T> @c t to descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Write(int fd, const std::optional<T> & t)
    {
      static_assert(descio_detail::IsWritable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     hasValue = t.has_value();
        ssize_t  bytesWritten = Write(fd, hasValue);
        if (bytesWritten > 0) {
          rc += bytesWritten;
          if (hasValue) {
            bytesWritten = Write(fd, t.value());
            if (bytesWritten > 0) {
              rc += bytesWritten;
            }
            else {
              rc = -1;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Reads a std::optional<T> @c t from descriptor @c fd.  Returns the
    //!  number of bytes read on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NRead(int fd, std::optional<T> & t)
    {
      static_assert(descio_detail::IsReadable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     hasValue = false;
        ssize_t  bytesRead = NRead(fd, hasValue);
        if (bytesRead > 0) {
          rc += bytesRead;
          if (hasValue) {
            if (! t.has_value()) {
              t = T();
            }
            bytesRead = NRead(fd, t.value());
            if (bytesRead > 0) {
              rc += bytesRead;
            }
            else {
              rc = -1;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Writes a std::optional<T> @c t to descriptor @c fd.  Returns the
    //!  number of bytes written on success, -1 on failure.
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NWrite(int fd, const std::optional<T> & t)
    {
      static_assert(descio_detail::IsWritable<T>);
      if (0 <= fd) {
        ssize_t  rc = 0;
        bool     hasValue = t.has_value();
        ssize_t  bytesWritten = NWrite(fd, hasValue);
        if (bytesWritten > 0) {
          rc += bytesWritten;
          if (hasValue) {
            bytesWritten = NWrite(fd, t.value());
            if (bytesWritten > 0) {
              rc += bytesWritten;
            }
            else {
              rc = -1;
            }
          }
        }
        else {
          rc = -1;
        }
        return rc;
      }
      return -1;
    }

    //------------------------------------------------------------------------
    //!  Writes an atomic @c t to descriptor @c fd, in network byte order
    //!  (MSB first).  Returns the number of bytes written on success, -1 on
    //!  failure.  
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Write(int fd, const std::atomic<T> & t)
    {
      T  val = t.load();
      return Write(fd, val);
    }

    //------------------------------------------------------------------------
    //!  Reads an atomic @c t from descriptor @c fd, in network byte order
    //!  (MSB first).  Returns the number of bytes read on success, -1 on
    //!  failure.  
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t Read(int fd, std::atomic<T> & t)
    {
      ssize_t  rc = -1;
      T  val;
      ssize_t  bytesRead = Read(fd, val);
      if (sizeof(val) == bytesRead) {
        t.store(val);
        rc = bytesRead;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  Writes an atomic @c t to descriptor @c fd, in native byte order.
    //!  Returns the number of bytes written on success, -1 on failure.  
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NWrite(int fd, const std::atomic<T> & t)
    {
      T  val = t.load();
      return NWrite(fd, val);
    }

    //------------------------------------------------------------------------
    //!  Reads an atomic @c t from descriptor @c fd, in native byte order.
    //!  Returns the number of bytes read on success, -1 on failure.  
    //------------------------------------------------------------------------
    template <typename T>
    static ssize_t NRead(int fd, std::atomic<T> & t)
    {
      ssize_t  rc = -1;
      T  val;
      ssize_t  bytesRead = NRead(fd, val);
      if (sizeof(val) == bytesRead) {
        t.store(val);
        rc = bytesRead;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  This is evil, and will not work reliably beyond the local host
    //!  environment (i.e. across the network).  But there are cases where we
    //!  need to transport a trivial union locally, mostly in old C code.
    //!
    //!  Writes the given union @c u to descriptor @c fd.  Returns  the
    //!  number of bytes written on success, which should be @c sizeof(u).
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_union_v<T>
    static ssize_t Write(int fd, const T & u)
    { return Write(fd, (void *)&u, sizeof(u)); }

    //------------------------------------------------------------------------
    //!  This is evil, and will not work reliably beyond the local host
    //!  environment (i.e. across the network).  But there are cases where we
    //!  need to transport a trivial union locally, mostly in old C code.
    //!
    //!  Reads the given union @c u from descriptor @c fd.  Returns the
    //!  number of bytes read on success, which should be @c sizeof(u).
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_union_v<T>
    static ssize_t Read(int fd, T & u)
    { return Read(fd, (void *)&u, sizeof(u)); }

    //------------------------------------------------------------------------
    //!  This is evil, and will not work reliably beyond the local host
    //!  environment (i.e. across the network).  But there are cases where we
    //!  need to transport a trivial union locally, mostly in old C code.
    //!
    //!  Writes the given union @c u to the descriptor @c fd.  Returns the
    //!  number of bytes written on success, which should be @c sizeof(u).
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_union_v<T>
    static ssize_t NWrite(int fd, const T & u)
    { return Write(fd, (void *)&u, sizeof(u)); }

    //------------------------------------------------------------------------
    //!  This is evil, and will not work reliably beyond the local host
    //!  environment (i.e. across the network).  But there are cases where we
    //!  need to transport a trivial union locally, mostly in old C code.
    //!
    //!  Reads the given union @c u from the descriptor @c fd.  Returns the
    //!  number of bytes read on success, which should be @c sizeof(u).
    //------------------------------------------------------------------------
    template <typename T>
    requires std::is_union_v<T>
    static ssize_t NRead(int fd, T & u)
    { return Read(fd, (void *)&u, sizeof(u)); }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static ssize_t Read(int fd, void *buf, size_t buflen);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static ssize_t Write(int fd, const void *buf, size_t buflen);

#if defined(DWM_CAN_USE_REFLECTION)
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    consteval static bool IsIOVCapable()
    {
      std::size_t  numIovCapable = 0;
      std::size_t  numNotSkipped = 0;
      
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (! io_detail::HasSkipAnnotation<mem>) {
          ++numNotSkipped;
          using  memberType = typename[:std::meta::type_of(mem):];
          if constexpr (std::is_arithmetic_v<memberType>) {
            ++numIovCapable;
          }
          else if constexpr (std::is_bounded_array_v<memberType>) {
            using  elementType = std::remove_all_extents_t<memberType>;
            if constexpr (std::is_arithmetic_v<elementType>) {
              ++numIovCapable;
            }
          }
#if 0
          else if constexpr (std::is_class_v<memberType>) {
            if constexpr (IsIOVCapable<memberType>()) {
              ++numIovCapable;
            }
          }
#endif
        }
      }
      return ((0 < numIovCapable) && (numIovCapable == numNotSkipped));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    consteval static bool IsScalarOrArrayScalar(std::meta::info info)
    {
      return (std::meta::is_arithmetic_type(info)
              || (is_bounded_array_type(info)
                  && is_arithmetic_type(remove_all_extents(info))));
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    constexpr static auto TestAggregate(T t = T())
    {
      constexpr auto ctx = std::meta::access_context::unchecked();
      auto  mems = define_static_array(nonstatic_data_members_of(^^T, ctx)
                                       | std::views::transform(std::meta::type_of)
                                       | std::views::transform(std::meta::remove_cvref)
                                       | std::views::filter(IsScalarOrArrayScalar)
                                       | std::ranges::to<std::vector>());
      auto tupleRefl = substitute(^^std::tuple,mems);
      return tupleRefl;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename A, typename T>
    static void FillAggregate(A & a, T t)
    {
      constexpr auto  ctx = std::meta::access_context::unchecked();
      size_t  I = 0;
      template for (constexpr auto mem :
                      define_static_array(nonstatic_data_members_of(^^T, ctx)
                                          | std::views::transform(std::meta::type_of)
                                          | std::views::transform(std::meta::remove_cvref)
                                          | std::views::filter(IsScalarOrArrayScalar)
                                          | std::ranges::to<std::vector>())) {
        std::get<0>(a) = t.[:mem:];
        break;
        ++I;
      }
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (true == IsIOVCapable<T>())
    static void ToHostByteOrder(T & t)
    {
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                      define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (! io_detail::Skip<decltype(t.[:mem:]),mem>()) {
          if constexpr (IsEndianSensitiveInteger<decltype(t.[:mem:])>) {
            t.[:mem:] = BE2Host(t.[:mem:]);
          }
          else if constexpr (std::is_bounded_array_v<decltype(t.[:mem:])>) {
            using  elementType = std::remove_all_extents_t<decltype(t.[:mem:])>;
            if constexpr (IsEndianSensitiveInteger<elementType>) {
              elementType  *ep = (elementType *)t.[:mem:];
              elementType  *endp = ep + (sizeof(t.[:mem:]) / sizeof(elementType));
              for (; ep != endp; ++ep) {
                *ep = BE2Host(*ep);
              }
            }
          }
        }
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (true == IsIOVCapable<T>())
    static void ToNetByteOrder(T & t)
    {
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                      define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (! io_detail::Skip<decltype(t.[:mem:]),mem>()) {
          if constexpr (IsEndianSensitiveInteger<decltype(t.[:mem:])>) {
            t.[:mem:] = Host2BE(t.[:mem:]);
          }
          else if constexpr (std::is_bounded_array_v<decltype(t.[:mem:])>) {
            using  elementType = std::remove_all_extents_t<decltype(t.[:mem:])>;
            if constexpr (IsEndianSensitiveInteger<elementType>) {
              elementType  *ep = (elementType *)t.[:mem:];
              elementType  *endp = ep + (sizeof(t.[:mem:]) / sizeof(elementType));
              for (; ep != endp; ++ep) {
                *ep = Host2BE(*ep);
              }
            }
          }
        }
      }
      return;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (true == IsIOVCapable<T>())
    static ssize_t ScatterRead(int fd, T & t)
    {
      using Dwm::io_detail::Skip;

      ssize_t  rc = -1;
      if (0 <= fd) {
        constexpr ssize_t  iovecSize = IOVecSize<T>();
        struct iovec       iovs[iovecSize];
        ssize_t            bytesExpected = SetReadIOVec<T>(t, iovs);
        if (bytesExpected == ::readv(fd, iovs, iovecSize)) {
          rc = bytesExpected;
          ToHostByteOrder(t);
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (true == IsIOVCapable<T>())
    static ssize_t GatherWrite(int fd, const T & t)
    {
      using Dwm::io_detail::Skip;
      
      ssize_t  rc = -1;
      if (0 <= fd) {
        //  I don't like this copy; I shouldn't require a type to be
        //  copyable, since it might have skipped fields that aren't
        //  copyable.  Ideally I'd build an aggregate via reflection...
        T  tc = t;
        ToNetByteOrder(tc);
        constexpr ssize_t   iovecSize = IOVecSize<T>();
        struct iovec        iovs[iovecSize];
        ssize_t             bytesExpected = SetWriteIOVec<T>(tc, iovs);
        if (bytesExpected == ::writev(fd, iovs, iovecSize)) {
          rc = bytesExpected;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (true == IsIOVCapable<T>())
    static ssize_t ScatterNRead(int fd, T & t)
    {
      using Dwm::io_detail::Skip;

      ssize_t  rc = -1;
      if (0 <= fd) {
        constexpr ssize_t  iovecSize = IOVecSize<T>();
        struct iovec       iovs[iovecSize];
        ssize_t            bytesExpected = SetReadIOVec<T>(t, iovs);
        if (bytesExpected == ::readv(fd, iovs, iovecSize)) {
          rc = bytesExpected;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (true == IsIOVCapable<T>())
    static ssize_t GatherNWrite(int fd, const T & t)
    {
      using Dwm::io_detail::Skip;
      
      ssize_t  rc = -1;
      if (0 <= fd) {
        constexpr ssize_t   iovecSize = IOVecSize<T>();
        struct iovec        iovs[iovecSize];
        ssize_t             i = 0, bytesExpected = 0;
        constexpr auto ctx = std::meta::access_context::unchecked();
        template for (constexpr auto mem :
                        define_static_array(nonstatic_data_members_of(^^T, ctx))) {
          if constexpr (! io_detail::Skip<decltype(t.[:mem:]),mem>()) {
            iovs[i].iov_base = (void *)&(t.[:mem:]);
            iovs[i].iov_len = sizeof(decltype(t.[:mem:]));
            bytesExpected += iovs[i].iov_len;
            ++i;
          }
        }
        assert(0 < i);
        if (bytesExpected == ::writev(fd, iovs, iovecSize)) {
          rc = bytesExpected;
        }
      }
      return rc;
    }
#endif  // defined(DWM_CAN_USE_REFLECTION)

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorRead<T>)
    and HasStreamRead<T>
    static ssize_t Read(int fd, T & t)
    { return ReadViaIstream(fd, t); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorWrite<T>)
    and HasStreamWrite<T>
    static ssize_t Write(int fd, const T & t)
    { return WriteViaOstream(fd, t); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorNRead<T>)
    and HasStreamNRead<T>
    static ssize_t NRead(int fd, T & t)
    { return NReadViaIstream(fd, t); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorNWrite<T>)
    and HasStreamNWrite<T>
    static ssize_t NWrite(int fd, const T & t)
    { return NWriteViaOstream(fd, t); }

#if defined(DWM_CAN_USE_REFLECTION)

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
      and (not io_detail::DirectlySupported<T>)
      and (not io_detail::SupportedContainer<T>)
      and (not io_detail::DenyType<T>)
      and (not HasDescriptorWrite<T>)
      and (not HasStreamWrite<T>)
    static ssize_t Write(int fd, T const & v)
    {
      return WriteNonstaticMembers(fd, v);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
      and (not io_detail::DirectlySupported<T>)
      and (not io_detail::SupportedContainer<T>)
      and (not io_detail::DenyType<T>)
      and (not HasDescriptorRead<T>)
      and (not HasStreamRead<T>)
    static ssize_t Read(int fd, T & v)
    {
      return ReadNonstaticMembers(fd, v);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
    static ssize_t WriteNonstaticMembers(int fd, const T & v)
    {
      ssize_t  rc = 0;
      if (0 <= fd) {
        using Dwm::iostream_detail::IsWritable;
        using Dwm::io_detail::Skip;
        using io_detail::SkipReason, io_detail::DenyReason;
        constexpr auto ctx = std::meta::access_context::unchecked();
        template for (constexpr auto mem :
                        define_static_array(nonstatic_data_members_of(^^T, ctx))) {
          if constexpr (Skip<decltype(v.[:mem:]),mem>()) {
            FSyslog(LOG_DEBUG, "Write of {}.{} of type '{}' skipped{}",
                    TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                    std::meta::display_string_of(std::meta::type_of(mem)),
                    SkipReason<decltype(v.[:mem:]),mem>());
          }
          else {
            if constexpr (IsWritable<decltype(v.[:mem:])>) {
              FSyslog(LOG_DEBUG, "Writing {}.{} of type '{}'",
                      TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                      std::meta::display_string_of(std::meta::type_of(mem)));
              ssize_t  bytesWritten = Write(fd, v.[:mem:]);
              if (bytesWritten > 0) {
                rc += bytesWritten;
              }
              else {
                rc = -1;
                break;
              }
            }
            else {
              rc = -1;
              FSyslog(LOG_ERR, "{}.{} of type '{}' is unwritable{}",
                      TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                      std::meta::display_string_of(std::meta::type_of(mem)),
                      DenyReason<mem>());
              break;
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <class T>
    requires std::is_class_v<T>
    static ssize_t ReadNonstaticMembers(int fd, T & v)
    {
      ssize_t  rc = 0;
      if (0 <= fd) {
        using Dwm::io_detail::Skip;
        using io_detail::SkipReason, io_detail::DenyReason;
        constexpr auto ctx = std::meta::access_context::unchecked();
        template for (constexpr auto mem :
                        define_static_array(nonstatic_data_members_of(^^T, ctx))) {
          if constexpr (Skip<decltype(v.[:mem:]),mem>()) {
            FSyslog(LOG_DEBUG, "Read of {}.{} of type '{}' skipped{}",
                    TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                    std::meta::display_string_of(std::meta::type_of(mem)),
                    SkipReason<decltype(v.[:mem:]),mem>());
          }
          else {
            if constexpr (iostream_detail::IsReadable<decltype(v.[:mem:])>) {
              FSyslog(LOG_DEBUG, "Reading {}.{} of type '{}'",
                      TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                      std::meta::display_string_of(std::meta::type_of(mem)));
              ssize_t  bytesRead = Read(fd, v.[:mem:]);
              if (0 < bytesRead) {
                rc += bytesRead;
              }
              else {
                rc = -1;
                break;
              }
            }
            else {
              rc = -1;
              FSyslog(LOG_ERR, "{}.{} of type '{}' is unreadable{}",
                      TypeName<decltype(v)>(), std::meta::identifier_of(mem),
                      std::meta::display_string_of(std::meta::type_of(mem)),
                      DenyReason<mem>());
              break;
            }
          }
        }
      }
      return rc;
    }
    
#endif

  private:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorRead<T>)
    and HasStreamRead<T>
    static ssize_t ReadViaIstream(int fd, T & t)
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        constexpr size_t  bufSize = io_detail::ConstStreamedLength<T>();
        std::string  s;
        try {
          s.resize(bufSize);
          if (bufSize == ::read(fd, s.data(), bufSize)) {
            std::istringstream  iss(std::move(s));
            if (t.Read(iss)) {
              rc = bufSize;
            }
          }
        }
        catch (...) {
          FSyslog(LOG_ERR, "Exception in Rusage::Read(int fd={})", fd);
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorNRead<T>)
    and HasStreamNRead<T>
    static ssize_t NReadViaIstream(int fd, T & t)
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        constexpr size_t  bufSize = io_detail::ConstStreamedLength<T>();
        std::string  s;
        try {
          s.resize(bufSize);
          if (bufSize == ::read(fd, s.data(), bufSize)) {
            std::istringstream  iss(std::move(s));
            if (t.NRead(iss)) {
              rc = bufSize;
            }
          }
        }
        catch (...) {
          FSyslog(LOG_ERR, "Exception in Rusage::Read(int fd={})", fd);
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorWrite<T>)
    and HasStreamWrite<T>
    static ssize_t WriteViaOstream(int fd, const T & t)
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        std::ostringstream  os;
        if (StreamIO::Write(os, t)) {
          std::string_view  ossv(os.view());
          rc = write(fd, ossv.data(), ossv.size());
          if (rc != ossv.size()) {
            rc = -1;
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (io_detail::HasConstStreamedLength<T>())
    and (not HasDescriptorNWrite<T>)
    and HasStreamNWrite<T>
    static ssize_t NWriteViaOstream(int fd, const T & t)
    {
      ssize_t  rc = -1;
      if (0 <= fd) {
        std::ostringstream  os;
        if (StreamIO::NWrite(os, t)) {
          std::string_view  ossv(os.view());
          rc = write(fd, ossv.data(), ossv.size());
          if (rc != ossv.size()) {
            rc = -1;
          }
        }
      }
      return rc;
    }
    
#if defined(DWM_CAN_USE_REFLECTION)
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static size_t SetReadIOVec(T & t, struct iovec *iovp)
    {
      size_t  rc = 0;
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (! io_detail::HasSkipAnnotation<mem>) {
          using  memberType = typename[:std::meta::type_of(mem):];
          if constexpr (std::is_arithmetic_v<memberType>) {
            iovp->iov_base = (void *)&(t.[:mem:]);
            iovp->iov_len = sizeof(decltype(t.[:mem:]));
            rc += iovp->iov_len;
            ++iovp;
          }
          else if constexpr (std::is_bounded_array_v<memberType>) {
            using  elementType = std::remove_all_extents_t<memberType>;
            if constexpr (std::is_arithmetic_v<elementType>) {
              iovp->iov_base = (void *)&(t.[:mem:]);
              iovp->iov_len = sizeof(decltype(t.[:mem:]));
              rc += iovp->iov_len;
              ++iovp;
            }
          }
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static size_t SetWriteIOVec(const T & t, struct iovec *iovp)
    {
      size_t  rc = 0;
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (! io_detail::HasSkipAnnotation<mem>) {
          using  memberType = typename[:std::meta::type_of(mem):];
          if constexpr (std::is_arithmetic_v<memberType>) {
            iovp->iov_base = (void *)&(t.[:mem:]);
            iovp->iov_len = sizeof(decltype(t.[:mem:]));
            rc += iovp->iov_len;
            ++iovp;
          }
          else if constexpr (std::is_bounded_array_v<memberType>) {
            using  elementType = std::remove_all_extents_t<memberType>;
            if constexpr (std::is_arithmetic_v<elementType>) {
              iovp->iov_base = (void *)&(t.[:mem:]);
              iovp->iov_len = sizeof(decltype(t.[:mem:]));
              rc += iovp->iov_len;
              ++iovp;
            }
          }
        }
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    requires (IsIOVCapable<T>())
    consteval static std::size_t IOVecSize()
    {
      std::size_t  sz = 0;
      constexpr auto ctx = std::meta::access_context::unchecked();
      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (! io_detail::HasSkipAnnotation<mem>) {
          using  memberType = typename[:std::meta::type_of(mem):];
          if constexpr (std::is_arithmetic_v<memberType>) {
            ++sz;
          }
          else if constexpr (std::is_bounded_array_v<memberType>) {
            using  elementType = std::remove_all_extents_t<memberType>;
            if constexpr (std::is_arithmetic_v<elementType>) {
              ++sz;
            }
          }
#if 0
          else if constexpr (is_class_v<memberType>) {
            sz += IOVecSize<memberType>();
          }
#endif
        }
      }
      return sz;
    }
#endif
    
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
    //!  
    //------------------------------------------------------------------------
    template <typename _inputIteratorT>
    static ssize_t NWrite(int fd, _inputIteratorT f, _inputIteratorT l)
    {
      ssize_t  rc = 0;
      if (fd >= 0) {
        for ( ; f != l; ++f) {
          ssize_t  bytesWritten = NWrite(fd, *f);
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
        EncodedU64  numEntries;
        ssize_t     bytesRead = numEntries.Read(fd);
        if (bytesRead > 0) {
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
    //!  Reads a container from a file descriptor.  Returns the number of
    //!  bytes read on success, -1 on failure.
    //!  We use this for deques, lists, vectors, sets and multisets.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static ssize_t ContainerNRead(int fd, _containerT & c)
    {
      if (! c.empty())
        c.clear();
      ssize_t  rc = -1;
      if (fd >= 0) {
        EncodedU64  numEntries;
        ssize_t     bytesRead = numEntries.NRead(fd);
        if (bytesRead > 0) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::value_type  val;
            bytesRead = NRead(fd, val);
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
        EncodedU64  numEntries = c.size();
        uint64_t    bytesWritten = numEntries.Write(fd);
        if (bytesWritten == numEntries.StreamedLength()) {
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
    //!  Writes a container to a file descriptor.  Returns the number of
    //!  bytes written on success, -1 on failure.
    //!  We use this for all containers.  
    //------------------------------------------------------------------------
    template <typename _containerT>
    static ssize_t ContainerNWrite(int fd, const _containerT & c)
    {
      ssize_t  rc = -1;
      if (fd >= 0) {
        EncodedU64  numEntries = c.size();
        uint64_t    bytesWritten = numEntries.NWrite(fd);
        if (bytesWritten == numEntries.StreamedLength()) {
          rc = bytesWritten;
          if (numEntries) {
            bytesWritten = 
              NWrite<typename _containerT::const_iterator>(fd,
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
        EncodedU64  numEntries;
        ssize_t     bytesRead = numEntries.Read(fd);
        if (bytesRead > 0) {
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

    //------------------------------------------------------------------------
    //!  Reads a PairAssociative container from a file descriptor.  Returns
    //!  the number of bytes read on success, -1 on failure.
    //!  We use this for map, multimap and hash_map.
    //------------------------------------------------------------------------
    template <typename _containerT>
    static ssize_t PairAssocContNRead(int fd, _containerT & m)
    {
      ssize_t  rc = -1;
      if (! m.empty())
        m.clear();
      if (fd >= 0) {
        EncodedU64  numEntries;
        ssize_t     bytesRead = numEntries.NRead(fd);
        if (bytesRead > 0) {
          rc = bytesRead;
          for (uint64_t i = 0; i < numEntries; ++i) {
            typename _containerT::key_type  key;
            bytesRead = NRead(fd, key);
            if (bytesRead > 0) {
              rc += bytesRead;
              typename _containerT::mapped_type  val;
              bytesRead = NRead(fd, val);
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

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be written to
  //!  a file descriptor via a DescriptorIO::Write() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsDescriptorWritable = requires(const T & t, int fd) {
    { DescriptorIO::Write(fd, t) } -> std::same_as<ssize_t>;
  };

  //--------------------------------------------------------------------------
  //!  Simple concept expressing that an instance of type T can be read from
  //!  a file descriptor via a DescriptorIO::Read() member.
  //--------------------------------------------------------------------------
  template <typename T>
  concept IsDescriptorReadable = requires(T & t, int fd) {
    { DescriptorIO::Read(fd, t) } -> std::same_as<ssize_t>;
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
