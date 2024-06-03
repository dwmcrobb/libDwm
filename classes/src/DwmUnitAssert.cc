//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006, 2007, 2009, 2015, 2020
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
//!  \file DwmUnitAssert.cc
//!  \author Daniel W. McRobb
//!  \brief Implementation of unit testing support classes
//---------------------------------------------------------------------------

#include <iomanip>
#include <string>

#include "DwmUnitAssert.hh"

namespace Dwm {

  std::mutex                            Assertions::_mutex;
  std::map<std::string,FileAssertions>  Assertions::_assertions;

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  Assertion::Assertion(const std::string & filename, 
                       const std::string & function,
                       int line, const std::string & expression)
      : _filename(filename), _function(function), _line(line), 
        _expression(expression)
  {
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Assertion::Filename()
  {
    return(_filename);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Assertion::Function()
  {
    return(_function);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Assertion::Line() const
  {
    return(_line);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & Assertion::Expression() const
  {
    return(_expression);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Assertion::operator < (const Assertion & assertion) const
  {
    if (_filename < assertion._filename)
      return(true);
    else if (_filename > assertion._filename)
      return(false);
    else if (_line < assertion._line)
      return(true);
    else if (_line > assertion._line)
      return(false);
    else if (_function < assertion._function)
      return(true);
    else if (_function > assertion._function)
      return(false);
    else if (_expression < assertion._expression)
      return(true);
    else if (_expression > assertion._expression)
      return(false);
    
    return(false);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const Assertion & assertion)
  {
    if (os) {
      os << assertion._filename << ":" << assertion._line 
         << "[" << assertion._function << "] "
         << "'" << assertion._expression << "'";
    }
    return(os);
  }
  

  //==========================================================================
  //==========================================================================


  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  AssertionCounter::AssertionCounter()
      : _passed(0), _failed(0)
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t AssertionCounter::Failed() const
  {
    return(_failed);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint64_t AssertionCounter::Passed() const
  {
    return(_passed);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void AssertionCounter::AddFailed()
  {
    ++_failed;
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void AssertionCounter::AddPassed()
  {
    ++_passed;
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  AssertionCounter & 
  AssertionCounter::operator += (const AssertionCounter & assertionCounter)
  {
    _failed += assertionCounter._failed;
    _passed += assertionCounter._passed;
    return(*this);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream &
  operator << (std::ostream & os, const AssertionCounter & assertionCounter)
  {
    if (os) {
      os << assertionCounter._passed << "/" 
         << (assertionCounter._passed + assertionCounter._failed);
    }
    return(os);
  }

      
  
  //==========================================================================
  //==========================================================================


  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void FunctionAssertions::Failed(const std::string & filename,
                                  const std::string & function,
                                  int lineNumber, const std::string & test)
  {
    if (_function.empty()) {
      _function = function;
    }
    Assertion  assertion(filename, function, lineNumber, test);
    _assertions[assertion].AddFailed();
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void FunctionAssertions::Passed(const std::string & filename,
                                  const std::string & function,
                                  int lineNumber, const std::string & test)
  {
    if (_function.empty()) {
      _function = function;
    }
    Assertion  assertion(filename, function, lineNumber, test);
    _assertions[assertion].AddPassed();
    return;
  }
  
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  AssertionCounter FunctionAssertions::Total() const
  {
    AssertionCounter  rc;
    for (auto i : _assertions) {
      rc += i.second;
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & FunctionAssertions::Print(std::ostream & os, 
                                           bool onlyFailed) const
  {
    if (os && (! _assertions.empty())) {
      AssertionCounter  total;
      for (auto i : _assertions) {
        if ((! onlyFailed) || (onlyFailed && i.second.Failed())) {
          os << "    " << i.second 
             << "  '" << i.first.Expression() << "' (line " 
             << i.first.Line() << ")" << std::endl;
        }
        total += i.second;
      }
      if (! onlyFailed) {
        os << "    " << total << " total passed '" << _function << "'" 
           << std::endl;
      }
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & FunctionAssertions::PrintJson(std::ostream & os,
                                               bool onlyFailed) const
  {
    if (! _assertions.empty()) {
      AssertionCounter  total;
      os << "            {\n"
         << "              \"" << _function << "\": {\n"
         << "                \"assertions\": [\n";
      for (auto it = _assertions.begin(); it != _assertions.end(); ++it) {
        if ((! onlyFailed) || (onlyFailed && it->second.Failed())) {
          if (it != _assertions.begin()) { os << ",\n"; }
          os << "                  {\n"
             << "                    \"failureCount\": " << it->second.Failed()
             << ",\n"
             << "                    \"passCount\": " << it->second.Passed()
             << ",\n"
             << "                    \"expression\": \""
             << it->first.Expression()
             << "\",\n"
             << "                    \"line\": " << it->first.Line() << "\n"
             << "                  }";
        }
        total += it->second;
      }
      os << "\n                ]";
      if (! onlyFailed) {
        os << ",\n                \"totalPassed\": " << total.Passed();
      }
      os << "\n              }"
         << "\n            }";
    }
    return os;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & FunctionAssertions::PrintXML(std::ostream & os, 
                                              bool onlyFailed) const
  {
    if (os && (! _assertions.empty())) {
      os << "      <function>\n"
         << "        <signature>" << _function << "</signature>\n"
         << "        <assertions>\n";
      AssertionCounter  total;
      for (auto i : _assertions) {
        if ((! onlyFailed) || (onlyFailed && i.second.Failed())) {
          os << "          <failureCount>" << i.second.Failed()
             << "</failureCount>\n"
             << "          <passCount>" << i.second.Passed()
             << "</passCount>\n"
             << "          <expression>" << i.first.Expression()
             << "</expression>\n"
             << "          <line>" << i.first.Line()
             << "</line>\n";
        }
        total += i.second;
      }
      os << "        </assertions>\n";
      
      if (! onlyFailed) {
        os << "        <totalPassed>" << total << "</totalPassed>\n";
      }
      
      os << "      </function>\n";
    }
    return(os);
  }
  
  //==========================================================================
  //==========================================================================

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void FileAssertions::Failed(const std::string & filename, 
                              const std::string & function, 
                              int lineNumber, const std::string & test)
  {
    if (_filename.empty()) {
      _filename = filename;
    }
    _assertions[function].Failed(filename, function, lineNumber, test);
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void FileAssertions::Passed(const std::string & filename,
                              const std::string & function,
                              int lineNumber, const std::string & test)
  {
    if (_filename.empty()) {
      _filename = filename;
    }
    _assertions[function].Passed(filename, function, lineNumber, test);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  AssertionCounter FileAssertions::Total() const
  {
    AssertionCounter  rc;
    for (auto i : _assertions) {
      rc += i.second.Total();
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream &
  FileAssertions::Print(std::ostream & os, bool onlyFailed) const
  {
    if (os && (! _assertions.empty())) {
      AssertionCounter  total;
      for (auto & i : _assertions) {
        if ((! onlyFailed) || (onlyFailed && i.second.Total().Failed())) {
          os << "  " << i.first << ":" << std::endl;
          i.second.Print(os, onlyFailed);
          total += i.second.Total();
        }
      }
      if (! onlyFailed) {
        os << "  " << total << " total passed " << _filename << std::endl;
      }
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream &
  FileAssertions::PrintJson(std::ostream & os, bool onlyFailed) const
  {
    if (os && (! _assertions.empty())) {
      AssertionCounter  total;
      os << "      {\n"
         << "        \"" << _filename << "\": {\n"
         << "          \"functions\": [\n";
      auto  lastit = _assertions.end();
      --lastit;
      for (auto it = _assertions.begin(); it != _assertions.end(); ++it) {
        if ((! onlyFailed) || (onlyFailed && it->second.Total().Failed())) {
          it->second.PrintJson(os, onlyFailed);
          if (it != lastit) {
            os << ',';
          }
          os << '\n';
          total += it->second.Total();
        }
      }
      os << "          ]";
      if (! onlyFailed) {
        os << ",\n"
           << "          \"totalPassed\": " << total.Passed();
      }
      os << '\n'
         << "        }\n"
         << "      }";
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream &
  FileAssertions::PrintXML(std::ostream & os, bool onlyFailed) const
  {
    if (os && (! _assertions.empty())) {
      AssertionCounter  total;
      os << "    <file>\n"
         << "      <name>" << _filename << "</name>\n"
         << "      <functions>\n";
      for (auto i : _assertions) {
        if ((! onlyFailed) || (onlyFailed && i.second.Total().Failed())) {
          i.second.PrintXML(os, onlyFailed);
          total += i.second.Total();
        }
      }
      os << "      </functions>\n";
      if (! onlyFailed) {
        os << "      <totalPassed>" << total << "</totalPassed>\n";
      }
      os << "    </file>\n";
    }
    return(os);
  }
  
  //==========================================================================
  //==========================================================================


  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Assertions::Failed(const std::string & filename,
                          const std::string & function,
                          int lineNumber, const std::string & test)
  {
    std::lock_guard<std::mutex>  lock(_mutex);
    _assertions[filename].Failed(filename, function, lineNumber, test);
    return(false);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool Assertions::Passed(const std::string & filename,
                          const std::string & function,
                          int lineNumber, const std::string & test)
  {
    std::lock_guard<std::mutex>  lock(_mutex);
    _assertions[filename].Passed(filename, function, lineNumber, test);
    return(true);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  AssertionCounter Assertions::Total()
  {
    std::lock_guard<std::mutex>   lock(_mutex);
    AssertionCounter  rc;
    for (auto i : _assertions) {
      rc += i.second.Total();
    }
    return(rc);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Assertions::Print(std::ostream & os, bool onlyFailed) 
  {
    std::lock_guard<std::mutex>  lock(_mutex);
    if (os && (! _assertions.empty())) {
      AssertionCounter  total;
      for (auto i : _assertions) {
        if ((! onlyFailed) || (onlyFailed && i.second.Total().Failed())) {
          os << i.first << ":" << std::endl;
          i.second.Print(os, onlyFailed);
          total += i.second.Total();
        }
      }
      if (! onlyFailed) {
        os << total << " total passed" << std::endl;
      }
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Assertions::PrintJson(std::ostream & os, bool onlyFailed) 
  {
    std::lock_guard<std::mutex>  lock(_mutex);
    if (os && (! _assertions.empty())) {
      os << "{\n  \"assertions\": {\n"
         << "    \"files\": [\n";
      AssertionCounter  total;
      auto  lastit = _assertions.end();
      --lastit;
      for (auto it = _assertions.begin(); it != _assertions.end(); ++it) {
        if ((! onlyFailed) || (onlyFailed && it->second.Total().Failed())) {
          it->second.PrintJson(os, onlyFailed);
          if (it != lastit) {
            os << ',';
          }
          os << '\n';
          total += it->second.Total();
        }
      }
      os << "    ]";
      if (! onlyFailed) {
        os << ",\n    \"totalPassed\": " << total.Passed();
      }
      os << "\n  }\n}\n";
    }
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream & Assertions::PrintXML(std::ostream & os, bool onlyFailed) 
  {
    std::lock_guard<std::mutex>  lock(_mutex);
    if (os && (! _assertions.empty())) {
      os << "<assertions>\n"
         << "  <files>\n";
      
      AssertionCounter  total;
      for (auto i : _assertions) {
        if ((! onlyFailed) || (onlyFailed && i.second.Total().Failed())) {
          i.second.PrintXML(os, onlyFailed);
          total += i.second.Total();
        }
      }
      os << "  </files>\n";
      if (! onlyFailed) {
        os << "  <totalPassed>" << total << "</totalPassed>\n";
      }
      os << "</assertions>\n";
    }
    return(os);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int Assertions::PrintAndReturn()
  {
    int  rc = 0;
    if (Total().Failed() > 0) {
      Print(std::cerr, true);
      rc = 1;
    }
    else {
      std::cout << Total() << " passed" << std::endl;
    }
    return rc;
  }

}  // namespace Dwm


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
