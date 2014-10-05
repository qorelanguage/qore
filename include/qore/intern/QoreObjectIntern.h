/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreObjectIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#ifndef _QORE_QOREOBJECTINTERN_H

#define _QORE_QOREOBJECTINTERN_H

#include <stdlib.h>
#include <assert.h>

#include <map>
#include <set>
#include <vector>

#include "intern/QoreClassIntern.h"
#include <qore/intern/qore_var_rwlock_priv.h>

#define OS_OK            0
#define OS_DELETED      -1

#define QRO_LVL 1

// object access constants
#define QOA_OK           0
#define QOA_PRIV_ERROR   1
#define QOA_PUB_ERROR    2

#ifdef _QORE_CYCLE_CHECK
#define QORE_DEBUG_OBJ_REFS 0
#else
#define QORE_DEBUG_OBJ_REFS 5
#endif

class LValueHelper;

/*
  Qore internal class data is stored against the object with this data structure
  against its qore_classid_t (class ID).  In a class hierarchy, for private data
  that is actually a C++ subclass of Qore parent classes, then we save the same
  private data against the qore class IDs of the parent classes, but we set the
  flag to true, meaning that we will not delete the private data when
  the parent classes' destructors are run, but rather only when the subclass that
  actually owns data has its turn to destroy private object data.

  So basically, the second boolean flag just means - does this class ID actually
  own the private data or not - if it's false, then it does not actually own the data,
  but is compatible with the data, so parent class builtin (C++) methods will get
  passed this private data as if it belonged to this class and as if it were saved
  directly to the object in the class' constructor.

  please note that this flag is called the "virtual" flag elsewhere in the code here,
  meaning that the data only "virtually" belongs to the class
 */
typedef std::pair<AbstractPrivateData*, bool> private_pair_t;

// mapping from qore class ID to the object data
typedef std::map<qore_classid_t, private_pair_t> keymap_t;

// for objects with multiple classes, private data has to be keyed
class KeyList {
private:
   keymap_t keymap;

public:
   DLLLOCAL AbstractPrivateData* getReferencedPrivateData(qore_classid_t key) const {
      keymap_t::const_iterator i = keymap.find(key);
      if (i == keymap.end())
	 return 0;

      AbstractPrivateData* apd = i->second.first;
      apd->ref();
      return apd;
   }

   DLLLOCAL void addToString(QoreString* str) const {
      for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	 str->sprintf("%d=<0x%p>, ", i->first, i->second.first);
   }

   DLLLOCAL void derefAll(ExceptionSink* xsink) const {
      for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	 if (!i->second.second)
	    i->second.first->deref(xsink);
   }

   DLLLOCAL AbstractPrivateData* getAndClearPtr(qore_classid_t key) {
      keymap_t::iterator i = keymap.find(key);
      if (i == keymap.end() || i->second.second)
	 return 0;

      return i->second.first;
   }

   DLLLOCAL void insert(qore_classid_t key, AbstractPrivateData* pd) {
      assert(pd);
      assert(keymap.find(key) == keymap.end());
      keymap.insert(std::make_pair(key, std::make_pair(pd, false)));
   }

   DLLLOCAL void insertVirtual(qore_classid_t key, AbstractPrivateData* pd) {
      assert(pd);
      if (keymap.find(key) == keymap.end())
	 keymap.insert(std::make_pair(key, std::make_pair(pd, true)));
   }
};

#ifdef DO_OBJ_RECURSIVE_CHECK
class qore_rsection_priv;
class RNotifier {
private:
   DLLLOCAL RNotifier(const RNotifier&);
   DLLLOCAL RNotifier& operator=(const RNotifier&);

public:
   bool setp;
   QoreCounter notify;
      
   DLLLOCAL RNotifier() : setp(false) {
   }

   DLLLOCAL void done() {
      assert(setp);
      notify.dec();
   }

   DLLLOCAL void set() {
      assert(!setp);
      setp = true;
      assert(!notify.getCount());
      notify.inc();
   }

   DLLLOCAL void wait() {
      if (setp) {
         //printd(5, "RNotifier::wait() waiting for notification\n");
         notify.waitForZero();
         setp = 0;
      }
   }
};

typedef std::list<RNotifier*> n_list_t;
#endif

