/*
  SQL/Objects/Query.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols
*/

#ifndef _QORE_SQL_OBJECTS_QUERY_H

#define _QORE_SQL_OBJECTS_QUERY_H

#include <qore/support.h>
#include <qore/intern/Variable.h>
#include <qore/intern/Context.h>

#include <string.h>
#include <ctype.h>

// query options to be binary-or'ed together in the options field
#define QO_STATIC         1
#define QO_DYNAMIC        2

#define QP_STRING         0
#define QP_QUERYREF       1
#define QP_VARREF         2

extern qore_classid_t CID_QUERY;
class QoreClass *initQueryQoreClass();

class QueryNode {
   public:
      class Query *q;
      class QueryNode *next, *prev;

      inline QueryNode(class Query *query) 
      {
	 q = query;
	 next = NULL;
      }
};

// list of all Queries
class QueryList : public QoreThreadLock
{
   private:
      class QueryNode *head, *tail;

      class QueryNode *qn findUnlocked(char *name)
      {
	 class QueryNode *w = head;
	 while (w)
	 {
	    if (!strcmp(w->q->name, name))
	       break;
	    w = w->next;
	 } 
	 return w;
      }
      class QueryNode *qn findUnlocked(class Query *q)
      {
	 class QueryNode *w = head;
	 while (w)
	 {
	    if (w->q == q)
	       break;
	    w = w->next;
	 } 
	 return w;
      }
   public:
      inline QueryList()
      {
	 head = tail = NULL;	 
      }
#ifdef DEBUG
      inline ~QueryList()
      {
	 if (head)
	    run_time_error("QueryList still has members (head=%08p, tail=%08p)", head, tail);
      }
#endif
      inline int register(class Query *q)
      {
	 rc = 0;
	 lock();
	 if (findUnlocked(q->name))
	    rc = -1;
	 else
	 {
	    class QueryNode *qn = new QueryNode(q);
	    if (tail)
	    {
	       tail->next = qn;
	       qn->prev = tail;
	    }
	    else
	    {
	       head = qn;
	       qn->prev = NULL;
	    }
	    tail = qn;
	 }
	 unlock();
	 return rc;
      }
      inline void deregister(class Query *q)
      {
	 lock();
	 class QueryNode *qn = findUnlocked(q);
#ifdef DEBUG
	 if (!qn)
	    run_time_error("can't deregister unknown query %08p \"%s\"", q, q->name);
#endif
	 if (qn->prev)
	    qn->prev->next = qn->next;
	 else // first in list
	 {
	    head = head->next;
	    if (head)
	       head->prev = NULL;
	 }
	 if (qn->next)
	    qn->next->prev = prev;
	 else
	 {
	    tail = qn->prev;
	    if (tail)
	       tail->next = NULL;
	 }
	 delete qn;
	 unlock();
      }
      inline class Query *find(char *name)
      {
	 lock();
	 class QueryNode *w = head;
	 while (w)
	 {
	    if (!strcmp(w->q->name, name))
	       break;
	    w = w->next;
	 } 
	 unlock();
	 if (w)
	    return w->q;
	 return NULL;
      }
};

class KeyNode {
   public:
      char *column;
      LocalVar *var;
      char *vcolumn;
};

class Query : public QoreReferenceCounter
{
      class KeyNode *keys;
      class QPartQoreListNode *qpl;
      class Datasource *datasource;
      class QoreString *last;
      char *name;
      int num_keys;
      int keys_allocated;
      int options;
      int registered;

      void validate(int &stc, class QList *&ql, class ExceptionSink *xsink);
      inline void setStatic() { options &= !QO_DYNAMIC; options |= QO_STATIC; }
      inline void setDynamic() { options &= !QO_STATIC; options |= QO_DYNAMIC; }
      inline void check_alloc();
      inline void new_key(char *col, LocalVar *ref, char *rcol);
      inline void delete_keys();

