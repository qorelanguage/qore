
#include <qore/Qore.h>
#include <qore/QoreSocket.h>

#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include <memory>
#include <deque>
#include <list>

#define DEF_PORT 8021

//#undef HTTP_HASH
#define HTTP_HASH 1

//#undef HTTP_RECV_HASH
#define HTTP_RECV_HASH 1

#ifdef HTTP_HASH
static QoreHashNode* hdr;
#else
static QoreString hdr;
#endif

static const char* msg = "test server";
static int msg_size;

class ThreadTask;
class ThreadPoolThread;

typedef std::deque<ThreadTask*> taskq_t;
typedef std::list<ThreadPoolThread*> tplist_base_t;

class tplist_t {
protected:
   tplist_base_t l;
   size_t len;

public:
   typedef tplist_base_t::iterator iterator;

   DLLLOCAL tplist_t() : len(0) {
   }

   DLLLOCAL tplist_t::iterator begin() {
      return l.begin();
   }

   DLLLOCAL tplist_t::iterator end() {
      return l.end();
   }

   DLLLOCAL ThreadPoolThread* front() {
      return l.front();
   }

   DLLLOCAL void pop_front() {
      l.pop_front();
      --len;
   }

   DLLLOCAL void push_back(ThreadPoolThread* tpt) {
      l.push_back(tpt);
      ++len;
   }

   DLLLOCAL size_t size() const {
      return len;
   }

   DLLLOCAL bool empty() const {
      return l.empty();
   }

   DLLLOCAL void erase(tplist_t::iterator i) {
      l.erase(i);
      --len;
   }

   DLLLOCAL void clear() {
      l.clear();
      len = 0;
   }
};

class ThreadTask {
protected:
   q_thread_t f;
   void* arg;

public:
   DLLLOCAL ThreadTask(q_thread_t n_f, void* a) : f(n_f), arg(a) {
   }

   DLLLOCAL ~ThreadTask() {
   }

   DLLLOCAL void run(ExceptionSink* xsink) {
      f(xsink, arg);
   }
};

class ThreadTaskHolder {
protected:
   ThreadTask* task;

public:
   DLLLOCAL ThreadTaskHolder(ThreadTask* t) : task(t) {
   }

   DLLLOCAL ~ThreadTaskHolder() {
      if (task)
	 delete task;
   }

   DLLLOCAL ThreadTask* release() {
      ThreadTask* rv = task;
      task = 0;
      return rv;
   }
};

class ThreadPool;

class ThreadPoolThread {
protected:
   int id;
   ThreadPool& tp;
   ThreadTask* task;
   QoreCondition c,
      *stopCond;
   QoreThreadLock m;
   tplist_t::iterator pos;
   bool stopflag,
      stopped;

   DLLLOCAL void finalize(ExceptionSink* xsink);

public:
   DLLLOCAL ThreadPoolThread(ThreadPool& n_tp, ExceptionSink* xsink);

   DLLLOCAL ~ThreadPoolThread() {
      delete stopCond;
   }

   DLLLOCAL void setPos(tplist_t::iterator p) {
      pos = p;
   }

   DLLLOCAL bool valid() const {
      return id != -1;
   }

   DLLLOCAL void worker(ExceptionSink* xsink);

   DLLLOCAL void stop() {
      AutoLocker al(m);
      assert(!stopflag);
      stopflag = true;
      c.signal();
      //printd(5, "ThreadPoolThread::stop() signaling stop for id %d\n", id);
   }

   DLLLOCAL void stopWait() {
      //printd(5, "ThreadPoolThread::stopWait() stopping id %d\n", id);
      assert(!stopCond);
      stopCond = new QoreCondition;

      AutoLocker al(m);
      assert(!stopflag);
      stopflag = true;
      c.signal();
   }

   DLLLOCAL void stopConfirm(ExceptionSink* xsink) {
      {
	 AutoLocker al(m);
	 assert(stopflag);
	 assert(stopCond);
	 while (!stopped)
	    stopCond->wait(m);
      }

      //printd(5, "ThreadPoolThread::stopConfirm() stopped id %d\n", id);
      finalize(xsink);
   }

   DLLLOCAL void submit(ThreadTask* t) {
      AutoLocker al(m);
      assert(!stopflag);
      assert(!task);
      task = t;
      c.signal();
   }

   DLLLOCAL int getId() const {
      return id;
   }

   DLLLOCAL tplist_t::iterator getPos() const {
      return pos;
   }
};

