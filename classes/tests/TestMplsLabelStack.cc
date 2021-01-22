//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/tests/TestMplsLabelStack.cc 8389 $
// @(#) $Id: TestMplsLabelStack.cc 8389 2016-04-17 04:31:36Z dwm $
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2007
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
//!  \file TestMplsLabel.cc
//!  \brief Unit tests for Dwm::MplsLabelStack
//---------------------------------------------------------------------------

extern "C" {
  #include <fcntl.h>   // for open()
  #include <unistd.h>  // for unlink(), getpid()
}

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "DwmSvnTag.hh"
#include "DwmMplsLabelStack.hh"
#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"

static const Dwm::SvnTag svntag("@(#) $DwmPath: dwm/libDwm/trunk/tests/TestMplsLabelStack.cc 8389 $");

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void FillLabels(vector<MplsLabel> & labels)
{
  if (! labels.empty())
    labels.clear();

  MplsLabel  label;
  label.Label(0xF0F0F);
  label.Exp(0x05);
  label.S(false);
  label.Ttl(170);
  labels.push_back(label);
  
  label.Label(0xF0F0);
  label.Exp(0x02);
  label.S(false);
  label.Ttl(127);
  labels.push_back(label);
  
  label.Label(3);
  label.Exp(0x03);
  label.S(true);
  label.Ttl(3);
  labels.push_back(label);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestStreamIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  vector<MplsLabel>  labels;
  FillLabels(labels);
  
  MplsLabelStack  labelStack;
  vector<MplsLabel>::const_iterator  i;
  for (i = labels.begin(); i != labels.end(); ++i)
    labelStack.Labels().push_back(*i);
  
  ofstream    ofs(os.str().c_str());
  assert(ofs);
  UnitAssert(labelStack.Write(ofs));
  ofs.close();
  
  UnitAssert(labelStack.Labels() == labels);
  
  MplsLabelStack  labelStack2;
  ifstream    ifs(os.str().c_str());
  assert(ifs);
  UnitAssert(labelStack2.Read(ifs));
  ifs.close();
  UnitAssert(labelStack2 == labelStack);
  
  unlink(os.str().c_str());

  labelStack2 = labelStack;
  UnitAssert(labelStack2 == labelStack);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestDescriptorIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  vector<MplsLabel>  labels;
  FillLabels(labels);
  
  MplsLabelStack  labelStack;
  vector<MplsLabel>::const_iterator  i;
  for (i = labels.begin(); i != labels.end(); ++i)
    labelStack.Labels().push_back(*i);
  
  int  fd = open(os.str().c_str(), O_CREAT|O_TRUNC|O_WRONLY, 0644);
  assert(fd >= 0);
  UnitAssert(labelStack.Write(fd));
  close(fd);
  
  UnitAssert(labelStack.Labels() == labels);
  
  MplsLabelStack  labelStack2;
  fd = open(os.str().c_str(), O_RDONLY);
  assert(fd >= 0);
  UnitAssert(labelStack2.Read(fd));
  close(fd);
  UnitAssert(labelStack2 == labelStack);
  
  unlink(os.str().c_str());

  labelStack2 = labelStack;
  UnitAssert(labelStack2 == labelStack);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestFileIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  vector<MplsLabel>  labels;
  FillLabels(labels);
  
  MplsLabelStack  labelStack;
  vector<MplsLabel>::const_iterator  i;
  for (i = labels.begin(); i != labels.end(); ++i)
    labelStack.Labels().push_back(*i);
  
  FILE  *f = fopen(os.str().c_str(), "w");
  assert(f);
  UnitAssert(labelStack.Write(f));
  fclose(f);
  
  UnitAssert(labelStack.Labels() == labels);
  
  MplsLabelStack  labelStack2;
  f = fopen(os.str().c_str(), "r");
  assert(f);
  UnitAssert(labelStack2.Read(f));
  fclose(f);
  UnitAssert(labelStack2 == labelStack);
  
  unlink(os.str().c_str());

  labelStack2 = labelStack;
  UnitAssert(labelStack2 == labelStack);
  
  return;
}

void TestGZIO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  vector<MplsLabel>  labels;
  FillLabels(labels);
  
  MplsLabelStack  labelStack;
  vector<MplsLabel>::const_iterator  i;
  for (i = labels.begin(); i != labels.end(); ++i)
    labelStack.Labels().push_back(*i);
  
  gzFile  gzf = gzopen(os.str().c_str(), "w");
  assert(gzf);
  UnitAssert(labelStack.Write(gzf));
  gzclose(gzf);
  
  UnitAssert(labelStack.Labels() == labels);
  
  MplsLabelStack  labelStack2;
  gzf = gzopen(os.str().c_str(), "r");
  assert(gzf);
  UnitAssert(labelStack2.Read(gzf));
  gzclose(gzf);
  UnitAssert(labelStack2 == labelStack);
  
  unlink(os.str().c_str());

  labelStack2 = labelStack;
  UnitAssert(labelStack2 == labelStack);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void TestBZ2IO()
{
  ostringstream  os;
  os << "TestMplsLabel." << getpid();
  
  vector<MplsLabel>  labels;
  FillLabels(labels);
  
  MplsLabelStack  labelStack;
  vector<MplsLabel>::const_iterator  i;
  for (i = labels.begin(); i != labels.end(); ++i)
    labelStack.Labels().push_back(*i);
  
  BZFILE  *bzf = BZ2_bzopen(os.str().c_str(), "w");
  assert(bzf);
  UnitAssert(labelStack.BZWrite(bzf));
  BZ2_bzclose(bzf);
  
  UnitAssert(labelStack.Labels() == labels);
  
  MplsLabelStack  labelStack2;
  bzf = BZ2_bzopen(os.str().c_str(), "r");
  assert(bzf);
  UnitAssert(labelStack2.BZRead(bzf));
  BZ2_bzclose(bzf);
  UnitAssert(labelStack2 == labelStack);
  
  unlink(os.str().c_str());

  labelStack2 = labelStack;
  UnitAssert(labelStack2 == labelStack);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestIO()
{
  TestStreamIO();
  TestDescriptorIO();
  TestFileIO();
  TestGZIO();
  TestBZ2IO();
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  SysLogger::Open("TestMplsLabelStack", LOG_PERROR, LOG_USER);
  SysLogger::ShowPriorities(true);
  SysLogger::ShowFileLocation(true);
  SysLogger::MinimumPriority(LOG_INFO);
  
  TestIO();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
  }
  
  exit(0);
}