   public:
      int exec_count;
      inline Query(char *nme, Datasource *ds, char *qstr, int stc, ExceptionSink *xsink);
      inline ~Query();
      inline class QoreHashNode *exec(ExceptionSink *xsink);
      inline class QoreHashNode *check(ExceptionSink *xsink);
      inline void setSQL(char *qstr, ExceptionSink *xsink);
      inline void deref();
      inline int isStatic() { return options & QO_STATIC; }
      inline int isDynamic() { return options & QO_DYNAMIC; }
      inline void key(char *field, char *var, char *vfield, ExceptionSink *xsink);
};

class QNode
{
   public:
      class Query *q;
      class QNode *next;
      inline QNode(class Query *qy, QNode *n) { q = qy; next = n; };
};

class QList
{
      class QNode *head;
   public:
      inline QList() { head = NULL; }
      inline ~QList();
      inline void add(class Query *q) { head = new QNode(q, head); }
      inline int inlist(class Query *q);
};

inline QList::~QList()
{
   QNode *p = head;
   while (p)
   {
      QNode *n = p->next;
      delete p;
      p = n;
   }
}

inline int QList::inlist(class Query *q)
{
   QNode *p = head;
   while (p)
   {
      if (p->q == q)
	 return 1;
      p = p->next;
   }
   return 0;
}

class QPartNode
{
      class AbstractQoreNode *getVarValue(class ExceptionSink *xsink);
      class AbstractQoreNode **getVarValuePtr(class VLock *vl);
   public:
      class LocalVar *vref;
      char *str;
      char *field;
      class AbstractQoreNode *deflt;
      int type;
      int sflag;
      inline QPartNode(char *s);
      inline QPartNode(char *s, ExceptionSink *xsink);
      inline QPartNode(char *name, char *f, char *def, ExceptionSink *xsink);
      inline ~QPartNode();
      inline char *getStrValue();
      inline AbstractQoreNode *getNodeValue(ExceptionSink *xsink, int dyn);
      inline class QoreObject *getQuery();
};

inline QPartNode::QPartNode(char *s)
{
   type = QP_STRING;
   str = s;
   vref = NULL;
   field = NULL;
   deflt = NULL;
   sflag = 0;
}

inline QPartNode::QPartNode(char *s, ExceptionSink *xsink)
{
   type = QP_VARREF;
   vref  = getVarRefByName(s);
   if (!vref)
      xsink->raiseException("QUERY-PARAMETER-EXCEPTION", "SQL references object \"%s\" that does not exist", s);
   str   = NULL;
   field = NULL;
   deflt = NULL;
   sflag = 0;
}

inline QPartNode::QPartNode(char *name, char *f, char *def, ExceptionSink *xsink)
{
   type  = QP_QUERYREF;
   vref  = getVarRefByName(name);
   if (!vref)
      xsink->raiseException("QUERY-PARAMETER-EXCEPTION", "SQL references object \"%s\" that does not exist", name);
   str   = NULL;
   field = f;
   if (def)
      deflt = new AbstractQoreNode(def);
   sflag = 0;
}

inline QPartNode::~QPartNode()
{
   if (str)
      free(str);
   if (field)
      free(field);
   if (deflt)
      deflt->deref(NULL);
   if (vref)
      delete vref;
}

inline class AbstractQoreNode *QPartNode::getVarValue(class ExceptionSink *xsink)
{
   return vref->eval(xsink);
}

inline class AbstractQoreNode **QPartNode::getVarValuePtr(class VLock *vl)
{
   return vref->getValuePtr(vl);
}

inline class QoreObject *QPartNode::getQuery()
{
   if (type == QP_STRING || type == QP_VARREF)
      return NULL;

   class VLock vl;
   AbstractQoreNode **n = getVarValuePtr(&vl);
   if (!(*n) || (*n)->type != NT_OBJECT || (*n)->val.object->getClass()->getID() != CID_QUERY)
      return NULL;
   (*n)->val.object->ref();
   return (*n)->val.object;
}

inline char *QPartNode::getStrValue()
{
   return str;
}

