//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2012, 2023
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
//!  \file DwmFFIVar.hh
//!  \author Daniel W. McRobb
//!  \brief Class templates for simple Freedom From Interference (FFI)
//!  Note that this implementation uses C++20 constraints and concepts.
//---------------------------------------------------------------------------

#ifndef _DWMFFIVAR_HH_
#define _DWMFFIVAR_HH_

#include <cassert>
#include <type_traits>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  The main idea here is to encpasulate a scalar type to provide
  //!  protection from interference.  We do this by keeping a copy of the
  //!  scalar, possibly bitwise inverted, and checking that the scalar
  //!  and its copy match.  This can be done manually by calling IsFFI(),
  //!  but various operators call it when a read occurs.  At the moment,
  //!  these are wrapped in assert(), but later I'll provide a means of
  //!  registering a callback or using a macro that can be redefined.
  //!  On a fully hosted system we might still want abort() to get a core
  //!  dump when desired, but in an embedded system we'll want something
  //!  else.
  //!
  //!  It's worth noting that throwing an exception could be an elegant
  //!  solution on a hosted system.  But in embedded environments,
  //!  and some hosted environments, exceptions might be intentionally
  //!  disabled.  If I use a macro to define what happens, theoretically
  //!  one could throw, or call a callback, etc.  I haven't done this
  //!  work yet, saince the few times I've used this code, assert() has
  //!  been what I wanted and the IsFFI() member of the class templates
  //!  covered the cases where I needed a check in production code.
  //!
  //!  I have 3 class templates, each specialized for specific types:
  //!  integral types, floating point types and pointer types.  I have
  //!  some trivial concepts that are used for the binary operators
  //!  defined outside of class templates.
  //--------------------------------------------------------------------------
  
  //==========================================================================

  //--------------------------------------------------------------------------
  //!  Top level template class, intentionally empty.
  //--------------------------------------------------------------------------
  template<class T, class Enable = void>
  class FFIVar {};

  //--------------------------------------------------------------------------
  //!  Specialization for integral types.  Encapsulate operators and
  //!  conversion to/from integral type T.  Our copy of the integral value
  //!  is the bitwise complement of the value.
  //--------------------------------------------------------------------------
  template <typename T>
  class FFIVar<T,typename std::enable_if<std::is_integral<T>::value>::type>
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    FFIVar()
        :  _value(0), _invertedValue((T)(~(T)0))
    { }

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~FFIVar()
    {
      _value = 0;
      _invertedValue = 0;
    }

    //------------------------------------------------------------------------
    //!  Copy constructor
    //------------------------------------------------------------------------
    FFIVar(const FFIVar & v) = default;

    //------------------------------------------------------------------------
    //!  Construct from a reference to a @c T
    //------------------------------------------------------------------------
    FFIVar(const T & t)
        : _value(t), _invertedValue(~_value)
    {}
  
    //------------------------------------------------------------------------
    //!  Move constructor
    //------------------------------------------------------------------------
    FFIVar(FFIVar && v) = default;

    //------------------------------------------------------------------------
    //!  Construct from an rvalue reference to a @c T
    //------------------------------------------------------------------------
    FFIVar(T && t)
        : _value(t), _invertedValue(~_value)
    {}
  
    //------------------------------------------------------------------------
    //!  Copy assignment
    //------------------------------------------------------------------------
    FFIVar & operator = (const FFIVar & v) = default;

    //------------------------------------------------------------------------
    //!  Assign from a @c T
    //------------------------------------------------------------------------
    FFIVar & operator = (const T & t)
    {
      _value = t;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Move assignment
    //------------------------------------------------------------------------
    FFIVar & operator = (FFIVar &&) = default;

    //------------------------------------------------------------------------
    //!  Assign from an rvalue reference to a @c T
    //------------------------------------------------------------------------
    FFIVar & operator = (T && t)
    {
      _value = t;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Conversion to @c T operator
    //------------------------------------------------------------------------
    operator T() const
    {
      assert(IsFFI());
      return _value;
    }

    //------------------------------------------------------------------------
    //!  Less-than-or-equal operator
    //------------------------------------------------------------------------
    bool operator <= (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (_value <= v._value);
    }

    //------------------------------------------------------------------------
    //!  Greater-than-or-equal operator
    //------------------------------------------------------------------------
    bool operator >= (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (_value >= v._value);
    }

    //------------------------------------------------------------------------
    //!  Less-than operator
    //------------------------------------------------------------------------
    bool operator < (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (! (_value >= v._value));
    }

    //------------------------------------------------------------------------
    //!  Greater-than operator
    //------------------------------------------------------------------------
    bool operator > (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (! (_value <= v._value));
    }
    
    //------------------------------------------------------------------------
    //!  Equality to an integral operator
    //------------------------------------------------------------------------
    template <typename IT>
    requires std::is_integral<IT>::value
    bool operator == (const IT & it) const
    {
      assert(IsFFI());
      return (it == _value);
    }
  
    //------------------------------------------------------------------------
    //!  Equality operator
    //------------------------------------------------------------------------
    bool operator == (const FFIVar & v) const
    {
      assert(v.IsFFI() && IsFFI());
      return(_value == v._value);
    }

    //------------------------------------------------------------------------
    //!  post-increment operator
    //------------------------------------------------------------------------
    FFIVar operator ++ (int)
    {
      assert(IsFFI());
      T  rc = *this;
      ++_value;
      _invertedValue = ~_value;
      return rc;
    }

    //------------------------------------------------------------------------
    //!  pre-increment operator
    //------------------------------------------------------------------------
    FFIVar & operator ++ ()
    {
      assert(IsFFI());
      ++_value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  post-decrement operator
    //------------------------------------------------------------------------
    FFIVar operator -- (int)
    {
      assert(IsFFI());
      T  rc = *this;
      --_value;
      _invertedValue = ~_value;
      return rc;
    }

    //------------------------------------------------------------------------
    //!  pre-decrement operator
    //------------------------------------------------------------------------
    FFIVar & operator -- ()
    {
      assert(IsFFI());
      --_value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound add and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator += (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value += v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound subtract and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator -= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value -= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound multiply and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator *= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value *= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound divide and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator /= (const FFIVar & v)
    {
      assert(v.IsFFI());
      assert(v._value);
      _value /= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound modulo and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator %= (const FFIVar & v)
    {
      assert(v.IsFFI());
      assert(v._value);
      _value %= v._value;
      _invertedValue = ~_value;
      return *this;
    }
    
    //------------------------------------------------------------------------
    //!  Compound right shift and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator >>= (const FFIVar & v)
    {
      assert(v.IsFFI());
      assert(v._value <= (sizeof(T) * 8));
      _value >>= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound left shift and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator <<= (const FFIVar & v)
    {
      assert(v.IsFFI());
      assert(v._value <= (sizeof(T) * 8));
      _value <<= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound bitwise 'and' and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator &= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value &= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound bitwise 'or' and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator |= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value |= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Compound exclusive 'or' and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator ^= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value ^= v._value;
      _invertedValue = ~_value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Complement operator
    //------------------------------------------------------------------------
    FFIVar operator ~ () const
    {
      return(FFIVar(~_value));
    }
    
    //------------------------------------------------------------------------
    //!  Returns true if object appears to be free from interference.
    //------------------------------------------------------------------------
    bool IsFFI() const
    {
      return (_value == (T)~(_invertedValue));
    }

  private:
    T  _value;
    T  _invertedValue;
  };

  //==========================================================================

  //--------------------------------------------------------------------------
  //!  Specialization for floating point types.  No bitwise operators.
  //--------------------------------------------------------------------------
  template <typename T>
  class FFIVar<T,typename std::enable_if<std::is_floating_point<T>::value>::type>
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    FFIVar()
        :  _value(0), _copiedValue(0)
    { }

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~FFIVar()
    {
      _value = 0;
      _copiedValue = 1;
    }

    //------------------------------------------------------------------------
    //!  Copy constructor
    //------------------------------------------------------------------------
    FFIVar(const FFIVar & v) = default;

    //------------------------------------------------------------------------
    //!  Construct from a reference to a @c T
    //------------------------------------------------------------------------
    FFIVar(const T & t)
        : _value(t), _copiedValue(t)
    {}
  
    //------------------------------------------------------------------------
    //!  Move constructor
    //------------------------------------------------------------------------
    FFIVar(FFIVar && v) = default;

    //------------------------------------------------------------------------
    //!  Construct from an rvalue reference to a @c T
    //------------------------------------------------------------------------
    FFIVar(T && t)
        : _value(t), _copiedValue(_value)
    {}
  
    //------------------------------------------------------------------------
    //!  Copy assignment
    //------------------------------------------------------------------------
    FFIVar & operator = (const FFIVar & v) = default;

    //------------------------------------------------------------------------
    //!  Assign from a @c T
    //------------------------------------------------------------------------
    FFIVar & operator = (const T & t)
    {
      _value = t;
      _copiedValue = _value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Move assignment
    //------------------------------------------------------------------------
    FFIVar & operator = (FFIVar &&) = default;

    //------------------------------------------------------------------------
    //!  Move assign from an rvalue reference to a @c T
    //------------------------------------------------------------------------
    FFIVar & operator = (T && t)
    {
      _value = t;
      _copiedValue = _value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Conversion to T operator
    //------------------------------------------------------------------------
    operator T() const
    {
      assert(IsFFI());
      return _value;
    }

    //------------------------------------------------------------------------
    //!  Less than or equal operator
    //------------------------------------------------------------------------
    bool operator <= (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (_value <= v._value);
    }

    //------------------------------------------------------------------------
    //!  Greater than or equal operator
    //------------------------------------------------------------------------
    bool operator >= (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (_value >= v._value);
    }

    //------------------------------------------------------------------------
    //!  Less-than operator
    //------------------------------------------------------------------------
    bool operator < (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (! (_value >= v._value));
    }

    //------------------------------------------------------------------------
    //!  Greater-than operator
    //------------------------------------------------------------------------
    bool operator > (const FFIVar & v) const
    {
      assert(IsFFI() & v.IsFFI());
      return (! (_value <= v._value));
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename IT>
    bool operator == (const IT & it) const
    {
      assert(IsFFI());
      return (it == _value);
    }
  
    //------------------------------------------------------------------------
    //!  Equality operator
    //------------------------------------------------------------------------
    bool operator == (const FFIVar & v) const
    {
      assert(v.IsFFI() && IsFFI());
      return(_value == v._value);
    }

    //------------------------------------------------------------------------
    //!  compound add and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator += (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value += v._value;
      _copiedValue = _value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  compound subtract and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator -= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value -= v._value;
      _copiedValue = _value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  compound multiply and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator *= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value *= v._value;
      _copiedValue = _value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  compound divide and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator /= (const FFIVar & v)
    {
      assert(v.IsFFI());
      assert(v._value);
      _value /= v._value;
      _copiedValue = _value;
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Returns true if object appears to be free from interference.
    //------------------------------------------------------------------------
    bool IsFFI() const
    {
      return (_value == _copiedValue);
    }

  private:
    T  _value;
    T  _copiedValue;
  };

  //==========================================================================

  //--------------------------------------------------------------------------
  //!  Specialization for pointers.  No bitwise operators, no multiply,
  //!  no divide, no modulo.
  //--------------------------------------------------------------------------
  template <typename T>
  class FFIVar<T,typename std::enable_if<std::is_pointer<T>::value>::type>
  {
  public:
    //------------------------------------------------------------------------
    //!  Default constructor
    //------------------------------------------------------------------------
    FFIVar()
        :  _value(nullptr)
    {
      SetFlippedValue();
    }

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~FFIVar()
    {
      _value = nullptr;
      _flippedValue = nullptr;
    }

    //------------------------------------------------------------------------
    //!  Copy constructor
    //------------------------------------------------------------------------
    FFIVar(const FFIVar & v) = default;

    //------------------------------------------------------------------------
    //!  Construct from a T
    //------------------------------------------------------------------------
    FFIVar(const T & t)
        : _value(t)
    {
      SetFlippedValue();
    }
  
    //------------------------------------------------------------------------
    //!  Move constructor
    //------------------------------------------------------------------------
    FFIVar(FFIVar && v) = default;

    //------------------------------------------------------------------------
    //!  Move construct from a T
    //------------------------------------------------------------------------
    FFIVar(T && t)
        : _value(t)
    {
      SetFlippedValue();
    }
  
    //------------------------------------------------------------------------
    //!  Assign from an FFIVar
    //------------------------------------------------------------------------
    FFIVar & operator = (const FFIVar & v) = default;

    //------------------------------------------------------------------------
    //!  Assign from a T
    //------------------------------------------------------------------------
    FFIVar & operator = (const T & t)
    {
      _value = t;
      SetFlippedValue();
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Move assignment from an FFIVar
    //------------------------------------------------------------------------
    FFIVar & operator = (FFIVar &&) = default;

    //------------------------------------------------------------------------
    //!  Move assignment from a T
    //------------------------------------------------------------------------
    FFIVar & operator = (T && t)
    {
      _value = t;
      SetFlippedValue();
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Conversion to T operator
    //------------------------------------------------------------------------
    operator T() const
    {
      assert(IsFFI());
      return _value;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename IT>
    bool operator == (const IT & it) const
    {
      assert(IsFFI());
      return (it == _value);
    }
  
    //------------------------------------------------------------------------
    //!  Equality operator.
    //------------------------------------------------------------------------
    bool operator == (const FFIVar & v) const
    {
      assert(v.IsFFI() && IsFFI());
      return(_value == v._value);
    }

    //------------------------------------------------------------------------
    //!  post-increment operator
    //------------------------------------------------------------------------
    FFIVar operator ++ (int)
    {
      assert(IsFFI());
      T  rc = *this;
      ++_value;
      SetFlippedValue();
      return rc;
    }

    //------------------------------------------------------------------------
    //!  pre-increment operator
    //------------------------------------------------------------------------
    FFIVar & operator ++ ()
    {
      assert(IsFFI());
      ++_value;
      SetFlippedValue();
      return *this;
    }

    //------------------------------------------------------------------------
    //!  post-decrement operator
    //------------------------------------------------------------------------
    FFIVar operator -- (int)
    {
      assert(IsFFI());
      T  rc = *this;
      --_value;
      SetFlippedValue();
      return rc;
    }

    //------------------------------------------------------------------------
    //!  pre-decrement operator
    //------------------------------------------------------------------------
    FFIVar & operator -- ()
    {
      assert(IsFFI());
      --_value;
      SetFlippedValue();
      return *this;
    }
  
    //------------------------------------------------------------------------
    //!  Add and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator += (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value += v._value;
      SetFlippedValue();
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Subtract and assignment operator
    //------------------------------------------------------------------------
    FFIVar & operator -= (const FFIVar & v)
    {
      assert(v.IsFFI());
      _value -= v._value;
      SetFlippedValue();
      return *this;
    }

    //------------------------------------------------------------------------
    //!  Returns true if object appears to be free from interference.
    //------------------------------------------------------------------------
    bool IsFFI() const
    {
      static_assert(sizeof(T) == sizeof(size_t));
      const size_t  *pv = (const size_t *)&_value;
      const size_t  *pf = (const size_t *)&_flippedValue;
      return (*pf == (size_t)~(*pv));
    }
  
  private:
    T    _value;
    T    _flippedValue;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void SetFlippedValue()
    {
      static_assert(sizeof(T) == sizeof(size_t));
      const size_t  *ps = (const size_t *)&_value;
      size_t        *pd = (size_t *)&_flippedValue;
      *pd = ~(*ps);
      return;
    }
  };

  //==========================================================================
  //!  Concepts to apply to binary operators.
  //==========================================================================
  
  //--------------------------------------------------------------------------
  //!  T2 can be added to T1 and yield a T1.  i.e. { T1 + T2 } -> T1
  //--------------------------------------------------------------------------
  template <typename T1, typename T2>
  concept AreFFI_Addable = requires(T1 a, T2 b) {
    { a + b } -> std::same_as<T1>;
  };

  //--------------------------------------------------------------------------
  //!  T2 can be subtracted from T1 and yield a T1.  i.e. { T1 - T2 } -> T1
  //--------------------------------------------------------------------------
  template <typename T1, typename T2>
  concept AreFFI_Subtractable = requires(T1 a, T2 b) {
    { a - b } -> std::same_as<T1>;
  };

  //--------------------------------------------------------------------------
  //!  T1 can be divided by T2 and yield a T1.  i.e. { T1 / T2 } -> T1
  //--------------------------------------------------------------------------
  template <typename T1, typename T2>
  concept AreFFI_Dividable = requires(T1 a, T2 b) {
    { a / b } -> std::same_as<T1>;
  };

  //--------------------------------------------------------------------------
  //!  T1 modulo T2 yields a T1.  i.e. { T1 % T2 } -> T1
  //--------------------------------------------------------------------------
  template <typename T1, typename T2>
  concept AreFFI_Moduloable = requires(T1 a, T2 b) {
    { a % b } -> std::same_as<T1>;
  };

  //--------------------------------------------------------------------------
  //!  T1 can be multiplied by T2 and yield a T1.  i.e. { T1 * T2 } -> T1
  //--------------------------------------------------------------------------
  template <typename T1, typename T2>
  concept AreFFI_Multipliable = requires(T1 a, T2 b) {
    { a * b } -> std::same_as<T1>;
  };
  
  //==========================================================================
  //!  Binary operators.
  //==========================================================================
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T1, typename T2,
            typename std::enable_if<AreFFI_Addable<T1,T2>>>
  FFIVar<T1> operator + (FFIVar<T1> lhs, FFIVar<T2> rhs)
  { lhs += rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T1, typename T2,
            typename std::enable_if<AreFFI_Subtractable<T1,T2>>>
  FFIVar<T1> operator - (FFIVar<T1> lhs, FFIVar<T2> rhs)
  { lhs -= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T1, typename T2,
            typename std::enable_if<AreFFI_Multipliable<T1,T2>>>
  FFIVar<T1> operator * (FFIVar<T1> lhs, FFIVar<T2> rhs)
  { lhs *= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T1, typename T2,
            typename std::enable_if<AreFFI_Dividable<T1,T2>>>
  FFIVar<T1> operator / (FFIVar<T1> lhs, FFIVar<T2> rhs)
  { lhs /= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T1, typename T2,
            typename std::enable_if<AreFFI_Moduloable<T1,T2>>>
  FFIVar<T1> operator % (FFIVar<T1> lhs, FFIVar<T2> rhs)
  { lhs %= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T,
            typename std::enable_if<std::is_integral<T>::value>::type>
  FFIVar<T> operator >> (FFIVar<T> lhs, FFIVar<T> rhs)
  { lhs >>= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T,
            typename std::enable_if<std::is_integral<T>::value>::type>
  FFIVar<T> operator << (FFIVar<T> lhs, FFIVar<T> rhs)
  { lhs <<= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T,
            typename std::enable_if<std::is_integral<T>::value>::type>
  FFIVar<T> operator & (FFIVar<T> lhs, FFIVar<T> rhs)
  { lhs &= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T,
            typename std::enable_if<std::is_integral<T>::value>::type>
  FFIVar<T> operator | (FFIVar<T> lhs, FFIVar<T> rhs)
  { lhs |= rhs;  return lhs; }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename T,
            typename std::enable_if<std::is_integral<T>::value>::type>
  FFIVar<T> operator ^ (FFIVar<T> lhs, FFIVar<T> rhs)
  { lhs ^= rhs;  return lhs; }
  

}  // namespace Dwm

#endif  // _DWMFFIVAR_HH_
