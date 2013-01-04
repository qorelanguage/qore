/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreThreadList.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#ifndef _QORE_QORETHREADLIST_H

#define _QORE_QORETHREADLIST_H

// FIXME: move to config.h or something like that
// not more than this number of threads can be running at the same time
#ifndef MAX_QORE_THREADS
#define MAX_QORE_THREADS 0x1000
#endif

class ThreadData;
class CallStack;
class CallNode;

#define QTS_AVAIL    0
#define QTS_NA       1
#define QTS_ACTIVE   2
#define QTS_RESERVED 3

#if defined(DARWIN) && MAX_QORE_THREADS > 2560 && !defined(__MAC_10_7)
// testing has revealed that darwin's pthread_create will not return an error when more than 2560 threads
// are running, however the threads are not actually started, therefore we set MAX_QORE_THREADS to 2560 on
// Darwin.  This should be much more than any program/script should need (famous last words? :-) )
// this bug is not present on 10.7.3 at least - in 10.7.3 pthread_create() returns an error after 2047
// threads have been created and therefore works reliably
#warning Darwin cannot support more than 2560 threads, MAX_QORE_THREADS set to 2560
#undef MAX_QORE_THREADS
#define MAX_QORE_THREADS 2560
#endif

class tid_node {
public:
   int tid;
   tid_node* next, *prev;

   DLLLOCAL tid_node(int ntid);
   DLLLOCAL ~tid_node();
};

// this structure holds all thread data that can be addressed with the qore tid
class ThreadEntry {
public:
   pthread_t ptid;
   tid_node* tidnode;
#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   CallStack* callStack;
#endif
   ThreadData* thread_data;
   unsigned char status;
   bool joined; // if set to true then pthread_detach should not be called on exit

   DLLLOCAL void cleanup();

   DLLLOCAL void allocate(tid_node* tn, int stat = QTS_NA);

   DLLLOCAL void activate(int tid, pthread_t n_ptid, QoreProgram* p, bool foreign = false);

   DLLLOCAL bool active() const {
      return status == QTS_ACTIVE;
   }

   DLLLOCAL bool available() const {
      return status == QTS_AVAIL;
   }
};

class QoreThreadList {
friend class QoreThreadListIterator;
friend class tid_node;
protected:
   mutable QoreThreadLock l;
   unsigned num_threads;
   ThreadEntry entry[MAX_QORE_THREADS];

   tid_node* tid_head, * tid_tail;

   // current TID to be issued next
   int current_tid;

   bool exiting;

   DLLLOCAL void releaseIntern(int tid) {
      // NOTE: cannot safely call printd here, because normally the thread_data has been deleted
      //printf("DEBUG: ThreadList.releaseIntern() TID %d terminated\n", tid);
      entry[tid].cleanup();
      if (tid)
         --num_threads;
   }

public:
   DLLLOCAL QoreThreadList() : num_threads(0), tid_head(0), tid_tail(0), current_tid(1), exiting(false) {
   }

   DLLLOCAL int get(int status = QTS_NA) {
      int tid = -1;
      AutoLocker al(l);

      if (current_tid == MAX_QORE_THREADS) {
         int i;
         // scan thread_list for free entry
         for (i = 1; i < MAX_QORE_THREADS; i++) {
            if (entry[i].available()) {
               tid = i;
               goto finish;
            }
         }
         if (i == MAX_QORE_THREADS)
            return -1;
      }
      else
         tid = current_tid++;

   finish:
      entry[tid].allocate(new tid_node(tid), status);
      ++num_threads;
      //printf("t%d cs=0\n", tid);

      return tid;
   }

   DLLLOCAL int getSignalThreadEntry() {
      AutoLocker al(l);
      entry[0].allocate(0);
      return 0;
   }

   DLLLOCAL void release(int tid) {
      AutoLocker al(l);
      releaseIntern(tid);
   }

   DLLLOCAL int releaseReserved(int tid) {
      AutoLocker al(l);
      if (entry[tid].status != QTS_RESERVED)
         return -1;

      releaseIntern(tid);
      return 0;
   }

   DLLLOCAL void activate(int tid, pthread_t ptid = pthread_self(), QoreProgram* p = 0, bool foreign = false) {
      AutoLocker al(l);
      entry[tid].activate(tid, ptid, p, foreign);
   }

   DLLLOCAL void setStatus(int tid, int status) {
      AutoLocker al(l);
      assert(entry[tid].status != status);
      entry[tid].status = status;
   }

   DLLLOCAL void deleteData(int tid);

   DLLLOCAL void deleteDataRelease(int tid);

   DLLLOCAL void deleteDataReleaseSignalThread();

   DLLLOCAL int activateReserved(int tid) {
      AutoLocker al(l);

      if (entry[tid].status != QTS_RESERVED)
         return -1;

      entry[tid].activate(tid, pthread_self(), 0, true);
      return 0;
   }

   DLLLOCAL unsigned getNumThreads() const {
      return num_threads;
   }

   DLLLOCAL unsigned cancelAllActiveThreads();

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   DLLLOCAL QoreHashNode* getAllCallStacks();

   DLLLOCAL void pushCall(CallNode* cn);

   DLLLOCAL void popCall(ExceptionSink* xsink);

   DLLLOCAL QoreListNode* getCallStackList();

   DLLLOCAL CallStack* getCallStack() {
      return entry[gettid()].callStack;
   }
#endif

};

DLLLOCAL extern QoreThreadList thread_list;

class QoreThreadListIterator : public AutoLocker {
protected:
   tid_node* w;

public:
   DLLLOCAL QoreThreadListIterator() : AutoLocker(thread_list.l), w(0) {
   }

   DLLLOCAL bool next() {
      do {
         w = w ? w->next : thread_list.tid_head;
      } while (w && (!w->tid || (thread_list.entry[w->tid].status != QTS_ACTIVE)));

      return (bool)w;
   }

   DLLLOCAL unsigned operator*() const {
      assert(w);
      return w->tid;
   }
};

#endif