inline AbstractQoreNode *QPartNode::getNodeValue(ExceptionSink *xsink, int dyn)
{
   printd(5, "QPartNode::getNodeValue() called for %s\n", str);

   if (type == QP_VARREF)
      return getVarValue(xsink);

   class VLock vl;
   AbstractQoreNode **n = getVarValuePtr(&vl);

   if (!n || !(*n) || (*n)->type != NT_OBJECT)
   {
      xsink->raiseException("METHOD-EVAL-ON-NON-OBJECT", "variable \"%s\" is not an object", str);
      return NULL;
   }

   QoreObject *obj = (*n)->val.object;
   obj->ref();
   vl.del();

   discard(obj->getClass()->evalMethod(obj, "check", NULL, xsink), xsink);
   obj->dereference(xsink);

   // in case of exception or object has deleted itself
   if (xsink->isEvent())
      return NULL;

   // now get value
   n = getVarValuePtr(&vl);
   if (!n || !(*n) || (*n)->type != NT_OBJECT)
   {
      xsink->raiseException("METHOD-EVAL-ON-NON-OBJECT", "variable \"%s\" is not an object", str);
      return NULL;
   }

   //printd(5, "QPartNode::getNodeValue(): retrieving \"%s.%s\"\n", str, field);
   AbstractQoreNode *rv;
   if (dyn)
   {
      rv = getContextObjectValue(*n, field, xsink);
      vl.del();
   }
   else
   {
      (*n)->val.object->ref();
      vl.del();
      rv = (*n)->val.object->evalMemberNoMethod(field, xsink);
      (*n)->val.object->dereference(xsink);
   }
   if (!rv && deflt)
      rv = deflt->refSelf();
   return rv;
}

class QPartList
{
      int allocated;
      void check_alloc();
      inline void addString(char *str, char *end);
      inline void addQueryReference(char *&p, ExceptionSink *xsink);
      inline void addVariableReference(char *&p, ExceptionSink *xsink);
   public:
      int len;
      QPartNode **pl;
      inline QPartQoreListNode(char *query, ExceptionSink *xsink);
      inline ~QPartQoreListNode();
      inline QoreString *getSQL(ExceptionSink *xsink, int dyn);
};

#define QP_BLOCK 10
inline void QPartQoreListNode::check_alloc()
{
   if (len == allocated)
   {
      allocated += QP_BLOCK;
      pl = (QPartNode **)realloc(pl, sizeof(QPartNode *) * allocated);
   }
}

inline void QPartQoreListNode::addString(char *str, char *end)
{
   check_alloc();
   char s = *end;
   *end = '\0';
   char *ns = strdup(str);
   printd(5, "QPartQoreListNode::addString() adding \"%s\"\n", ns);
   *end = s;
   pl[len++] = new QPartNode(ns);
}

static inline char *find_end(char *p)
{
   while (*p && *p != ')' && *p != ' ')
      p++;
   return p;
}

inline void QPartQoreListNode::addQueryReference(char *&p, ExceptionSink *xsink)
{
   check_alloc();
   char *ep;
   if (!(ep = strchr(p, ':')))
   {
      QoreString *t = new QoreString(*p);
      pl[len++] = new QPartNode(strdup(t->getBuffer()));
      delete t;
      p++;
      return;
   }
   char *fs;
   if (!(fs = strchr(p, '.')) || fs > ep)
   {
      QoreString *t = new QoreString(*p);
      pl[len++] = new QPartNode(strdup(t->getBuffer()));
      delete t;
      p++;
      return;
   }
   char *ls, s3;
   ls = find_end(p);
   s3 = *ls;
   *ls = '\0';
   char s1 = *ep;
   *ep = '\0';
   char s2 = *fs;
   *fs = '\0';
   // note: do not do a strdup() on the default parameter
   // or on the name; they will be duplicated later
   pl[len++] = new QPartNode(p + 1, strdup(fs+1), ep + 1, xsink);
   *fs = s2;
   *ep = s1;
   if (ls)
      *ls = s3;
   p = ls;
}

