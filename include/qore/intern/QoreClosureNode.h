/*
   QoreClosureNode.h

   Qore Programming Language

   Copyright 2003 - 2008 David Nichols

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

#ifndef _QORE_QORECLOSURENODE_H

#define _QORE_QORECLOSURENODE_H 

#include <map>

typedef std::map<const LocalVar *, ClosureVarValue *> cvar_map_t;

class ClosureRuntimeEnvironment {
   private:
      cvar_map_t cmap;

   public:
      DLLLOCAL ClosureRuntimeEnvironment(const lvar_set_t *vlist);
      DLLLOCAL ~ClosureRuntimeEnvironment();
      DLLLOCAL ClosureVarValue *find(const LocalVar *id);
      DLLLOCAL void del(ExceptionSink *xsink);
};

class QoreClosureNode : ResolvedCallReferenceNode
{
   private:
      QoreProgram *pgm;
      mutable ClosureRuntimeEnvironment closure_env;
      const QoreClosureParseNode *closure;

      DLLLOCAL QoreClosureNode(const QoreClosureNode&); // not implemented
      DLLLOCAL QoreClosureNode& operator=(const QoreClosureNode&); // not implemented

   protected:
      DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);
      
   public:
      DLLLOCAL QoreClosureNode(const QoreClosureParseNode *n_closure);
      DLLLOCAL virtual ~QoreClosureNode();
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;

      DLLLOCAL virtual QoreProgram *getProgram() const
      {
	 return pgm;
      }

      DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
      {
	 str.sprintf("function closure (%slambda, 0x%08p)", closure->isLambda() ? "" : "non-", this);
	 return 0;
      }

      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const
      {
	 del = true;
	 QoreString *rv = new QoreString();
	 getAsString(*rv, foff, xsink);
	 return rv;
      }

      DLLLOCAL virtual const char *getTypeName() const
      {
	 return getStaticTypeName();
      }

      DLLLOCAL bool isLambda() const { return closure->isLambda(); }

      DLLLOCAL static const char *getStaticTypeName()
      {
         return "runtime function closure";
      }
};

class QoreObjectClosureNode : ResolvedCallReferenceNode
{
   private:
      QoreObject *obj;
      mutable ClosureRuntimeEnvironment closure_env;
      const QoreClosureParseNode *closure;

      DLLLOCAL QoreObjectClosureNode(const QoreClosureNode&); // not implemented
      DLLLOCAL QoreObjectClosureNode& operator=(const QoreClosureNode&); // not implemented

   protected:
      DLLLOCAL virtual bool derefImpl(ExceptionSink *xsink);

   public:
      DLLLOCAL QoreObjectClosureNode(QoreObject *n_obj, const QoreClosureParseNode *n_closure);
      DLLLOCAL ~QoreObjectClosureNode();
      DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const;

      DLLLOCAL virtual QoreProgram *getProgram() const
      {
	 return obj->getProgram();
      }

      DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
      {
	 str.sprintf("function closure (%slambda, in object of class '%s', 0x%08p)", closure->isLambda() ? "" : "non-", obj->getClassName(), this);
	 return 0;
      }

      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const
      {
	 del = true;
	 QoreString *rv = new QoreString();
	 getAsString(*rv, foff, xsink);
	 return rv;
      }

      DLLLOCAL virtual const char *getTypeName() const
      {
	 return getStaticTypeName();
      }

      DLLLOCAL bool isLambda() const { return closure->isLambda(); }

      DLLLOCAL static const char *getStaticTypeName()
      {
         return "runtime function closure";
      }
};

#endif