// rwlock with stardard read and write lock handling, and special "rsection" handling
// the rsection is grabbed with the read lock but only one thread can have the rsection lock at once
// leaving other threads to read the object normally
class qore_rsection_priv : public qore_var_rwlock_priv {
protected:
#ifdef DO_OBJ_RECURSIVE_CHECK
   int rs_tid;          // tid of thread holding the rsection lock

   // list of ObjectRSetHelper objects for notifications for rsection management
   n_list_t list;

   // notify rsection threads that the rsection lock has been released
   DLLLOCAL virtual void notifyIntern() {
      for (n_list_t::iterator i = list.begin(), e = list.end(); i != e; ++i)
         (*i)->done();
      list.clear();
   }

   DLLLOCAL void setNotificationIntern(RNotifier* rn) {
      assert(write_tid != -1 || rs_tid != -1);
      list.push_back(rn);
      rn->set();
   }
#endif

public:
#ifdef DO_OBJ_RECURSIVE_CHECK
   DLLLOCAL qore_rsection_priv() : rs_tid(-1) {
      has_notify = true;
   }
#else
   DLLLOCAL qore_rsection_priv() {
   }
#endif

   DLLLOCAL virtual ~qore_rsection_priv() {
#ifdef DO_OBJ_RECURSIVE_CHECK
      assert(rs_tid == -1);
      assert(list.empty());
#endif
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   // does not block under any circumstances, returns -1 if the lock cannot be acquired and sets a notification
   DLLLOCAL int tryRSectionLockNotifyWaitRead(RNotifier* rn) {
      assert(has_notify);

      int tid = gettid();

      AutoLocker al(l);
      assert(write_tid != tid);

      if (write_tid == -1) {
         // if we already have the rsection, then return
         if (rs_tid == tid)
            return 0;

         if (rs_tid == -1) {
            // grab the read lock
            ++readers;

            // grab the rsection
            rs_tid = tid;
            return 0;
         }
      }

      setNotificationIntern(rn);

      return -1;
   }

   DLLLOCAL void rSectionUnlock() {
      AutoLocker al(l);
      assert(write_tid == -1);
      assert(rs_tid == gettid());
      assert(readers);

      // unlock rsection
      rs_tid = -1;

      qore_rsection_priv::notifyIntern();

      if (!--readers)
         unlock_read_signal();
   }

   DLLLOCAL bool hasRSectionLock(int tid = gettid()) {
      return rs_tid == tid;
   }

   DLLLOCAL bool checkRSectionExclusive(int tid = gettid()) {
      return (rs_tid == tid || write_tid == tid);
   }

   DLLLOCAL int rSectionTid() const {
      return rs_tid;
   }
#endif
};

class RSectionLock : public QoreVarRWLock {
public:
   DLLLOCAL RSectionLock() : QoreVarRWLock(new qore_rsection_priv) {
   }

   DLLLOCAL ~RSectionLock() {
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   // does not block under any circumstances, returns -1 if the lock cannot be acquired and sets a notification
   DLLLOCAL int tryRSectionLockNotifyWaitRead(RNotifier* rn) {
      return static_cast<qore_rsection_priv*>(priv)->tryRSectionLockNotifyWaitRead(rn);
   }

   DLLLOCAL void rSectionUnlock() {
      static_cast<qore_rsection_priv*>(priv)->rSectionUnlock();
   }

   DLLLOCAL bool hasRSectionLock(int tid = gettid()) {
      return static_cast<qore_rsection_priv*>(priv)->hasRSectionLock(tid);
   }

   DLLLOCAL bool checkRSectionExclusive(int tid = gettid()) {
      return static_cast<qore_rsection_priv*>(priv)->checkRSectionExclusive(tid);
   }

   DLLLOCAL int rSectionTid() const {
      return static_cast<qore_rsection_priv*>(priv)->rSectionTid();
   }
#endif
};

#ifdef DO_OBJ_RECURSIVE_CHECK

#define ORS_LOCK_ERROR  -1
#define ORS_NO_MATCH     0
#define ORS_MATCH        1

/* Qore object recursive reference handling works as follows: objects are sorted into sets making up
   directed cyclic graphs.

   The objects go out of scope when all nodes have references = the number of recursive references.

   If any one member still has valid references (meaning a reference count > # of recursive refs), then *none*
   of the members of the graph can be dereferenced.
 */

// set of objects in recursive directed graph
class ObjectRSet {
protected:
   obj_set_t set;
   // for O(1) size comparisons; std::set::size() can be slow
   size_t ssize;
   QoreRWLock rwl;
   int acnt;
   bool valid, in_del;