class ThreadPool : public AbstractPrivateData {
protected:
   int max,    // maximum number of threads in pool (if <= 0 then unlimited)
      minidle, // minimum number of idle threads
      maxidle, // maximum number of idle threads
      release_ms;  

   // mutex for atomicity
   QoreThreadLock m;

   // worker thread condition variable
   QoreCondition cond;

   // stop condition variable
   QoreCondition stopCond;

   tplist_t ah,  // allocated thread list
      fh;        // free thread list

   // quit flag
   bool quit;

   // master task queue
   taskq_t q;

   // task waiting flag
   bool waiting;

   bool stopflag,   // stop flag
      stopped,      // stopped flag
      confirm;      // confirm member thread stop

   DLLLOCAL int checkStopUnlocked(const char* m, ExceptionSink* xsink) {
      if (stopflag) {
	 xsink->raiseException("THREADPOOL-ERROR", "ThreadPool::%s() cannot be executed because the ThreadPool is being destroyed");
	 return -1;
      }
      return 0;
   }

   DLLLOCAL int addIdleWorker(ExceptionSink* xsink) {
      std::auto_ptr<ThreadPoolThread> tpth(new ThreadPoolThread(*this, xsink));
      if (!tpth->valid()) {
	 assert(*xsink);
	 return -1;
      }

      ThreadPoolThread* tpt = tpth.release();
      fh.push_back(tpt);
#ifdef DEBUG
      // set to an invalid iterator
      tpt->setPos(fh.end());
#endif
      return 0;
   }

   DLLLOCAL ThreadPoolThread* getThreadUnlocked(ExceptionSink* xsink) {
      while (!stopflag && fh.empty() && max && ah.size() == max) {
	 waiting = true;
	 cond.wait(m);
	 waiting = false;
      }

      if (stopflag)
	 return 0;

      ThreadPoolThread* tpt;

      if (!fh.empty()) {
	 tpt = fh.front();
	 fh.pop_front();
	 //printf("ThreadPool::getThreadUnlocked() got idle thread %p\n", tpt);
      }
      else {
	 std::auto_ptr<ThreadPoolThread> tpt_pt(new ThreadPoolThread(*this, xsink));
	 if (!tpt_pt->valid()) {
	    assert(*xsink);
	    return 0;
	 }
	 tpt = tpt_pt.release();
      }
      
      ah.push_back(tpt);
      tplist_t::iterator i = ah.end();
      --i;
      tpt->setPos(i);
      return tpt;
   }

public:
   DLLLOCAL ThreadPool(ExceptionSink* xsink, int n_max = 0, int n_minidle = 0, int m_maxidle = 0, int n_release_ms = 5000);

   DLLLOCAL ~ThreadPool() {
      assert(q.empty());
      assert(ah.empty());
      assert(fh.empty());
      assert(stopped);
   }

   DLLLOCAL void toString(QoreString& str) {
      AutoLocker al(m);
      
      str.sprintf("ThreadPool %p total: %d max: %d minidle: %d maxidle: %d release_ms: %d running: [", this, ah.size() + fh.size(), max, minidle, maxidle, release_ms);
      for (tplist_t::iterator i = ah.begin(), e = ah.end(); i != e; ++i) {
	 if (i != ah.begin())
	    str.concat(", ");
	 str.sprintf("%d", (*i)->getId());
      }

      str.concat("] idle: [");

      for (tplist_t::iterator i = fh.begin(), e = fh.end(); i != e; ++i) {
	 if (i != fh.begin())
	    str.concat(", ");
	 str.sprintf("%d", (*i)->getId());
      }

      str.concat(']');
   }

   // does not return until the thread pool has been stopped
   DLLLOCAL void stop() {
      AutoLocker al(m);
      if (!stopflag) {
	 stopflag = true;
	 cond.signal();
      }

      while (!stopped)
	 stopCond.wait(m);
   }

   DLLLOCAL int stopWait(ExceptionSink* xsink) {
      AutoLocker al(m);
      if (stopflag && !confirm) {
	 xsink->raiseException("THREADPOOL-ERROR", "cannot call ThreadPool::stopWait() after ThreadPool::stop() has been called since child threads have been detached and can no longer be traced");
	 return -1;
      }

      if (!stopflag) {
	 stopflag = true;
	 confirm = true;
	 cond.signal();
      }

      while (!stopped)
	 stopCond.wait(m);

      return 0;
   }

