/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  RSet.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_INTERN_RSETHELPER_H

#define _QORE_INTERN_RSETHELPER_H

#include <qore/intern/RSection.h>

#include <set>

//#define _QORE_CYCLE_CHECK 1
#ifdef _QORE_CYCLE_CHECK
#define QORE_DEBUG_OBJ_REFS 0
#else
#define QORE_DEBUG_OBJ_REFS 5
#endif

class RSet;
class RSetHelper;

class RObject {
public:
   // read-write lock with special rsection handling
   mutable RSectionLock rml;
   // weak references
   QoreReferenceCounter tRefs;

   // this lock is needed for the condition variable
   mutable QoreThreadLock rlck;
   QoreCondition rdone; // recursive scan done flag

   int rscan,   // TID flag for starting a recursive scan
      rcount,   // the number of unique recursive references to this object
      rwaiting, // the number of threads waiting for a scan of this object
      rcycle;   // the recursive cycle number to see if the object has been scanned since a transaction restart

   // set of objects in a cyclic directed graph
   RSet* rset;

   // reference count
   int& references;

   // do we need to call isValidImpl()
   bool needs_is_valid;

   DLLLOCAL RObject(int& n_refs, bool niv = false) : rscan(0), rcount(0), rwaiting(0), rcycle(0), rset(0), references(n_refs), needs_is_valid(niv) {
   }

   DLLLOCAL virtual ~RObject();

   DLLLOCAL void tRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "RObject::tRef() this: %p tref %d->%d\n", this, tRefs.reference_count(), tRefs.reference_count() + 1);
#endif
      tRefs.ROreference();
   }

   DLLLOCAL void tDeref() {
#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "RObject::tDeref() this: %p tref %d->%d\n", this, tRefs.reference_count(), tRefs.reference_count() - 1);
#endif
      if (tRefs.ROdereference())
	 deleteObject();
   }

   DLLLOCAL int refs() const {
      return references;
   }

   DLLLOCAL void setRSet(RSet* rs, int rcnt);

   DLLLOCAL void invalidateRSet();

   DLLLOCAL void removeInvalidateRSet();

   DLLLOCAL bool scanCheck(RSetHelper& rsh, AbstractQoreNode* n);

   // if the object is valid (and can be deleted)
   DLLLOCAL bool isValid() const {
      return !needs_is_valid ? true : isValidImpl();
   }

   // if the object is valid (and can be deleted)
   DLLLOCAL virtual bool isValidImpl() const {
      assert(false);
      return true;
   }

   // returns true if a lock error has occurred and the transaction should be aborted or restarted; the rsection lock is held when this function is called
   DLLLOCAL virtual bool scanMembers(RSetHelper& rsh) = 0;

   // returns true if the object needs to be scanned for recursive references (ie could contain an object or closure or a container containing one of those)
   DLLLOCAL virtual bool needsScan() const = 0;

   // deletes the object itself
   DLLLOCAL virtual void deleteObject() = 0;

   // returns the name of the object
   DLLLOCAL virtual const char* getName() const = 0;
};

typedef std::set<RObject*> rset_t;

/* Qore recursive reference handling works as follows: objects are sorted into sets making up
   directed cyclic graphs.

   The objects go out of scope when all nodes have references = the number of recursive references.

   If any one member still has valid references (meaning a reference count > # of recursive refs), then *none*
   of the members of the graph can be dereferenced.
 */

// set of objects in recursive directed graph
class RSet {
protected:
   // we assume set::size() is O(1); this should be a safe assumption
   rset_t set;
   int acnt;
   bool valid;

   DLLLOCAL void invalidateIntern() {
      assert(valid);
      valid = false;
      // remove the weak references to all contained objects
      for (rset_t::iterator i = begin(), e = end(); i != e; ++i)
         (*i)->tDeref();
      clear();
      //printd(6, "RSet::invalidateIntern() this: %p\n", this);
   }

public:
   QoreRWLock rwl;

   DLLLOCAL RSet() : acnt(0), valid(true) {
   }

   DLLLOCAL RSet(RObject* o) : acnt(0), valid(true) {
      set.insert(o);
   }

   DLLLOCAL ~RSet() {
      //printd(5, "RSet::~RSet() this: %p (acnt: %d)\n", this, acnt);
      assert(!acnt);
   }

   DLLLOCAL void deref() {
      bool del = false;
      {
         QoreAutoRWWriteLocker al(rwl);
         if (valid)
            valid = false;
         //printd(5, "RSet::deref() this: %p %d -> %d\n", this, acnt, acnt - 1);
         assert(acnt > 0);
         del = !--acnt;
      }
      if (del)
         delete this;
   }

