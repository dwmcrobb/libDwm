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
//!  @file DwmEtcServices.hh
//!  @author Daniel W. McRobb
//!  @brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMETCSERVICES_HH_
#define _DWMETCSERVICES_HH_

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  class EtcServices
  {
  public:
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Entry
    {
    public:
      Entry() = default;
      Entry(const Entry &) = default;
      Entry(Entry &&) = default;
      Entry(const std::string & name, const std::string & protocol,
            uint16_t port, const std::vector<std::string> & aliases)
          : _name(name), _aliases(aliases), _protocol(protocol), _port(port)
      {}
      Entry & operator = (const Entry &) = default;
      Entry & operator = (Entry &&) = default;
      ~Entry() = default;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Name() const  { return _name; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Name(const std::string & name)
      { return _name = name; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<std::string> & Aliases() const
      { return _aliases; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::vector<std::string> &
      Aliases(const std::vector<std::string> & aliases) const
      { return _aliases; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Protocol() const  { return _protocol; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Protocol(const std::string & protocol)
      { return _protocol = protocol; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint16_t Port() const  { return _port; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      uint16_t Port(uint16_t port)  { return _port = port; }

      bool operator == (const Entry &) const = default;
      
    private:
      std::string               _name;
      std::vector<std::string>  _aliases;
      std::string               _protocol;
      uint16_t                  _port;
      
    };

    EtcServices(const std::string & path = "/etc/services");
    ~EtcServices() = default;

    std::optional<Entry> GetEntry(const std::string & name) const;

    std::optional<Entry> GetEntry(uint16_t port,
                                  const std::string & proto) const;

  private:
    std::vector<Entry>                              _entries;
    std::unordered_map<std::string, const Entry *>  _byname;
    std::unordered_map<std::string, const Entry *>  _bypp;
    
  };
  
}  // namespace Dwm

#endif  // _DWMETCSERVICES_HH_
