//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2024
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
//!  \file DwmProcessTree.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::ProcessTree class declaration
//---------------------------------------------------------------------------

#include "DwmProcessTable.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  Encapsulates a process tree, i.e. a process and its descendants.
  //--------------------------------------------------------------------------
  class ProcessTree
  {
  public:
    //------------------------------------------------------------------------
    //!  Defaulted constructors and assignment operators.
    //------------------------------------------------------------------------
    ProcessTree() = default;
    ProcessTree(const ProcessTree &) = default;
    ProcessTree(ProcessTree &&) = default;
    ProcessTree & operator = (const ProcessTree &) = default;
    ProcessTree & operator = (ProcessTree &&) = default;
    
    //------------------------------------------------------------------------
    //!  Create a process tree rooted at the given process ID @c pid.
    //------------------------------------------------------------------------
    ProcessTree(pid_t pid);

    //------------------------------------------------------------------------
    //!  Returns the process info for the process.
    //------------------------------------------------------------------------
    inline const ProcessInfo & ProcInfo() const
    { return _processInfo; }

    //------------------------------------------------------------------------
    //!  Returns the children of the process.
    //------------------------------------------------------------------------
    inline const std::map<pid_t,ProcessTree> & Children() const
    { return _children; }

    //------------------------------------------------------------------------
    //!  Prints the process tree to an ostream.  This isn't very useful,
    //!  it's mostly for debugging.
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const ProcessTree & pt);

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    nlohmann::json ToJson() const;
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    friend bool ToJson(const ProcessTree & processTree, nlohmann::json & json);
    
  private:
    ProcessInfo                  _processInfo;
    std::map<pid_t,ProcessTree>  _children;

    ProcessTree(const ProcessTable & pt, const ProcessInfo & procInfo);
    void AddChildren(const ProcessTable & pt);
    std::ostream & Print(std::ostream & os, std::string indent) const;
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool ToJson(const ProcessTree & processTree, nlohmann::json & json);

}  // namespace Dwm
