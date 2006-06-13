/*
  Namespace.cc

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

#include <qore/config.h>
#include <qore/Qore.h>
#include <qore/Namespace.h>
#include <qore/support.h>
#include <qore/common.h>
#include <qore/QoreClass.h>
#include <qore/Object.h>
#include <qore/thread.h>
#include <qore/params.h>
#include <qore/Namespace.h>
#include <qore/ErrnoConstants.h>
#include <qore/TypeConstants.h>
#include <qore/QoreProgram.h>
#include <qore/ParserSupport.h>
#include <qore/Function.h>
#include <qore/CallStack.h>
#include <qore/QoreRegexBase.h>
#include <qore/DBI.h>
#include <qore/AutoNamespaceList.h>

// include files for default object classes
#include <qore/QC_Socket.h>
#include <qore/QC_Program.h>
#include <qore/QC_File.h>
#include <qore/QC_GetOpt.h>
#include <qore/QC_FtpClient.h>

#include <qore/QoreFile.h>

#include <string.h>
#include <stdlib.h>
#include <pcre.h>

class AutoNamespaceList ANSL;

int resolveSimpleConstant(class QoreNode **node, int level)
{
   printd(5, "resolveSimpleConstant(%s, %d)\n", (*node)->val.c_str, level);

   // if constant is not found, then a parse error will be raised
   class QoreNode *rv = findConstantValue((*node)->val.c_str, level);
   if (!rv)
      return -1;

   printd(5, "resolveSimpleConstant(%s, %d) %08x %s-> %08x %s\n", 
	  (*node)->val.c_str, level, *node, (*node)->type->name, rv, rv->type->name);
   
   (*node)->deref(NULL);
   *node = rv->RefSelf();
   return 0;
}

int resolveScopedConstant(class QoreNode **node, int level)
{
   printd(5, "resolveScopedConstant(%s, %d)\n", (*node)->val.scoped_ref->ostr, level);

   // if constant is not found, then a parse error will be raised
   class QoreNode *rv = findConstantValue((*node)->val.scoped_ref, level);
   if (!rv)
      return -1;

   (*node)->deref(NULL);
   *node = rv->RefSelf();
   return 0;
}

int parseInitConstantHash(class Hash *h, int level)
{
   // cannot use an iterator here because we change the hash
   List *keys = h->getKeys();
   for (int i = 0; i < keys->size(); i++)
   {
      char *k = keys->retrieve_entry(i)->val.String->getBuffer();

      class QoreNode **value = h->getKeyValuePtr(k);

      if (parseInitConstantValue(value, level + 1))
	 return -1;

      // resolve constant references in keys
      if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST)
      {
	 QoreNode *n;
	 if (k[0] == HE_TAG_CONST)
	 {
	    n = new QoreNode(NT_BAREWORD);
	    n->val.c_str = strdup(k + 1);
	 }
	 else
	    n = new QoreNode(new NamedScope(strdup(k + 1)));
	 if (parseInitConstantValue(&n, level + 1))
	 {
	    if (n)
	       n->deref(NULL);
	    return -1;
	 }

	 if (n)
	 {
	    if (n->type != NT_STRING)
	    {
	       QoreNode *t = n;
	       n = n->convert(NT_STRING);
	       // not possible to have an exception here
	       t->deref(NULL);
	    }
	 
	    // reference value for new hash
	    (*value)->ref();
	    // not possible to have an exception here
	    h->setKeyValue(n->val.String->getBuffer(), *value, NULL);
	    // or here
	    h->deleteKey(k, NULL);
	    // or here
	    n->deref(NULL);
	 }
      }
   }
   delete keys;
   return 0;
}

int parseInitConstantValue(class QoreNode **val, int level)
{
   if (!(*val))
      return 0;

   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("maximum recursion level exceeded resolving constant definition");
      return -1;
   }

   while (true)
   {
      if ((*val)->type == NT_BAREWORD)
      {
	 if (resolveSimpleConstant(val, level + 1))
	    return -1;
      }
      else if ((*val)->type == NT_CONSTANT)
      {
	 if (resolveScopedConstant(val, level + 1))
	    return -1;
      }
      else
	 break;
   }
   if ((*val)->type == NT_LIST)
      for (int i = 0; i < (*val)->val.list->size(); i++)
      {
	 if (parseInitConstantValue((*val)->val.list->get_entry_ptr(i), level + 1))
	    return -1;
      }
   else if ((*val)->type == NT_HASH)
   {
      if (parseInitConstantHash((*val)->val.hash, level))
	 return -1;
   }
   else if ((*val)->type == NT_TREE)
   {
      if (parseInitConstantValue(&((*val)->val.tree.left), level + 1))
	 return -1;
      if ((*val)->val.tree.right)
	 if (parseInitConstantValue(&((*val)->val.tree.right), level + 1))
	    return -1;
   }
   // if it's an expression or container type, then evaluate in case it contains immediate expressions
   if ((*val)->type == NT_TREE || (*val)->type == NT_LIST || (*val)->type == NT_HASH)
   {
      class ExceptionSink xsink;
      class QoreNode *n = (*val)->eval(&xsink);
      (*val)->deref(&xsink);
      *val = n;
   }
   return 0;
}

// NamespaceList::parseResolveNamespace()
// does a recursive breadth-first search to resolve a namespace declaration
class Namespace *NamespaceList::parseResolveNamespace(class NamedScope *name, int *matched)
{
   tracein("NamespaceList::parseResolveNamespace()");

   class Namespace *w = head, *ns = NULL;

   // search first level of all sub namespaces
   while (w)
   {
      if ((ns = w->parseMatchNamespace(name, matched)))
	 break;
      w = w->next;
   }
      //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);

   if (!ns)
   {
      // now search in all sub namespace lists
      w = head;
      while (w)
      {
	 if ((ns = w->nsl->parseResolveNamespace(name, matched)))
	    break;
	 if ((ns = w->pendNSL->parseResolveNamespace(name, matched)))
	    break;
	 //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);
	 w = w->next;
      }
   }

   traceout("NamespaceList::parseResolveNamespace()");
   return ns;
}

// NamespaceList::parseFindConstantValue()
class QoreNode *NamespaceList::parseFindConstantValue(char *cname)
{
   tracein("NamespaceList::parseFindConstantValue()");
   
   class QoreNode *rv = NULL;
   class Namespace *w = head;
   // see if a match can be found at the first level
   while (w)
   {
      if ((rv = w->getConstantValue(cname)))
	 break;
      w = w->next;
   }

   if (!rv) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((rv = w->nsl->parseFindConstantValue(cname)))
	    break;
	 if ((rv = w->pendNSL->parseFindConstantValue(cname)))
	    break;
	 w = w->next;
      }
   }

   traceout("NamespaceList::parseFindConstantValue()");
   return rv;
}

/*
static inline void showNSL(class NamespaceList *nsl)
{
   printd(5, "showNSL() dumping %08x\n", nsl);
   for (int i = 0; i < nsl->num_namespaces; i++)
      printd(5, "showNSL()  %d: %08x %s (list: %08x)\n", i, nsl->nslist[i], nsl->nslist[i]->name, nsl->nslist[i]->nsl);
}
*/

