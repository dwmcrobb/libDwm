//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/include/DwmOptArgs.hh 8401 $
// @(#) $Id: DwmOptArgs.hh 8401 2016-04-17 06:44:31Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2016
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
//!  \file DwmOptArgs.hh
//!  \brief Dwm::Argument, Dwm::OptArg and Dwm::OptArgs definitions
//---------------------------------------------------------------------------

#ifndef _DWMOPTARGS_HH_
#define _DWMOPTARGS_HH_

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "DwmStringUtils.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Pre-declare the Argument class template.
  //--------------------------------------------------------------------------
  template <typename T> class Argument;
  
  //--------------------------------------------------------------------------
  //!  ostream operator << for Argument instances.
  //--------------------------------------------------------------------------
  template <typename T>
  std::ostream & operator << (std::ostream & os, const Argument<T> & arg)    
  {
    os << arg._value;
    return(os);
  }
  
  //--------------------------------------------------------------------------
  //!  Simple class template for a command-line argument.  This can be
  //!  used directly, thought it typically is only used by OptArgs.
  //--------------------------------------------------------------------------
  template <typename T>
  class Argument
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Argument<T>()
    : _value()
    {}
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Argument<T> operator = (const std::string & s)
    {
      StringUtils::StringTo(s, _value);
      return(*this);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    operator T () const
    {
      return(_value);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    friend std::ostream & 
    operator << <> (std::ostream & os, const Argument<T> & arg);
    
  protected:
    T  _value;
  };
  
  //--------------------------------------------------------------------------
  //!  Encapsulates a command line option and its argument (if it takes
  //!  an argument).  Like getopt(), an option whose short name is followed
  //!  by a colon will have an argument.  An option whoe short name does
  //!  not have an argument is a boolean.
  //--------------------------------------------------------------------------
  class OptArg
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    OptArg();
    
    OptArg(const std::string & c, const std::string & longName = "", 
           bool isRequired = false, const std::string & value = "",
           const std::string & help = "");

    //------------------------------------------------------------------------
    //!  Returns the name of the option.
    //------------------------------------------------------------------------
    const std::string & Name() const;

    //------------------------------------------------------------------------
    //!  Returns the long name for the option.
    //------------------------------------------------------------------------
    const std::string & LongName() const;
    
    //------------------------------------------------------------------------
    //!  Returns whether or not the option is required.
    //------------------------------------------------------------------------
    bool IsRequired() const;
    
    //------------------------------------------------------------------------
    //!  Sets whether or not the option is required.
    //------------------------------------------------------------------------
    bool IsRequired(bool isRequired);
    
    //------------------------------------------------------------------------
    //!  Returns true if an argument is required for the option.
    //------------------------------------------------------------------------
    bool NeedsArg() const;
    
    //------------------------------------------------------------------------
    //!  Returns the value of the argument.
    //------------------------------------------------------------------------
    const std::string & Value() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the value of the argument.
    //------------------------------------------------------------------------
    const std::string & Value(const std::string & value);

    //------------------------------------------------------------------------
    //!  Returns the help text for the argument.
    //------------------------------------------------------------------------
    const std::string & Help() const;
    
    //------------------------------------------------------------------------
    //!  Sets and returns the help text for the argument.
    //------------------------------------------------------------------------
    const std::string & Help(const std::string & help);
    
  protected:
    std::string  _name;
    std::string  _longName;
    bool        _isRequired;
    bool        _needsArg;
    std::string  _value;
    std::string  _help;
  };

  //--------------------------------------------------------------------------
  //!  Class to encapsulate command line options and parse them.
  //--------------------------------------------------------------------------
  class OptArgs
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    OptArgs();
    
    //------------------------------------------------------------------------
    //!  Adds an optional argument.
    //------------------------------------------------------------------------
    void AddOptArg(const OptArg & optarg);
    
    //------------------------------------------------------------------------
    //!  Adds an optional argument.
    //------------------------------------------------------------------------
    void AddOptArg(const std::string & name, const std::string & longName,
                   bool isRequired = false, const std::string & def = "",
                   const std::string & help = "");

    void AddNormalArg(const std::string & name, bool required);
    
    //------------------------------------------------------------------------
    //!  Fetches the value of an optional argument by its short name.
    //------------------------------------------------------------------------
    template <typename T>
    T Get(int c)
    {
      Argument<T>  rc;
      std::map<int,OptArg>::const_iterator i = _args.find(c);
      if (i != _args.end()) {
        rc = i->second.Value();
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Fetches the value of an optional argument by its long name.
    //------------------------------------------------------------------------
    template <typename T>
    T Get(const std::string & s)
    {
      Argument<T>  rc;
      std::map<std::string,OptArg *>::const_iterator i = _longArgs.find(s);
      if (i != _longArgs.end()) {
        rc = i->second->Value();
      }
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  Parses all optional arguments.  Returns the next argument index,
    //!  which the caller should compare against argc.  If parsing fails,
    //!  will emit a usage statement and exit.
    //------------------------------------------------------------------------
    int Parse(int argc, const std::string argv[]);

    //------------------------------------------------------------------------
    //!  Parses all optional arguments.  Returns the next argument index,
    //!  which the caller should compare against argc.  If parsing fails,
    //!  will emit a usage statement and exit.
    //------------------------------------------------------------------------
    int Parse(int argc, char *argv[]);
     
    //------------------------------------------------------------------------
    //!  Prints the usage statement.
    //------------------------------------------------------------------------
    void Usage(const std::string & argv0) const;
                    
  protected:
    std::map<int,OptArg>            _args;
    std::map<std::string,OptArg *>  _longArgs;

    std::vector<std::pair<std::string,bool> >  _normalArgs;
  };
  

}  // namespace Dwm

#endif  // _DWMOPTARGS_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++/la
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
