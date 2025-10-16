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
//!  \file DwmConcepts.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMCONCEPTS_HH_
#define _DWMCONCEPTS_HH_


#include <array>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#if defined(__cpp_impl_reflection)
#  if defined(__cpp_expansion_statements)
#    if __has_include(<meta>)
#      include <meta>
#      if defined(__cpp_lib_define_static)
#        define DWM_CAN_USE_REFLECTION 1
#      endif
#    endif
#  endif
#endif

// #undef DWM_CAN_USE_REFLECTION

namespace Dwm {

  namespace Concepts {

#if defined(DWM_CAN_USE_REFLECTION)
    //  With reflection, it's easy to define a concept that can test if
    //  a given type matches a given template, since we have template_of().
    //  The macro here is just to reduce the amount of boilerplate code
    //  to maintain in order to have a list of concepts to match specific
    //  templates from the std namespace.
    //  dwm TODO: I probably don't need the has_template_arguments() test.
    #define MAKE_IS_STD_CONT_CONCEPT(x)                                    \
      template <class T> concept is_std_##x =                              \
        has_template_arguments(^^T) && template_of(^^T) == ^^std::x

    //  With reflection, std::array can be matched in the same manner as
    //  other containers.
    MAKE_IS_STD_CONT_CONCEPT(array);
#else
    //  Without reflection, things are a little bit more awkward.  There
    //  is probably a cleaner way to do what's here, I just haven't dug
    //  deeply yet.  The main issue is that std::array's second template
    //  parameter is a non-type parameter, while all the other containers'
    //  template parameters are type parameters.  So I need a separate
    //  set of traits for std::array.
    template <class T>
    inline constexpr bool is_std_array = false;
    
    template <class T, size_t _Size>
    inline constexpr bool is_std_array<std::array<T, _Size>> = true;

    template <class T, template <class...> class _Template>
    inline constexpr bool is_std_container = false;
    
    template <template <class...> class _Tmpl, class... _Args>
    inline constexpr bool is_std_container<_Tmpl<_Args...>, _Tmpl> = true;
    
    #define MAKE_IS_STD_CONT_CONCEPT(x) \
    template <class T> concept is_std_##x = is_std_container<T,std::x>
#endif

    MAKE_IS_STD_CONT_CONCEPT(atomic);
    MAKE_IS_STD_CONT_CONCEPT(pair);
    MAKE_IS_STD_CONT_CONCEPT(list);
    MAKE_IS_STD_CONT_CONCEPT(deque);
    MAKE_IS_STD_CONT_CONCEPT(map);
    MAKE_IS_STD_CONT_CONCEPT(multimap);
    MAKE_IS_STD_CONT_CONCEPT(set);
    MAKE_IS_STD_CONT_CONCEPT(multiset);
    MAKE_IS_STD_CONT_CONCEPT(optional);
    MAKE_IS_STD_CONT_CONCEPT(shared_ptr);
    MAKE_IS_STD_CONT_CONCEPT(unique_ptr);
    MAKE_IS_STD_CONT_CONCEPT(tuple);
    MAKE_IS_STD_CONT_CONCEPT(unordered_map);
    MAKE_IS_STD_CONT_CONCEPT(unordered_multimap);
    MAKE_IS_STD_CONT_CONCEPT(unordered_set);
    MAKE_IS_STD_CONT_CONCEPT(unordered_multiset);
    MAKE_IS_STD_CONT_CONCEPT(variant);
    MAKE_IS_STD_CONT_CONCEPT(vector);

    template <class T> concept is_std_associative_container =
      is_std_set<T>
      or is_std_multiset<T>
      or is_std_unordered_set<T>
      or is_std_unordered_multiset<T>;
    
    template <typename T> concept is_std_pair_associative_container =
      is_std_map<T>
      or is_std_multimap<T>
      or is_std_unordered_map<T>
      or is_std_unordered_multimap<T>;
    
    template <typename T> concept is_std_sequence_container =
      is_std_array<T>
      or is_std_deque<T>
      or is_std_list<T>
      or (is_std_vector<T>
          and (not std::is_same_v<typename T::value_type,bool>));

    template <typename T> concept is_dynamic_size_container =
      is_std_deque<T>
      or is_std_list<T>
      or is_std_vector<T>
      or is_std_pair_associative_container<T>
      or is_std_associative_container<T>;
    
#if defined(DWM_CAN_USE_REFLECTION)
    //------------------------------------------------------------------------
    //!  Returns true if the the given reflection @c info has an annotation
    //!  of type @c A.
    //------------------------------------------------------------------------
    template <std::meta::info info, typename A>
    consteval bool has_annotation_type()
    {
      return (! annotations_of(info,^^A).empty());
    }
#endif
    
  }  // namespace Concepts

#if defined(DWM_CAN_USE_REFLECTION)
  //--------------------------------------------------------------------------
  //!  Used by I/O classes to skip class/struct members when serializing
  //!  or deserializing via C++ reflection (P2996).  'Dwm::skip_io' can be
  //!  used as an annotation (P3394) during declaration to explicitly skip a
  //!  class/struct member.  For example:
  //!
  //!   struct Foo {
  //!     [[=Dwm::skip_io]] int  i;
  //!     std::string            s;
  //!   };
  //!
  //!   Foo  f { 42, "hello" };
  //!   Dwm::StreamIO::Write(std::cout, f); // only writes the 's' member of f
  //!   Dwm::StreamIO::Read(std::cin, f);   // only reads the 's' member of f
  //!
  //--------------------------------------------------------------------------
  typedef struct {} skip_io_t;
  inline constexpr auto  skip_io = skip_io_t();
  
  //--------------------------------------------------------------------------
  //!  Can be used to deny serialization and deserialization via reflection
  //!  (P2996).  'Dwm::deny_io' as an annotation (P3394) will cause library
  //!  Write() and Read() members to fail when attempting to use reflection
  //!  for serialization/deserialization.
  //--------------------------------------------------------------------------
  typedef struct {} deny_io_t;
  inline constexpr auto  deny_io = deny_io_t();

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  consteval bool AllMembersArithmetic()
  {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
      if constexpr (! std::is_arithmetic_v<typename[:std::meta::type_of(mem):]>) {
        return false;
      }
    }
    return true;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T>
  consteval bool AllMembersArithmeticPacked()
  {
    std::size_t  sz = 0;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, ctx))) {
      if constexpr (! std::is_arithmetic_v<typename[:std::meta::type_of(mem):]>) {
        return false;
      }
      sz += sizeof(typename[:std::meta::type_of(mem):]);
      
    }
    return (sizeof(T) == sz);
  }
  
#endif
  
}  // namespace Dwm

#endif  // _DWMCONCEPTS_HH_