// NamespaceList::parseFindScopedConstantValue()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
class QoreNode *NamespaceList::parseFindScopedConstantValue(class NamedScope *name, int *matched)
{
   class QoreNode *rv = NULL;

   tracein("NamespaceList::parseFindScopedConstantValue()");
   printd(5, "NamespaceList::parseFindScopedConstantValue(this=%08x) target: %s\n", this, name->ostr);

   //showNSL(this);
   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((rv = w->parseMatchScopedConstantValue(name, matched)))
	 break;
      w = w->next;
   }

   if (!rv) // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((rv = w->nsl->parseFindScopedConstantValue(name, matched)))
	    break;
	 if ((rv = w->pendNSL->parseFindScopedConstantValue(name, matched)))
	    break;
	 w = w->next;
      }
   }

   traceout("NamespaceList::parseFindScopedConstantValue()");
   return rv;
}

/*
// parseFindOTInNSL()
class QoreClass *parseFindOTInNSL(class NamespaceList *nsl, char *otname)
{
   class QoreClass *ot;
   // see if a match can be found at the first level
   for (int i = 0; i < nsl->num_namespaces; i++)
      if ((ot = nsl->nslist[i]->classList->find(otname)))
	 return ot;

   // check all levels
   for (int i = 0; i < nsl->num_namespaces; i++)
      if ((ot = findOTInNSL(nsl->nslist[i]->nsl, otname)))
	 return ot;

   parse_error("reference to undefined object type '%s'", otname);
   return NULL;
}
*/

// NamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
class QoreClass *NamespaceList::parseFindScopedClassWithMethod(class NamedScope *name, int *matched)
{
   QoreClass *oc = NULL;

   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched)))
	 break;
      w = w->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->pendNSL->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 if ((oc = w->nsl->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

// NamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
/*
class QoreClass *NamespaceList::parseFindScopedClassWithMethod(class NamedScope *name, int *matched, class QoreClassList **plist, bool *is_pending)
{
   QoreClass *oc = NULL;

   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched, plist, is_pending)))
	 break;
      w = w->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->pendNSL->parseFindScopedClassWithMethod(name, matched, plist, is_pending)))
	    break;
	 if ((oc = w->nsl->parseFindScopedClassWithMethod(name, matched, plist, is_pending)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}
*/

/*
class QoreClass *NamespaceList::parseFindClass(char *ocname, class QoreClassList **plist, bool *is_pending)
{
   class QoreClass *oc = NULL;

   // see if a match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      // check pending classes
      if ((oc = w->pendClassList->find(ocname)))
      {
	 (*plist) = w->classList;
	 (*is_pending) = true;
	 break;
      }
      if ((oc = w->classList->find(ocname)))
      {
	 (*plist) = w->pendClassList;
	 break;
      }
      w = w->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindClass(ocname, plist, is_pending)))
	    break;
	 if ((oc = w->pendNSL->parseFindClass(ocname, plist, is_pending)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}
*/

class QoreClass *NamespaceList::parseFindClass(char *ocname)
{
   class QoreClass *oc = NULL;

   // see if a match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->classList->find(ocname)))
	 break;
      // check pending classes
      if ((oc = w->pendClassList->find(ocname)))
	 break;

      w = w->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindClass(ocname)))
	    break;
	 if ((oc = w->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

class QoreClass *NamespaceList::parseFindChangeClass(char *ocname)
{
   class QoreClass *oc = NULL;

   // see if a match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->classList->findChange(ocname)))
	 break;
      // check pending classes
      if ((oc = w->pendClassList->find(ocname)))
	 break;

      w = w->next;
   }

   if (!oc) // check all levels
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindChangeClass(ocname)))
	    break;
	 if ((oc = w->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

// NamespaceList::parseFindScopedClass()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
class QoreClass *NamespaceList::parseFindScopedClass(class NamedScope *name, int *matched)
{
   class QoreClass *oc = NULL;

   // see if a complete match can be found at the first level
   Namespace *w = head;
   while (w)
   {
      if ((oc = w->parseMatchScopedClass(name, matched)))
	 break;
      w = w->next;
   }

   if (!oc)  // now search all sub namespaces
   {
      w = head;
      while (w)
      {
	 if ((oc = w->nsl->parseFindScopedClass(name, matched)))
	    break;
	 if ((oc = w->pendNSL->parseFindScopedClass(name, matched)))
	    break;
	 w = w->next;
      }
   }

   return oc;
}

class QoreNode *get_file_constant(class QoreClass *fc, int fd)
{
   class ExceptionSink xsink;

   class QoreNode *rv = fc->execSystemConstructor(NULL, &xsink);
   class File *f = (File *)rv->val.object->getReferencedPrivateData(CID_FILE);
   f->makeSpecial(fd);
   f->deref();

   return rv;
}

// non-zero return value means error
int addMethodToClass(class NamedScope *name, class Method *qcmethod)
{
   // find class
   //class QoreClassList *plist;
   //bool is_pending = false;
   class QoreClass *oc;

   char *cname  = name->strlist[name->elements - 2];
   char *method = name->strlist[name->elements - 1];

   // if there is no namespace specified, then just find class
   if (name->elements == 2)
   {
      oc = getRootNS()->rootFindClass(cname);
      if (!oc)
      {
	 parse_error("reference to undefined class '%s' while trying to add method '%s'", cname, method);
	 return -1;
      }
   }
   else
   {
      int m = 0;
      oc = getRootNS()->rootFindScopedClassWithMethod(name, &m);
      if (!oc)
      {
	 if (m != (name->elements - 2))
	    parse_error("cannot resolve namespace '%s' in '%s()'", name->strlist[m], name->ostr);
	 else
	    parse_error("class '%s' does not exist", cname);
	 return -1;
      }
   }

   oc->addMethod(qcmethod);

   return 0;
}

class QoreClass *parseFindScopedClass(class NamedScope *name)
{
   class QoreClass *oc;
   // if there is no namespace specified, then just find class
   if (name->elements == 1)
   {
      oc = getRootNS()->rootFindClass(name->strlist[0]);
      if (!oc)
	 parse_error("reference to undefined class '%s'", name->ostr);
   }
   else
   {
      int m = 0;
      oc = getRootNS()->rootFindScopedClass(name, &m);

      if (!oc)
	 if (m != (name->elements - 1))
	    parse_error("cannot resolve namespace '%s' in '%s()'", name->strlist[m], name->ostr);
	 else
	 {
	    QoreString err;
	    err.sprintf("cannot find class '%s' in any namespace '",
			name->getIdentifier());
	    for (int i = 0; i < (name->elements - 1); i++)
	    {
	       err.concat(name->strlist[i]);
	       if (i != (name->elements - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
   }
   printd(5, "parseFindScopedClass('%s') returning %08x\n", name->ostr, oc);
   return oc;
}

class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name)
{
   class QoreClass *oc;

   int m = 0;
   oc = getRootNS()->rootFindScopedClassWithMethod(name, &m);
   
   if (!oc)
      if (m != (name->elements - 1))
	 parse_error("cannot resolve namespace '%s' in '%s()'", name->strlist[m], name->ostr);
      else
      {
	 QoreString err;
	 err.sprintf("cannot find class '%s' in any namespace '",
		     name->getIdentifier());
	 for (int i = 0; i < (name->elements - 1); i++)
	 {
	    err.concat(name->strlist[i]);
	    if (i != (name->elements - 2))
	       err.concat("::");
	 }
	 err.concat("'");
	 parse_error(err.getBuffer());
      }
   
   printd(5, "parseFindScopedClassWithMethod('%s') returning %08x\n", name->ostr, oc);
   return oc;
}

// called in 2nd stage of parsing to resolve constant references
class QoreNode *findConstantValue(class NamedScope *name, int level)
{
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH)
   {
      parse_error("recursive constant definitions too deep resolving '%s'", name->ostr);
      return NULL;
   }

   class QoreNode *rv;

   if (name->elements == 1)
   {
      rv = getRootNS()->rootFindConstantValue(name->ostr);
      if (!rv)
      {
	 parse_error("constant '%s' cannot be resolved in any namespace", name->ostr);
	 return NULL;
      }
   }
   else
   {
      int m = 0;
      rv = getRootNS()->rootFindScopedConstantValue(name, &m);
      if (!rv)
      {
	 if (m != (name->elements - 1))
	    parse_error("cannot resolve namespace '%s' in '%s'", name->strlist[m], name->ostr);
	 else
	 {
	    QoreString err;
	    err.sprintf("cannot find constant '%s' in any namespace '", name->getIdentifier());
	    for (int i = 0; i < (name->elements - 1); i++)
	    {
	       err.concat(name->strlist[i]);
	       if (i != (name->elements - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
	 return NULL;
      }
   }
   return rv;
}

// public, only called either in single-threaded initialization or
// while the program-level parse lock is held
void Namespace::addClass(class QoreClass *oc)
{
   tracein("Namespace::addClass()");
   //printd(5, "Namespace::addClass() adding str=%s (%08x)\n", oc->name, oc);
   // raise an exception if object name collides with a namespace
   if (nsl->find(oc->name))
   {
      parse_error("class name '%s' collides with previously-defined namespace '%s'", oc->name, oc->name);
      oc->deref();
   }
   else if (pendNSL->find(oc->name))
   {
      parse_error("class name '%s' collides with pending namespace '%s'", oc->name, oc->name);
      oc->deref();
   }
   else if (classList->find(oc->name))
   {
      parse_error("class '%s' already exists in namespace '%s'", oc->name, name);
      oc->deref();
   }
   else if (pendClassList->add(oc))
   {
      parse_error("class '%s' is already pending in namespace '%s'", oc->name, name);
      oc->deref();
   }
   traceout("Namespace::addClass()");
}

void Namespace::assimilate(class Namespace *ns)
{
   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   pendConstant->assimilate(ns->pendConstant, constant, name);

   // assimilate classes
   pendClassList->assimilate(ns->pendClassList, classList, nsl, pendNSL, name);

   // assimilate sub namespaces
   Namespace *nw = ns->pendNSL->head;
   while (nw)
   {
      // throw parse exception if name is already defined
      if (nsl->find(nw->name))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'",
		     nw->name, name);
      else if (pendNSL->find(nw->name))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'",
		     nw->name, name);
      else if (classList->find(nw->name))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     nw->name, name);
      else if (pendClassList->find(nw->name))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     nw->name, name);

      nw = nw->next;
   }
   // assimilate target list
   pendNSL->assimilate(ns->pendNSL);

   // delete source namespace
   delete ns;
}

// Namespace::parseMatchNamespace()
// will only be called if there is a match with the name and nscope->elements > 1
class Namespace *Namespace::parseMatchNamespace(class NamedScope *nscope, int *matched)
{
   // see if starting name matches this namespace
   if (!strcmp(nscope->strlist[0], name))
   {
      class Namespace *ns = this;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // check for a match of the structure in this namespace
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    break;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
      return ns;
   }

   return NULL;
}

#define NS_BLOCK 5
NamedScope::NamedScope(char *str)
{
   allocated = 0;
   elements  = 0;
   strlist = NULL;
   ostr = str;

   while (char *p = strstr(str, "::"))
   {
      // resize array if needed
      if (elements == allocated)
      {
	 allocated += NS_BLOCK;
	 strlist = (char **)realloc(strlist, sizeof(char *) * allocated);
      }
      strlist[elements] = (char *)malloc(sizeof(char) * (p - str + 1));
      strncpy(strlist[elements], str, (p - str));
      strlist[elements][p - str] = '\0';
      elements++;
      str = p + 2;
   }
   // add last field
   // resize array if needed
   if (elements == allocated)
   {
      allocated++;
      strlist = (char **)realloc(strlist, sizeof(char *) * allocated);
   }
   strlist[elements] = (char *)malloc(sizeof(char) * (strlen(str) + 1));
   strcpy(strlist[elements], str);
   elements++;
}

/*
class QoreClass *Namespace::parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched, class QoreClassList **plist, bool *is_pending)
{
   printd(5, "Namespace::parseMatchScopedClassWithMethod(this=%08x) %s class=%s (%s)\n", this, name, nscope->strlist[nscope->elements - 2], nscope->ostr);

   Namespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      // if first namespace doesn't match, then return NULL
      if (strcmp(nscope->strlist[0], name))
	 return NULL;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++)
      {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   class QoreClass *rv = ns->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (rv)
   {
      (*plist) = ns->classList;
      (*is_pending) = true;
   }
   else if ((rv = ns->classList->find(nscope->strlist[nscope->elements - 2])))
   {
      (*plist) = ns->pendClassList;
   }
   return rv;
}
*/

class QoreClass *Namespace::parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched)
{
   printd(5, "Namespace::parseMatchScopedClassWithMethod(this=%08x) %s class=%s (%s)\n", this, name, nscope->strlist[nscope->elements - 2], nscope->ostr);

   Namespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      // if first namespace doesn't match, then return NULL
      if (strcmp(nscope->strlist[0], name))
	 return NULL;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++)
      {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   class QoreClass *rv = ns->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (!rv)
      rv = ns->classList->find(nscope->strlist[nscope->elements - 2]);

   return rv;
}


class QoreClass *Namespace::parseMatchScopedClass(class NamedScope *nscope, int *matched)
{
   if (strcmp(nscope->strlist[0], name))
      return NULL;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   printd(5, "Namespace::parseMatchScopedClass() matched %s in %s\n", name, nscope->ostr);

   Namespace *ns = this;
   
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
   {
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   class QoreClass *rv = ns->classList->find(nscope->strlist[nscope->elements - 1]);
   if (!rv)
      rv = ns->pendClassList->find(nscope->strlist[nscope->elements - 1]);
   return rv;
}

class QoreNode *Namespace::parseMatchScopedConstantValue(class NamedScope *nscope, int *matched)
{
   printd(5, "Namespace::parseMatchScopedConstantValue() trying to find %s in %s (%08x)\n", 
	  nscope->getIdentifier(), name, getConstantValue(nscope->getIdentifier()));

   if (strcmp(nscope->strlist[0], name))
      return NULL;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   class Namespace *ns = this;

   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2)
      for (int i = 1; i < (nscope->elements - 1); i++)
      {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return NULL;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }

   return ns->getConstantValue(nscope->getIdentifier());
}

class Hash *Namespace::getConstantInfo()
{
   return constant->getInfo();
}

class Hash *Namespace::getClassInfo()
{
   return classList->getInfo();
}

// returns a hash of namespace information
class Hash *Namespace::getInfo()
{
   class Hash *h = new Hash();

   h->setKeyValue("constants", new QoreNode(getConstantInfo()), NULL);
   h->setKeyValue("classes", new QoreNode(getClassInfo()), NULL);

   if (nsl->head)
   {
      class Hash *nsh = new Hash();
      
      class Namespace *w = nsl->head;
      while (w)
      {
	 nsh->setKeyValue(w->name, new QoreNode(w->getInfo()), NULL);
	 w = w->next;
      }

      h->setKeyValue("subnamespaces", new QoreNode(nsh), NULL);
   }

   return h;
}

// sets up the initial Qore namespaces
class Namespace *getRootNamespace(class Namespace **QoreNS)
{
   tracein("getRootNamespace");

   class Namespace *qns = new Namespace("Qore");

   class QoreClass *File;
   // add system object types
   qns->addSystemClass(initSocketClass());
   qns->addSystemClass(initProgramClass());
   qns->addSystemClass(File = initFileClass());
   qns->addSystemClass(initGetOptClass());
   qns->addSystemClass(initFtpClientClass());

   qns->addInitialNamespace(get_thread_ns());

   // add boolean constants for true and false
   qns->addConstant("True",          boolean_true());
   qns->addConstant("False",         boolean_false());

   // add File object constants for stdin (0), stdout (1), stderr (2)
   qns->addConstant("stdin",         get_file_constant(File, 0));
   qns->addConstant("stdout",        get_file_constant(File, 1));
   qns->addConstant("stderr",        get_file_constant(File, 2));

   // add constants for exception types
   qns->addConstant("ET_System",     new QoreNode("System"));
   qns->addConstant("ET_User",       new QoreNode("User"));

   // create constants for call types
   qns->addConstant("CT_User",       new QoreNode((int64)CT_USER));
   qns->addConstant("CT_Builtin",    new QoreNode((int64)CT_BUILTIN));
   qns->addConstant("CT_NewThread",  new QoreNode((int64)CT_NEWTHREAD));
   qns->addConstant("CT_Rethrow",    new QoreNode((int64)CT_RETHROW));

   // create constants for version information
   qns->addConstant("VersionString", new QoreNode(qore_version_string));
   qns->addConstant("VersionMajor",  new QoreNode((int64)qore_version_major));
   qns->addConstant("VersionMinor",  new QoreNode((int64)qore_version_minor));
   qns->addConstant("VersionSub",    new QoreNode((int64)qore_version_sub));
   qns->addConstant("Build",         new QoreNode((int64)qore_build_number));

   // add constants for regex() function options
   qns->addConstant("RE_Caseless",   new QoreNode((int64)PCRE_CASELESS));
   qns->addConstant("RE_DotAll",     new QoreNode((int64)PCRE_DOTALL));
   qns->addConstant("RE_Extended",   new QoreNode((int64)PCRE_EXTENDED));
   qns->addConstant("RE_MultiLine",  new QoreNode((int64)PCRE_MULTILINE));
   // note that the following constant is > 23-bits
   qns->addConstant("RE_Global",     new QoreNode((int64)QRE_GLOBAL));

   // create Qore::SQL namespace
   qns->addInitialNamespace(getSQLNamespace());

   // create get Qore::Err namespace with ERRNO constants
   qns->addInitialNamespace(get_errno_ns());

   // create Qore::Type namespace with type constants
   qns->addInitialNamespace(get_type_ns());

   // add file constants
   addFileConstants(qns);

   // add parse option constants to Qore namespace
   addProgramConstants(qns);

   class Namespace *rns = new Namespace("");
   rns->addInitialNamespace(qns);

   // add all changes in loaded modules
   ANSL.init(rns, qns);

   *QoreNS = qns;
   traceout("getRootNamespace");
   return rns;
}