   DLLLOCAL int submit(q_thread_t f, void* arg, ExceptionSink* xsink) {
      // optimistically create the task object outside the lock
      ThreadTaskHolder task(new ThreadTask(f, arg));

      AutoLocker al(m);
      if (checkStopUnlocked("submit", xsink))
	  return -1;

      if (q.empty())
	 cond.signal();
      q.push_back(task.release());

      return 0;
   }

   DLLLOCAL void threadCounts(int& idle, int& running) {
      AutoLocker al(m);
      idle = fh.size();
      running = ah.size();
   }

   DLLLOCAL int done(ThreadPoolThread* tpt) {
      //printd(0, "ThreadPool::done() releasing thread %p\n", tpt);
      {
	 AutoLocker al(m);
	 if (!stopped && !confirm) {
	    tplist_t::iterator i = tpt->getPos();
	    ah.erase(i);
	    
	    if (!stopflag) {
	       // requeue thread if possible
	       if (fh.size() < maxidle || q.size() > fh.size()) {
		  fh.push_back(tpt);
		  if (waiting)
		     cond.signal();
		  return 0;
	       }
	    }
	 }
      }

      return -1;
   }

   DLLLOCAL void worker(ExceptionSink* xsink);
};

static void tpt_start_thread(ExceptionSink* xsink, ThreadPoolThread* tpt) {
   tpt->worker(xsink);
}

ThreadPoolThread::ThreadPoolThread(ThreadPool& n_tp, ExceptionSink* xsink) : tp(n_tp), task(0), stopCond(0), stopflag(false), stopped(false) {
   id = q_start_thread(xsink, (q_thread_t)tpt_start_thread, this);
   if (id > 0)
      tp.ref();
}

void ThreadPoolThread::worker(ExceptionSink* xsink) {
   SafeLocker sl(m);
      
   while (!stopflag) {
      if (!task) {
         //printd(5, "ThreadPoolThread::worker() id %d about to wait stopflag: %d task: %p\n", id, stopflag, task);
         c.wait(m);
         if (stopflag && !task)
            break;
      }
      
      assert(task);
      
      sl.unlock();
      task->run(xsink);
      sl.lock();
      task = 0;
      
      if (stopflag || tp.done(this))
         break;
   }

   //printd(5, "ThreadPoolThread::worker() stopping id %d: %s\n", id, stopCond ? "wait" : "after detach");

   if (stopCond) {
      stopped = true;
      stopCond->signal();
   }
   else {
      sl.unlock();
      finalize(xsink);
   }
}

void ThreadPoolThread::finalize(ExceptionSink* xsink) {
   tp.deref(xsink);
   delete this;
}

static void tp_start_thread(ExceptionSink* xsink, ThreadPool* tp) {
   tp->worker(xsink);
}

ThreadPool::ThreadPool(ExceptionSink* xsink, int n_max, int n_minidle, int n_maxidle, int n_release_ms) : 
   max(n_max), minidle(n_minidle), maxidle(n_maxidle), release_ms(n_release_ms), quit(false), waiting(false), stopflag(false), stopped(false), confirm(false) {
   if (max < 0)
      max = 0;
   if (minidle < 0)
      minidle = 0;
   if (maxidle <= 0)
      maxidle = minidle;
   
   if (q_start_thread(xsink, (q_thread_t)tp_start_thread, this) == -1) {
      assert(*xsink);
      stopped = true;
   }
}