   DLLLOCAL void invalidateIntern() {
      assert(valid);
      valid = false;
      // remove the weak references to all contained objects
      for (obj_set_t::iterator i = begin(), e = end(); i != e; ++i)
         (*i)->tDeref();
      clear();
      //printd(6, "ObjectRSet::invalidateIntern() this: %p\n", this);
   }
   
public:
   DLLLOCAL ObjectRSet() : ssize(0), acnt(0), valid(true), in_del(false) {
   }

   DLLLOCAL ObjectRSet(QoreObject* o) : ssize(1), acnt(0), valid(true), in_del(false) {
      set.insert(o);
   }

   DLLLOCAL ~ObjectRSet() {
      //printd(5, "ObjectRSet::~ObjectRSet() this: %p (acnt: %d)\n", this, acnt);
      assert(!acnt);
   }

   DLLLOCAL void deref() {
      bool del = false;
      {
         QoreAutoRWWriteLocker al(rwl);
         if (!in_del)
            in_del = true;
         //printd(5, "ObjectRSet::deref() this: %p %d -> %d\n", this, acnt, acnt - 1);
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
      return valid && !in_del;
   }

   DLLLOCAL int canDelete();

#ifdef DEBUG
   DLLLOCAL void dbg();

   DLLLOCAL bool isValid() const {
      return this ? valid : false;
   }

   DLLLOCAL bool isInDel() const {
      return this ? in_del : false;
   }
#endif

   DLLLOCAL bool assigned() const {
      return (bool)acnt;
   }

   DLLLOCAL void insert(QoreObject* o) {
      set.insert(o);
      ++ssize;
   }

   DLLLOCAL void clear() {
      set.clear();
      ssize = 0;
   }

   DLLLOCAL obj_set_t::iterator begin() {
      return set.begin();
   }

   DLLLOCAL obj_set_t::iterator end() {
      return set.end();
   }

   /*
   DLLLOCAL obj_set_t::reverse_iterator rbegin() {
      return set.rbegin();
   }

   DLLLOCAL obj_set_t::reverse_iterator rend() {
      return set.rend();
   }
   */

   DLLLOCAL obj_set_t::iterator find(QoreObject* o) {
      return set.find(o);
   }

   DLLLOCAL size_t size() const {
      return ssize;
   }

   DLLLOCAL bool pop() {
      assert(!set.empty());
      assert(ssize);
      set.erase(set.begin());
      --ssize;
      return !ssize;
   }
};

typedef std::vector<QoreObject*> obj_vec_t;

class ObjectRSetHelper;

class qore_object_private;

typedef std::set<ObjectRSet*> rs_set_t;

struct RSetStat {
   ObjectRSet* rset;
   int rcount;
   bool in_cycle : 1,
      ok : 1;

   DLLLOCAL RSetStat() : rset(0), rcount(0), in_cycle(false), ok(false) {
   }

   DLLLOCAL RSetStat(const RSetStat& old) : rset(old.rset), rcount(old.rcount), in_cycle(old.in_cycle), ok(old.ok) {
   }

   DLLLOCAL void finalize(ObjectRSet* rs = 0) {
      assert(!ok);
      assert(!rset);
      rset = rs;
   }
};

class ObjectRSetHelper {
private:
   DLLLOCAL ObjectRSetHelper(const ObjectRSetHelper&);
   
protected:
   typedef std::map<QoreObject*, RSetStat> omap_t;
   // map of all objects scanned to rset (rset = finalized, 0 = not finalized, in current list)
   omap_t fomap;

   typedef std::vector<omap_t::iterator> ovec_t;
   // current objects being scanned, used to establish a cycle
   ovec_t ovec;

   // list of fomap iterators to current recursive objects found
   //ovec_t frvec;

   // list of ObjectRSet objects to be invalidated when the transaction is committed
   rs_set_t tr_invalidate;
   
