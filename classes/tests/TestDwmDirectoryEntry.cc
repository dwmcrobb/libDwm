//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDirectoryEntry.cc 10621 $
// @(#) $Id: TestDwmDirectoryEntry.cc 10621 2020-05-02 21:51:49Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2011, 2023
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
//!  \file TestDwmDirectoryEntry.cc
//!  \brief Unit tests for Dwm::DirectoryEntry
//---------------------------------------------------------------------------

#include <cstdlib>
#include <memory>
#include <string>

#include "DwmSvnTag.hh"
#include "DwmUnitAssert.hh"
#include "DwmDirectoryEntry.hh"

using namespace std;
using Dwm::DirectoryEntry;

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestDwmDirectoryEntry.cc 10621 $");

struct TestPathResult {
  string  orig;
  string  path;
  string  name;
  string  extension;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
class DirectoryEntryTestFunc
  : public Dwm::DirectoryEntryFunctor
{
public:
  DirectoryEntryTestFunc(const string & path)
      : _path(path), _foundSelf(false)
  {}
  
  bool operator () (DirectoryEntry & de)
  {
    bool  rc = UnitAssert(de.Path().find(_path) == 0);
    if (de.BaseName() == "TestDwmDirectoryEntry") {
      _foundSelf = true;
    }
    return rc;
  }

  bool FoundSelf() const
  {
    return _foundSelf;
  }
  
  void Reset()
  {
    _foundSelf = false;
    return;
  }
  
private:
  string  _path;
  bool  _foundSelf;
};

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestPaths()
{
  static TestPathResult  expected[] = {
    { "/",                "/",                "/",          "" },
    { "//",               "/",                "/",          "" },
    { "///",              "/",                "/",          "" },
    { "//tmp/",           "/tmp",             "tmp",        "" },
    { "///tmp//",         "/tmp",             "tmp",        "" },
    { ".",                ".",                ".",          "" },
    { "..",               "..",               "..",         "" },
    { "../",              "..",               "..",         "" },
    { "../..",            "../..",            "..",         "" },
    { "/tmp",             "/tmp",             "tmp",        "" },
    { "/tmp/",            "/tmp",             "tmp",        "" },
    { "tmp/",             "tmp",              "tmp",        "" },
    { "/index.php",       "/index.php",       "index.php",  "php" },
    { "index.php/",       "index.php",        "index.php",  "php" },
    { "/index.php/",      "/index.php",       "index.php",  "php" },
    { "/tmp/foo",         "/tmp/foo",         "foo",        "" },
    { "/tmp/foo/",        "/tmp/foo",         "foo",        "" },
    { "tmp/foo/",         "tmp/foo",          "foo",        "" },
    { "/tmp/index.html",  "/tmp/index.html",  "index.html", "html" },
    { "tmp/index.html/",  "tmp/index.html",   "index.html", "html" },
    { "/tmp/index.html/", "/tmp/index.html",  "index.html", "html" },
    { "", "", "", "" }
  };
  
  TestPathResult  *tpr = expected;
  for ( ; ! tpr->orig.empty(); ++tpr) {
    DirectoryEntry  de(tpr->orig);
    UnitAssert(de.Path() == tpr->path);
    UnitAssert(de.BaseName() == tpr->name);
    UnitAssert(de.Extension() == tpr->extension);
    UnitAssert(de.Parent() == 0);
  }

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestExists()
{
  DirectoryEntry  deRoot("/");
  UnitAssert(deRoot.Exists());

  DirectoryEntry  deTmp("/tmp/");
  UnitAssert(deTmp.Exists());

  DirectoryEntry  deDot(".");
  UnitAssert(deDot.Exists());
  
  DirectoryEntry  deParent(".."); 
  UnitAssert(deParent.Exists());
 
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestDirNames()
{
  unique_ptr<DirectoryEntry> de = make_unique<DirectoryEntry>("/tmp");
  UnitAssert(de->DirName() == "/");
  de = make_unique<DirectoryEntry>("tmp/");
  UnitAssert(de->DirName() == ".");
  de = make_unique<DirectoryEntry>("/tmp/");
  UnitAssert(de->DirName() == "/");
  de = make_unique<DirectoryEntry>("/");
  UnitAssert(de->DirName() == "/");

  de = make_unique<DirectoryEntry>("../");
  UnitAssert(de->DirName() == ".");
  de = make_unique<DirectoryEntry>(".");
  UnitAssert(de->DirName() == ".");
  de = make_unique<DirectoryEntry>("../tmp");
  UnitAssert(de->DirName() == "..");

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIsDirectory()
{
  unique_ptr<DirectoryEntry>  de = make_unique<DirectoryEntry>("/");
  UnitAssert(de->IsDirectory());
  de = make_unique<DirectoryEntry>(".");
  UnitAssert(de->IsDirectory());
  
  de = make_unique<DirectoryEntry>("TestDwmDirectoryEntry");
  UnitAssert(! de->IsDirectory());

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGetChildren()
{
  unique_ptr<DirectoryEntry>  de = make_unique<DirectoryEntry>(".");
  vector<DirectoryEntry>  dirEntries;
  UnitAssert(de->GetChildren(dirEntries));
  vector<DirectoryEntry>::const_iterator  i = dirEntries.begin();
  for ( ; i != dirEntries.end(); ++i) {

    UnitAssert(i->Parent() == de.get());
    UnitAssert(i->Parent()->BaseName() == ".");

    if (i->Type() != DirectoryEntry::e_typeSymbolicLink) {
      UnitAssert(i->RealPath().find(i->Parent()->RealPath()) == 0);
    }
    UnitAssert(i->Path().find(i->Parent()->Path()) == 0);
    UnitAssert(i->Path().length() ==
               (i->Parent()->Path().length() + 1
                + i->BaseName().length()));
    

    if (i->IsDirectory()) {
      DirectoryEntry  subdir(i->Path(), de.get());
      vector<DirectoryEntry>  subdirEntries;
      if (subdir.GetChildren(subdirEntries)) {
        vector<DirectoryEntry>::const_iterator  si = subdirEntries.begin();
        for ( ; si != subdirEntries.end(); ++si) {
          UnitAssert(si->Parent() == &subdir);
          UnitAssert(si->Parent()->Parent() == de.get());

          UnitAssert(si->RealPath().find(si->Parent()->RealPath())
                     != string::npos);
          UnitAssert(si->RealPath().length() ==
                     (si->Parent()->RealPath().length() + 1
                      + si->BaseName().length()));
          
          UnitAssert(si->Path().find(si->Parent()->Path()) == 0);
          UnitAssert(si->Path().length() ==
                     (si->Parent()->Path().length() + 1
                      + si->BaseName().length()));

          UnitAssert(si->RealPath().find(si->Parent()->Parent()->RealPath())
                     == 0);
          UnitAssert(si->Path().find(si->Parent()->Parent()->Path()) == 0);
          
        }  //  END 'for ( ; si != subdirEntries.end(): ++si)'

      }  //  END 'if (subdir.GetChildren(subdirEntries)'

    }  //  END 'if (i->IsDirectory())'

  }  //  END 'for ( ; i != dirEntries.end(); ++i)'

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGetSubdirs()
{
  unique_ptr<DirectoryEntry>  de = make_unique<DirectoryEntry>("..");
  vector<DirectoryEntry>  dirEntries;
  UnitAssert(de->GetChildren(dirEntries, DirectoryEntry::e_typeDirectory));
  vector<DirectoryEntry>::const_iterator  i = dirEntries.begin();
  for ( ; i != dirEntries.end(); ++i) {
    UnitAssert(i->Parent() == de.get());
    UnitAssert(i->Parent()->BaseName() == "..");
    UnitAssert(i->Type() == DirectoryEntry::e_typeDirectory);
    UnitAssert(i->Parent()->Type() == DirectoryEntry::e_typeDirectory);
  }

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestGetRegularFiles()
{
  unique_ptr<DirectoryEntry>  de = make_unique<DirectoryEntry>(".");
  vector<DirectoryEntry>  dirEntries;
  UnitAssert(de->GetChildren(dirEntries, DirectoryEntry::e_typeRegular));
  vector<DirectoryEntry>::const_iterator  i = dirEntries.begin();
  for ( ; i != dirEntries.end(); ++i) {
    UnitAssert(i->Parent() == de.get());
    UnitAssert(i->Parent()->BaseName() == ".");
    UnitAssert(i->Type() == DirectoryEntry::e_typeRegular);
    UnitAssert(i->Parent()->Type() == DirectoryEntry::e_typeDirectory);
  }

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRecurse()
{
  unique_ptr<DirectoryEntry>  de = make_unique<DirectoryEntry>("..");
  DirectoryEntryTestFunc  detf(de->Path());
  de->Recurse(detf, true);
  UnitAssert(detf.FoundSelf());
  detf.Reset();
  de->Recurse(detf, false);
  UnitAssert(detf.FoundSelf());
  
  UnitAssert(de->Recurse(detf, true) == de->Recurse(detf, false));
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestRecurseDepth()
{
  unique_ptr<DirectoryEntry>  de = make_unique<DirectoryEntry>("..");
  DirectoryEntryTestFunc  detf(de->Path());

  UnitAssert(de->Recurse(detf, true, 0) == 1);
  UnitAssert(de->Recurse(detf, true, 1) > 7);
  UnitAssert(de->Recurse(detf, true, 2) > 150);

  UnitAssert(de->Recurse(detf, false, 0) == 1);
  UnitAssert(de->Recurse(detf, false, 1) > 7);
  UnitAssert(de->Recurse(detf, false, 2) > 150);
  
  UnitAssert(de->Recurse(detf, true, 1) == de->Recurse(detf, false, 1));
  UnitAssert(de->Recurse(detf, true, 2) == de->Recurse(detf, false, 2));
  
  UnitAssert(de->Recurse(detf, true, 2) > de->Recurse(detf, true, 1));

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  TestPaths();
  TestExists();
  TestDirNames();
  TestIsDirectory();
  TestGetChildren();
  TestGetSubdirs();
  TestGetRegularFiles();
  TestRecurse();
  TestRecurseDepth();
  
  if (Dwm::Assertions::Total().Failed() > 0) {
    Dwm::Assertions::Print(std::cerr, true);
    exit(1);
  }
  else
    std::cout << Dwm::Assertions::Total() << " passed" << std::endl;

  exit(0);
}
