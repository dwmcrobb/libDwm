//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2025
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
//!  \file DwmIOConcepts.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMIOCONCEPTS_HH_
#define _DWMIOCONCEPTS_HH_

#include "DwmConcepts.hh"

namespace Dwm {

  namespace io_detail {
    
    //------------------------------------------------------------------------
    //!  Used for the cases where we try to use reflection for serialization
    //!  and deserialization...
    //!
    //!  We explicitly deny serialization / deserialization of certain types
    //!  as well as pointers (since there's no way to know whether they point
    //!  to a single object or an array of objects of indeterminate length).
    //!  We deny serialization of const types simply because there isn't a
    //!  clean way to deserialize them since they're declared immutable.
    //------------------------------------------------------------------------
    template <typename T>
    concept DenyType =
    std::is_pointer_v<T>
      or std::is_const_v<T>;
    
    template <typename T>
    concept SkipType =
    std::same_as<T,std::mutex>
      or std::same_as<T,std::recursive_mutex>
      or std::same_as<T,std::condition_variable>
      or std::same_as<T,std::lock_guard<std::mutex>>
      or std::same_as<T,std::unique_lock<std::mutex>>;
    
#if defined(DWM_CAN_USE_REFLECTION)
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <std::meta::info info>
    concept HasSkipAnnotation = Concepts::has_annotation_type<info,skip_io_t>();

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <std::meta::info info>
    concept HasDenyAnnotation = Concepts::has_annotation_type<info,deny_io_t>();
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename DeclType, std::meta::info info>
    consteval bool Skip()
    {
      if constexpr (SkipType<DeclType>) {
        return true;
      }
      if constexpr (std::meta::is_type(info)) {
        if constexpr (SkipType<typename[:std::meta::type_of(info):]>) {
          return true;
        }
      }
      else if constexpr (HasSkipAnnotation<info>) {
        return true;
      }
      return false;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <std::meta::info info>
    consteval bool Deny()
    {
      if constexpr (std::meta::is_type(info)) {
        if constexpr (DenyType<typename[:std::meta::type_of(info):]>) {
          return true;
        }
      }
      if constexpr (Concepts::has_annotation_type<info,deny_io_t>()) {
        return true;
      }
      return false;
    }
    
#endif  // if defined(DWM_CAN_USE_REFLECTION)

    //------------------------------------------------------------------------
    //!  Concept to match STL containers.
    //------------------------------------------------------------------------
    template <class T>
    concept SupportedContainer =
      Concepts::is_std_associative_container<T>
      or Concepts::is_std_pair_associative_container<T>
      or Concepts::is_std_sequence_container<T>;
    
  }  // namespace io_detail

}  // namespace Dwm

#endif  // _DWMIOCONCEPTS_HH_