   // set of QoreObjects not participating in any recursive set
   obj_set_t tr_out;

   // RSectionLock notification helper when waiting on locks
   RNotifier notifier;

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
   DLLLOCAL void commit(QoreObject& obj);

   // returns true if a lock error has occurred, false if otherwise
   DLLLOCAL bool checkIntern(QoreObject& obj);
   // returns true if a lock error has occurred, false if otherwise
   DLLLOCAL bool checkIntern(AbstractQoreNode* n);

   // queues nodes not scanned to tr_invalidate and tr_out
   DLLLOCAL int removeInvalidate(ObjectRSet* ors, int tid = gettid());

   DLLLOCAL bool inCurrentSet(omap_t::iterator fi) {
      for (size_t i = 0; i < ovec.size(); ++i)
         if (ovec[i] == fi)
            return true;
      return false;
   }

   DLLLOCAL bool addToRSet(omap_t::iterator oi, ObjectRSet* rset, int tid);

   DLLLOCAL void mergeRSet(int i, ObjectRSet*& rset);

   DLLLOCAL bool makeChain(int i, omap_t::iterator fi, int tid);

public:
   DLLLOCAL ObjectRSetHelper(QoreObject& obj);

   DLLLOCAL ~ObjectRSetHelper() {
      assert(ovec.empty());
      //assert(frvec.empty());
      assert(!lcnt);
   }
};
#endif

class qore_object_private {
public:
   const QoreClass* theclass;
   int status;
   //mutable QoreRWLock rwl;

   // read-write lock with special rsection handling
   mutable RSectionLock rml;

   // used for weak references, to ensure that assignments will not deadlock when the object is locked for update
   mutable QoreThreadLock ref_mutex;
   KeyList* privateData;
   QoreReferenceCounter tRefs;  // weak references
   QoreHashNode* data;
   QoreProgram* pgm;

   bool system_object, delete_blocker_run, in_destructor, pgm_ref;
#ifdef DO_OBJ_RECURSIVE_CHECK
   bool recursive_ref_found;

   QoreThreadLock rlck;
   QoreCondition rdone; // recursive scan done flag
   
   int rscan,   // TID flag for starting a recursive scan
      rcount,   // the number of unique recursive references to this object
      rwaiting, // the number of threads waiting for a scan of this object
      rcycle;   // the recursive cycle number to see if the object has been scanned since a transaction restart

   // set of objects in a cyclic directed graph
   ObjectRSet* rset;
#endif
   QoreObject* obj;

   DLLLOCAL qore_object_private(QoreObject* n_obj, const QoreClass *oc, QoreProgram* p, QoreHashNode* n_data);

   DLLLOCAL ~qore_object_private() {
      assert(!pgm);
      assert(!data);
      assert(!privateData);
#ifdef DO_OBJ_RECURSIVE_CHECK
      assert(!rset);
#endif
   }

   DLLLOCAL void plusEquals(const AbstractQoreNode* v, AutoVLock& vl, ExceptionSink* xsink) {
      if (!v)
         return;

      // do not need ensure_unique() for objects
      if (v->getType() == NT_OBJECT) {
         ReferenceHolder<QoreHashNode> h(const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(v))->copyData(xsink), xsink);
         if (h)
            merge(*h, vl, xsink);
      }
      else if (v->getType() == NT_HASH)
         merge(reinterpret_cast<const QoreHashNode* >(v), vl, xsink);
   }

   DLLLOCAL void merge(const QoreHashNode* h, AutoVLock& vl, ExceptionSink* xsink);

   DLLLOCAL int getLValue(const char* key, LValueHelper& lvh, bool internal, bool for_remove, ExceptionSink* xsink) const;

   DLLLOCAL AbstractQoreNode* *getMemberValuePtr(const char* key, AutoVLock *vl, const QoreTypeInfo*& typeInfo, ExceptionSink* xsink) const;

   DLLLOCAL QoreStringNode* firstKey(ExceptionSink* xsink) {
      QoreAutoVarRWReadLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return 0;
      }

      if (qore_class_private::runtimeCheckPrivateClassAccess(*theclass)) {
         const char* str = data->getFirstKey();
         //printd(5, "qore_object_private::firstKey() got %p (%s)\n", str, str ? str : "<null>");
         return !str ? 0 : new QoreStringNode(str);
      }

