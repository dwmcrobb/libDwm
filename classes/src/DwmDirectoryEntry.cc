//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2011
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
//!  \file DwmDirectoryEntry.cc
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

extern "C" {
  #include <sys/types.h>
  #include <sys/param.h>
  #include <sys/stat.h>
  #include <dirent.h>
  #include <libgen.h>
  #include <unistd.h>
}

#include <cstdlib>
#include <cstring>
#include <map>

#include "DwmSysLogger.hh"
#include "DwmDirectoryEntry.hh"

using namespace std;

namespace Dwm {

  std::mutex  DirectoryEntry::_readdirMtx;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DirectoryEntry::DirectoryEntry(const std::string & path)
      : _path(path), _parent(0), _type(e_typeUnknown)
  {
    if (_path.length() > 1) {
      while ((_path[_path.length() - 1] == '/')
             && (_path.length() > 1)) {
        _path.erase(_path.length() - 1, 1);
      }
      size_t  idx;
      while ((idx = _path.find("//")) != string::npos) {
        _path.erase(idx, 1);
      }
    }
    
    if (! _path.empty()) {
      struct stat  s;
      if (stat(_path.c_str(), &s) == 0) {
        _type = GetType(s);
      }
    }
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const std::string & DirectoryEntry::Path() const
  {
    return _path;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DirectoryEntry::RelativePath() const
  {
    string  rc = BaseName();
    const DirectoryEntry  *parent = _parent;
    while (parent) {
      rc = parent->BaseName() + string("/") + rc;
      parent = parent->_parent;
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DirectoryEntry::TypeEnum DirectoryEntry::Type() const
  {
    return _type;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DirectoryEntry::BaseName() const
  {
    string  rc(_path);
    size_t  idx = _path.find_last_of('/');
    if (idx != string::npos) {
      if ((idx + 1) < _path.length()) {
        rc = _path.substr(idx + 1);
      }
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DirectoryEntry::DirName() const
  {
    string        rc;
    static mutex  mtx;
    
    if (! _path.empty()) {
      mtx.lock();
      rc = dirname((char *)_path.c_str());
      mtx.unlock();
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DirectoryEntry::RealPath() const
  {
    string  rc;
    char    rpbuf[PATH_MAX];

    memset(rpbuf, 0, sizeof(char) * PATH_MAX);
    if (realpath(_path.c_str(), rpbuf) == rpbuf) {
      rc = rpbuf;
    }
    
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  std::string DirectoryEntry::Extension() const
  {
    string  rc;
    string  name = BaseName();
    size_t  dotidx = name.find_last_of('.');
    if (dotidx != string::npos) {
      if ((dotidx + 1) < name.length()) {
        rc = name.substr(dotidx + 1);
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  const DirectoryEntry *DirectoryEntry::Parent() const
  {
    return _parent;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DirectoryEntry::Exists() const
  {
    bool  rc = false;
    if (! _path.empty()) {
      rc = (access(_path.c_str(), F_OK) == 0);
    }
    return rc;
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DirectoryEntry::IsDirectory() const
  {
    return (_type == e_typeDirectory);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool
  DirectoryEntry::GetChildren(std::vector<DirectoryEntry> & children,
                              uint8_t typeMask) const
  {
    bool  rc = false;
    
    children.clear();

    if (IsDirectory()) {
      DIR  *dir = opendir(_path.c_str());
      if (dir) {
        struct dirent  *dirEnt;
        bool            done = false;
        while (! done) {
          lock_guard<std::mutex>  readdirLock(_readdirMtx);
          if ((dirEnt = readdir(dir)) != nullptr) {
            if (string(dirEnt->d_name) != "."
              && string(dirEnt->d_name) != "..") {
              if (GetType(*dirEnt) & typeMask) {
                children.push_back(DirectoryEntry(_path 
                                                  + string("/") 
                                                  + string(dirEnt->d_name), 
                                                  this));
              }
            }
          }
          else {
            done = true;
          }
        }
        closedir(dir);
      }
    }
    return (! children.empty());
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  bool DirectoryEntry::GetChildren(std::vector<DirectoryEntry> & children,
                                   DirectoryEntryFunctor & dirEntryFunc) const
  {
    bool  rc = false;
    
    children.clear();

    if (IsDirectory()) {
      DIR  *dir = opendir(_path.c_str());
      if (dir) {
        struct dirent  *dirEnt;
        bool            done = false;
        while (! done) {
          lock_guard<std::mutex>  readdirLock(_readdirMtx);
          dirEnt = readdir(dir);
          if (dirEnt != nullptr) {
            if (string(dirEnt->d_name) != "."
                && string(dirEnt->d_name) != "..") {
              DirectoryEntry  de(_path + string("/") + string(dirEnt->d_name),
                                 this);
              if (dirEntryFunc(de)) {
                children.push_back(de);
              }
            }
          }
          else {
            done = true;
          }
        }
        closedir(dir);
      }
    }
    return (! children.empty());
  }
    
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t DirectoryEntry::Recurse(DirectoryEntry & de,
                                   DirectoryEntryFunctor & dirEntryFunc,
                                   int depth, bool depthFirst, int maxDepth)
  {
    uint32_t  rc = 0;
    if ((depth <= maxDepth)
        || (maxDepth < 0)) {
      if (! depthFirst) {
        rc += dirEntryFunc(de);
      }
      
      if (de.IsDirectory()) {
        ++depth;
        vector<DirectoryEntry>  children;
        uint8_t                 filter;
        filter = depthFirst ? e_typeDirectory : ~((uint8_t)e_typeDirectory);
        if (de.GetChildren(children, filter)) {
          vector<DirectoryEntry>::iterator i = children.begin();
          for ( ; i != children.end(); ++i) {
            rc += i->Recurse(*i, dirEntryFunc, depth, depthFirst, maxDepth);
          }
        }
        filter = depthFirst ? ~((uint8_t)e_typeDirectory) : e_typeDirectory;
        if (de.GetChildren(children, filter)) {
          vector<DirectoryEntry>::iterator i = children.begin();
          for ( ; i != children.end(); ++i) {
            rc += i->Recurse(*i, dirEntryFunc, depth, depthFirst, maxDepth);
          }
        }
      }
      if (depthFirst) {
        rc += dirEntryFunc(de);
      }
    }
    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  uint32_t DirectoryEntry::Recurse(DirectoryEntryFunctor & dirEntryFunc,
                                   bool depthFirst, int maxDepth)
  {
    return Recurse(*this, dirEntryFunc, 0, depthFirst, maxDepth);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DirectoryEntry::DirectoryEntry(const std::string & path, 
                                 const DirectoryEntry *parent)
      : _path(path), _parent(parent), _type(e_typeUnknown)
  {
    if (_path.length() > 1) {
      while ((_path[_path.length() - 1] == '/')
             && (_path.length() > 1)) {
        _path.erase(_path.length() - 1, 1);
      }
      size_t  idx;
      while ((idx = _path.find("//")) != string::npos) {
        _path.erase(idx, 1);
      }
    }
    struct stat  s;
    if (stat(_path.c_str(), &s) == 0) {
      _type = GetType(s);
    }
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DirectoryEntry::TypeEnum
  DirectoryEntry::GetType(const struct stat & statStruct)
  {
    TypeEnum  rc = e_typeUnknown;
    
    static map<mode_t,TypeEnum>  typeMap;
    static mutex                 mtx;
    mtx.lock();
    if (typeMap.empty()) {
      typeMap[S_IFIFO]  = e_typeFifo;
      typeMap[S_IFCHR]  = e_typeCharacterSpecial;
      typeMap[S_IFDIR]  = e_typeDirectory;
      typeMap[S_IFBLK]  = e_typeBlockSpecial;
      typeMap[S_IFREG]  = e_typeRegular;
      typeMap[S_IFLNK]  = e_typeSymbolicLink;
      typeMap[S_IFSOCK] = e_typeSocket;
    }
    mtx.unlock();
    
    map<mode_t,TypeEnum>::const_iterator  i = 
      typeMap.find(statStruct.st_mode & S_IFMT);
    if (i != typeMap.end()) {
      rc = i->second;
    }

    return rc;
  }
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  DirectoryEntry::TypeEnum  
  DirectoryEntry::GetType(const struct dirent & dirEntry)
  {
    TypeEnum  rc = e_typeUnknown;
    
    static map<uint8_t,TypeEnum>  typeMap;
    static mutex                  mtx;
    mtx.lock();
    if (typeMap.empty()) {
      typeMap[DT_FIFO] = e_typeFifo;
      typeMap[DT_CHR]  = e_typeCharacterSpecial;
      typeMap[DT_DIR]  = e_typeDirectory;
      typeMap[DT_BLK]  = e_typeBlockSpecial;
      typeMap[DT_REG]  = e_typeRegular;
      typeMap[DT_LNK]  = e_typeSymbolicLink;
      typeMap[DT_SOCK] = e_typeSocket;
    }
    mtx.unlock();
    
    map<uint8_t,TypeEnum>::const_iterator  i = 
      typeMap.find(dirEntry.d_type);
    if (i != typeMap.end()) {
      rc = i->second;
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
