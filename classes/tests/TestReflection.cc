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
//!  \file TestReflection.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include <iostream>
#include <ranges>
#include <string>
#include <tuple>

#include "DwmConcepts.hh"

//----------------------------------------------------------------------------
template <class T, template <typename> class Trait>
concept has_trait = Trait<T>::value;

#if defined(DWM_CAN_USE_REFLECTION)
//----------------------------------------------------------------------------
template <typename T, template <typename> typename Trait>
requires (template_of(^^T) == ^^std::tuple)
consteval bool tuple_param_has_trait()
{
  constexpr auto tmpl_args = define_static_array(template_arguments_of(^^T));
  template for (constexpr auto tmpl_arg : tmpl_args) {
    if (std::meta::is_type(tmpl_arg)) {
      //  template parameter is a type
      if constexpr (has_trait<typename[:tmpl_arg:],Trait>) {
        //  template parameter matches Trait
        return true;
      }
    }
  }
  return false;
}

template <template <typename> typename Trait, class T>
consteval bool tuple_param_has_trait(T && t)
{
  return tuple_param_has_trait<T,Trait>();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if 1
consteval auto type_struct_to_tuple(std::meta::info type) -> std::meta::info {
  constexpr auto ctx = std::meta::access_context::current();
  return substitute(^^std::tuple,
                    nonstatic_data_members_of(type, ctx)
                    | std::views::transform(std::meta::type_of)
                    | std::views::transform(std::meta::remove_cvref)
                    | std::ranges::to<std::vector>());
}

template <typename To, typename From, std::meta::info ... members>
constexpr auto struct_to_tuple_helper(From const& from) -> To {
  return To(from.[:members:]...);
}

template<typename From>
consteval auto get_struct_to_tuple_helper() {
  using To = [:type_struct_to_tuple(^^From):];
  auto ctx = std::meta::access_context::current();

  std::vector args = {^^To, ^^From};
  for (auto mem : nonstatic_data_members_of(^^From, ctx)) {
    args.push_back(reflect_constant(mem));
    // args.push_back(reflect_object(mem));
  }

  /*
  Alternatively, with Ranges:
  args.append_range(
    nonstatic_data_members_of(^^From, ctx)
    | std::views::transform(std::meta::reflect_constant)
    );
  */

  return extract<To(*)(From const&)>(
    substitute(^^struct_to_tuple_helper, args));
}

template <typename From>
constexpr auto struct_to_tuple(From const& from) {
  return get_struct_to_tuple_helper<From>()(from);
}

#else

template <typename T>
constexpr auto struct_to_tuple(T const& t) {
  constexpr auto ctx = std::meta::access_context::current();

  constexpr std::size_t N = nonstatic_data_members_of(^^T, ctx).size();
  auto members = nonstatic_data_members_of(^^T, ctx);

  constexpr auto indices = []{
    std::array<int, N> indices;
    std::ranges::views::iota(indices, 0);
    return indices;
  }();

  constexpr auto [...Is] = indices;
  return std::make_tuple(t.[: members[Is] :]...);
}

#endif

#endif  // defined(DWM_CAN_USE_REFLECTION)

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#if defined(DWM_CAN_USE_REFLECTION)  
  static_assert(tuple_param_has_trait<std::tuple<int,const std::string>,
                                      std::is_const>());
  static_assert(! tuple_param_has_trait<std::tuple<int,std::string>,
                                        std::is_const>());

  struct S {
    int          i;
    std::string  s;
    uint32_t     u32[2][2];
  };

  S  s1 = { 42, "hello", { {42, 99}, {0xDEADBEEF, 0xA1C0FFEE } } };
  S  s2 = s1;

#if 0
  auto  t = struct_to_tuple(s1);
  std::cout << "std::get<0>(t): " << std::get<0>(t) << "\nstd::get<1>(t): "
            << std::get<1>(t) << '\n'
            << std::get<2>(t)[0][0] << ' ' << std::get<2>(t)[0][1] << ' '
            << std::hex
            << std::get<2>(t)[1][0] << ' ' << std::get<2>(t)[1][1] << '\n';
#endif

#endif  // defined(DWM_CAN_USE_REFLECTION)
}
