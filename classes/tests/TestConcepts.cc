//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2025
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
//!  \file TestConcepts.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Concepts unit tests
//---------------------------------------------------------------------------

#include "DwmConcepts.hh"
#include "DwmUnitAssert.hh"

using namespace std;
using namespace Dwm;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestContainers()
{
  UnitAssert((Concepts::is_std_array<array<int,5>>));
  UnitAssert((Concepts::is_std_array<array<string,5>>));
  UnitAssert((Concepts::is_std_pair<pair<int,string>>));
  UnitAssert((Concepts::is_std_pair<pair<string,int>>));
  UnitAssert((Concepts::is_std_list<list<int>>));
  UnitAssert((Concepts::is_std_list<list<string>>));
  UnitAssert((Concepts::is_std_deque<deque<int>>));
  UnitAssert((Concepts::is_std_deque<deque<string>>));
  UnitAssert((Concepts::is_std_map<map<int,string>>));
  UnitAssert((Concepts::is_std_map<map<string,int>>));
  UnitAssert((Concepts::is_std_multimap<multimap<int,string>>));
  UnitAssert((Concepts::is_std_multimap<multimap<string,int>>));
  UnitAssert((Concepts::is_std_set<set<int>>));
  UnitAssert((Concepts::is_std_set<set<string>>));
  UnitAssert((Concepts::is_std_multiset<multiset<int>>));
  UnitAssert((Concepts::is_std_multiset<multiset<string>>));
  UnitAssert((Concepts::is_std_tuple<tuple<int,string,bool>>));
  UnitAssert((Concepts::is_std_tuple<tuple<string,bool,int>>));
  UnitAssert((Concepts::is_std_unordered_map<unordered_map<int,string>>));
  UnitAssert((Concepts::is_std_unordered_map<unordered_map<string,int>>));
  UnitAssert((Concepts::is_std_unordered_multimap<unordered_multimap<int,string>>));
  UnitAssert((Concepts::is_std_unordered_multimap<unordered_multimap<string,int>>));
  UnitAssert((Concepts::is_std_unordered_set<unordered_set<int,string>>));
  UnitAssert((Concepts::is_std_unordered_set<unordered_set<string,int>>));
  UnitAssert((Concepts::is_std_unordered_multiset<unordered_multiset<int,string>>));
  UnitAssert((Concepts::is_std_unordered_multiset<unordered_multiset<string,int>>));
  UnitAssert((Concepts::is_std_variant<variant<int,string,bool>>));
  UnitAssert((Concepts::is_std_variant<variant<string,bool,int>>));
  UnitAssert((Concepts::is_std_vector<vector<int>>));
  UnitAssert((Concepts::is_std_vector<vector<string>>));

  UnitAssert((Concepts::is_std_sequence_container<array<int,5>>));
  UnitAssert((Concepts::is_std_sequence_container<deque<int>>));
  UnitAssert((Concepts::is_std_sequence_container<list<int>>));
  UnitAssert((Concepts::is_std_sequence_container<vector<int>>));
  
  UnitAssert((Concepts::is_std_associative_container<set<int>>));
  UnitAssert((Concepts::is_std_associative_container<multiset<int>>));
  UnitAssert((Concepts::is_std_associative_container<unordered_set<int>>));
  UnitAssert((Concepts::is_std_associative_container<unordered_multiset<int>>));

  UnitAssert((Concepts::is_std_pair_associative_container<map<string,int>>));
  UnitAssert((Concepts::is_std_pair_associative_container<multimap<string,int>>));
  UnitAssert((Concepts::is_std_pair_associative_container<unordered_map<string,int>>));
  UnitAssert((Concepts::is_std_pair_associative_container<unordered_multimap<string,int>>));

  UnitAssert((! Concepts::is_std_sequence_container<map<int,string>>));
  UnitAssert((! Concepts::is_std_sequence_container<multimap<int,string>>));
  UnitAssert((! Concepts::is_std_sequence_container<set<int>>));
  UnitAssert((! Concepts::is_std_sequence_container<multiset<int>>));
  UnitAssert((! Concepts::is_std_sequence_container<unordered_map<int,string>>));
  UnitAssert((! Concepts::is_std_sequence_container<unordered_multimap<int,string>>));
  UnitAssert((! Concepts::is_std_sequence_container<unordered_set<int>>));
  UnitAssert((! Concepts::is_std_sequence_container<unordered_multiset<int>>));

  UnitAssert((! Concepts::is_std_associative_container<array<int,5>>));
  UnitAssert((! Concepts::is_std_associative_container<deque<int>>));
  UnitAssert((! Concepts::is_std_associative_container<list<int>>));
  UnitAssert((! Concepts::is_std_associative_container<vector<int>>));
  UnitAssert((! Concepts::is_std_associative_container<map<int,string>>));
  UnitAssert((! Concepts::is_std_associative_container<multimap<int,string>>));
  UnitAssert((! Concepts::is_std_associative_container<unordered_map<int,string>>));
  UnitAssert((! Concepts::is_std_associative_container<unordered_multimap<int,string>>));
  
  UnitAssert((! Concepts::is_std_pair_associative_container<array<int,5>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<deque<int>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<list<int>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<vector<int>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<set<int>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<multiset<int>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<unordered_set<int>>));
  UnitAssert((! Concepts::is_std_pair_associative_container<unordered_multiset<int>>));

  UnitAssert(! Concepts::is_std_sequence_container<vector<bool>>);

  UnitAssert(Concepts::is_std_atomic<std::atomic<int>>);
  UnitAssert(Concepts::is_std_atomic<std::atomic<uint64_t>>);
  UnitAssert(! Concepts::is_std_atomic<std::pair<char,char>>);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestAllMembersArithmetic()
{
  struct s1 {
    uint64_t  i;
    uint64_t  j;
  };
  UnitAssert(AllMembersArithmetic<s1>());
  struct s2 {
    bool  b;
    char  c;
  };
  UnitAssert(AllMembersArithmetic<s2>());
  struct s3 {
    short   s;
    int     i;
    float   f;
    double  d;
  };
  UnitAssert(AllMembersArithmetic<s3>());
  struct s4 {
    int     i;
    int    *ip;
  };
  UnitAssert(! AllMembersArithmetic<s4>());
  struct s5 {
    int          i;
    std::string  s;
  };
  UnitAssert(! AllMembersArithmetic<s5>());

  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void TestAllMembersArithmeticPacked()
{
  struct s1 {
    uint64_t  i;
    uint64_t  j;
  };
  UnitAssert(AllMembersArithmeticPacked<s1>());
  UnitAssert(std::is_trivially_copyable_v<s1>);

  struct s2 {
    uint32_t  i;
    uint64_t  j;
  };
  UnitAssert(! AllMembersArithmeticPacked<s2>());
  UnitAssert(std::is_trivially_copyable_v<s2>);

  struct s3 {
    uint32_t  i;
    uint64_t  j;
  } __attribute__((packed));
  UnitAssert(AllMembersArithmeticPacked<s3>());
  UnitAssert(std::is_trivially_copyable_v<s3>);

  struct s4 {
    std::string  s;
  } __attribute__((packed));
  UnitAssert(! AllMembersArithmeticPacked<s4>());
  UnitAssert(! std::is_trivially_copyable_v<s4>);

  struct s5 {
    int  *p;
  } __attribute__((packed));
  UnitAssert(! AllMembersArithmeticPacked<s5>());
  UnitAssert(std::is_trivially_copyable_v<s5>);
  
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  TestContainers();
  TestAllMembersArithmetic();
  TestAllMembersArithmeticPacked();
  
  if (Assertions::Total().Failed()) {
    Assertions::Print(cerr, true);
    exit(1);
  }
  else {
    cout << Assertions::Total() << " passed" << endl;
    exit(0);
  }
}
