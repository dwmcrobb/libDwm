//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2021
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
//!  \file DwmUnitAssert.hh
//!  \brief Dwm::Assertions class definition and UnitAssert() macro for unit
//!  tests.
//---------------------------------------------------------------------------

#ifndef _DWMUNITASSERT_HH_
#define _DWMUNITASSERT_HH_

#include <cstdint>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Holds 'passed' and 'failed' counters.
  //--------------------------------------------------------------------------
  class AssertionCounter
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    AssertionCounter();

    //------------------------------------------------------------------------
    //!  Returns the failed counter.
    //------------------------------------------------------------------------
    uint64_t Failed() const;

    //------------------------------------------------------------------------
    //!  Returns the passed counter.
    //------------------------------------------------------------------------
    uint64_t Passed() const;

    //------------------------------------------------------------------------
    //!  Increments the failed counter.
    //------------------------------------------------------------------------
    void AddFailed();

    //------------------------------------------------------------------------
    //!  Increments the passed counter.
    //------------------------------------------------------------------------
    void AddPassed();

    //------------------------------------------------------------------------
    //!  operator +=
    //------------------------------------------------------------------------
    AssertionCounter & operator += (const AssertionCounter & assertionCounter);
    
    //------------------------------------------------------------------------
    //!  Prints to an ostream.
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const AssertionCounter & assertionCounter);
    
  private:
    uint64_t  _passed;
    uint64_t  _failed;
  };

  //--------------------------------------------------------------------------
  //!  Holds assertion information.
  //--------------------------------------------------------------------------
  class Assertion
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor.
    //------------------------------------------------------------------------
    Assertion(const std::string & filename, const std::string & function,
              int line, const std::string & expression);

    //------------------------------------------------------------------------
    //!  Returns the name of the file where the assertion is located. 
    //------------------------------------------------------------------------
    const std::string & Filename();

    //------------------------------------------------------------------------
    //!  Returns the function where the assertion is located.
    //------------------------------------------------------------------------
    const std::string & Function();

    //------------------------------------------------------------------------
    //!  Returns the line number where the assertion is located.
    //------------------------------------------------------------------------
    int Line() const;

    //------------------------------------------------------------------------
    //!  Returns the assertion test expression.
    //------------------------------------------------------------------------
    const std::string & Expression() const;

    //------------------------------------------------------------------------
    //!  operator <
    //------------------------------------------------------------------------
    bool operator < (const Assertion & assertion) const;

    //------------------------------------------------------------------------
    //!  Prints to an ostream.
    //------------------------------------------------------------------------
    friend std::ostream & 
    operator << (std::ostream & os, const Assertion & assertion);
    
  private:
    std::string   _filename;
    std::string   _function;
    int           _line;
    std::string   _expression;
  };
  
  //--------------------------------------------------------------------------
  //!  A container class to hold all assertions called from a single
  //!  function.
  //--------------------------------------------------------------------------
  class FunctionAssertions
  {
  public:
    //------------------------------------------------------------------------
    //!  Add a failed assertion.
    //------------------------------------------------------------------------
    void Failed(const std::string & filename, const std::string & function,
                int lineNumber, const std::string & test);

    //------------------------------------------------------------------------
    //!  Add a passed assertion.
    //------------------------------------------------------------------------
    void Passed(const std::string & filename, const std::string & function,
                int lineNumber, const std::string & test);

    //------------------------------------------------------------------------
    //!  Returns the total passed and failed counter.
    //------------------------------------------------------------------------
    AssertionCounter Total() const;

    //------------------------------------------------------------------------
    //!  Prints to an ostream.
    //------------------------------------------------------------------------
    std::ostream & Print(std::ostream & os, bool onlyFailed = false) const;

    std::ostream & PrintXML(std::ostream & os, bool onlyFailed = false) const;

  private:
    std::string                           _function;
    std::map<Assertion,AssertionCounter>  _assertions;
  };

  //--------------------------------------------------------------------------
  //!  A container class to hold all assertions called from a single
  //!  source file.
  //--------------------------------------------------------------------------
  class FileAssertions
  {
  public:
    //------------------------------------------------------------------------
    //!  Add a failed assertion.
    //------------------------------------------------------------------------
    void Failed(const std::string & filename, const std::string & function,
                int lineNumber, const std::string & test);

    //------------------------------------------------------------------------
    //!  Add a passed assertion.
    //------------------------------------------------------------------------
    void Passed(const std::string & filename, const std::string & function,
                int lineNumber, const std::string & test);

    //------------------------------------------------------------------------
    //!  Returns the total passed and failed counter.
    //------------------------------------------------------------------------
    AssertionCounter Total() const;

    //------------------------------------------------------------------------
    //!  Prints to an ostream.
    //------------------------------------------------------------------------
    std::ostream & Print(std::ostream & os, bool onlyFailed = false) const;

    //------------------------------------------------------------------------
    //!  Prints XML to an ostream.
    //------------------------------------------------------------------------
    std::ostream & PrintXML(std::ostream & os, bool onlyFailed = false) const;
    
  private:
    std::string                               _filename;
    std::map<std::string,FunctionAssertions>  _assertions;
  };
  
  //--------------------------------------------------------------------------
  //!  A container to hold all assertions.  Note everything is static here.
  //--------------------------------------------------------------------------
  class Assertions
  {
  public:
    //------------------------------------------------------------------------
    //!  Adds a failed assertion.  Not called directly; UnitAssert() calls
    //!  this member.  Always returns false.
    //------------------------------------------------------------------------
    static bool Failed(const std::string & filename,
                       const std::string & function,
                       int lineNumber, const std::string & test);

    //------------------------------------------------------------------------
    //!  Adds a passed assertion.  Not called directly; UnitAssert() calls
    //!  this member.  Always returns true.
    //------------------------------------------------------------------------
    static bool Passed(const std::string & filename, 
                       const std::string & function,
                       int lineNumber, const std::string & test);

    //------------------------------------------------------------------------
    //!  Returns the total passed and failed counter.
    //------------------------------------------------------------------------
    static AssertionCounter Total();

    //------------------------------------------------------------------------
    //!  Prints to an ostream.  If @c onlyFailed is true, prints only
    //!  information for failed assertions.
    //------------------------------------------------------------------------
    static std::ostream & Print(std::ostream & os, bool onlyFailed = false);

    //------------------------------------------------------------------------
    //!  Prints failed tests to cerr and successful test count to cout in
    //!  a simple form that I use in unit test applications.  Returns 0 if
    //!  all unit tests passed, else returns 1.  The return value can be
    //!  used as the exit value of a unit test application to signal to the
    //!  shell as to whether or not all unit tests passed.
    //------------------------------------------------------------------------
    static int PrintAndReturn();
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::ostream & PrintXML(std::ostream & os, bool onlyFailed = false);

  private:
    static std::map<std::string,FileAssertions>  _assertions;
    static std::mutex                            _mutex;
  };
  
  //--------------------------------------------------------------------------
  //!  This macro is used just like assert(), but populates Assertions
  //!  so we can report results at the end of tests using the members of
  //!  Assertions.
  //--------------------------------------------------------------------------
#define UnitAssert(e) ((e) ?                                             \
  Dwm::Assertions::Passed(__FILE__, __PRETTY_FUNCTION__, __LINE__, #e) : \
  Dwm::Assertions::Failed(__FILE__, __PRETTY_FUNCTION__, __LINE__, #e))
  
}  // namespace Dwm

#endif  // _DWMUNITASSERT_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
