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
    
    //------------------------------------------------------------------------
    //!  Types we'll automatically skip over when using reflection for
    //!  serialization and deserialization.
    //------------------------------------------------------------------------
    template <typename T>
    concept SkipType =
    std::same_as<T,std::mutex>
      or std::same_as<T,std::recursive_mutex>
      or std::same_as<T,std::condition_variable>
      or std::same_as<T,std::lock_guard<std::mutex>>
      or std::same_as<T,std::unique_lock<std::mutex>>;

    //------------------------------------------------------------------------
    //!  Types for which serialization and deserialization is directly
    //!  supported by the library.
    //------------------------------------------------------------------------
    template <typename T>
    concept DirectlySupported =
      std::same_as<T,char>
      or std::same_as<T,int8_t>
      or std::same_as<T,uint8_t>
      or std::same_as<T,int16_t>
      or std::same_as<T,uint16_t>
      or std::same_as<T,int32_t>
      or std::same_as<T,uint32_t>
      or std::same_as<T,int64_t>
      or std::same_as<T,uint64_t>
      or std::same_as<T,bool>
      or std::same_as<T,float>
      or std::same_as<T,double>
      or std::same_as<T,std::string>
      or std::is_enum_v<T>
      or std::same_as<T,std::vector<bool>>;
    
