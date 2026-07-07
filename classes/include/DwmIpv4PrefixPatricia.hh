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
//!  \file DwmIpv4PrefixPatricia.hh
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
//!
//!  6/11/2026 - some performance numbers using a ValueType of std::string,
//!  using the 901,114 prefixes in ../tests/IPV4_prefixes.20210123:
//!
//!    - roughly 10.6 million lookups/second on a Mac Studio M1 Ultra.
//!    - roughly 8.7 million lookups/second on an AMD Threadripper 3960X.
//!    - roughly 7.2 million lookups/second on a Xeon E3-1270 V2 @ 3.50GHz.
//!    - roughly 5.1 million lookups/second on an i5-2405S @ 2.50GHz.
//!    - roughly 4.7 million lookups/second on a Xeon L5640 @ 2.27GHz.
//!    - roughly 2.54 million lookups/second on a Raspberry Pi 4B.
//---------------------------------------------------------------------------

#ifndef _DWMIPV4PREFIXPATRICIA_HH_
#define _DWMIPV4PREFIXPATRICIA_HH_

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

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
  //!  either original) and has no value of its own.
  //!
  //!  @tparam ValueType  The type of the value associated with each prefix.
  //--------------------------------------------------------------------------
  template <typename ValueType>
  class Ipv4PrefixPatricia
  {
    //----------------------------------------------------------------------
    //!  Internal trie node.
    //!
    //!  Each node stores a pair of (prefix, value), a flag indicating
    //!  whether a value is present, and two child pointers for the
    //!  0-bit and 1-bit branches.  The prefix stored at a node is
    //!  always at least as long as the prefix of its parent (or
    //!  strictly longer for value-holding leaf nodes).
    //!
    //!  The pair uses const Key to match std::map semantics, preventing
    //!  modification of the key through iterator access.
    //----------------------------------------------------------------------
    struct Node
    {
      std::pair<const Ipv4Prefix, ValueType>  _pair;
      bool                                    _hasValue;
      Node                                   *_child[2];

      //----------------------------------------------------------------------
      Node(const Ipv4Prefix & p, const ValueType & v, bool hv = true)
          : _pair{p, v}, _hasValue(hv), _child{nullptr, nullptr}
      {}

      //----------------------------------------------------------------------
      friend std::ostream & operator << (std::ostream & os, const Node & node)
      {
        if (node._hasValue) {
          os << node._pair.first << ' ' << node._pair.second << '\n';
        }
        if (node._child[0]) {
          os << *(node._child[0]);
        }
        if (node._child[1]) {
          os << *(node._child[1]);
        }
        return os;
      }
    };

  public:
    // Type aliases matching std::map convention
    using key_type        = Ipv4Prefix;
    using mapped_type     = ValueType;
    using value_type      = std::pair<const Ipv4Prefix, ValueType>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    // Forward declarations for iterator types
    class iterator;
    class const_iterator;

    //----------------------------------------------------------------------
    //!  Default constructor.  Creates an empty trie.
    //----------------------------------------------------------------------
    Ipv4PrefixPatricia()
        : _root(nullptr), _size(0)
    {}

    //----------------------------------------------------------------------
    //!  Destructor.  Deletes all nodes.
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
      const Node  *node = _root;
      std::optional<std::pair<Ipv4Prefix,ValueType>>  result;

      while (node) {
        if (node->_pair.first.Contains(prefix)) {
          if (node->_hasValue) {
            result = std::make_pair(node->_pair.first, node->_pair.second);
          }
          if (node->_pair.first.MaskLength() >= prefix.MaskLength()) {
            break;
          }
          uint8_t  b = prefix.Bit(node->_pair.first.MaskLength());
          node = node->_child[b];
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

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    iterator erase(iterator it)
    {
      iterator  rc = it;
      if (rc != end()) {
        ++rc;
        Remove(it._current->_pair.first);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    iterator erase(const_iterator it)
    {
      const_iterator  rc = it;
      if (rc != cend()) {
        ++rc;
        Remove(it._current->_pair.first);
      }
      return rc;
    }
    
    //----------------------------------------------------------------------
    //!  Find the node whose key exactly matches @c key.
    //!  Returns an iterator to the matching node, or end() if not found.
    //----------------------------------------------------------------------
    iterator find(const Ipv4Prefix & key)
    {
      Node  *node = _root;

      while (node) {
        if (node->_pair.first == key) {
          if (node->_hasValue) {
            return iterator(_root, node);
          }
          // Exact prefix match but no value: not a stored entry
          break;
        }
        if (node->_pair.first.Contains(key)) {
          uint8_t  b = key.Bit(node->_pair.first.MaskLength());
          node = node->_child[b];
        }
        else {
          break;
        }
      }
      return end();
    }

    //----------------------------------------------------------------------
    //!  Find the node whose key exactly matches @c key.
    //!  Returns a const_iterator to the matching node, or end() if not found.
    //----------------------------------------------------------------------
    const_iterator find(const Ipv4Prefix & key) const
    {
      const Node  *node = _root;

      while (node) {
        if (node->_pair.first == key) {
          if (node->_hasValue) {
            return const_iterator(_root, node);
          }
          // Exact prefix match but no value: not a stored entry
          break;
        }
        if (node->_pair.first.Contains(key)) {
          uint8_t  b = key.Bit(node->_pair.first.MaskLength());
          node = node->_child[b];
        }
        else {
          break;
        }
      }
      return end();
    }

    //----------------------------------------------------------------------
    //!  Returns the total number of nodes in the trie.
    //----------------------------------------------------------------------
    size_t Size() const
    { return _size; }

    //----------------------------------------------------------------------
    //!  Return an iterator to the first value-holding node.
    //!  Traversal visits nodes in pre-order (current, left, right),
    //!  producing prefixes sorted by network address.
    //----------------------------------------------------------------------
    iterator begin()
    { return iterator(_root); }

    //----------------------------------------------------------------------
    //!  Return the past-the-end iterator.
    //----------------------------------------------------------------------
    iterator end()
    { return iterator{}; }

    //----------------------------------------------------------------------
    //!  Return a const_iterator to the first value-holding node.
    //----------------------------------------------------------------------
    const_iterator begin() const
    { return const_iterator(_root); }

    //----------------------------------------------------------------------
    //!  Return the past-the-end const_iterator.
    //----------------------------------------------------------------------
    const_iterator end() const
    { return const_iterator{}; }

    //----------------------------------------------------------------------
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const   { return end(); }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const Ipv4PrefixPatricia & pat)
    {
      if (pat._root) {
        os << *(pat._root);
      }
      return os;
    }

    //----------------------------------------------------------------------
    //!  Forward iterator over value-holding nodes in pre-order traversal
    //!  (current, left, right), which produces sorted-by-address output
    //!  for a Patricia trie.  Models std::forward_iterator.  Dereferences
    //!  to a reference to std::pair<const Ipv4Prefix, ValueType>, matching
    //!  std::map semantics.
    //----------------------------------------------------------------------
    class iterator
    {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type        = std::pair<const Ipv4Prefix, ValueType>;
      using difference_type   = std::ptrdiff_t;
      using reference         = value_type &;
      using pointer           = value_type *;

      iterator() = default;

      //------------------------------------------------------------------
      reference operator * ()
      { return _current->_pair; }

      //------------------------------------------------------------------
      pointer operator -> ()
      { return &(_current->_pair); }

      //------------------------------------------------------------------
      iterator &operator++()
      {
        _advance();
        return *this;
      }

      //------------------------------------------------------------------
      iterator operator++(int)
      {
        iterator tmp = *this;
        _advance();
        return tmp;
      }

      //------------------------------------------------------------------
      bool operator==(const iterator & other) const
      { return _current == other._current; }

      //------------------------------------------------------------------
      bool operator!=(const iterator & other) const
      { return !(*this == other); }

    private:
      Node               *_current   = nullptr;
      Node               *_root      = nullptr;
      std::vector<Node *> _stack;

      friend class Ipv4PrefixPatricia;
      friend class const_iterator;

      explicit iterator(Node * root)
          : _root(root)
      {
        if (root) {
          _stack.push_back(root);
          _advance();
        }
      }

      //! Construct an iterator pointing directly to @c currentNode.
      //! Used internally by find().
      iterator(Node * root, Node * currentNode)
          : _current(currentNode), _root(root)
      {}

      //! Advance to the next value-holding node using pre-order traversal
      //! (current, left, right), which produces sorted-by-address output
      //! for a Patricia trie.
      void _advance()
      {
        // Push children of current node onto stack (right first, then left,
        // so that left is popped first)
        if (_current) {
          if (_current->_child[1]) {
            _stack.push_back(_current->_child[1]);
          }
          if (_current->_child[0]) {
            _stack.push_back(_current->_child[0]);
          }
        }
        _current = nullptr;

        // Pop until we find a value-holding node
        while (!_stack.empty()) {
          Node *node = _stack.back();
          _stack.pop_back();
          if (node->_hasValue) {
            _current = node;
            return;
          }
          // Expand non-value node: push children (right first, then left)
          if (node->_child[1]) {
            _stack.push_back(node->_child[1]);
          }
          if (node->_child[0]) {
            _stack.push_back(node->_child[0]);
          }
        }
      }
    };

    //----------------------------------------------------------------------
    //!  Const forward iterator over value-holding nodes in pre-order
    //!  traversal (current, left, right), which produces sorted-by-address
    //!  output for a Patricia trie.  Models std::forward_iterator.
    //!  Dereferences to a const reference to std::pair<const Ipv4Prefix,
    //!  ValueType>.
    //----------------------------------------------------------------------
    class const_iterator
    {
    public:
      using iterator_category = std::forward_iterator_tag;
      using value_type        = std::pair<const Ipv4Prefix, ValueType>;
      using difference_type   = std::ptrdiff_t;
      using reference         = const value_type &;
      using pointer           = const value_type *;

      const_iterator() = default;

      //! Allow construction from a mutable iterator.
      const_iterator(const iterator & it)
          : _current(it._current), _root(it._root)
      {
        // Copy the traversal stack, converting Node* to const Node*
        for (Node *p : it._stack) {
          _stack.push_back(p);
        }
      }

      //------------------------------------------------------------------
      reference operator*() const
      { return _current->_pair; }

      //------------------------------------------------------------------
      pointer operator->() const
      { return &(_current->_pair); }

      //------------------------------------------------------------------
      const_iterator &operator++()
      {
        _advance();
        return *this;
      }

      //------------------------------------------------------------------
      const_iterator operator++(int)
      {
        const_iterator tmp = *this;
        _advance();
        return tmp;
      }

      //------------------------------------------------------------------
      bool operator==(const const_iterator & other) const
      { return _current == other._current; }

      //------------------------------------------------------------------
      bool operator!=(const const_iterator & other) const
      { return !(*this == other); }

    private:
      const Node                *_current = nullptr;
      const Node                *_root    = nullptr;
      std::vector<const Node *>  _stack;

      friend class Ipv4PrefixPatricia;

      explicit const_iterator(const Node * root)
          : _root(root)
      {
        if (root) {
          _stack.push_back(root);
          _advance();
        }
      }

      //! Construct a const_iterator pointing directly to @c currentNode.
      //! Used internally by find().
      const_iterator(const Node * root, const Node * currentNode)
          : _current(currentNode), _root(root)
      {}

      //! Advance to the next value-holding node using pre-order traversal
      //! (current, left, right), which produces sorted-by-address output
      //! for a Patricia trie.
      void _advance()
      {
        // Push children of current node onto stack (right first, then left,
        // so that left is popped first)
        if (_current) {
          if (_current->_child[1]) {
            _stack.push_back(_current->_child[1]);
          }
          if (_current->_child[0]) {
            _stack.push_back(_current->_child[0]);
          }
        }
        _current = nullptr;

        // Pop until we find a value-holding node
        while (!_stack.empty()) {
          const Node *node = _stack.back();
          _stack.pop_back();
          if (node->_hasValue) {
            _current = node;
            return;
          }
          // Expand non-value node: push children (right first, then left)
          if (node->_child[1]) {
            _stack.push_back(node->_child[1]);
          }
          if (node->_child[0]) {
            _stack.push_back(node->_child[0]);
          }
        }
      }
    };

  private:
    Node *  _root;
    size_t  _size;

    //----------------------------------------------------------------------
    //!  Recursively delete all nodes under @c n and @c n itself.
    //----------------------------------------------------------------------
    void clear(Node *n)
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
    Node *copyNode(Node *n)
    {
      if (! n)  return nullptr;
      Node *c = new Node(n->_pair.first, n->_pair.second, n->_hasValue);
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
    Node *addNode(Node *node, const Ipv4Prefix & prefix,
                  const ValueType & value)
    {
      if (! node) {
        ++_size;
        return new Node(prefix, value);
      }

      int diffBit = firstDiffBit(node->_pair.first, prefix,
                                 std::min(node->_pair.first.MaskLength(),
                                          prefix.MaskLength()));

      if (diffBit < 0) {
        if (node->_pair.first == prefix) {
          node->_pair.second = value;
          node->_hasValue    = true;
          return node;
        }

        if (prefix.MaskLength() > node->_pair.first.MaskLength()) {
          uint8_t bit = prefix.Bit(node->_pair.first.MaskLength());
          node->_child[bit] = addNode(node->_child[bit], prefix, value);
          return node;
        }
        else {
          ++_size;
          Node     *parent = new Node(prefix, value);
          uint8_t   bit   = node->_pair.first.Bit(prefix.MaskLength());
          parent->_child[bit] = node;
          return parent;
        }
      }

      // Prefixes diverge at diffBit.  Create a branching node that
      // holds the common prefix (mask length = diffBit).  The
      // existing node and the new node become its two children.
      Ipv4Prefix common(Ipv4Address(node->_pair.first.NetworkRaw()), diffBit);
      ++_size;
      Node  *branch = new Node(common, ValueType{}, false);

      uint8_t bitForExisting = node->_pair.first.Bit(diffBit);
      uint8_t bitForNew      = prefix.Bit(diffBit);

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
    Node *removeNode(Node *node, const Ipv4Prefix & prefix, bool & removed)
    {
      if (! node) {
        return nullptr;
      }
      
      if ((node->_hasValue) && (node->_pair.first == prefix)) {
        removed         = true;
        node->_hasValue = false;

        if ((! node->_child[0]) && (! node->_child[1])) {
          delete node;
          --_size;
          return nullptr;
        }
        if ((! node->_child[0]) || (! node->_child[1])) {
          Node  *kid = node->_child[0] ? node->_child[0] : node->_child[1];
          delete node;
          --_size;
          return kid;
        }
        return node;
      }

      if (prefix.MaskLength() <= node->_pair.first.MaskLength()) {
        return node;
      }

      uint8_t   bit = prefix.Bit(node->_pair.first.MaskLength());
      Node     *child = node->_child[bit];

      if (child && child->_pair.first.Contains(prefix)) {
        node->_child[bit] = removeNode(child, prefix, removed);
      }

      if (removed) {
        if (! node->_hasValue) {
          if ((! node->_child[0]) && (! node->_child[1])) {
            delete node;
            --_size;
            return nullptr;
          }
          if ((! node->_child[0]) || (! node->_child[1])) {
            Node  *kid = node->_child[0] ? node->_child[0] : node->_child[1];
            delete node;
            --_size;
            return kid;
          }
        }
      }
      return node;
    }

    //----------------------------------------------------------------------
    //!  Return the bit index (0 = MSB) of the first bit where two
    //!  prefixes differ, considering only the first @c maxBits bits.
    //!  Returns -1 if they are identical within that range.
    //----------------------------------------------------------------------
    static int firstDiffBit(const Ipv4Prefix & a, const Ipv4Prefix & b,
                            uint8_t maxBits)
    {
      if (0 == maxBits) {
        return -1;
      }
      uint32_t  aH = ntohl(a.NetworkRaw());
      uint32_t  bH = ntohl(b.NetworkRaw());
      uint32_t  x  = aH ^ bH;
      if (0 == x) {
        return -1;
      }
      int  msbPos  = 31 - __builtin_clz(x);
      int  ipBit   = 31 - msbPos;
      return (ipBit < maxBits) ? ipBit : -1;
    }
  };

}  // namespace Dwm

#endif  // _DWMIPV4PREFIXPATRICIA_HH_
