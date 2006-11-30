/*
  Hash.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_HASH_H

#define _QORE_HASH_H

#include <qore/config.h>
#include <qore/hash_map.h>

#include <stdio.h>
#include <string.h>

// FIXME: use STL list instead
// to maintain the order of inserts
class HashMember {
   public:
      class QoreNode *node;
      char *key;
      class HashMember *next;
      class HashMember *prev;
};

class HashIterator
{
   private:
      class HashMember *head;
      class HashMember *ptr;

   public:
      DLLEXPORT inline HashIterator(class HashMember *h) { ptr = NULL; head = h; }
      DLLEXPORT inline HashIterator(class Hash *h);

      DLLEXPORT inline class HashMember *next() 
      { 
	 if (ptr) 
	    ptr = ptr->next;
	 else
	    ptr = head;
	 return ptr;
      }
      DLLEXPORT inline char *getKey() const
      { 
	 if (!ptr)
	    return NULL;

	 return ptr->key;
      }
      DLLEXPORT class QoreString *getKeyString() const;
      DLLEXPORT inline class QoreNode *getValue() const
      {
	 if (ptr)
	    return ptr->node;
	 return NULL;
      }
      DLLEXPORT inline class QoreNode **getValuePtr() const
      {
	 if (ptr)
	    return &(ptr->node);
	 return NULL;
      }
      DLLEXPORT class QoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT inline bool last() const 
      { 
	 return (bool)(ptr ? !ptr->next : false); 
      } 
      //DLLEXPORT inline void setValue(class QoreNode *val, class ExceptionSink *xsink);
};

class Hash
{
      friend class HashIterator;

   private:
      class HashMember *member_list;
      class HashMember *tail;
      hm_hm_t hm;
      bool needs_eval;

      DLLLOCAL class QoreNode **newKeyValue(char *key, class QoreNode *value);
      DLLLOCAL void internDeleteKey(class HashMember *m, class ExceptionSink *xsink);
      DLLLOCAL inline void deref_intern(class ExceptionSink *xsink);

  protected:
      DLLEXPORT ~Hash();

   public:
      DLLEXPORT Hash(bool ne = false);

      DLLEXPORT char *getFirstKey() const { return member_list ? member_list->key :NULL; }
      DLLEXPORT char *getLastKey() const { return tail ? tail->key : NULL; }
      DLLEXPORT class QoreNode *getKeyValueExistence(char *key) const;
      DLLEXPORT class QoreNode *getKeyValueExistence(class QoreString *key, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *getKeyValue(class QoreString *key, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *getKeyValue(char *key) const;
      DLLEXPORT class Hash *copy() const;

      // APIs suitable for objects below
      DLLEXPORT class QoreNode **getKeyValuePtr(class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getKeyValuePtr(char *key);
      DLLEXPORT class QoreNode **getExistingValuePtr(class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getExistingValuePtr(char *key);
      DLLEXPORT void merge(class Hash *h, class ExceptionSink *xsink);
      DLLEXPORT void assimilate(class Hash *h, class ExceptionSink *xsink);
      DLLEXPORT class Hash *eval(class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *evalKey(char *key, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *evalKeyExistence(char *key, class ExceptionSink *xsink) const;
      DLLEXPORT void setKeyValue(class QoreString *key, class QoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void setKeyValue(char *key, class QoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(char *key, class ExceptionSink *xsink);
      DLLEXPORT class List *getKeys() const;
      DLLEXPORT bool compareSoft(class Hash *h, class ExceptionSink *xsink) const;
      DLLEXPORT bool compareHard(class Hash *h) const;
      DLLEXPORT class QoreNode *evalFirstKeyValue(class ExceptionSink *xsink) const;
      DLLEXPORT void dereference(class ExceptionSink *xsink);
      DLLEXPORT void derefAndDelete(class ExceptionSink *xsink);

      DLLEXPORT inline int size() const 
      { 
	 return hm.size(); 
      }
      DLLEXPORT inline bool needsEval() const
      {
	 return needs_eval;
      }
      DLLEXPORT inline void clearNeedsEval()
      {
	 needs_eval = false;
      }
};

inline HashIterator::HashIterator(class Hash *h) 
{ 
   ptr = NULL;
   head = h->member_list; 
}

#endif // _QORE_HASH_H
