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
//!  \file DwmIpv6PrefixPatricia.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Ipv6PrefixPatricia class template definition
//!
//!  A memory-efficient Patricia trie (compressed binary trie) for
//!  longest-prefix matching on Ipv6Prefix keys.  Path compression
//!  eliminates internal nodes with degree 1, combining shared prefixes
//!  into single edge labels.  Node splitting occurs during insertion
//!  when a new prefix diverges from an existing prefix at a bit position
//!  within the shorter of their mask lengths, creating a branching node
//!  that captures the shared common prefix.
//!
//!  6/11/2026 - some performance numbers using a ValueType of std::string,
//!  using the 901,114 prefixes in ../tests/IPV4_prefixes.20210123:
//!
//!    - roughly 10 million lookups/second on a Mac Studio M1 Ultra.
//!    - roughly 8.7 million lookups/second on an AMD Threadripper 3960X.
//!    - roughly 7.2 million lookups/second on a Xeon E3-1270 V2 @ 3.50GHz.
//!    - roughly 5.1 million lookups/second on an i5-2405S @ 2.50GHz.
//!    - roughly 4.7 million lookups/second on a Xeon L5640 @ 2.27GHz.
//!    - roughly 2.54 million lookups/second on a Raspberry Pi 4B.
//---------------------------------------------------------------------------

#ifndef _DWMIPV6PREFIXPATRICIA_HH_
#define _DWMIPV6PREFIXPATRICIA_HH_

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

