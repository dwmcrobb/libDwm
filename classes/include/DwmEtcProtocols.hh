//===========================================================================
//  Copyright (c) Daniel W. McRobb 2026
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
//!  @file DwmEtcProtocols.hh
//!  @author Daniel W. McRobb
//!  @brief Dwm::EtcProtocols class definition
//---------------------------------------------------------------------------

#ifndef _DWMETCPROTOCOLS_HH_
#define _DWMETCPROTOCOLS_HH_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates /etc/protocols
  //--------------------------------------------------------------------------
  class EtcProtocols
  {
  public:
    //------------------------------------------------------------------------
    //!  Encapsulates a single /etc/protocols entry.
    //------------------------------------------------------------------------
    class Entry
    {
    public:
      //----------------------------------------------------------------------
      //!  Default constructor
      //----------------------------------------------------------------------
      Entry() = default;
      
      //----------------------------------------------------------------------
      //!  Default copy constructor
      //----------------------------------------------------------------------
      Entry(const Entry &) = default;
      
      //----------------------------------------------------------------------
      //!  Default move constructor
      //----------------------------------------------------------------------
      Entry(Entry &&) = default;

      //----------------------------------------------------------------------
      //!  Construct from the given @c name, @c number and @c aliases.
      //----------------------------------------------------------------------
      Entry(const std::string & name, int number,
            const std::vector<std::string> & aliases)
          : _name(name), _number(number), _aliases(aliases)
      {}

      //----------------------------------------------------------------------
      //!  Construct from the given @c name and @c number.
      //----------------------------------------------------------------------
      Entry(const std::string & name, int number)
          : _name(name), _number(number), _aliases()
      {}
      
      //----------------------------------------------------------------------
      //!  Copy assignment
      //----------------------------------------------------------------------
      Entry & operator = (const Entry &) = default;

      //----------------------------------------------------------------------
      //!  Move assignment
      //----------------------------------------------------------------------
      Entry & operator = (Entry &&) = default;
      
      //----------------------------------------------------------------------
      //!  Destructor
      //----------------------------------------------------------------------
      ~Entry() = default;
      
      //----------------------------------------------------------------------
      //!  Returns the official protocol name.
      //----------------------------------------------------------------------
      const std::string & Name() const
      { return _name; }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the official protocol name.
      //----------------------------------------------------------------------
      const std::string & Name(const std::string & name)
      { return _name = name; }
      
      //----------------------------------------------------------------------
      //!  Returns the protocol aliases.
      //----------------------------------------------------------------------
      const std::vector<std::string> & Aliases() const
      { return _aliases; }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the protocol aliases.
      //----------------------------------------------------------------------
      const std::vector<std::string> &
      Aliases(const std::vector<std::string> & aliases)
      { return _aliases = aliases; }
      
      //----------------------------------------------------------------------
      //!  Returns the protocol number.
      //----------------------------------------------------------------------
      int Number() const
      { return _number; }
      
      //----------------------------------------------------------------------
      //!  Sets and returns the protocol number.
      //----------------------------------------------------------------------
      int Number(int number)
      { return _number = number; }

      //----------------------------------------------------------------------
      //!  Equality operator
      //----------------------------------------------------------------------
      bool operator == (const Entry &) const = default;
      
    private:
      std::string               _name;
      std::vector<std::string>  _aliases;
      int                       _number;
    };

    //------------------------------------------------------------------------
    //!  Construct by parsing the contents of the file at the given @c path.
    //------------------------------------------------------------------------
    EtcProtocols(const std::string & path = "/etc/protocols");

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~EtcProtocols() = default;

    //------------------------------------------------------------------------
    //!  Returns the number of entries.
    //------------------------------------------------------------------------
    size_t NumEntries() const  { return _entries.size(); }

    //------------------------------------------------------------------------
    //!  Searches for the entry with the given @c name (which may match the
    //!  protocol's official name or one of its aliases).  Returns a populated
    //!  optional if a match is found, else returns an empty optional.
    //------------------------------------------------------------------------
    std::optional<Entry> GetEntry(const std::string & name) const;
    
    //------------------------------------------------------------------------
    //!  Searches for the entry with the given @c number.  Returns a populated
    //!  optional if a match is found, else returns an empty optional.
    //------------------------------------------------------------------------
    std::optional<Entry> GetEntry(int num) const;
    
  private:
    std::vector<Entry>                             _entries;
    std::unordered_map<std::string,const Entry *>  _byname;
    std::unordered_map<int,const Entry *>          _bynum;

    void AddEntry(const Entry & entry);
  };

}  // namespace Dwm

#endif  // _DWMETCPROTOCOLS_HH_
