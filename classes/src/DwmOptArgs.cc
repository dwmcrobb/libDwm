//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/src/DwmOptArgs.cc 11120 $
// @(#) $Id: DwmOptArgs.cc 11120 2020-09-07 23:14:33Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007, 2020
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
//!  \file DwmOptArgs.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::OptArg and Dwm::OptArgs class implementations
//---------------------------------------------------------------------------

#include <string>

#include "DwmOptArgs.hh"
#include "DwmSvnTag.hh"

static const Dwm::SvnTag  svntag("@(#) $DwmPath: dwm/libDwm/trunk/src/DwmOptArgs.cc 11120 $");

using namespace std;

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  OptArg::OptArg()
      : _name(), _needsArg(false), _longName(), _isRequired(false), _value()
  {}
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  OptArg::OptArg(const string & c, const string & longName, bool isRequired, 
                 const string & value, const string & help)
      : _name(c),  _longName(longName), _isRequired(isRequired),
        _needsArg(false), _value(value), _help(help)
  {
    if (c.length() == 2) {
      if (c[1] == ':') {
        _needsArg = true;
      }
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & OptArg::Name() const
  {
    return(_name);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & OptArg::LongName() const
  {
    return(_longName);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool OptArg::IsRequired() const
  {
    return(_isRequired);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool OptArg::IsRequired(bool isRequired)
  {
    _isRequired = isRequired;
    return(_isRequired);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool OptArg::NeedsArg() const
  {
    return(_needsArg);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & OptArg::Value() const
  {
    return(_value);
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & OptArg::Value(const string & value)
  {
    _value = value;
    return(_value);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & OptArg::Help() const
  {
    return(_help);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const string & OptArg::Help(const string & help)
  {
    _help = help;
    return(help);
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  OptArgs::OptArgs()
      : _args(), _longArgs(), _normalArgs()
  {}
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void OptArgs::AddOptArg(const OptArg & optarg)
  {
    _args[optarg.Name()[0]] = optarg;
    _longArgs[optarg.LongName()] = &(_args[optarg.Name()[0]]);
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void OptArgs::AddOptArg(const string & name, const string & longName,
                          bool isRequired, const string & def, 
                          const string & help)
  {
    OptArg  arg(name, longName);
    arg.Value(def);
    arg.IsRequired(isRequired);
    arg.Help(help);
    _args[name[0]] = arg;
    _longArgs[longName] = &(_args[name[0]]);
  }
       
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void OptArgs::AddNormalArg(const string & name, bool required)
  {
    _normalArgs.push_back(pair<string,bool>(name, required));
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void OptArgs::Usage(const string & argv0) const
  {
    cerr << "usage: " << argv0;
    map<int,OptArg>::const_iterator  i;
    for (i = _args.begin(); i != _args.end(); ++i) {
      cerr << " ";
      if (! i->second.IsRequired())
        cerr << "[";
      cerr << "-" << (char)i->first << "|--" << i->second.LongName();
      if (i->second.NeedsArg())
        cerr << " value";
      if (! i->second.IsRequired())
        cerr << "]";
    }
    vector<pair<string,bool> >::const_iterator  ni;
    for (ni = _normalArgs.begin(); ni != _normalArgs.end(); ++ni) {
      cerr << " ";
      if (! ni->second)
        cerr << "[";
      cerr << ni->first;
      if (! ni->second)
        cerr << "]";
    }
    cerr << endl;
    for (i = _args.begin(); i != _args.end(); ++i) {
      if (! i->second.Help().empty()) {
        if (! i->second.IsRequired())
          cerr << "[";
        cerr << "-" << (char)i->first 
             << "|--" << i->second.LongName();
        if (i->second.NeedsArg())
          cerr << " " << i->second.LongName();
        if (! i->second.IsRequired())
          cerr << "]";
        cerr << ":\n";
        cerr << "  " << i->second.Help() << "\n";
      }
    }
    return;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  int OptArgs::Parse(int argc, const string argv[])
  {
    int  i = 1;
    for ( ; i < argc; ++i) {
      string  arg(argv[i]);
      if (arg.substr(0,2) == "--") {
        if (arg.length() == 2) {
          ++i;
          break;
        }
        string  longName(&(argv[i][2]));
        map<string,OptArg *>::iterator  iter = _longArgs.find(longName);
        if (iter != _longArgs.end()) {
          if (iter->second->NeedsArg()) {
            if (++i < argc) {
              iter->second->Value(argv[i]);
            }
            else {
              Usage(argv[0]);
              exit(1);
            }
          }
          else {
            iter->second->Value("1");
          }
        }
        else {
          Usage(argv[0]);
          exit(1);
        }
      }
      else if (argv[i][0] == '-') {
        if (argv[i].length() == 2) {
          map<int,OptArg>::iterator  iter = _args.find(argv[i][1]);
          if (iter != _args.end()) {
            if (iter->second.NeedsArg()) {
              if (++i < argc) {
                iter->second.Value(argv[i]);
              }
              else {
                Usage(argv[0]);
                exit(1);
              }
            }
            else {
              iter->second.Value("1");
            }
          }
          else {
            Usage(argv[0]);
            exit(1);
          }
        }
        else {
          break;
        }
      }
      else {
        break;
      }
    }

    map<int,OptArg>::const_iterator  iter;
    for (iter = _args.begin(); iter != _args.end(); ++iter) {
      if (iter->second.IsRequired() && iter->second.Value().empty()) {
        Usage(argv[0]);
        exit(1);
      }
    }
    
    return(i);
  }
    
  //------------------------------------------------------------------------
  //!  
  //------------------------------------------------------------------------
  int OptArgs::Parse(int argc, char *argv[])
  {
    string  *args = new string[argc];
    for (uint32_t i = 0; i < argc; ++i) {
      args[i] = argv[i];
    }
    int rc = Parse(argc, args);
    delete[] args;
    return rc;
  }
     

}  // namespace Dwm