void ThreadPool::worker(ExceptionSink* xsink) {
   SafeLocker sl(m);

   if (minidle)
      for (int i = 0; i < minidle; ++i) {
         if (addIdleWorker(xsink)) {
            xsink->handleExceptions();
            break;
         }
      }

   while (!stopflag) {
      if (q.empty()) {
	 if (release_ms && fh.size() > minidle) {
            if (cond.wait(m, release_ms) && q.empty()) {
               // timeout occurred: terminate an idle thread
               ThreadPoolThread* tpt = fh.front();
               //printd(0, "ThreadPool::worker() this: %p release_ms: %d timeout - stopping idle thread %p (minidle: %d maxidle: %d fh.size(): %ld)\n", this, release_ms, tpt, minidle, maxidle, fh.size());
               fh.pop_front();
               tpt->stop();
               continue;
            }
         }
         else
	    cond.wait(m);
      }

      if (stopflag)
         break;

      while (!q.empty()) {
         ThreadPoolThread* tpt = getThreadUnlocked(xsink);
         if (!tpt) {
            xsink->handleExceptions();
            break;
         }
         tpt->submit(q.front());
         q.pop_front();
      }

      if (minidle)
         while (fh.size() < minidle && (!max || (fh.size() + ah.size() < max))) {
            if (addIdleWorker(xsink)) {
               xsink->handleExceptions();
               break;
            }
         }
   }

   // clear tasks if any
   for (taskq_t::iterator i = q.begin(), e = q.end(); i != e; ++i)
      delete *i;

   // idle threads can be terminated with stop() in all cases
   for (tplist_t::iterator i = fh.begin(), e = fh.end(); i != e; ++i)
      (*i)->stop();

   // terminate all worker threads
   if (confirm) {
      for (tplist_t::iterator i = ah.begin(), e = ah.end(); i != e; ++i)
         (*i)->stopWait();

      sl.unlock();

      for (tplist_t::iterator i = ah.begin(), e = ah.end(); i != e; ++i)
         (*i)->stopConfirm(xsink);

      sl.lock();
   }
   else {
      for (tplist_t::iterator i = ah.begin(), e = ah.end(); i != e; ++i)
         (*i)->stop();
   }

#ifdef DEBUG
   q.clear();
   ah.clear();
   fh.clear();
#endif

   stopped = true;
   stopCond.broadcast();
}

void log(const char* fmt, ...) {
   QoreString str;

   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = str.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }

   printf("T%d: %s\n", gettid(), str.getBuffer());
}

void error(const char* fmt, ...) {
   QoreString str;

   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = str.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }

   printf("ERROR: %s\n", str.getBuffer());
   exit(1);
}

void error(int en, const char* fmt, ...) {
   QoreString str;

   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = str.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }

   printf("ERROR: %d: %s: %s\n", en, strerror(en), str.getBuffer());
   exit(1);
}

void show_socket_info(const QoreHashNode* sih) {
   const QoreStringNode* ad = reinterpret_cast<const QoreStringNode*>(sih->getKeyValue("address_desc"));
   log("accepted connection from %s", ad ? ad->getBuffer() : "unknown");
}

struct HttpTestThreadData {
   QoreSocket* ns;
#ifdef HTTP_RECV_HASH
   typedef QoreHashNode req_type_t;
#else
   typedef QoreStringNode req_type_t;
#endif
   req_type_t* req;
   bool close;

   DLLLOCAL HttpTestThreadData(QoreSocket* n_ns, req_type_t* n_req = 0) : ns(n_ns), req(n_req), close(false) {
   }   

   DLLLOCAL ~HttpTestThreadData() {
      if (req)
	 req->deref(0);

      if (ns) {
	 ns->shutdown();
	 ns->close();
	 delete ns;
      }
   }

   DLLLOCAL req_type_t* takeRequest() {
      if (!req)
	 return 0;
      req_type_t* rv = req;
      req = 0;
      return rv;
   }

   DLLLOCAL int getReq(ExceptionSink& xsink, int timeout_ms = 0) {
      if (req)
	 req->deref(0);

#ifdef HTTP_RECV_HASH
      req = ns->readHTTPHeader(&xsink, 0, timeout_ms);
#else
      req = ns->readHTTPHeaderString(&xsink, timeout_ms);
#endif
      return (bool)req;
   }

   DLLLOCAL int showReq() {
#ifdef HTTP_RECV_HASH
      // show request
      const QoreStringNode* version = (const QoreStringNode*)req->getKeyValue("http_version");
      const QoreStringNode* method = (const QoreStringNode*)req->getKeyValue("method");
      const QoreStringNode* path = (const QoreStringNode*)req->getKeyValue("path");
      const QoreStringNode* conn = 0;
      
      if (version && *version == "1.1") {
	 conn = (const QoreStringNode*)req->getKeyValue("connection");
	 if (conn && conn->bindex("close", 0) != -1)
	    close = true;
      }
      else {
	 conn = (const QoreStringNode*)req->getKeyValue("connection");
	 if (!conn || conn->bindex("Keep-Alive", 0) == -1)
	    close = true;
      }
      
      log("%s %s HTTP/%s (conn: %s close: %d)", method ? method->getBuffer() : "n/a", path ? path->getBuffer() : "n/a", version ? version->getBuffer() : "n/a", conn ? conn->getBuffer() : "n/a", close);

      return close;
#else
      qore_offset_t i = req->bindex("HTTP/", 0);
      if (i == -1) {
	 log("error: no HTTP version found in request header: %s", req->getBuffer());
	 return 1;
      }
      
      bool http11 = !strncmp(req->getBuffer() + i + 5, "1.1", 3);
      i = req->bindex("Connection: ", i + 10);
      bool close = false;
      if (i == -1) {
	 if (!http11)
	    close = true;
      }
      else
	 close = strncmp(req->getBuffer() + i + 12, "Keep-Alive", 10);

      log("??? ??? HTTP/%s (conn: ??? close: %d)", http11 ? "1.1" : "1.0", close);

      //printf("hdr: %s", req->getBuffer());
      return close;
#endif
   }

