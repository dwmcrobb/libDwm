//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2006-2007, 2020
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
//!  \file DwmPatricia.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _PATRICIA_HH_
#define _PATRICIA_HH_

extern "C" {
  #include <stdlib.h>
}

#include <algorithm>
#include <iostream>
#include <vector>

#include "DwmIOUtils.hh"
#include "DwmDescriptorIO.hh"
#include "DwmStreamIO.hh"

namespace Dwm {

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename KeyType>
  inline int GetBit(const KeyType & key, int bit)
  {
    if (bit < 0)
      return(2);
    if ((unsigned int)bit < sizeof(KeyType) * 8) {
      const uint8_t *p = (const uint8_t *)(&key);
      return(((p[bit >> 3]) >> (7 - (bit & 0x7))) & 0x1);
    }
    return(0);
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  template <typename KeyType>
  inline int BitFirstDifferent(const KeyType & key1, const KeyType & key2)
  {
    unsigned int n = 0;
    int d = 0;
    const uint8_t  *p1 = (const uint8_t *)&key1;
    const uint8_t  *p2 = (const uint8_t *)&key2;
    for ( ; n < sizeof(KeyType); ++n) {
      if (p1[n] != p2[n])
        break;
    }
    if (n == sizeof(KeyType)) {
      return(sizeof(KeyType) * 8);
    }
    else {
      while (GetBit<uint8_t>(p1[n], d) == GetBit<uint8_t>(p2[n], d) && d < 8)
        d++;
      return ((n << 3) + d);
    }
  }

  template <typename KeyType>
  inline bool IsMatch(const KeyType & key1, const KeyType & key2)
  {
    return(key2 == key1);
  }
  
  //--------------------------------------------------------------------------
  //!  Pre-declare the Patricia class template.
  //--------------------------------------------------------------------------
  template <typename KeyType, typename ValueType, 
            typename KeyEqual> class Patricia;
  
  //------------------------------------------------------------------------
  //!  A node in a Patricia trie.
  //!  Each node stores one key, and the data associated with that key.
  //------------------------------------------------------------------------
  template <typename KeyType, typename ValueType>
  class PatriciaNode
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    PatriciaNode()
        :  _bitIndex(-1), _key(), _data()
    {
      _left  = this;
      _right = this;
    }

    PatriciaNode(const PatriciaNode & pn)
    {
      _key = pn._key;
      _data = pn._data;
      _bitIndex = pn._bitIndex;
      if (pn._left != & pn)
        _left = new PatriciaNode<KeyType,ValueType>(pn._left);
      else
        _left = this;
      if (pn.right != &pn)
        _right = new PatriciaNode<KeyType,ValueType>(pn._right);
      else
        _right = this;
    }

    PatriciaNode<KeyType,ValueType> & operator = (const PatriciaNode<KeyType,ValueType> & pn)
    {
      if (&pn != this) {
        _key = pn._key;
        _data = pn._data;
        _bitIndex = pn._bitIndex;
        if (pn._left != & pn)
          _left = new PatriciaNode<KeyType,ValueType>(pn._left);
        else
          _left = this;
        if (pn.right != &pn)
          _right = new PatriciaNode<KeyType,ValueType>(pn._right);
        else
          _right = this;
      }
      return(*this);
    }
    
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    PatriciaNode(const KeyType & key, const ValueType & data, int bitIdx, 
                 PatriciaNode<KeyType,ValueType> *left,
                 PatriciaNode<KeyType,ValueType> *right)
        : _bitIndex(bitIdx), _key(key), _data(data), _left(left), _right(right)
    {
    }

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~PatriciaNode()
    {
    }
    
    //------------------------------------------------------------------------
    //!  Initialize this node with the given data.
    //------------------------------------------------------------------------
    void Initialize(const KeyType & key,
                    const ValueType & data,
                    int bitIdx,
                    PatriciaNode<KeyType,ValueType> *left,
                    PatriciaNode<KeyType,ValueType> *right)
    {
      _key      = key;
      _data     = data;
      _bitIndex  = bitIdx;
      _left     = left;
      _right    = right;
    }
    
    //------------------------------------------------------------------------
    //!  Returns the data field.
    //------------------------------------------------------------------------
    ValueType & GetData()
    {
      return(_data);
    }

    //------------------------------------------------------------------------
    //!  Sets the data field.
    //------------------------------------------------------------------------
    bool SetData(const ValueType & data)
    {
      _data = data;
      return(true);
    }
    
    //------------------------------------------------------------------------
    //!  Returns the key field.
    //------------------------------------------------------------------------
    KeyType GetKey() const
    {
      return(_key);
    }
    
    //------------------------------------------------------------------------
    //!  Returns the left branch.
    //------------------------------------------------------------------------
    PatriciaNode<KeyType,ValueType> * GetLeft()
    {
      return(_left);
    }

    //------------------------------------------------------------------------
    //!  Returns the right branch.
    //------------------------------------------------------------------------
    PatriciaNode<KeyType,ValueType> * GetRight()
    {
      return(_right);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const
    {
      return(IOUtils::StreamedLength(_key) + IOUtils::StreamedLength(_data));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool operator == (const PatriciaNode<KeyType,ValueType> & n) const
    {
      return((_key == n._key) && (_data == n._data));
    }
    
  private:
    friend class Patricia<KeyType, ValueType, std::equal_to<KeyType> >;
    int                         _bitIndex;
    KeyType                      _key;
    ValueType                    _data;
    PatriciaNode<KeyType,ValueType>  *_left;
    PatriciaNode<KeyType,ValueType>  *_right;
  };

  
  class __PatriciaIterator__
  {};
  
  //------------------------------------------------------------------------
  //!  Implements a Patricia trie structure with keys of
  //!  type KeyType.
  //------------------------------------------------------------------------
  template <typename KeyType, typename ValueType, 
            typename KeyEqual = std::equal_to<KeyType> >
  class Patricia 
    : public DescriptorIOCapable, public StreamIOCapable,
      public StreamedLengthCapable
  {
  public:
    //------------------------------------------------------------------------
    //!  Constructor
    //------------------------------------------------------------------------
    Patricia() 
    {
      // Create the head of the structure. The head is never moved
      // around in the trie (i.e. it always stays at the top of the structure).
      // This prevents further complications having to do with node removal.
      _head = new PatriciaNode<KeyType,ValueType>();
      _size = 0;
    }

    //------------------------------------------------------------------------
    //!  Destructor
    //------------------------------------------------------------------------
    ~Patricia()
    {
      RecursiveRemove(_head);
      delete(_head);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint32_t Size() const
    {
      return(_size);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Clear()
    {
      RecursiveRemove(_head);
      _head->_left = _head;
      _head->_right = _head;
    }
    
    //------------------------------------------------------------------------
    //!  Inserts a new (key, data) pair in the Patricia trie, and
    //!  returns the new node.
    //------------------------------------------------------------------------
    PatriciaNode<KeyType,ValueType> *
    Insert(const KeyType & key, const ValueType & data)
    {
      // Start at the root of the trie
      PatriciaNode<KeyType,ValueType>  *prev = _head;
      PatriciaNode<KeyType,ValueType>  *last = _head->_right;
     
      // Navigate down the trie and look for the key
      while (prev->_bitIndex < last->_bitIndex) {
        prev = last;
        last = (bit_get(key, last->_bitIndex) ? last->_right : last->_left);
      }
     
      if (key_compare(key, last->_key))
        return(last);  // key is already in the trie
     
      // Find the first bit that does not match.
      int i = BitFirstDifferent<KeyType>(key, last->_key);

      // Find the place in the trie to insert the new node
      prev = _head;
      last = prev->_right;
      while ((prev->_bitIndex < last->_bitIndex) && (last->_bitIndex < i)) {
        prev = last;
        last = (bit_get(key, last->_bitIndex) ? last->_right : last->_left);
      }
     
      // create new node
      PatriciaNode<KeyType,ValueType>  *node = 
        new PatriciaNode<KeyType,ValueType>();
      node->Initialize(key, data, i, 
                       (bit_get(key, i) ? last : node),
                       (bit_get(key, i) ? node : last));

      // Rewire the parent node's branch
      if (bit_get(key, prev->_bitIndex))
        prev->_right = node;
      else
        prev->_left = node;
     
      _size += 1;

      return(node);
    }
    
    //------------------------------------------------------------------------
    //!  Searches for the given key.  If the key is found, Copies the 
    //!  associated value into \c value and returns true.  If the key
    //!  is not found, returns false.
    //------------------------------------------------------------------------
    bool Lookup(const KeyType & key, ValueType & value) const
    {
      bool  rc = false;
      PatriciaNode<KeyType,ValueType>  *node = LookupNode(key);
      if (node) {
        value = node->_data;
        rc = true;
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  Search for the given key, and returns the node that
    //!  contains it (or NULL).  XXX - does this work properly?
    //------------------------------------------------------------------------
    PatriciaNode<KeyType,ValueType> * LookupNode(const KeyType & key) const
    {
      // Traverse the Patricia (downward) until we find an upward link.
      // If the node at the end of thu upward link has a key that exactly
      // matches k, return the node.  Else return 0.
      PatriciaNode<KeyType,ValueType>  *prev = _head;
      PatriciaNode<KeyType,ValueType>  *last = _head->_right;
     
      while (prev->_bitIndex < last->_bitIndex) {
        prev = last;
        last = (bit_get(key, last->_bitIndex) ? last->_right : last->_left);
      }
     
      if (key_compare(key, last->_key))
        return(last);
      else
        return(0);
    }

    PatriciaNode<KeyType,ValueType> *
    LongestMatchNode(const KeyType & key) const
    {
      // Traverse the Patricia (downward) until we find an upward link.
      // If the node at the end of thu upward link has a key that exactly
      // matches k, return the node.  Else return 0.
      PatriciaNode<KeyType,ValueType>  *prev = _head;
      PatriciaNode<KeyType,ValueType>  *last = _head->_right;
      PatriciaNode<KeyType,ValueType>  *save = _head;
     
      while (prev->_bitIndex < last->_bitIndex) {
        if (IsMatch<KeyType>(prev->_key, key))
          save = prev;
        prev = last;
        if (IsMatch<KeyType>(prev->_key, key))
          save = prev;
        last = (bit_get(key, last->_bitIndex) ? last->_right : last->_left);
      }
     
      if (IsMatch<KeyType>(last->_key, key))
        return(last);
      else if (save != _head)
        return(save);
      else
        return(0);
    }

    //------------------------------------------------------------------------
    //!  DOESN'T WORK PROPERLY
    //------------------------------------------------------------------------
    PatriciaNode<KeyType,ValueType> *
    LongestMatchNode2(const KeyType & key) const
    {
      // Traverse the Patricia (downward) until we find an upward link.
      // If the node at the end of thu upward link has a key that exactly
      // matches k, return the node.  Else return 0.
      PatriciaNode<KeyType,ValueType>  *prev = _head;
      PatriciaNode<KeyType,ValueType>  *last = _head->_right;
     
      // last = (bit_get(key, prev->_bitIndex) ? last->_right : last->_left);
     
      // Navigate down the trie and look for the key
      while (prev->_bitIndex < last->_bitIndex) {
        prev = last;
        last = (bit_get(key, last->_bitIndex) ? last->_right : last->_left);
      }
     
      if (key_compare(key, last->_key))
        return(last);  // key is already in the trie
     
      // RecursivePrint(last);
     
      std::cerr << "last->left: " << last->_left->_key 
                << " last->_right: " << last->_right->_key
                << std::endl;
     
      // Find the first bit that does not match.
      int i = BitFirstDifferent<KeyType>(key, last->_key);

      // Find the place in the trie to insert the new node
      // prev = _head;
      last = prev->_right;
      last = (bit_get(key, prev->_bitIndex) ? last->_right : last->_left);
      while ((prev->_bitIndex < last->_bitIndex) && (last->_bitIndex < i)) {
        std::cerr << "prev: " << prev->_key << " last: " << last->_key 
                  << std::endl;
        prev = last;
        last = (bit_get(key, last->_bitIndex) ? last->_right : last->_left);
      }
      std::cerr << "last->left: " << last->_left->_key 
                << " last->_right: " << last->_right->_key
                << std::endl;
     
      if (IsMatch<KeyType>(last->_key, key))
        return(last);
      else if (prev != _head)
        return(prev);
      else
        return(0);
    }

    //------------------------------------------------------------------------
    //!  Remove the node containing the given key. Returns
    //!  true if the operation succeeded, false otherwise.
    //------------------------------------------------------------------------
    bool Delete(const KeyType & key)
    {
      PatriciaNode<KeyType,ValueType>  *x = 0;
      PatriciaNode<KeyType,ValueType>  *pp = 0;
     
      // Start at the root
      PatriciaNode<KeyType,ValueType>  *p = _head;
      PatriciaNode<KeyType,ValueType>  *t = (PatriciaNode<KeyType,ValueType>*)(p->_right);
     
      // Navigate down the tree and look for the key
      while (p->_bitIndex < t->_bitIndex) {
        pp = p;
        p  = t;
        t  = bit_get(key, t->_bitIndex) ? t->_right : t->_left;
      }

      /*
        int  i;
        do {
        i = t->_bitIndex;
        pp = p;
        p  = t;
        t  = bit_get(key, t->_bitIndex) ? t->_right : t->_left;
        } while (i < t->_bitIndex);
      */

      if (! key_compare(key, t->_key)) {
        //  key not found
        return(false);
      }

      // Copy p's key to t
      if (t != p)
        key_copy(p, t);
     
      if ((p->_left->_bitIndex > p->_bitIndex) || 
          (p->_right->_bitIndex > p->_bitIndex)) {
       
        // There is at least one downward edge
       
        if (p != t) {
          // Look for a new (intermediate) key
          KeyType  newkey = p->_key;
        
          PatriciaNode<KeyType,ValueType>  *lp = p;
          x  = bit_get(newkey, p->_bitIndex) ? p->_right : p->_left;
        
          while (lp->_bitIndex < x->_bitIndex) {
            lp = x;
            x  = bit_get(newkey, x->_bitIndex) ? x->_right : x->_left;
          }
        
          // If the intermediate key was not found, we have a problem..
          if (! key_compare(newkey, x->_key)) {
            return(false); // The key could not be found!
          }
        
          // Rewire the leaf (lp) to point to t
          if (bit_get(newkey, lp->_bitIndex))
            lp->_right = t;
          else
            lp->_left = t;
        
        }
       
        // Rewire the parent to point to the real child of p
        if (pp != p) {
          PatriciaNode<KeyType,ValueType>* ch = 
            bit_get(key, p->_bitIndex) ? p->_left : p->_right;
          if (bit_get(key, pp->_bitIndex))
            pp->_right = ch;
          else
            pp->_left = ch;
        }
       
      } 
      else {
        // Both edges (left, right) are pointing upwards or to the 
        // node (self-edges).
       
        // Rewire the parent
        if (pp != p) {
          if (bit_get(key, pp->_bitIndex))
            pp->_right = (((p->_left ==  p->_right) && (p->_left == p)) ? 
                          pp : ((p->_left == p) ? p->_right : p->_left));
          else
            pp->_left  = (((p->_left == p->_right) && (p->_left == p)) ?
                          pp : ((p->_left == p) ? p->_right : p->_left));
        }
      }
     
      // Deallocate p (no longer needed)
      delete(p);
      p = 0;

      _size -= 1;
     
      // Success!
      return(true);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void RecursivePrint() const
    {
      RecursivePrint(_head);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint32_t StreamedLength() const override
    {
      uint32_t  rc = IOUtils::StreamedLength(_size);
      RecursiveStreamedLength(_head, rc);
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::istream & Read(std::istream & is) override
    {
      Clear();
      if (is) {
        uint32_t  numEntries;
        if (StreamIO::Read(is, numEntries)) {
          for (uint32_t i = 0; i < numEntries; ++i) {
            KeyType  key;
            if (StreamIO::Read(is, key)) {
              ValueType  value;
              if (! StreamIO::Read(is, value))
                break;
              Insert(key, value);
            }
            else {
              break;
            }
          }
        }
      }
      return(is);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & Write(std::ostream & os) const override
    {
      if (os) {
        if (StreamIO::Write(os, _size)) {
          RecursiveWrite(_head, os);
        }
      }
      return(os);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t Read(int fd) override
    {
      Clear();
      ssize_t  totalBytes = 0;
      if (fd >= 0) {
        uint32_t  numEntries;
        uint32_t  bytesRead = DescriptorIO::Read(fd, numEntries);
        if (bytesRead <= 0)
          return(-1);
        totalBytes = bytesRead;
        for (uint32_t i = 0; i < numEntries; ++i) {
          KeyType  key;
          bytesRead = DescriptorIO::Read(fd, key);
          if (bytesRead > 0) {
            totalBytes += bytesRead;
            ValueType  value;
            bytesRead = DescriptorIO::Read(fd, value);
            if (bytesRead > 0) {
              totalBytes += bytesRead;
            }
            else {
              return(-1);
            }
            Insert(key, value);
          }
          else {
            return(-1);
          }
        }
      }
      return(totalBytes);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t Write(int fd) const override
    {
      ssize_t  totalBytes = 0;
      if (fd >= 0) {
        ssize_t  bytesWritten = DescriptorIO::Write(fd, _size);
        if (bytesWritten > 0) {
          totalBytes += bytesWritten;
          if (_size > 0) {
            bytesWritten = RecursiveWrite(_head, fd);
            if (bytesWritten > 0) {
              totalBytes += bytesWritten;
            }
            else {
              return(-1);
            }
          }
        }
        else {
          return(-1);
        }
      }
      return(totalBytes);
    }
    
    bool operator == (const Patricia<KeyType,ValueType> & pt) const
    {
      if (_size != pt._size)
        return(false);
      return(RecursiveEqual(_head, pt));
    }
    
  private:
    bool RecursiveEqual(PatriciaNode<KeyType,ValueType> *root,
                        const Patricia<KeyType,ValueType> & pt) const
    {
      bool  rc = true;
      PatriciaNode<KeyType,ValueType> *l = root->_left;
      PatriciaNode<KeyType,ValueType> *r = root->_right;
      // check the left branch
      if ((l->_bitIndex >= root->_bitIndex) && (l != root) && (l != _head)) {
        if (! RecursiveEqual(l, pt)) {
          return(false);
        }
      }
     
      // check the right branch
      if ((r->_bitIndex >= root->_bitIndex) && (r != root) && (r != _head)) {
        if (! RecursiveEqual(r, pt)) {
          return(false);
        }
      }
     
      if (root != _head) {
        if (! pt.LookupNode(root->GetKey())) {
          return(false);
        }
      }
     
      return(true);
    }
     
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void RecursivePrint(PatriciaNode<KeyType,ValueType> * root) const
    {
      PatriciaNode<KeyType,ValueType> *l = root->_left;
      PatriciaNode<KeyType,ValueType> *r = root->_right;
     
      // Print the right branch
      if ((r->_bitIndex > root->_bitIndex) && (r != root) && (r != _head))
        RecursivePrint(r);
     
      // Print the left branch
      if ((l->_bitIndex > root->_bitIndex) && (l != root) && (l != _head))
        RecursivePrint(l);
     
      // Print the root
      std::cout << root->_key << " " << root->_data << std::endl;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void RecursiveStreamedLength(PatriciaNode<KeyType,ValueType> * root,
                                 uint32_t & len) const
    {
      PatriciaNode<KeyType,ValueType> *l = root->_left;
      PatriciaNode<KeyType,ValueType> *r = root->_right;
     
      // count the left branch
      if ( (l->_bitIndex >= root->_bitIndex) && (l != root) && (l != _head) )
        RecursiveStreamedLength(l, len);
     
      // count the right branch
      if ( (r->_bitIndex >= root->_bitIndex) && (r != root) && (r != _head) )
        RecursiveStreamedLength(r, len);
     
      // count the root
      len += root->StreamedLength();
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & RecursiveWrite(PatriciaNode<KeyType,ValueType> * root,
                                  std::ostream & os) const
    {
      if (os) {
        PatriciaNode<KeyType,ValueType> *l = root->_left;
        PatriciaNode<KeyType,ValueType> *r = root->_right;
     
        // write the left branch
        if ((l->_bitIndex >= root->_bitIndex) && (l != root) && (l != _head))
          if (! RecursiveWrite(l, os))
            return(os);
       
        // write the right branch
        if ((r->_bitIndex >= root->_bitIndex) && (r != root) && (r != _head))
          if (! RecursiveWrite(r, os))
            return(os);
       
        // write the root
        if (StreamIO::Write(os, root->_key))
          StreamIO::Write(os, root->_data);
      }
      return(os);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ssize_t 
    RecursiveWrite(PatriciaNode<KeyType,ValueType> * root, int fd) const
    {
      ssize_t  rc = 0;
     
      if (fd >= 0) {
        PatriciaNode<KeyType,ValueType> *l = root->_left;
        PatriciaNode<KeyType,ValueType> *r = root->_right;
     
        // write the left branch
        if ((l->_bitIndex >= root->_bitIndex) && (l != root) && (l != _head)) {
          ssize_t  bytesWritten = RecursiveWrite(l, fd);
          if (bytesWritten < 0)
            return(-1);
          rc += bytesWritten;
        }
       
        // write the right branch
        if ((r->_bitIndex >= root->_bitIndex) && (r != root) && (r != _head)) {
          ssize_t  bytesWritten = RecursiveWrite(r, fd);
          if (bytesWritten < 0)
            return(-1);
          rc += bytesWritten;
        }
       
        // write the root
        ssize_t  bytesWritten = DescriptorIO::Write(fd, root->_key);
        if (bytesWritten <= 0)
          return(-1);
        rc += bytesWritten;
        bytesWritten = DescriptorIO::Write(fd, root->_data);
        if (bytesWritten <= 0)
          return(-1);
        rc += bytesWritten;
      }
      return(rc);
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void RecursiveRemove(PatriciaNode<KeyType,ValueType> *node)
    {
      PatriciaNode<KeyType,ValueType>  *l = node->_left;
      PatriciaNode<KeyType,ValueType>  *r = node->_right;
     
      // remove the left branch
      if (l 
          && (l->_bitIndex >= node->_bitIndex) 
          && (l != node) 
          && (l != _head)) {
        RecursiveRemove(l);
      }
     
      // remove the right branch
      if (r
          && (r->_bitIndex >= node->_bitIndex)
          && (r != node)
          && (r != _head)) {
        RecursiveRemove(r);
      }
     
      // remove the node
      if (node && (node != _head)) {
        // delete(node);
        node = 0;
      }
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int bit_get(const KeyType & k, int n) const
    {
      int  rc = 2;  // psuedo-value
      if (n >= 0)
        rc = GetBit<KeyType>(k, n);
      return(rc);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    inline int bit_first_different(const KeyType & k1, const KeyType & k2)
    {
      return(BitFirstDifferent<KeyType>(k1, k2));
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void key_copy(PatriciaNode<KeyType,ValueType> *src, 
                  PatriciaNode<KeyType,ValueType> *dest)
    {
      if (src == dest)
        return;
     
      // Copy the key from src to dest
      dest->_key = src->_key;
     
      // Copy the data from src to dest
      dest->_data = src->_data;
     
      // How about the bit index?
      // dest->_bitIndex = src->_bitIndex;
    }

    KeyEqual                          key_compare;
    PatriciaNode<KeyType,ValueType>  *_head;
    uint32_t                          _size;
  };
  

}  // namespace Dwm


#endif  // _PATRICIA_HH_

//---------------------------- emacs settings -----------------------------
//  Local Variables:
//  mode: C++
//  tab-width: 2
//  indent-tabs-mode: nil
//  c-basic-offset: 2
//  End:
//-------------------------------------------------------------------------