      // get first public member
      ConstHashIterator hi(data);
      while (hi.next()) {
         //printd(5, "qore_object_private::firstKey() checking '%s'\n", hi.getKey());
         if (!checkMemberAccessIntern(hi.getKey(), false, false))
            return new QoreStringNode(hi.getKey());
         //printd(5, "qore_object_private::firstKey() skipping '%s' (private)\n", hi.getKey());
      }

      return 0;
   }

   DLLLOCAL QoreStringNode* lastKey(ExceptionSink* xsink) {
      QoreAutoVarRWReadLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return 0;
      }

      if (qore_class_private::runtimeCheckPrivateClassAccess(*theclass)) {
         const char* str = data->getLastKey();
         return !str ? 0 : new QoreStringNode(str);
      }

      // get first public member
      ReverseConstHashIterator hi(data);
      while (hi.next()) {
         if (!checkMemberAccessIntern(hi.getKey(), false, false))
            return new QoreStringNode(hi.getKey());
      }

      return 0;
   }

   DLLLOCAL QoreHashNode* getSlice(const QoreListNode* l, ExceptionSink* xsink) const {
      QoreSafeVarRWReadLocker sl(rml);

      if (status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, theclass->getName());
	 return 0;
      }

      bool has_public_members = theclass->runtimeHasPublicMembersInHierarchy();
      bool private_access_ok = qore_class_private::runtimeCheckPrivateClassAccess(*theclass);

      // check key list if necessary
      if (has_public_members || !private_access_ok) {
	 ReferenceHolder<QoreListNode> nl(new QoreListNode, xsink);
	 ReferenceHolder<QoreListNode> mgl(theclass->hasMemberGate() ? new QoreListNode : 0, xsink);

	 ConstListIterator li(l);
	 while (li.next()) {
	    QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
	    if (*xsink)
	       return 0;

	    int rc = checkMemberAccessIntern(key->getBuffer(), has_public_members, private_access_ok);
	    if (!rc)
	       nl->push(new QoreStringNode(*key));
	    else {
	       if (theclass->hasMemberGate())
	          mgl->push(new QoreStringNode(*key));
	       else if (rc == QOA_PUB_ERROR) {
	          doPublicException(key->getBuffer(), xsink);
	          return 0;
	       }
	       else {
	          doPrivateException(key->getBuffer(), xsink);
	          return 0;
	       }
	    }
	 }

	 ReferenceHolder<QoreHashNode> rv(data->getSlice(*nl, xsink), xsink);
	 if (*xsink)
	    return 0;
	 if (mgl && !mgl->empty()) {
	    // unlock lock and execute memberGate() method for each method in the memger gate list (mgl)
	    sl.unlock();

	    ConstListIterator mgli(*mgl);
	    while (mgli.next()) {
	       const QoreStringNode* k = reinterpret_cast<const QoreStringNode*>(mgli.getValue());
	       AbstractQoreNode* n = theclass->evalMemberGate(obj, k, xsink);
	       if (*xsink)
	          return 0;
	       rv->setKeyValue(k->getBuffer(), n, xsink);
	    }
	 }
	 return rv.release();
      }

