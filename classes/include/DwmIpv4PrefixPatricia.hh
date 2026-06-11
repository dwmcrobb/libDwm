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
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
//  PURPOSE, OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY 
//  PATENT, TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmIpv4PrefixPatricia2.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv4PrefixPatricia class template definition
//!
//!  A memory-efficient Patricia trie (compressed binary trie) for
//!  longest-prefix matching on Ipv4Prefix keys.  Path compression
//!  eliminates internal nodes with degree 1, combining shared prefixes
//!  into single edge labels.  Node splitting occurs during insertion
//!  when a new prefix diverges from an existing prefix at a bit position
//!  within the shorter of their mask lengths, creating a branching node
//!  that captures the shared common prefix.
//---------------------------------------------------------------------------

#ifndef _DWMIPV4PREFIXPATRICIA_HH_
#define _DWMIPV4PREFIXPATRICIA_HH_

#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>

#include "DwmIpv4Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A Patricia trie (Practical Algorithm To Retrieve Information Coded
  //!  In Alphanumeric) for Ipv4Prefix keys, with path compression.
  //!
  //!  Path compression (also called "radix tree" compression) ensures that
  //!  internal nodes with exactly one child are eliminated.  The edge
  //!  between a parent and its descendant can therefore span multiple bits
  //!  -- those bits are encoded in the descendant's prefix rather than
  //!  requiring intermediate nodes.
  //!
  //!  Node splitting during insertion:
  //!  When a new prefix shares some initial bits with an existing node's
  //!  prefix but diverges at a later bit position (within the shorter
  //!  of the two mask lengths), a new branching node is created.  This
  //!  branching node stores the common prefix (which is shorter than
  //!  either original) and has no value of its own.  The existing node
  //!  and the new node become its two children, arranged according to
  //!  the value of the diverging bit.
  //!
  //!  @tparam ValueType  The type of the value associated with each prefix.
  //--------------------------------------------------------------------------
  template <typename ValueType>
  class Ipv4PrefixPatricia
  {
  public:
    //----------------------------------------------------------------------
    //!  Default constructor.  Creates an empty trie.
    //----------------------------------------------------------------------
    Ipv4PrefixPatricia()
        : _root(nullptr), _size(0)
    {}

    //----------------------------------------------------------------------
    //!  Destructor.  Frees all nodes.
    //----------------------------------------------------------------------
    ~Ipv4PrefixPatricia()
    { clear(_root); }

    //----------------------------------------------------------------------
    //!  Copy constructor.
    //----------------------------------------------------------------------
    Ipv4PrefixPatricia(const Ipv4PrefixPatricia & other)
        : _root(copyNode(other._root)), _size(other._size)
    {}

    //----------------------------------------------------------------------
    //!  Copy assignment operator.
    //----------------------------------------------------------------------
    Ipv4PrefixPatricia & operator = (const Ipv4PrefixPatricia & other)
    {
      if (this != &other) {
        clear(_root);
        _root = copyNode(other._root);
        _size = other._size;
      }
      return *this;
    }

    //----------------------------------------------------------------------
    //!  Move constructor.
    //----------------------------------------------------------------------
    Ipv4PrefixPatricia(Ipv4PrefixPatricia && other) noexcept
        : _root(other._root), _size(other._size)
    {
      other._root = nullptr;
      other._size = 0;
    }

    //----------------------------------------------------------------------
    //!  Move assignment operator.
    //----------------------------------------------------------------------
    Ipv4PrefixPatricia & operator = (Ipv4PrefixPatricia && other) noexcept
    {
      if (this != &other) {
        clear(_root);
        _root       = other._root;
        _size       = other._size;
        other._root = nullptr;
        other._size = 0;
      }
      return *this;
    }

    //----------------------------------------------------------------------
    //!  Add a prefix with its associated value to the trie.
    //!
    //!  If the prefix already exists, its value is updated.
    //!
    //!  @param prefix  The Ipv4Prefix key to insert.
    //!  @param value   The value to associate with the prefix.
    //----------------------------------------------------------------------
    void Add(const Ipv4Prefix & prefix, const ValueType & value)
    { _root = addNode(_root, prefix, value); }

    //----------------------------------------------------------------------
    //!  Find the longest stored prefix that matches the given prefix.
    //!
    //!  Traverses the trie by following the bits of @c prefix.  At each
    //!  node the stored prefix is verified against the query (necessary
    //!  because path compression can cause a descent into a node whose
    //!  full stored prefix does not actually match the query).  The
    //!  deepest (longest mask length) matching node with a value is
    //!  returned.
    //!
    //!  @param prefix  The Ipv4Prefix to search for.
    //!  @return  std::optional containing the longest matching prefix
    //!           and its value, or std::nullopt if no match exists.
    //----------------------------------------------------------------------
    std::optional<std::pair<Ipv4Prefix,ValueType>>
    LongestMatch(const Ipv4Prefix & prefix) const
    {
      Node * node = _root;
      std::optional<std::pair<Ipv4Prefix,ValueType>>  result;

      while (node) {
        if (prefixMatches(node->_prefix, prefix)) {
          if (node->_hasValue) {
            result = std::make_pair(node->_prefix, node->_value);
          }
          if (node->_prefix.MaskLength() >= prefix.MaskLength()) {
            break;
          }
          uint8_t  bit = bitOf(prefix, node->_prefix.MaskLength());
          node = node->_child[bit];
        }
        else {
          break;
        }
      }
      return result;
    }

    //----------------------------------------------------------------------
    //!  Remove a prefix from the trie.
    //!
    //!  After removal, nodes that no longer carry a value and have no
    //!  children are deleted (compaction).  Internal nodes that become
    //!  single-child nodes with no value are also bypassed, maintaining
    //!  the path-compressed structure.
    //!
    //!  @param prefix  The Ipv4Prefix to remove.
    //!  @return  true if the prefix was found and removed, false otherwise.
    //----------------------------------------------------------------------
    bool Remove(const Ipv4Prefix & prefix)
    {
      bool  removed = false;
      _root = removeNode(_root, prefix, removed);
      return removed;
    }

    //----------------------------------------------------------------------
    //!  Returns the total number of nodes in the trie.
    //----------------------------------------------------------------------
    size_t Size() const
    { return _size; }

  private:
    //----------------------------------------------------------------------
    //!  Internal trie node.
    //!
    //!  Each node stores a prefix (the key), an optional value, and
    //!  two child pointers for the 0-bit and 1-bit branches.  The
    //!  prefix stored at a node is always at least as long as the
    //!  prefix of its parent (or strictly longer for value-holding
    //!  leaf nodes).
    //----------------------------------------------------------------------
    struct Node
    {
      Ipv4Prefix  _prefix;
      ValueType   _value;
      bool        _hasValue;
      Node *      _child[2];

      Node(const Ipv4Prefix & p, const ValueType & v, bool hv = true)
          : _prefix(p), _value(v), _hasValue(hv), _child{nullptr, nullptr}
      {}
    };

    Node *  _root;
    size_t  _size;

    //----------------------------------------------------------------------
    //!  Recursively clear (delete) all nodes in the subtree.
    //----------------------------------------------------------------------
    void clear(Node * n)
    {
      if (n) {
        clear(n->_child[0]);
        clear(n->_child[1]);
        delete n;
        --_size;
      }
    }

    //----------------------------------------------------------------------
    //!  Recursively deep-copy a subtree.
    //----------------------------------------------------------------------
    Node * copyNode(Node * n)
    {
      if (! n)  return nullptr;
      Node * c = new Node(n->_prefix, n->_value, n->_hasValue);
      ++_size;
      c->_child[0] = copyNode(n->_child[0]);
      c->_child[1] = copyNode(n->_child[1]);
      return c;
    }

    //----------------------------------------------------------------------
    //!  Recursive insertion helper.
    //!
    //!  Cases handled:
    //!    1. Empty subtree -> create a new leaf.
    //!    2. Exact prefix match -> update the value.
    //!    3. New prefix is longer (extends an existing prefix) ->
    //!       recurse into the appropriate child.
    //!    4. New prefix is shorter (existing node extends beyond it) ->
    //!       create a new node above the existing one (path compression).
    //!    5. Prefixes diverge at some bit position within both mask
    //!       lengths -> split: create a branching node that holds the
    //!       common prefix, and place both the existing node and the
    //!       new node as its children (node splitting).
    //!----------------------------------------------------------------------
    Node *addNode(Node * node, const Ipv4Prefix & prefix,
                  const ValueType & value)
    {
      if (! node) {
        ++_size;
        return new Node(prefix, value);
      }

      int diffBit = firstDiffBit(node->_prefix, prefix,
                                 std::min(node->_prefix.MaskLength(),
                                          prefix.MaskLength()));

      if (diffBit < 0) {
        if (node->_prefix == prefix) {
          node->_value    = value;
          node->_hasValue = true;
          return node;
        }

        if (prefix.MaskLength() > node->_prefix.MaskLength()) {
          uint8_t bit = bitOf(prefix, node->_prefix.MaskLength());
          node->_child[bit] = addNode(node->_child[bit], prefix, value);
          return node;
        }
        else {
          ++_size;
          Node * parent = new Node(prefix, value);
          uint8_t bit   = bitOf(node->_prefix, prefix.MaskLength());
          parent->_child[bit] = node;
          return parent;
        }
      }

      // Prefixes diverge at diffBit.  Create a branching node that
      // holds the common prefix (mask length = diffBit).  The
      // existing node and the new node become its two children.
      Ipv4Prefix common(Ipv4Address(node->_prefix.NetworkRaw()), diffBit);
      ++_size;
      Node * branch = new Node(common, ValueType{}, false);

      uint8_t bitForExisting = bitOf(node->_prefix, diffBit);
      uint8_t bitForNew      = bitOf(prefix, diffBit);

      ++_size;
      branch->_child[bitForExisting] = node;
      branch->_child[bitForNew]      = new Node(prefix, value);
      return branch;
    }

    //----------------------------------------------------------------------
    //!  Recursive removal helper.
    //!
    //!  Returns the (possibly null) replacement node.  Post-removal
    //!  compaction: if a node has no value and zero children it is
    //!  deleted; if it has no value and exactly one child it is
    //!  bypassed (path compression maintained).
    //!----------------------------------------------------------------------
    Node * removeNode(Node * node, const Ipv4Prefix & prefix,
                      bool & removed)
    {
      if (! node)  return nullptr;

      if (node->_hasValue && node->_prefix == prefix) {
        removed        = true;
        node->_hasValue = false;

        if (! node->_child[0] && ! node->_child[1]) {
          delete node;
          --_size;
          return nullptr;
        }
        if (! node->_child[0] || ! node->_child[1]) {
          Node * kid = node->_child[0] ? node->_child[0] : node->_child[1];
          delete node;
          --_size;
          return kid;
        }
        return node;
      }

      if (prefix.MaskLength() <= node->_prefix.MaskLength()) {
        return node;
      }

      uint8_t bit = bitOf(prefix, node->_prefix.MaskLength());
      Node * child = node->_child[bit];

      if (child && prefixMatches(child->_prefix, prefix)) {
        node->_child[bit] = removeNode(child, prefix, removed);
      }

      if (removed) {
        if (! node->_hasValue) {
          if (! node->_child[0] && ! node->_child[1]) {
            delete node;
            --_size;
            return nullptr;
          }
          if (! node->_child[0] || ! node->_child[1]) {
            Node * kid = node->_child[0] ? node->_child[0] : node->_child[1];
            delete node;
            --_size;
            return kid;
          }
        }
      }
      return node;
    }

    //----------------------------------------------------------------------
    //!  Extract bit position @c i (0 = MSB of the address, 31 = LSB)
    //!  from an Ipv4Prefix.
    //----------------------------------------------------------------------
    static bool bitOf(const Ipv4Prefix & p, uint8_t i)
    {
      return (ntohl(p.NetworkRaw()) >> (31 - i)) & 1;
    }

    //----------------------------------------------------------------------
    //!  Return the bit index (0 = MSB) of the first bit where two
    //!  prefixes differ, considering only the first @c maxBits bits.
    //!  Returns -1 if they are identical within that range.
    //----------------------------------------------------------------------
    static int firstDiffBit(const Ipv4Prefix & a, const Ipv4Prefix & b,
                            uint8_t maxBits)
    {
      if (maxBits == 0)  return -1;
      uint32_t aH = ntohl(a.NetworkRaw());
      uint32_t bH = ntohl(b.NetworkRaw());
      uint32_t x  = aH ^ bH;
      if (x == 0)  return -1;
      int msbPos  = 31 - __builtin_clz(x);
      int ipBit   = 31 - msbPos;
      return (ipBit < maxBits) ? ipBit : -1;
    }

    //----------------------------------------------------------------------
    //!  Return true if @c prefix is a prefix of @c query, i.e. all
    //!  bits of @c prefix (up to its mask length) match the
    //!  corresponding bits of @c query.
    //----------------------------------------------------------------------
    static bool prefixMatches(const Ipv4Prefix & prefix,
                              const Ipv4Prefix & query)
    {
      uint8_t  mLen = prefix.MaskLength();
      if (mLen == 0)  return true;
      uint32_t pRaw = ntohl(prefix.NetworkRaw());
      uint32_t qRaw = ntohl(query.NetworkRaw());
      uint32_t mask;
      if (mLen >= 32)  mask = 0xFFFFFFFF;
      else             mask = ~((1u << (32 - mLen)) - 1);
      return ((pRaw & mask) == (qRaw & mask));
    }
  };

}  // namespace Dwm

#endif  // _DWMIPV4PREFIXPATRICIA_HH_
