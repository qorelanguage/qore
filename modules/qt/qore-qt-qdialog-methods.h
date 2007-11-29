
#include "qore-qt-widget-events.h"

#if 0
class T {
#endif


   public:
      DLLLOCAL virtual void accept() 
      { 
	 if (!m_accept) {
	    QOREQTYPE::accept();
	    return;
	 }

	 dispatch_event(qore_obj, m_accept, 0);
      }
      DLLLOCAL virtual void done(int r)
      {
	 if (!m_done) {
	    QOREQTYPE::done(r);
	    return;
	 }
	 
	 class QoreList *args = new QoreList();
	 args->push(new QoreNode((int64)r));

	 dispatch_event(qore_obj, m_done, args);
      }

      DLLLOCAL virtual void reject() 
      {
	 if (!m_reject) {
	    QOREQTYPE::reject();
	    return;
	 }

	 dispatch_event(qore_obj, m_reject, 0);
      }

      DLLLOCAL virtual void parent_accept() 
      {
	 QOREQTYPE::accept();
      }

      DLLLOCAL virtual void parent_done(int r) 
      {
	 QOREQTYPE::done(r);
      }

      DLLLOCAL virtual void parent_reject() 
      {
	 QOREQTYPE::reject();
      }

#if 0
}
#endif

