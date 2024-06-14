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
//!  \file DwmProcessTree.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::ProcessTree class implementation
//---------------------------------------------------------------------------

#include "DwmProcessTree.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessTree::ProcessTree(pid_t pid)
  {
    ProcessTable  pt;
    if (GetProcessTable(pt)) {
      auto  myit = pt.find(pid);
      if (myit != pt.end()) {
        _processInfo = myit->second;
        AddChildren(pt);
      }
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  ProcessTree::ProcessTree(const ProcessTable & pt,
                           const ProcessInfo & procInfo)
      : _processInfo(procInfo)
  {
    for (const auto & pte : pt) {
      if (pte.second.ppid == _processInfo.pid) {
        _children[pte.second.pid] =
          ProcessTree(pt, pte.second);
      }
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void ProcessTree::AddChildren(const ProcessTable & pt)
  {
    for (const auto & pte : pt) {
      if (pte.second.ppid == _processInfo.pid) {
        _children[pte.second.pid] = ProcessTree(pt, pte.second);
      }
    }
    return;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::ostream &
  ProcessTree::Print(std::ostream & os, std::string indent) const
  {
    os << indent << _processInfo << '\n';
    for (const auto & child : _children) {
      child.second.Print(os, indent + "  ");
    }
    return os;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  nlohmann::json ProcessTree::ToJson() const
  {
    nlohmann::json  j = _processInfo.ToJson();
    if (! _children.empty()) {
      j["children"] = nlohmann::json::array();
      size_t i = 0;
      for (const auto & child : _children) {
        j["children"][i] = child.second.ToJson();
        ++i;
      }
    }
    return j;
  }
  
  //----------------------------------------------------------------------------
  //!  
  //----------------------------------------------------------------------------
  std::ostream & operator << (std::ostream & os, const Dwm::ProcessTree & pt)
  {
    return pt.Print(os, "");
  }

  bool ToJson(const ProcessTree & processTree, nlohmann::json & j)
  {
    j = processTree._processInfo.ToJson();
    if (! processTree._children.empty()) {
      j["children"] = nlohmann::json::array();
      size_t i = 0;
      for (const auto & child : processTree._children) {
        ToJson(child.second, j["children"][i]);
        ++i;
      }
    }
    return true;
  }
  
  

}  // namespace Dwm