#include "DwmIpv6Prefix.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  A Patricia trie (Practical Algorithm To Retrieve Information Coded
  //!  In Alphanumeric) for Ipv6Prefix keys, with path compression.
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
  class Ipv6PrefixPatricia
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
      std::pair<const Ipv6Prefix, ValueType>   _pair;
      std::array<Node *, 2>                    _children;
      Node                                    *_parent;
      bool                                     _hasValue;

      //----------------------------------------------------------------------
      //!  Construct from the given @c prefix, @c value, @c hasValue and
      //!  @c parent.
      //----------------------------------------------------------------------
      Node(const Ipv6Prefix & prefix, const ValueType & value,
           bool hasValue = true, Node *parent = nullptr)
          : _pair{prefix, value}, _hasValue(hasValue),
            _children{nullptr, nullptr}, _parent(parent)
      {}

      //----------------------------------------------------------------------
      //!  ostream output operator
      //----------------------------------------------------------------------
      friend std::ostream & operator << (std::ostream & os, const Node & node)
      {
        if (node._hasValue) {
          os << node._pair.first << ' ' << node._pair.second << '\n';
        }
        if (node._children[0]) {
          os << *(node._children[0]);
        }
        if (node._children[1]) {
          os << *(node._children[1]);
        }
        return os;
      }
    };

  public:
    //------------------------------------------------------------------------
    //!  Type aliases matching std::map convention.
    //------------------------------------------------------------------------
    using key_type        = Ipv6Prefix;
    using mapped_type     = ValueType;
    using value_type      = std::pair<const Ipv6Prefix, ValueType>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;

    //------------------------------------------------------------------------
    //!  Forward declarations for iterator types.
    //------------------------------------------------------------------------
    class iterator;
    class const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    //----------------------------------------------------------------------
    //!  Default constructor.  Creates an empty trie.
    //----------------------------------------------------------------------
    Ipv6PrefixPatricia()
        : _root(nullptr), _size(0)
    {}

    //----------------------------------------------------------------------
    //!  Destructor.  Deletes all nodes.
    //----------------------------------------------------------------------
    ~Ipv6PrefixPatricia()
    { clear(_root); }

    //----------------------------------------------------------------------
    //!  Copy constructor.
    //----------------------------------------------------------------------
    Ipv6PrefixPatricia(const Ipv6PrefixPatricia & other)
        : _root(copyNode(other._root)), _size(other._size)
    {}

    //----------------------------------------------------------------------
    //!  Copy assignment operator.
    //----------------------------------------------------------------------
    Ipv6PrefixPatricia & operator = (const Ipv6PrefixPatricia & other)
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
    Ipv6PrefixPatricia(Ipv6PrefixPatricia && other) noexcept
        : _root(other._root), _size(other._size)
    {
      other._root = nullptr;
      other._size = 0;
    }

    //----------------------------------------------------------------------
    //!  Move assignment operator.
    //----------------------------------------------------------------------
    Ipv6PrefixPatricia & operator = (Ipv6PrefixPatricia && other) noexcept
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
    //!  Insert a new element into the trie.  This is modeled after standard
    //!  associative container insertion.
    //!
    //!  If the prefix already exists in the trie, no insertion takes place
    //!  and the node remains unmodified.
    //!
    //!  @param value  The pair of prefix and value to insert.
    //!  @return  A pair containing an iterator to the element and a boolean
    //!           indicating whether an insertion took place.
    //----------------------------------------------------------------------
    std::pair<iterator, bool> insert(const value_type & value)
    {
      Node  *result = nullptr;
      bool   inserted = false;
      _root = addNode(_root, value.first, value.second, result, inserted,
                      false, nullptr);
      return { iterator(_root, result), inserted };
    }

    //----------------------------------------------------------------------
    //!  Access the value associated with @c key.  If the key does not exist,
    //!  it is inserted with a default-constructed value.
    //!
    //!  @param key  The Ipv6Prefix to look up.
    //!  @return  A reference to the value associated with the key.
    //----------------------------------------------------------------------
    ValueType & operator [] (const Ipv6Prefix & key)
    {
      Node  *result = nullptr;
      bool   inserted = false;
      _root = addNode(_root, key, ValueType{}, result, inserted, false,
                      nullptr);
      return result->_pair.second;
    }

    //----------------------------------------------------------------------
    //!  Remove a prefix from the trie.
    //!
    //!  After removal, nodes that no longer carry a value and have no
    //!  children are deleted (compaction).  Internal nodes that become
    //!  single-child nodes with no value are also bypassed, maintaining
    //!  the path-compressed structure.
    //!
    //!  @param prefix  The Ipv6Prefix to remove.
    //!  @return  true if the prefix was found and removed, false otherwise.
    //----------------------------------------------------------------------
    bool Remove(const Ipv6Prefix & prefix)
    {
      bool  removed = false;
      _root = removeNode(_root, prefix, removed);
      if (_root) {
        _root->_parent = nullptr;
      }
      return removed;
    }

    //------------------------------------------------------------------------
    //!  Erases the node at @c it and returns an iterator referencing the
    //!  next node in the trie.
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
    //!  Erases the node at @c it and returns an iterator referencing the
    //!  next node in the trie.
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

    //------------------------------------------------------------------------
    //!  Erases the node for prefix @c pfx in the trie and returns 1 if a node
    //!  was removed, 0 if @c pfx was not found in the trie.
    //------------------------------------------------------------------------
    size_type erase(const Ipv6Prefix & pfx)
    {
      return (Remove(pfx) ? 1 : 0);
    }
    
    //----------------------------------------------------------------------
    //!  Find the node whose key exactly matches @c key.
    //!  Returns an iterator to the matching node, or end() if not found.
    //----------------------------------------------------------------------
    iterator find(const Ipv6Prefix & key)
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
          node = node->_children[b];
        }
        else {
          break;
        }
      }
      return end();
    }

    //----------------------------------------------------------------------
    //!  Find the node whose prefix exactly matches @c pfx.  Returns a
    //!  const_iterator to the matching node, or end() if not found.
    //----------------------------------------------------------------------
    const_iterator find(const Ipv6Prefix & pfx) const
    {
      const Node  *node = _root;

      while (node) {
        if (node->_pair.first == pfx) {
          if (node->_hasValue) {
            return const_iterator(_root, node);
          }
          // Exact prefix match but no value: not a stored entry
          break;
        }
        if (node->_pair.first.Contains(pfx)) {
          uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
          node = node->_children[b];
        }
        else {
          break;
        }
      }
      return end();
    }

    //------------------------------------------------------------------------
    //!  Finds the node with the longest match to the given prefix @c pfx.
    //!  Returns end() if no match is found.
    //------------------------------------------------------------------------
    iterator find_longest(const Ipv6Prefix & pfx)
    {
      auto      *node = _root;
      iterator   it = end();
      while (node) {
        if (node->_pair.first.Contains(pfx)) {
          if (node->_hasValue) {
            it = iterator(_root, node);
          }
          if (node->_pair.first.MaskLength() >= pfx.MaskLength()) {
            break;
          }
          uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
          node = node->_children[b];
        }
        else {
          break;
        }
      }
      return it;
    }

    //------------------------------------------------------------------------
    //!  Finds the node with the longest match to the given address @c addr.
    //!  Returns end() if no match is found.
    //------------------------------------------------------------------------
    iterator find_longest(const Ipv6Address & addr)
    { return find_longest(Ipv6Prefix(addr, 128)); }
      
    //------------------------------------------------------------------------
    //!  Finds the node with the longest match to the given prefix @c pfx.
    //!  Returns end() if no match is found.
    //------------------------------------------------------------------------
    const_iterator find_longest(const Ipv6Prefix & pfx) const
    { return const_iterator(find_longest(pfx)); }

    //------------------------------------------------------------------------
    //!  Finds the node with the longest match to the given address @c addr.
    //!  Returns end() if no match is found.
    //------------------------------------------------------------------------
    const_iterator find_longest(const Ipv6Address & addr) const
    { return find_longest(Ipv6Prefix(addr, 128)); }
    
    //------------------------------------------------------------------------
    //!  Finds all nodes that match prefix @c pfx, placing them in @c matches.
    //!  Returns true if matches were found, else returns false.
    //------------------------------------------------------------------------
    bool find_matches(const Ipv6Prefix & pfx,
                      std::vector<value_type> & matches) const
    {
      matches.clear();
      auto      *node = _root;
      while (node) {
        if (node->_pair.first.Contains(pfx)) {
          if (node->_hasValue) {
            matches.push_back(node->_pair);
          }
          if (node->_pair.first.MaskLength() >= pfx.MaskLength()) {
            break;
          }
          uint8_t  b = pfx.Bit(node->_pair.first.MaskLength());
          node = node->_children[b];
        }
        else {
          break;
        }
      }
      return (! matches.empty());
    }

    //------------------------------------------------------------------------
    //!  Finds all nodes that match address @c addr, placing them in
    //!  @c matches.  Returns true if matches were found, else returns false.
    //------------------------------------------------------------------------
    bool find_matches(const Ipv6Address & addr,
                      std::vector<value_type> & matches) const
    { return find_matches(Ipv6Prefix(addr, 128)); }
    
    //----------------------------------------------------------------------
    //!  Returns the total number of nodes in the trie.
    //----------------------------------------------------------------------
    size_t size() const
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
    { return iterator(_root, nullptr); }

    //----------------------------------------------------------------------
    //!  Return a const_iterator to the first value-holding node.
    //----------------------------------------------------------------------
    const_iterator begin() const
    { return const_iterator(_root); }

    //----------------------------------------------------------------------
    //!  Return the past-the-end const_iterator.
    //----------------------------------------------------------------------
    const_iterator end() const
    { return const_iterator(_root, nullptr); }

    //----------------------------------------------------------------------
    //!  Returns a const_iterator for the first value-holding node.
    //------------------------------------------------------------------------
    const_iterator cbegin() const { return begin(); }

    //------------------------------------------------------------------------
    //!  Return the past-the-end const_iterator.
    //------------------------------------------------------------------------
    const_iterator cend() const   { return end(); }

    //------------------------------------------------------------------------
    //!  Returns a reverse_iterator to the first value-holding element of the
    //!  reversed trie.
    //------------------------------------------------------------------------
    reverse_iterator rbegin() { return reverse_iterator(end()); }

    //------------------------------------------------------------------------
    //!  Returns a reverse_iterator to the element following the last
    //!  value-holding element of the reversed trie.
    //------------------------------------------------------------------------
    reverse_iterator rend() { return reverse_iterator(begin()); }

    //------------------------------------------------------------------------
    //!  Returns a const_reverse_iterator to the first value-holding element
    //!  of the reversed trie.
    //------------------------------------------------------------------------
    const_reverse_iterator rbegin() const
    { return const_reverse_iterator(end()); }
    
    //------------------------------------------------------------------------
    //!  Returns a const_reverse_iterator to the element following the last
    //!  value-holding element of the reversed trie.
    //------------------------------------------------------------------------
    const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }
    
    //------------------------------------------------------------------------
    //!  Returns a const_reverse_iterator to the first value-holding element
    //!  of the reversed trie.
    //------------------------------------------------------------------------
    const_reverse_iterator crbegin() const
    { return const_reverse_iterator(cend()); }
    
    //------------------------------------------------------------------------
    //!  Returns a const_reverse_iterator to the element following the last
    //!  value-holding element of the reversed trie.
    //------------------------------------------------------------------------
    const_reverse_iterator crend() const
    { return const_reverse_iterator(cbegin()); }

    //------------------------------------------------------------------------
    //!  ostream output operator
    //------------------------------------------------------------------------
    friend std::ostream &
    operator << (std::ostream & os, const Ipv6PrefixPatricia & pat)
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
    //!  to a reference to std::pair<const Ipv6Prefix, ValueType>, matching
    //!  std::map semantics.
    //----------------------------------------------------------------------
    class iterator
    {
    public:
      using iterator_category = std::bidirectional_iterator_tag;
      using value_type        = std::pair<const Ipv6Prefix, ValueType>;
      using difference_type   = std::ptrdiff_t;
      using reference         = value_type &;
      using pointer           = value_type *;

      iterator() = default;

      //------------------------------------------------------------------
      reference operator * () const
      { return _current->_pair; }

      //------------------------------------------------------------------
      pointer operator -> () const
      { return &(_current->_pair); }

      //------------------------------------------------------------------
      iterator & operator ++ ()
      {
        _current = _findSuccessor(_current);
        return *this;
      }

      //------------------------------------------------------------------
      iterator operator ++ (int)
      {
        iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      //------------------------------------------------------------------
      iterator & operator -- ()
      {
        if (_current == nullptr) {
          _current = _findLastValueNode(_root);
        } else {
          _current = _findPredecessor(_current);
        }
        return *this;
      }

      //------------------------------------------------------------------
      iterator operator -- (int)
      {
        iterator tmp = *this;
        --(*this);
        return tmp;
      }

      //------------------------------------------------------------------
      bool operator == (const iterator & other) const
      { return _current == other._current; }

      //------------------------------------------------------------------
      bool operator != (const iterator & other) const
      { return !(*this == other); }

    private:
      Node               *_current   = nullptr;
      Node               *_root      = nullptr;

      friend class Ipv6PrefixPatricia;
      friend class const_iterator;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      explicit iterator(Node *root)
          : _root(root)
      {
        if (root) {
          _current = _findFirstValueNode(root);
        }
      }

      //----------------------------------------------------------------------
      //! Construct an iterator pointing directly to @c currentNode.
      //! Used internally by find().
      //----------------------------------------------------------------------
      iterator(Node *root, Node *currentNode)
          : _current(currentNode), _root(root)
      {}

      //----------------------------------------------------------------------
      //! Find the first value-holding node in the subtree rooted at @c n.
      //----------------------------------------------------------------------
      Node *_findFirstValueNode(Node *n)
      {
        if (! n) {
          return nullptr;
        }
        if (n->_hasValue) {
          return n;
        }
        if (n->_children[0]) {
          Node  *res = _findFirstValueNode(n->_children[0]);
          if (res) {
            return res;
          }
        }
        if (n->_children[1]) {
          Node  *res = _findFirstValueNode(n->_children[1]);
          if (res) {
            return res;
          }
        }
        return nullptr;
      }

      //----------------------------------------------------------------------
      //! Find the pre-order successor of the given node.
      //----------------------------------------------------------------------
      Node *_findSuccessor(Node *n)
      {
        if (! n) {
          return nullptr;
        }
        if (n->_children[0]) {
          Node  *res = _findFirstValueNode(n->_children[0]);
          if (res) {
            return res;
          }
        }
        if (n->_children[1]) {
          Node  *res = _findFirstValueNode(n->_children[1]);
          if (res) {
            return res;
          }
        }
        Node  *curr = n;
        while (curr->_parent) {
          Node  *p = curr->_parent;
          if (p->_children[0] == curr) {
            if (p->_children[1]) {
              Node  *res = _findFirstValueNode(p->_children[1]);
              if (res) {
                return res;
              }
            }
          }
          curr = p;
        }
        return nullptr;
      }

      //----------------------------------------------------------------------
      //! Find the pre-order predecessor of the given node.
      //----------------------------------------------------------------------
      Node *_findPredecessor(Node *n)
      {
        if (! n) {
          return nullptr;
        }
        Node  *p = n->_parent;
        if (! p) {
          return nullptr;
        }

        if (p->_children[1] == n) {
          Node  *left = p->_children[0];
          if (left) {
            return _findLastValueNode(left);
          }
          if (p->_hasValue) return p;
          return _findPredecessor(p);
        }
        else {
          if (p->_hasValue) return p;
          return _findPredecessor(p);
        }
      }

      //----------------------------------------------------------------------
      //! Find the rightmost value-holding node in the subtree rooted at @c n.
      //----------------------------------------------------------------------
      Node *_findLastValueNode(Node *n)
      {
        if (! n) {
          return nullptr;
        }
        Node  *last = _findLastValueNode(n->_children[1]);
        if (last) {
          return last;
        }
        last = _findLastValueNode(n->_children[0]);
        if (last) {
          return last;
        }
        if (n->_hasValue) {
          return n;
        }
        return nullptr;
      }

    };

    //------------------------------------------------------------------------
    //!  Forward const_iterator over value-holding nodes in pre-order
    //!  traversal (current, left, right), which produces sorted-by-address
    //!  output for a Patricia trie.  Models std::forward_iterator.
    //!  Dereferences to a reference to std::pair<const Ipv6Prefix, ValueType>,
    //!  matching std::map semantics.
    //------------------------------------------------------------------------
    class const_iterator
    {
    public:
      using iterator_category = std::bidirectional_iterator_tag;
      using value_type        = std::pair<const Ipv6Prefix, ValueType>;
      using difference_type   = std::ptrdiff_t;
      using reference         = const value_type &;
      using pointer           = const value_type *;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const_iterator() = default;

      //----------------------------------------------------------------------
      //! Allow construction from a mutable iterator.
      //----------------------------------------------------------------------
      const_iterator(const iterator & it)
          : _current(it._current), _root(it._root)
      {}

      //------------------------------------------------------------------
      reference operator * () const
      { return _current->_pair; }

      //------------------------------------------------------------------
      pointer operator -> () const
      { return &(_current->_pair); }

      //------------------------------------------------------------------
      const_iterator & operator ++ ()
      {
        _current = _findSuccessor(_current);
        return *this;
      }

      //------------------------------------------------------------------
      const_iterator operator ++ (int)
      {
        const_iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      //------------------------------------------------------------------
      const_iterator & operator -- ()
      {
        if (_current == nullptr) {
          _current = _findLastValueNode(_root);
        } else {
          _current = _findPredecessor(_current);
        }
        return *this;
      }

      //------------------------------------------------------------------
      const_iterator operator -- (int)
      {
        const_iterator tmp = *this;
        --(*this);
        return tmp;
      }

      //------------------------------------------------------------------
      bool operator == (const const_iterator & other) const
      { return _current == other._current; }

      //------------------------------------------------------------------
      bool operator != (const const_iterator & other) const
      { return !(*this == other); }

    private:
      const Node                *_current = nullptr;
      const Node                *_root    = nullptr;

      friend class Ipv6PrefixPatricia;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      explicit const_iterator(const Node *root)
          : _root(root)
      {
        if (root) {
          _current = _findFirstValueNode(root);
        }
      }

      //----------------------------------------------------------------------
      //! Construct a const_iterator pointing directly to @c currentNode.
      //! Used internally by find().
      //----------------------------------------------------------------------
      const_iterator(const Node *root, const Node *currentNode)
          : _current(currentNode), _root(root)
      {}

      //----------------------------------------------------------------------
      //! Find the first value-holding node in the subtree rooted at @c n.
      //----------------------------------------------------------------------
      const Node *_findFirstValueNode(const Node *n)
      {
        if (! n) {
          return nullptr;
        }
        if (n->_hasValue) {
          return n;
        }
        if (n->_children[0]) {
          const Node  *res = _findFirstValueNode(n->_children[0]);
          if (res) {
            return res;
          }
        }
        if (n->_children[1]) {
          const Node *res = _findFirstValueNode(n->_children[1]);
          if (res) {
            return res;
          }
        }
        return nullptr;
      }

      //----------------------------------------------------------------------
      //! Find the pre-order successor of the given node.
      //----------------------------------------------------------------------
      const Node *_findSuccessor(const Node *n)
      {
        if (! n) {
          return nullptr;
        }
        if (n->_children[0]) {
          const Node  *res = _findFirstValueNode(n->_children[0]);
          if (res) {
            return res;
          }
        }
        if (n->_children[1]) {
          const Node *res = _findFirstValueNode(n->_children[1]);
          if (res) {
            return res;
          }
        }
        const Node  *curr = n;
        while (curr->_parent) {
          const Node  *p = curr->_parent;
          if (p->_children[0] == curr) {
            if (p->_children[1]) {
              const Node  *res = _findFirstValueNode(p->_children[1]);
              if (res) {
                return res;
              }
            }
          }
          curr = p;
        }
        return nullptr;
      }

      //----------------------------------------------------------------------
      //! Find the pre-order predecessor of the given node.
      //----------------------------------------------------------------------
      const Node *_findPredecessor(const Node *n)
      {
        if (! n) {
          return nullptr;
        }
        const Node * p = n->_parent;
        if (! p) {
          return nullptr;
        }

        if (p->_children[1] == n) {
          const Node  *left = p->_children[0];
          if (left) {
            return _findLastValueNode(left);
          }
          if (p->_hasValue) {
            return p;
          }
          return _findPredecessor(p);
        }
        else {
          if (p->_hasValue) {
            return p;
          }
          return _findPredecessor(p);
        }
      }

      //----------------------------------------------------------------------
      //! Find the rightmost value-holding node in the subtree rooted at @c n.
      //----------------------------------------------------------------------
      const Node *_findLastValueNode(const Node *n)
      {
        if (! n) {
          return nullptr;
        }
        const Node  *last = _findLastValueNode(n->_children[1]);
        if (last) {
          return last;
        }
        last = _findLastValueNode(n->_children[0]);
        if (last) {
          return last;
        }
        if (n->_hasValue) {
          return n;
        }
        return nullptr;
      }

    };

  private:
    Node    *_root;
    size_t   _size;

    //----------------------------------------------------------------------
    //!  Recursively delete all nodes under @c n and @c n itself.
    //----------------------------------------------------------------------
    void clear(Node *n)
    {
      if (n) {
        clear(n->_children[0]);
        clear(n->_children[1]);
        if (n->_hasValue) {
          --_size;
        }
        delete n;
      }
    }

    //----------------------------------------------------------------------
    //!  Recursively deep-copy a subtree.
    //----------------------------------------------------------------------
    Node *copyNode(Node *n)
    {
      if (! n) {
        return nullptr;
      }
      Node  *c = new Node(n->_pair.first, n->_pair.second, n->_hasValue);
      if (n->_hasValue) {
        ++_size;
      }
      c->_children[0] = copyNode(n->_children[0]);
      c->_children[1] = copyNode(n->_children[1]);
      return c;
    }

    //----------------------------------------------------------------------
    //!  Recursive insertion helper.
    //!
    //!  Cases handled:
    //!    1. Empty subtree -> create a new leaf.
    //!    2. Exact prefix match -> update the value (if updateExisting is
    //!       true).
    //!    3. New prefix is longer (extends an existing prefix) ->
    //!       recurse into the appropriate child.
    //!    4. New prefix is shorter (existing node extends beyond it) ->
    //!       create a new node above the existing one (path compression).
    //!    5. Prefixes diverge at some bit position within both mask
    //!       lengths -> split: create a branching node that holds the
    //!       common prefix, and place both the existing node and the
    //!       new node as its children (node splitting).
    //!----------------------------------------------------------------------
    Node *addNode(Node *node, const Ipv6Prefix & prefix,
                  const ValueType & value,
                  Node * & resultNode, bool & inserted, bool updateExisting,
                  Node *parent = nullptr)
    {
      if (! node) {
        inserted = true;
        Node * newNode = new Node(prefix, value, true, parent);
        resultNode = newNode;
        ++_size;
        return newNode;
      }

      int diffBit = firstDiffBit(node->_pair.first, prefix,
                                 std::min(node->_pair.first.MaskLength(),
                                          prefix.MaskLength()));

      if (diffBit < 0) {
        if (node->_pair.first == prefix) {
          if (node->_hasValue) {
            inserted = false;
            resultNode = node;
            if (updateExisting) {
              node->_pair.second = value;
            }
            return node;
          }
          else {
            // Node exists but has no value (branching node)
            inserted = true;
            node->_pair.second = value;
            node->_hasValue = true;
            resultNode = node;
            ++_size;
            return node;
          }
        }

        if (prefix.MaskLength() > node->_pair.first.MaskLength()) {
          uint8_t bit = prefix.Bit(node->_pair.first.MaskLength());
          node->_children[bit] = addNode(node->_children[bit], prefix, value,
                                      resultNode, inserted, updateExisting,
                                      node);
          return node;
        }
        else {
          inserted = true;
          Node     *parentNode = new Node(prefix, value, true, parent);
          resultNode = parentNode;
          uint8_t   bit   = node->_pair.first.Bit(prefix.MaskLength());
          parentNode->_children[bit] = node;
          node->_parent = parentNode;
          ++_size;
          return parentNode;
        }
      }

      // Prefixes diverge at diffBit.  Create a branching node that
      // holds the common prefix (mask length = diffBit).  The
      // existing node and the new node become its two children.
      Ipv6Prefix common(node->_pair.first.Network(), diffBit);
      Node  *branch = new Node(common, ValueType{}, false, parent);

      uint8_t bitForExisting = node->_pair.first.Bit(diffBit);
      uint8_t bitForNew      = prefix.Bit(diffBit);

      inserted = true;
      Node * newLeaf = new Node(prefix, value, true, branch);
      resultNode = newLeaf;
      ++_size;
      branch->_children[bitForExisting] = node;
      node->_parent = branch;
      branch->_children[bitForNew] = newLeaf;
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
    Node *removeNode(Node *node, const Ipv6Prefix & prefix, bool & removed)
    {
      if (! node) {
        return nullptr;
      }
      
      if ((node->_hasValue) && (node->_pair.first == prefix)) {
        removed = true;
        node->_hasValue = false;
        --_size;

        if ((! node->_children[0]) && (! node->_children[1])) {
          delete node;
          return nullptr;
        }
        if ((! node->_children[0]) || (! node->_children[1])) {
          Node  *kid =
            node->_children[0] ? node->_children[0] : node->_children[1];
          delete node;
          return kid;
        }
        return node;
      }

      if (prefix.MaskLength() <= node->_pair.first.MaskLength()) {
        return node;
      }

      uint8_t   bit = prefix.Bit(node->_pair.first.MaskLength());
      Node     *child = node->_children[bit];

      if (child && child->_pair.first.Contains(prefix)) {
        node->_children[bit] = removeNode(child, prefix, removed);
        if (node->_children[bit]) {
          node->_children[bit]->_parent = node;
        }
      }

      if (removed) {
        if (! node->_hasValue) {
          if ((! node->_children[0]) && (! node->_children[1])) {
            delete node;
            return nullptr;
          }
          if ((! node->_children[0]) || (! node->_children[1])) {
            Node  *kid = node->_children[0]
              ? node->_children[0] : node->_children[1];
            delete node;
            return kid;
          }
        }
      }
      return node;
    }

#if 1
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static int firstDiffBit(const Ipv6Prefix & a, const Ipv6Prefix & b,
                            uint8_t maxBits)
    {
      if (0 == maxBits) {
        return -1;
      }
      const uint64_t  *ap = (const uint64_t *)(a.In6Addr().s6_addr);
      uint64_t  AH = be64toh(*ap++);
      const uint64_t  *bp = (const uint64_t *)(b.In6Addr().s6_addr);
      uint64_t  BH = be64toh(*bp++);
      uint64_t  XH = AH ^ BH;
      int  ipBit = std::countl_zero(XH);
      if (ipBit < 64) {
        return (ipBit < maxBits) ? ipBit : -1;
      }
      AH = be64toh(*ap);
      XH = AH ^ (be64toh(*bp));
      ipBit = std::countl_zero(XH);
      if (ipBit < 64) {
        return ((ipBit+64) < maxBits) ? ipBit + 64 : -1;
      }
      return -1;
    }
#else
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static int firstDiffBit(const Ipv6Prefix & a, const Ipv6Prefix & b,
                            uint8_t maxBits)
    {
      if (0 == maxBits) {
        return -1;
      }
      const uint64_t  *p = (const uint64_t *)(a.In6Addr().s6_addr);
      unsigned __int128  AH = (unsigned __int128)be64toh(*p++) << 64;
      AH = AH | (unsigned __int128)be64toh(*p);

      p = (const uint64_t *)(b.In6Addr().s6_addr);
      unsigned __int128  BH = (unsigned __int128)be64toh(*p++) << 64;
      BH = BH | (unsigned __int128)be64toh(*p);

      unsigned __int128  x = AH ^ BH;
      if (0 == x) {
        return -1;
      }
      int  ipBit = __builtin_clzg(x);
      return (ipBit < maxBits) ? ipBit : -1;
    }
#endif
    
#if 0    
    //----------------------------------------------------------------------
    //!  Return the bit index (0 = MSB) of the first bit where two
    //!  prefixes differ, considering only the first @c maxBits bits.
    //!  Returns -1 if they are identical within that range.
    //----------------------------------------------------------------------
    static int firstDiffBit(const Ipv6Prefix & a, const Ipv6Prefix & b,
                            uint8_t maxBits)
    {
      for (uint8_t i = 0; i < maxBits; ++i) {
        if (a.Bit(i) != b.Bit(i)) {
          return i;
        }
      }
      return -1;
    }
#endif
  };

}  // namespace Dwm

#endif  // _DWMIPV6PREFIXPATRICIA_HH_