      return data->getSlice(l, xsink);
   }

   DLLLOCAL int checkMemberAccessIntern(const char* mem, bool has_public_members, bool private_access_ok) const {
      // check public access
      if (has_public_members) {
	 bool priv_member;
	 if (!theclass->isPublicOrPrivateMember(mem, priv_member))
	    return QOA_PUB_ERROR;

	 if (priv_member && !private_access_ok)
	    return QOA_PRIV_ERROR;

	 return QOA_OK;
      }

      // if accessed outside the class and the member is a private member
      return (!private_access_ok && theclass->isPrivateMember(mem)) ? QOA_PRIV_ERROR : QOA_OK;
   }

   DLLLOCAL int checkMemberAccess(const char* mem) const {
      // check public access
      if (theclass->runtimeHasPublicMembersInHierarchy()) {
	 bool priv_member;
         const qore_class_private* qc = qore_class_private::isPublicOrPrivateMember(*theclass, mem, priv_member);
	 if (!qc)
	    return QOA_PUB_ERROR;

	 if (priv_member && !qc->runtimeCheckPrivateClassAccess())
	    return QOA_PRIV_ERROR;

	 return QOA_OK;
      }

      // if accessed outside the class and the member is a private member
      return (!qore_class_private::runtimeCheckPrivateClassAccess(*theclass) && theclass->isPrivateMember(mem)) ? QOA_PRIV_ERROR : QOA_OK;
   }

   DLLLOCAL int checkMemberAccess(const char* mem, ExceptionSink* xsink) const {
      int rc = checkMemberAccess(mem);
      if (!rc)
	 return 0;

      if (rc == QOA_PRIV_ERROR)
	 doPrivateException(mem, xsink);
      else
	 doPublicException(mem, xsink);
      return -1;
   }

   DLLLOCAL int checkMemberAccessGetTypeInfo(ExceptionSink* xsink, const char* mem, const QoreTypeInfo*& typeInfo, bool check_access = true) const {
      bool priv;
      if (theclass->runtimeGetMemberInfo(mem, typeInfo, priv)) {
	 if (priv && check_access && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass)) {
	    doPrivateException(mem, xsink);
	    return -1;
	 }

	 return 0;
      }

      // member is not declared
      if (theclass->runtimeHasPublicMembersInHierarchy()) {
	 doPublicException(mem, xsink);
	 return -1;
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* takeMember(ExceptionSink* xsink, const char* mem, bool check_access = true);

   DLLLOCAL AbstractQoreNode* takeMember(LValueHelper& lvh, const char* mem);

   DLLLOCAL void takeMembers(QoreLValueGeneric& rv, LValueHelper& lvh, const QoreListNode* l);

   // lock not held on entry
   DLLLOCAL void doDeleteIntern(ExceptionSink* xsink) {
      printd(5, "qore_object_private::doDeleteIntern() execing destructor() obj=%p\n", obj);

      // increment reference count temporarily for destructor
      {
	 AutoLocker slr(ref_mutex);
	 ++obj->references;
      }

      theclass->execDestructor(obj, xsink);

      QoreHashNode* td;
      {
         QoreAutoVarRWWriteLocker al(rml);
	 assert(status != OS_DELETED);
	 assert(data);
	 status = OS_DELETED;
	 td = data;
	 data = 0;

#ifdef DO_OBJ_RECURSIVE_CHECK
         removeInvalidateRSet();
#endif
      }

      cleanup(xsink, td);

      obj->deref(xsink);
   }

   DLLLOCAL void cleanup(ExceptionSink* xsink, QoreHashNode* td) {
      if (privateData) {
         printd(5, "qore_object_private::cleanup() this: %p privateData: %p\n", this, privateData);
	 delete privateData;
#ifdef DEBUG
	 privateData = 0;
#endif
      }

      {
         QoreAutoVarRWWriteLocker al(rml);

         if (pgm) {
            if (pgm_ref) {
               printd(5, "qore_object_private::cleanup() obj=%p (%s) calling QoreProgram::depDeref() (%p)\n", obj, theclass->getName(), pgm);
               //pgm->depDeref(xsink);
               pgm->deref(xsink);
            }
            pgm = 0;
         }
      }

      td->deref(xsink);
   }

   DLLLOCAL void derefProgramCycle(QoreProgram* p) {
      QoreAutoVarRWWriteLocker al(rml);

      if (pgm && pgm_ref) {
         //pgm->depDeref(0);
         pgm->deref(0);
         pgm_ref = 0;
      }
   }

   // this method is called when there is an exception in a constructor and the object should be deleted
   DLLLOCAL void obliterate(ExceptionSink* xsink) {
      printd(5, "qore_object_private::obliterate() obj=%p class=%s %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::obliterate() obj=%p class=%s: references %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);
#endif

      {
	 AutoLocker slr(ref_mutex);
	 if (--obj->references)
	    return;
      }

      {
         QoreSafeVarRWWriteLocker sl(rml);

	 if (in_destructor || status != OS_OK) {
	    printd(5, "qore_object_private::obliterate() obj=%p data=%p in_destructor=%d status=%d\n", obj, data, in_destructor, status);
	    //printd(5, "Object lock %p unlocked (safe)\n", &rml);
	    sl.unlock();
	    tDeref();
	    return;
	 }

	 //printd(5, "Object lock %p locked   (safe)\n", &rml);
	 printd(5, "qore_object_private::obliterate() obj=%p class=%s\n", obj, theclass->getName());

	 status = OS_DELETED;
	 QoreHashNode* td = data;
	 data = 0;

#ifdef DO_OBJ_RECURSIVE_CHECK
         removeInvalidateRSet();
#endif

	 //printd(5, "Object lock %p unlocked (safe)\n", &rml);
	 sl.unlock();

	 if (privateData)
	    privateData->derefAll(xsink);

	 cleanup(xsink, td);
      }
      tDeref();
   }

   DLLLOCAL void doPrivateException(const char* mem, ExceptionSink* xsink) const {
      xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, theclass->getName());
   }

   DLLLOCAL void doPublicException(const char* mem, ExceptionSink* xsink) const {
      xsink->raiseException("INVALID-MEMBER", "'%s' is not a registered member of class '%s'", mem, theclass->getName());
   }

   DLLLOCAL void tRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::tRef() obj=%p class=%s: tref %d->%d\n", obj, theclass->getName(), tRefs.reference_count(), tRefs.reference_count() + 1);
#endif
      tRefs.ROreference();
   }

   DLLLOCAL void tDeref() {
#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::tDeref() obj=%p class=%s: tref %d->%d\n", obj, status == OS_OK ? theclass->getName() : "<deleted>", tRefs.reference_count(), tRefs.reference_count() - 1);
#endif
      if (tRefs.ROdereference())
	 delete obj;
   }

   DLLLOCAL void setPrivate(qore_classid_t key, AbstractPrivateData* pd) {
      if (!privateData)
         privateData = new KeyList();
      //printd(5, "qore_object_private::setPrivate() this: %p 2:privateData: %p (%s) key: %d pd: %p\n", this, privateData, theclass->getName(), key, pd);
      privateData->insert(key, pd);
      addVirtualPrivateData(key, pd);
   }

   // add virtual IDs for private data to class list
   DLLLOCAL void addVirtualPrivateData(qore_classid_t key, AbstractPrivateData* apd) {
      // first get parent class corresponding to "key"
      QoreClass* qc = theclass->getClass(key);
      //printd(5, "qore_object_private::addVirtualPrivateData() this: %p privateData: %p key: %d apd: %p qc: %p '%s'\n", this, privateData, key, apd, qc, qc->getName());
      assert(qc);
      BCSMList* sml = qc->getBCSMList();
      //printd(5, "qore_object_private::addVirtualPrivateData() this: %p qc: %p '%s' sml: %p\n", this, qc, qc->getName(), sml);
      if (!sml)
	 return;

      for (class_list_t::const_iterator i = sml->begin(), e = sml->end(); i != e; ++i) {
         //printd(5, "qore_object_private::addVirtualPrivateData() this: %p i: %p '%s' key: %d virt: %s\n", this, i->first, i->first->getName(), i->first->getID(), i->second ? "true" : "false");
	 if (i->second)
	    privateData->insertVirtual(i->first->getID(), apd);
      }
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   DLLLOCAL void setRSet(ObjectRSet* rs, int rcnt) {
      assert(rml.checkRSectionExclusive());
      printd(QRO_LVL, "qore_object_private::setRSet() this: %p obj: %p (%s) rs: %p rcnt: %d\n", this, obj, obj->getClassName(), rs, rcnt);
      if (rset) {
         // invalidating the rset removes the weak references to all contained objects
         rset->invalidate();
         rset->deref();
      }
      rset = rs;
      rcount = rcnt;
      if (rs) {
         rs->ref();
         // we make a weak reference from the rset to the object to ensure that it does not disappear while the rset is valid
         tRef();
      }
      // increment transaction count
      ++rcycle;
   }

   DLLLOCAL void invalidateRSet() {
      assert(rml.checkRSectionExclusive());
      // invalidating the rset removes the weak references to all contained objects
      if (rset)
         rset->invalidate();
    }

   DLLLOCAL void removeInvalidateRSet() {
      assert(rml.checkRSectionExclusive());
      if (rset) {
         // invalidating the rset removes the weak references to all contained objects
         rset->invalidate();
         rset->deref();
         rset = 0;
         rcount = 0;
      }
   }
#endif

#ifdef DO_OBJ_RECURSIVE_CHECK
   DLLLOCAL unsigned getObjectCount();

   DLLLOCAL void incObjectCount(int dt);
#endif

   DLLLOCAL static qore_object_private* get(QoreObject& obj) {
      return obj.priv;
   }

   DLLLOCAL static AbstractQoreNode* takeMember(QoreObject& obj, ExceptionSink* xsink, const char* mem, bool check_access = true) {
      return obj.priv->takeMember(xsink, mem, check_access);
   }

   DLLLOCAL static AbstractQoreNode* takeMember(QoreObject& obj, LValueHelper& lvh, const char* mem) {
      return obj.priv->takeMember(lvh, mem);
   }

   DLLLOCAL static void takeMembers(QoreObject& o, QoreLValueGeneric& rv, LValueHelper& lvh, const QoreListNode* l) {
      o.priv->takeMembers(rv, lvh, l);
   }

   DLLLOCAL static int getLValue(const QoreObject& obj, const char* key, LValueHelper& lvh, bool internal, bool for_remove, ExceptionSink* xsink) {
      return obj.priv->getLValue(key, lvh, internal, for_remove, xsink);
   }

   DLLLOCAL static AbstractQoreNode* *getMemberValuePtr(const QoreObject* obj, const char* key, AutoVLock *vl, const QoreTypeInfo*& typeInfo, ExceptionSink* xsink) {
      return obj->priv->getMemberValuePtr(key, vl, typeInfo, xsink);
   }

   DLLLOCAL static void plusEquals(QoreObject* obj, const AbstractQoreNode* v, AutoVLock& vl, ExceptionSink* xsink) {
      obj->priv->plusEquals(v, vl, xsink);
   }

   DLLLOCAL static void derefProgramCycle(QoreObject* obj, QoreProgram* p) {
      obj->priv->derefProgramCycle(p);
   }

   DLLLOCAL static QoreStringNode* firstKey(QoreObject* obj, ExceptionSink* xsink) {
      return obj->priv->firstKey(xsink);
   }

   DLLLOCAL static QoreStringNode* lastKey(QoreObject* obj, ExceptionSink* xsink) {
      return obj->priv->lastKey(xsink);
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   DLLLOCAL static unsigned getObjectCount(const QoreObject& o) {
      return o.priv->getObjectCount();
   }

   DLLLOCAL static void incObjectCount(const QoreObject& o, int dt) {
      o.priv->incObjectCount(dt);
   }
#endif
};

class qore_object_lock_handoff_helper {
private:
   qore_object_private* pobj;
   AutoVLock& vl;

public:
   DLLLOCAL qore_object_lock_handoff_helper(qore_object_private* n_pobj, AutoVLock& n_vl) : pobj(n_pobj), vl(n_vl) {
      if (pobj->obj == vl.getObject()) {
	 assert(vl.getRWL() == &pobj->rml);
	 vl.clear();
	 return;
      }

      // reference current object
      pobj->obj->tRef();

      // unlock previous lock and release from AutoVLock structure
      vl.del();

      // lock current object
      pobj->rml.wrlock();
   }

   DLLLOCAL ~qore_object_lock_handoff_helper() {
      // unlock if lock not saved in AutoVLock structure
      if (pobj) {
	 //printd(5, "Object lock %p unlocked (handoff)\n", &pobj->rml);
	 pobj->rml.unlock();
	 pobj->obj->tDeref();
      }
   }

   DLLLOCAL void stay_locked() {
      vl.set(pobj->obj, &pobj->rml);
      pobj = 0;
   }
};

class qore_object_recursive_lock_handoff_helper {
private:
   qore_object_private* pobj;
   bool locked;

public:
   DLLLOCAL qore_object_recursive_lock_handoff_helper(qore_object_private* n_pobj, AutoVLock& n_vl) : pobj(n_pobj) /*, vl(n_vl)*/ {
      // try to lock current object
      locked = !pobj->rml.trywrlock();
   }

   DLLLOCAL ~qore_object_recursive_lock_handoff_helper() {
      // unlock current object
      if (locked)
         pobj->rml.unlock();
   }

   DLLLOCAL operator bool() const {
      return locked;
   }
};

#endif