   DLLLOCAL void invalidate() {
      QoreAutoRWWriteLocker al(rwl);
      if (valid)
         invalidateIntern();
   }

   DLLLOCAL void ref() {
      ++acnt;
   }

   DLLLOCAL bool active() const {
      return valid;
   }

   DLLLOCAL int canDelete(int ref_copy, int rcount);

#ifdef DEBUG
   DLLLOCAL void dbg();

   DLLLOCAL bool isValid() const {
      return qore_check_this(this) ? valid : false;
   }
#endif

   DLLLOCAL bool assigned() const {
      return (bool)acnt;
   }

   DLLLOCAL void insert(RObject* o) {
      set.insert(o);
   }

   DLLLOCAL void clear() {
      set.clear();
   }

   DLLLOCAL rset_t::iterator begin() {
      return set.begin();
   }

   DLLLOCAL rset_t::iterator end() {
      return set.end();
   }

   DLLLOCAL rset_t::iterator find(RObject* o) {
      return set.find(o);
   }

   DLLLOCAL size_t size() const {
      return set.size();
   }

   DLLLOCAL bool pop() {
      assert(!set.empty());
      set.erase(set.begin());
      return set.empty();
   }
};

typedef std::vector<RObject*> rvec_t;
class RSetHelper;
typedef std::set<RSet*> rs_set_t;

struct RSetStat {
   RSet* rset;
   int rcount;
   bool in_cycle : 1,
      ok : 1;

   DLLLOCAL RSetStat() : rset(0), rcount(0), in_cycle(false), ok(false) {
   }

   DLLLOCAL RSetStat(const RSetStat& old) : rset(old.rset), rcount(old.rcount), in_cycle(old.in_cycle), ok(old.ok) {
   }

   DLLLOCAL void finalize(RSet* rs = 0) {
      assert(!ok);
      assert(!rset);
      rset = rs;
   }
};

class RSetHelper {
   friend class RSectionScanHelper;
   friend class RObject;
private:
   DLLLOCAL RSetHelper(const RSetHelper&);

protected:
   typedef std::map<RObject*, RSetStat> omap_t;
   typedef std::set<QoreClosureBase*> closure_set_t;
   // map of all objects scanned to rset (rset = finalized, 0 = not finalized, in current list)
   omap_t fomap;

   typedef std::vector<omap_t::iterator> ovec_t;
   // current objects being scanned, used to establish a cycle
   ovec_t ovec;

   // list of RSet objects to be invalidated when the transaction is committed
   rs_set_t tr_invalidate;

   // set of RObjects not participating in any recursive set
   rset_t tr_out;

   // RSectionLock notification helper when waiting on locks
   RNotifier notifier;

   // set of scanned closures
   closure_set_t closure_set;

   // fomap size, to detect changes in the scanned set
   unsigned fomap_size;

#ifdef DEBUG
   int lcnt;
   DLLLOCAL void inccnt() { ++lcnt; }
   DLLLOCAL void deccnt() { --lcnt; }
#else
   DLLLOCAL void inccnt() {}
   DLLLOCAL void deccnt() {}
#endif

   // rollback transaction due to lock error
   DLLLOCAL void rollback();

   // commit transaction
   DLLLOCAL void commit(RObject& obj);

   // returns true if a lock error has occurred, false if otherwise
   DLLLOCAL bool checkIntern(RObject& obj);
   // returns true if a lock error has occurred, false if otherwise
   DLLLOCAL bool checkIntern(AbstractQoreNode* n);

   // queues nodes not scanned to tr_invalidate and tr_out
   DLLLOCAL bool removeInvalidate(RSet* ors, int tid = gettid());

   DLLLOCAL bool inCurrentSet(omap_t::iterator fi) {
      for (size_t i = 0; i < ovec.size(); ++i)
         if (ovec[i] == fi)
            return true;
      return false;
   }

   DLLLOCAL bool addToRSet(omap_t::iterator oi, RSet* rset, int tid);

   DLLLOCAL void mergeRSet(int i, RSet*& rset);

   DLLLOCAL bool makeChain(int i, omap_t::iterator fi, int tid);

public:
   DLLLOCAL RSetHelper(RObject& obj);

   DLLLOCAL ~RSetHelper() {
      assert(ovec.empty());
      assert(!lcnt);
   }

   DLLLOCAL unsigned size() const {
      return fomap_size;
   }

   DLLLOCAL void add(RObject* ro) {
      if (fomap.find(ro) != fomap.end())
         return;
      rset_t::iterator i = tr_out.lower_bound(ro);
      if (i == tr_out.end() || *i != ro)
         tr_out.insert(i, ro);
   }
};

#endif
