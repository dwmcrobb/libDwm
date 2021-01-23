//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2011, 2016, 2017
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
//!  \file DwmDirectoryEntry.hh
//!  \brief Dwm::DirectoryEntry class definition
//---------------------------------------------------------------------------

#ifndef _DWMDIRECTORYENTRY_HH_
#define _DWMDIRECTORYENTRY_HH_

extern "C" {
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <dirent.h>
}

#include <mutex>
#include <string>
#include <vector>

namespace Dwm {

  //--------------------------------------------------------------------------
  //  pre-declare DirectoryEntry
  //--------------------------------------------------------------------------
  class DirectoryEntry;
  
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  class DirectoryEntryFunctor
  {
  public:
    virtual bool operator () (DirectoryEntry & dirEntry) 
    {
      return false;
    }
  };
  
  //--------------------------------------------------------------------------
  //!  Encapsulates a directory entry (file, directory, FIFO, et. al.).
  //--------------------------------------------------------------------------
  class DirectoryEntry
  {
  public:
    //------------------------------------------------------------------------
    //!  The types of DirectoryEntry objects.
    //------------------------------------------------------------------------
    typedef enum {
      e_typeUnknown          = 0x00,
      e_typeFifo             = 0x01,
      e_typeCharacterSpecial = 0x02,
      e_typeDirectory        = 0x04,
      e_typeBlockSpecial     = 0x08,
      e_typeRegular          = 0x10,
      e_typeSymbolicLink     = 0x20,
      e_typeSocket           = 0x40
    } TypeEnum;

    //------------------------------------------------------------------------
    //!  Construct from a path.
    //------------------------------------------------------------------------
    DirectoryEntry(const std::string & path);
    
    //------------------------------------------------------------------------
    //!  Construct from a path and a parent.
    //------------------------------------------------------------------------
    DirectoryEntry(const std::string & path, const DirectoryEntry *parent);

    //------------------------------------------------------------------------
    //!  Returns the DirectoryEntry's type.
    //------------------------------------------------------------------------
    TypeEnum Type() const;
    
    //------------------------------------------------------------------------
    //!  Returns the DirectoryEntry's path.
    //------------------------------------------------------------------------
    const std::string & Path() const;

    std::string RelativePath() const;

    //------------------------------------------------------------------------
    //!  Returns the final component of the DirectoryEntry's path.
    //------------------------------------------------------------------------
    std::string BaseName() const;

    //------------------------------------------------------------------------
    //!  Returns the name of the directory in which DirectoryEntry resides.
    //------------------------------------------------------------------------
    std::string DirName() const;
    
    //------------------------------------------------------------------------
    //!  If the DirectoryEntry exists in the filesystem, returns its
    //!  path with all symlinks, extra '/' characters and references to
    //!  '/./' and '/../' resolved.  Else returns an empty string.
    //------------------------------------------------------------------------
    std::string RealPath() const;

    //------------------------------------------------------------------------
    //!  If the DirectoryEntry has dots ('.') followed by other characters
    //!  in its BaseName(), returns the string of characters beyond the last
    //!  dot ('.').  Else returns an empty string.
    //------------------------------------------------------------------------
    std::string Extension() const;

    //------------------------------------------------------------------------
    //!  If the DirectoryEntry has a parent, returns a pointer to it.
    //!  Else returns (DirectoryEntry *)0.
    //------------------------------------------------------------------------
    const DirectoryEntry *Parent() const;
    
    //------------------------------------------------------------------------
    //!  Returns true if the DirectoryEntry exists in the filesystem.
    //------------------------------------------------------------------------
    bool Exists() const;

    //------------------------------------------------------------------------
    //!  Returns true if the DirectoryEntry is a directory.
    //------------------------------------------------------------------------
    bool IsDirectory() const;
    
    //------------------------------------------------------------------------
    //!  If the DirectoryEntry is a directory and has children, fills
    //!  @c children with its direct children and returns true.  Else
    //!  clears @c children and returns false.
    //------------------------------------------------------------------------
    bool GetChildren(std::vector<DirectoryEntry> & children,
                     uint8_t typeMask = 0xFF) const;
    
    bool GetChildren(std::vector<DirectoryEntry> & children,
                     DirectoryEntryFunctor & dirEntryFunc) const;

    uint32_t Recurse(DirectoryEntryFunctor & dirEntryFunc, 
                     bool depthFirst = false,
                     int maxDepth = -1);

  protected:
    static std::mutex      _readdirMtx;
    std::string            _path;
    const DirectoryEntry  *_parent;
    TypeEnum               _type;

    static TypeEnum GetType(const struct stat & statStruct);
    static TypeEnum GetType(const struct dirent & dirEntry);

    uint32_t Recurse(DirectoryEntry & de,
                     DirectoryEntryFunctor & dirEntryFunc,
                     int depth, bool depthFirst, int maxDepth);
  };

}  // namespace Dwm

#endif  // _DWMDIRECTORYENTRY_HH_


//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