   DLLLOCAL void sendResponse(const char* msg, size_t msg_size, ExceptionSink& xsink) {
#ifdef HTTP_HASH
      // setup header for response
      ReferenceHolder<QoreHashNode> mh(hdr->copy(), &xsink);

      // add Date header
      DateTime dt;
      dt.setNow(0);
      QoreStringNode* dstr = new QoreStringNode;
      dt.format(*dstr, "Dy, DD Mon YYYY HH:mm:SS");
      dstr->concat(" GMT");
      mh->setKeyValue("Date", dstr, 0);
      mh->setKeyValue("Connection", new QoreStringNode(close ? "close" : "Keep-Alive"), 0);
      
      ns->sendHTTPResponse(&xsink, 200, "OK", "1.1", *mh, msg, msg_size);
#else
      DateTime dt;
      dt.setNow(0);
      QoreString dstr;
      dt.format(dstr, "Dy, DD Mon YYYY HH:mm:SS");
      
      QoreString hstr("HTTP/1.1 200 OK\r\n");
      hstr.concat(&hdr, &xsink);
      hstr.sprintf("Date: %s GMT\r\n", dstr.getBuffer());
      hstr.sprintf("Connection: %s\r\n", close ? "close" : "Keep-Alive");
      hstr.sprintf("Content-Length: %d\r\n\r\n%s", msg_size, msg);

      ns->send(&hstr, &xsink);
#endif
   }
};

void* op_conn_thread(void* x) {
   HttpTestThreadData* td = (HttpTestThreadData*)x;
   std::auto_ptr<HttpTestThreadData> tdh(td);

   QoreForeignThreadHelper tfth;
   ExceptionSink xsink;

   //ReferenceHolder<QoreHashNode> ph(td->ns->getPeerInfo(&xsink, false), &xsink);
   //show_socket_info(*ph);  

   if (!td->req && !td->getReq(xsink, 10))
      return 0;

   td->showReq();

   while (true) {
      td->sendResponse(msg, msg_size, xsink);

      if (td->close || xsink) {
	 //log("Connection: close (%d)", close);
	 break;
      }
      //log("Connection: Keep-Alive");
      
      if (!td->getReq(xsink, 10))
	 break;

      td->showReq();
   }

   log("terminating socket connection");
   return 0;
}

static void tp_conn_thread(ExceptionSink* xsink, void* x) {
   op_conn_thread(x);
}

int main(int argc, char *argv[]) {   
   msg_size = strlen(msg);
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   int port = DEF_PORT;
   if (argc > 1)
      port = atoi(argv[1]);

   qore_init();

   log("binding to port %d", port);

   ExceptionSink xsink;

   // setup global header for response
#ifdef HTTP_HASH
   ReferenceHolder<QoreHashNode> hh(new QoreHashNode, &xsink);
   hh->setKeyValue("Server", new QoreStringNode("Qorus-DBG-HTTP-Server/0.1"), 0);
   hh->setKeyValue("Content-Type", new QoreStringNode("text/plain"), 0);
   hdr = *hh;
#else
   hdr.concat("Server: Qorus-DBG-HTTP-Server/0.1\r\n");
   hdr.concat("Content-Type: text/plain\r\n");
#endif

   QoreSocket ss;

   if (ss.bind(port, true))
      error(errno, "error binding to port %d", port);

   if (ss.listen(120))
      error(errno, "error listening on port %d", port);

   log("bound and listening on all interfaces on port %d", port);

   ThreadPool tp(&xsink, -1, 20);

   while (true) {
      QoreSocket* ns = ss.accept(-1, &xsink);
      if (!ns) {
	 xsink.handleExceptions();
	 continue;
      }

      std::auto_ptr<HttpTestThreadData> td(new HttpTestThreadData(ns));
      tp.submit(tp_conn_thread, td.release(), &xsink);
   }

   qore_cleanup();

   pthread_attr_destroy(&attr);
   return 0;
}