#if defined(DWM_CAN_USE_REFLECTION)
    //------------------------------------------------------------------------
    //!  True if the given reflection @c info has an annotation of the
    //!  form [[=Dwm::skip_io]].
    //------------------------------------------------------------------------
    template <std::meta::info info>
    concept HasSkipAnnotation = Concepts::has_annotation_type<info,skip_io_t>();

    //------------------------------------------------------------------------
    //!  True if the given reflection @c info has an annotation of the
    //!  form [[=Dwm::deny_io]].
    //------------------------------------------------------------------------
    template <std::meta::info info>
    concept HasDenyAnnotation = Concepts::has_annotation_type<info,deny_io_t>();
    
    //------------------------------------------------------------------------
    //!  Returns true if we should skip the given type @c DeclType (whose
    //!  reflection is @c info) during serialization and deserialization,
    //!  either because it's in the list of types we skip (see the SkipType
    //!  concept) or because it has an annotation of the form
    //!  [[=Dwm::skip_io]].  This also works for variables that have
    //!  annotation of the form [[=Dwm::skip_io]].
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
    //!  Returns true if we should deny the given type @c DeclType (whose
    //!  reflection is @c info) during serialization and deserialization,
    //!  either because it's in the list of types we deny (see the DenyType
    //!  concept) or because it has an annotation of the form
    //!  [[=Dwm::deny_io]].  This also works for variables that have
    //!  annotation of the form [[=Dwm::deny_io]].
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

    //------------------------------------------------------------------------
    //!  When we deny serialization or deserialization, we use this to
    //!  syslog the reason.
    //------------------------------------------------------------------------
    template <std::meta::info info>
    static constexpr std::string DenyReason()
    {
      if constexpr (std::meta::is_const(info)) {
        return " (immutable)";
      }
      if constexpr (Deny<info>()) {
        return " (denied)";
      }
      return "";
    }

    //------------------------------------------------------------------------
    //!  When we skip serialization or deserialization, wwe use this when
    //!  we syslog the reason.
    //------------------------------------------------------------------------
    template <typename DeclType, std::meta::info info>
    static constexpr std::string SkipReason()
    {
      if constexpr (SkipType<DeclType>) {
        return " (skipped type)";
      }
      else if constexpr (HasSkipAnnotation<info>) {
        return " (has skip_io annotation)";
      }
      else {
        return "";
      }
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


    //------------------------------------------------------------------------
    // Predeclare our function template that let us determine if a type
    // T is writable.  We need this so we can recurse through a data type
    // while also not having all of the recursion (diving into containers,
    // etc.) in one function.
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W> consteval bool Writable();

    //------------------------------------------------------------------------
    // Predeclare our function template that let us determine if a type
    // T is readable.  We need this so we can recurse through a data type
    // while also not having all of the recursion (diving into containers,
    // etc.) in one function.
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R> consteval bool Readable();

    //------------------------------------------------------------------------
    //!  Returns true if the given @c PairType (a @c std::pair) is
    //!  conceptually writable, meaning each member of the pair has direct
    //!  support for writing by the library or has a member function to write
    //!  it to the sink of interest.  For example, if we want to know if
    //!  @c PairType can conceptually be written to a @c std::ostream, @c W
    //!  would be @c Dwm::HasStreamWrite_t.
    //------------------------------------------------------------------------
    template <typename PairType, template <typename> typename W>
    requires Concepts::is_std_pair<PairType>
    consteval bool PairWritable()
    {
      return (Writable<typename PairType::first_type,W>()
              && Writable<typename PairType::second_type,W>());
    }

    //------------------------------------------------------------------------
    //!  Returns true if the given @c PairType (a @c std::pair) is
    //!  conceptually readable, meaning each member of the pair has direct
    //!  support for reading by the library or has a member function to read
    //!  it from the source of interest.  For example, if we want to know if
    //!  @c PairType can conceptually be read from a @c std::istream, @c W
    //!  would be @c Dwm::HasStreamRead_t.
    //------------------------------------------------------------------------
    template <typename PairType, template <typename> typename R>
    requires Concepts::is_std_pair<PairType>
    consteval bool PairReadable()
    {
      return (Readable<typename PairType::first_type,R>()
              && Readable<typename PairType::second_type,R>());
    }
    
#if defined(DWM_CAN_USE_REFLECTION)
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W, size_t ParamCount = 0>
    requires (Concepts::is_std_tuple<T>
              or Concepts::is_std_variant<T>
              or Concepts::is_std_pair<T>)
    consteval bool TemplateTypeParamsWritable()
    {
      constexpr const auto tmpl_args =
        define_static_array(template_arguments_of(^^T));
      size_t  numParams = 0, numTypes = 0, numWritable = 0;
      template for (constexpr auto tmpl_arg : tmpl_args) {
        ++numParams;
        if (ParamCount && (numParams > ParamCount)) {
          break;
        }
        if (std::meta::is_type(tmpl_arg)) {
          ++numTypes;
          if constexpr (! Writable<typename[:tmpl_arg:],W>()) {
            break;
          }
          ++numWritable;
        }
      }
      return (numTypes == numWritable);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R, size_t ParamCount = 0>
    requires (Concepts::is_std_tuple<T>
              or Concepts::is_std_variant<T>
              or Concepts::is_std_pair<T>)
    consteval bool TemplateTypeParamsReadable()
    {
      constexpr const auto tmpl_args =
        define_static_array(template_arguments_of(^^T));
      size_t  numParams = 0, numTypes = 0, numWritable = 0;
      template for (constexpr auto tmpl_arg : tmpl_args) {
        ++numParams;
        if (ParamCount && (numParams > ParamCount)) {
          break;
        }
        if (std::meta::is_type(tmpl_arg)) {
          ++numTypes;
          if constexpr (! Readable<typename[:tmpl_arg:],R>()) {
            break;
          }
          ++numWritable;
        }
      }
      return (numTypes == numWritable);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W>
    requires Concepts::is_std_tuple<T>
    consteval bool TupleWritable() { return TemplateTypeParamsWritable<T,W>(); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R>
    requires Concepts::is_std_tuple<T>
    consteval bool TupleReadable() { return TemplateTypeParamsReadable<T,R>(); }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W>
    requires Concepts::is_std_variant<T>
    consteval bool VariantWritable() { return TemplateTypeParamsWritable<T,W>(); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R>
    requires Concepts::is_std_variant<T>
    consteval bool VariantReadable() { return TemplateTypeParamsReadable<T,R>(); }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W>
    requires std::is_class_v<T>
    consteval bool ReflectionWritable()
    {
      constexpr auto ctx = std::meta::access_context::unchecked();
      constexpr auto members =
        define_static_array(nonstatic_data_members_of(^^T, ctx));
      if constexpr (! members.size()) {
        return false;
      }
      template for (constexpr auto mem : members) {
        if constexpr ((! Writable<typename[:std::meta::type_of(mem):],W>())
                      || io_detail::Deny<mem>()) {
          return false;
        }
      }
      return true;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R>
    requires std::is_class_v<T>
    consteval bool ReflectionReadable()
    {
      constexpr auto ctx = std::meta::access_context::unchecked();
      constexpr auto members =
        define_static_array(nonstatic_data_members_of(^^T, ctx));
      if constexpr (! members.size()) {
        return false;
      }
      template for (constexpr auto mem : members) {
        if constexpr ((! Readable<typename[:std::meta::type_of(mem):],R>())
                      || io_detail::Deny<mem>()) {
          return false;
        }
      }
      return true;
    }
    
#else

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W, size_t elt = 0>
    consteval bool TupleWritable()
    {
      if constexpr (elt < std::tuple_size_v<T>) {
        if constexpr (Writable<std::tuple_element_t<elt,T>,W>()) {
          return TupleWritable<T,W,elt+1>();
        }
        else {
          return false;
        }
      }
      return true;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W, size_t elt = 0>
    consteval bool TupleReadable()
    {
      if constexpr (elt < std::tuple_size_v<T>) {
        if constexpr (Readable<std::tuple_element_t<elt,T>,W>()) {
          return TupleReadable<T,W,elt+1>();
        }
        else {
          return false;
        }
      }
      return true;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W, size_t I = 0>
    requires Concepts::is_std_variant<T>
    consteval bool VariantWritable()
    {
      if constexpr (I < std::variant_size_v<T>) {
        if constexpr (Writable<std::variant_alternative_t<I,T>,W>()) {
          return VariantWritable<T,W,I+1>();
        }
        else {
          return false;
        }
      }
      return true;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R, size_t I = 0>
    requires Concepts::is_std_variant<T>
    consteval bool VariantReadable()
    {
      if constexpr (I < std::variant_size_v<T>) {
        if constexpr (Readable<std::variant_alternative_t<I,T>,R>()) {
          return VariantReadable<T,R,I+1>();
        }
        else {
          return false;
        }
      }
      return true;
    }
    
#endif

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W>
    requires std::same_as<std::remove_const_t<decltype(W<T>::value)>,bool>
    consteval bool ConceptuallyWritable()
    {
#if defined(DWM_CAN_USE_REFLECTION)
      if constexpr (io_detail::HasDenyAnnotation<^^T>) { return false; }
#endif
      if constexpr (io_detail::DirectlySupported<T>)   { return true; }
      else if constexpr (W<T>::value)                  { return true; }
      else if constexpr (io_detail::SkipType<T>)       { return true;  }
      else if constexpr (io_detail::DenyType<T>)       { return false; }
      else if constexpr (Concepts::is_std_optional<T>) {
        return Writable<typename T::value_type,W>();
      }
      else if constexpr (Concepts::is_std_unique_ptr<T>) {
        return Writable<typename T::element_type,W>();
      }
      else if constexpr (std::is_bounded_array_v<T>) {
        return Writable<std::remove_all_extents_t<T>,W>();
      }
      else if constexpr (Concepts::is_std_sequence_container<T>) {
        return Writable<typename T::value_type,W>();
      }
      else if constexpr (Concepts::is_std_pair<T>) {
        return PairWritable<T,W>();
      }
      else if constexpr (Concepts::is_std_tuple<T>) {
        return TupleWritable<T,W>();
      }
      else if constexpr (Concepts::is_std_variant<T>) {
        return VariantWritable<T,W>();
      }
      else if constexpr (Concepts::is_std_associative_container<T>) {
        return Writable<typename T::value_type,W>();
      }
      else if constexpr (Concepts::is_std_pair_associative_container<T>) {
        return PairWritable<std::pair<typename T::key_type,
                                      typename T::mapped_type>,W>();
      }
#if defined(DWM_CAN_USE_REFLECTION)
      else if constexpr (std::is_class_v<T>) {
        return ReflectionWritable<T,W>();
      }
#endif
      return false;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R>
    requires std::same_as<std::remove_const_t<decltype(R<T>::value)>,bool>
    consteval bool ConceptuallyReadable()
    {
#if defined(DWM_CAN_USE_REFLECTION)
      if constexpr (io_detail::HasDenyAnnotation<^^T>) { return false; }
#endif
      if constexpr (std::is_const_v<T>)                { return false; }
      if constexpr (io_detail::DirectlySupported<T>)   { return true;  }
      else if constexpr (R<T>::value)                  { return true;  }
      else if constexpr (io_detail::SkipType<T>)       { return true;  }
      else if constexpr (io_detail::DenyType<T>)       { return false; }
      else if constexpr (Concepts::is_std_optional<T>) {
        return Readable<typename T::value_type,R>();
      }
      else if constexpr (Concepts::is_std_unique_ptr<T>) {
        return Readable<typename T::element_type,R>();
      }
      else if constexpr (std::is_bounded_array_v<T>) {
        return Readable<std::remove_all_extents_t<T>,R>();
      }
      else if constexpr (Concepts::is_std_sequence_container<T>) {
        return Readable<typename T::value_type,R>();
      }
      else if constexpr (Concepts::is_std_pair<T>) {
        return PairReadable<T,R>();
      }
      else if constexpr (Concepts::is_std_tuple<T>) {
        return TupleReadable<T,R>();
      }
      else if constexpr (Concepts::is_std_variant<T>) {
        return VariantReadable<T,R>();
      }
      else if constexpr (Concepts::is_std_associative_container<T>) {
        return Readable<typename T::value_type,R>();
      }
      else if constexpr (Concepts::is_std_pair_associative_container<T>) {
        return PairReadable<std::pair<typename T::key_type,
                                      typename T::mapped_type>,R>();
      }
#if defined(DWM_CAN_USE_REFLECTION)
      else if constexpr (std::is_class_v<T>) {
        return ReflectionReadable<T,R>();
      }
#endif
      return false;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename W>
    consteval bool Writable()
    {
      return ConceptuallyWritable<T,W>();
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T, template <typename> typename R>
    consteval bool Readable()
    {
      return ConceptuallyReadable<T,R>();
    }

    
  }  // namespace io_detail

}  // namespace Dwm

#endif  // _DWMIOCONCEPTS_HH_