inline void QPartQoreListNode::addVariableReference(char *&p, ExceptionSink *xsink)
{
   check_alloc();
   char *ep = p + 1;
   while (*ep && (isalpha(*ep) || isdigit(*ep) || (*ep == '_')))
      ep++;
   char s = *ep;
   *ep = '\0';
   printd(5, "QPartQoreListNode::addVariableReference() adding \"%s\"\n", p);
   // note: do not do a strdup on p, it will be duplicated later
   pl[len++] = new QPartNode(p + 1, xsink);
   *ep = s;
   p = ep;
}

inline QPartQoreListNode::QPartQoreListNode(char *query, ExceptionSink *xsink)
{
   len = 0;
   allocated = 0;
   pl = NULL;

   char *p = query;
   char *last = query;
   printd(5, "QPartQoreListNode::QPartQoreListNode() \"%s\"\n", query);
   while (*p)
   {
      if (*p == '^' || *p == '~' || *p == '$')
      {
	 addString(last, p);
	 if (*p == '^')
	    addQueryReference(p, xsink);
	 else if (*p == '~')
	 {
	    addQueryReference(p, xsink);
	    pl[len - 1]->sflag = 1;
	 }
	 else
	    addVariableReference(p, xsink);
	 last = p;
      }
      else
	 p++;
      //if (xsink->isEvent())
      // return;
   }
   if (last != p)
      addString(last, p);
}

inline QPartQoreListNode::~QPartQoreListNode()
{
   for (int i = 0; i < len; i++)
      delete pl[i];
   free(pl);
}

inline QoreString *QPartQoreListNode::getSQL(ExceptionSink *xsink, int dyn)
{
   QoreString *str = new QoreString();
   for (int i = 0; i < len; i++)
      if (pl[i]->type == QP_STRING)
	 str->concat(pl[i]->getStrValue());
      else
      {
	 class AbstractQoreNode *n = pl[i]->getNodeValue(xsink, dyn);
	 if (xsink->isEvent())
	 {
	    if (n) n->deref(xsink);
	    delete str;
	    return NULL;
	 }
	 if (!n) continue;
	 if (pl[i]->sflag)
	    str->concat('\'');
	 if (n->type == NT_STRING)
	 {
	    printd(5, "QPartQoreListNode::getSQL() adding node value \"%s\"\n",
		   n->val.String->getBuffer());
	    str->concat(n->val.String);
	 }
	 else if (n->type == NT_LIST)
	 {
	    for (int j = 0; j < n->val.list->size(); j++)
	    {
	       AbstractQoreNode *v = n->val.list->retrieve_entry(j);
	       if (v)
	       {
		  if (v->type == NT_STRING)
		  {
		     printd(5, "QPartQoreListNode::getSQL() adding list %d/%d node value \"%s\"\n",
			    j, n->val.list->size(),
			    v->val.String->getBuffer());
		     str->concat(v->val.String);
		  }
		  else
		  {
		     AbstractQoreNode *t = v->convert(NT_STRING);
		     printd(5, "QPartQoreListNode::getSQL() adding list %d/%d node value \"%s\"\n",
			    j, n->val.list->size(),
			    t->val.String->getBuffer());
		     str->concat(t->val.String);
		     t->deref(NULL);

		  }
	       }
	       if (j != (n->val.list->size() - 1))
	       {
		  if (pl[i]->sflag)
		     str->concat('\'');
		  str->concat(", ");
		  if (pl[i]->sflag)
		     str->concat('\'');
	       }
	    }
	 }
	 else
	 {
	    AbstractQoreNode *t = n->convert(NT_STRING);
	    printd(5, "QPartQoreListNode::getSQL() adding node value \"%s\"\n",
		   t->val.String->getBuffer());
	    str->concat(t->val.String);
	    t->deref(NULL);
	 }
	 if (n) n->deref(xsink);
	 if (pl[i]->sflag)
	    str->concat('\'');	 
      }
   printd(5, "QPartQoreListNode::getSQL() (len=%d) returning \"%s\"\n", 
	  len, str->getBuffer());
   return str;
}

inline Query::Query(char *nme, class Datasource *ds, char *qstr, int stc, ExceptionSink *xsink)
{
   printd(5, "Query::Query(\"%s\", %08p, %08p=%s)\n", nme, ds, qstr, qstr);
   name = nme ? strdup(nme) : NULL;
   regustered = 0;
   datasource = ds;
   last = NULL;
   QList *l = NULL;
   num_keys = 0;
   keys_allocated = 0;
   keys = NULL;
   qpl = new QPartQoreListNode(qstr, xsink);
   if (xsink->isEvent())
   {
      delete qpl;
      qpl = NULL;
      return;
   }
   if (stc)
      setStatic();
   else
      setDynamic();
   int s = 0;
   validate(s, l, xsink);
   if (!xsink->isEvent())
   {
      if (!queryList.register(this))
	 registered = 1;
      else
	 xsink->raiseException("DUPLICATE-QUERY-NAME", "another query already exists with name \"%s\"", name);
   }

   if (l) delete l;
}

inline Query::~Query()
{
   if (name)
      free(name);
   datasource->deref();
   if (last)
      delete last;
   if (qpl)
      delete qpl;
   delete_keys();
}

inline class QoreHashNode *Query::exec(ExceptionSink *xsink)
{
   if (!qpl)
      return NULL;
   class QoreString *str = qpl->getSQL(xsink, isDynamic());
   if (xsink->isEvent())
      return NULL;
   class QoreHashNode *h = datasource->select(str->getBuffer(), xsink);
   delete str;
   return h;
}

inline class QoreHashNode *Query::check(ExceptionSink *xsink)
{
   if (!qpl)
      return NULL;
   class QoreString *str = qpl->getSQL(xsink, isDynamic());
   if (xsink->isEvent())
      return NULL;
   if (!last || (last && str->compare(last)))
   {
      if (last)
	 delete last;
      last = str;
      return datasource->select(str->getBuffer(), xsink);
   }
   delete str;
   return NULL;
}

inline void Query::setSQL(char *qstr, ExceptionSink *xsink)
{
   if (qpl)
      delete qpl;
   qpl = new QPartQoreListNode(qstr, xsink);
   if (xsink->isEvent())
   {
      delete qpl;
      qpl = NULL;
      return;
   }
   QList *l = NULL;
   int s = 0;
   validate(s, l, xsink);
   if (l) delete l;
}

inline void Query::deref()
{
   if (ROdereference())
      delete this;
}

inline void Query::delete_keys()
{
   for (int i = 0; i < num_keys; i++)
   {
      free(keys[i].column);
      delete keys[i].var;
      free(keys[i].vcolumn);
   }
   if (keys)
      free(keys);
}

#define KEYS_BLOCK 5
inline void Query::check_alloc()
{
   if (num_keys == keys_allocated)
   {
      keys_allocated += KEYS_BLOCK;
      keys = (KeyNode *)realloc(keys, sizeof(KeyNode) * keys_allocated);
   }
}

inline void Query::new_key(char *col, LocalVar *ref, char *rcol)
{
   check_alloc();
   keys[num_keys].column  = strdup(col);
   keys[num_keys].var     = ref;
   keys[num_keys].vcolumn = strdup(rcol);
   num_keys++;
}

inline void Query::key(char *field, char *var, char *vfield, ExceptionSink *xsink)
{
   QoreString str('$');
   str.concat(var);
   LocalVar *vr = getVarRefByName(str.getBuffer());
   if (!vr)
   {
      xsink->raiseException("QUERY-KEY-PARAMETER-EXCEPTION", "object \"%s\" does not exist", str.getBuffer());
      return;
   }
   new_key(field, vr, vfield);
}


#endif //_QORE_SQL_OBJECTS_QUERY_H
