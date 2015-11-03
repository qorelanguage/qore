/*
  QoreGetOpt.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/QoreGetOpt.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

QoreGetOptNode::QoreGetOptNode(const char *n, char so, char *lo, qore_type_t at, int o)
{
   name = n  ? strdup(n) : 0;
   short_opt = so;
   long_opt = lo ? strdup(lo) : 0;
   argtype = at;
   option = o;
}

QoreGetOptNode::~QoreGetOptNode()
{
   if (name)
      free(name);
   if (long_opt)
      free(long_opt);
}

QoreGetOpt::QoreGetOpt()
{
}

QoreGetOpt::~QoreGetOpt()
{
   getopt_node_list_t::iterator i;
   while ((i = node_list.begin()) != node_list.end())
   {
      class QoreGetOptNode *n = *i;
      node_list.erase(i);
      delete n;
   }
   long_map.clear();
   short_map.clear();
}

class QoreGetOptNode *QoreGetOpt::find(const char *opt) const
{
   getopt_long_map_t::const_iterator i = long_map.find((char *)opt);
   if (i != long_map.end())
      return i->second;
   return 0;
}

class QoreGetOptNode *QoreGetOpt::find(char opt) const
{
   getopt_short_map_t::const_iterator i = short_map.find(opt);
   if (i != short_map.end())
      return i->second;
   return 0;
}

int QoreGetOpt::add(const char *name, char short_opt, char *long_opt, qore_type_t argtype, int option)
{
   // check input for validity
   if (!name || !name[0])
      return QGO_ERR_NO_NAME;
   if (!short_opt && (!long_opt || !long_opt[0]))
      return QGO_ERR_NO_OPTION;

   //printf("QoreGetOpt::add(%s, %03d (%c), %s, %d, %d)\n", name, short_opt, short_opt ? short_opt : '-', long_opt, argtype, option);
   // look for duplicate entries
   if (short_opt && find(short_opt))
      return QGO_ERR_DUP_SHORT_OPT;
   if (long_opt && find(long_opt))
      return QGO_ERR_DUP_LONG_OPT;

   class QoreGetOptNode *n = new QoreGetOptNode(name, short_opt, long_opt, argtype, option);
   if (short_opt)
      short_map[short_opt] = n;
   if (long_opt)
      long_map[n->long_opt] = n;
   node_list.push_back(n);
   
   return 0;
}

static void inline addError(class QoreHashNode *h, QoreStringNode *err)
{
   //printd(5, "addError() adding: %s\n", err->getBuffer());
   QoreListNode **v = reinterpret_cast<QoreListNode **>(h->getKeyValuePtr("_ERRORS_"));
   if (!(*v))
      (*v) = new QoreListNode();
   (*v)->push(err);
}

// private, static method
AbstractQoreNode *QoreGetOpt::parseDate(const char *val)
{
   // check for ISO-8601 or qore date formats 
   // 2006-01-01              (10)
   // 2006-01-01T10:00:00     (19)
   // 2006-01-01T10:00:00.000 (23)
   int len = strlen(val);
   
   if (len >= 10)
   {
      const char *c = strchr(val, '-');
      if (c == (val + 4))
      {
	 QoreString str(val, 4);
	 str.concat(val + 5, 2);
	 str.concat(val + 8, 2);

	 // if time component is there
	 if (len >= 19 && (val[10] == 'T' || val[10] == '-'))
	 {
	    str.concat(val + 11, 2);
	    str.concat(val + 14, 2);
	    str.concat(val + 17, 2);
	    if (len == 23)
	       str.concat(val + 19);
	 }
	 return new DateTimeNode(str.getBuffer());
      }
      // fall through to default date parse below
   }

   return new DateTimeNode(val);
}

void QoreGetOpt::doOption(class QoreGetOptNode *n, class QoreHashNode *h, const char *val)
{
   // get current value
   AbstractQoreNode **cv = h->getKeyValuePtr(n->name);

   // get a value ready
   if (n->argtype == -1)
   {
      if (*cv)
	 return;
      (*cv) = boolean_true();
      return;
   }

   // handle option values
   if (!val)
   {
      if (n->option & QGO_OPT_ADDITIVE) {
	 if (n->argtype == NT_INT)
	 {
	    if (!(*cv))
	       (*cv) = new QoreBigIntNode(1);
	    else {
	       QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(*cv);
	       b->val++;
	    }
	 }
	 else
	 {
	    if (!(*cv))
	       (*cv) = new QoreFloatNode(0.0);
	    else {
	       QoreFloatNode *f = reinterpret_cast<QoreFloatNode *>(*cv);
	       f->f++;
	    }
	 }
      }
      else if (!*cv)
	 (*cv) = boolean_true();
      return;
   }

   AbstractQoreNode *v;
   if (n->argtype == NT_STRING)
      v = new QoreStringNode(val);
   else if (n->argtype == NT_INT)
      v = new QoreBigIntNode(strtoll(val, 0, 10));
   else if (n->argtype == NT_FLOAT)
      v = new QoreFloatNode(strtod(val, 0));
   else if (n->argtype == NT_DATE)
      v = parseDate(val);
   else if (n->argtype == NT_BOOLEAN)
      v = get_bool_node((bool)strtol(val, 0, 10));
   else // default string
      v = new QoreStringNode(val);
   
   if (!(n->option & QGO_OPT_LIST_OR_ADD))
   {
      if (*cv)
	 (*cv)->deref(0);
      (*cv) = v;
      return;
   }

   if (n->option & QGO_OPT_LIST)
   {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(*cv);
      if (!(*cv)) {
	 l = new QoreListNode();
	 (*cv) = l;
      }
      //else printf("cv->getType()=%s\n", cv->getTypeName());
      l->push(v);
      return;
   }
   
   // additive
   if (*cv) 
   {
      if (n->argtype == NT_INT) {
	 QoreBigIntNode *b = reinterpret_cast<QoreBigIntNode *>(*cv);
	 b->val += reinterpret_cast<QoreBigIntNode *>(v)->val;
      }
      else { // float
	 QoreFloatNode *f = reinterpret_cast<QoreFloatNode *>(*cv);
	 f->f += reinterpret_cast<const QoreFloatNode *>(v)->f;
      }
      v->deref(0);
      return;
   }

   (*cv) = v;
}

char *QoreGetOpt::getNextArgument(QoreListNode *l, class QoreHashNode *h, unsigned &i, const char *lopt, char sopt)
{
   if (i < (l->size() - 1))
   {
      i++;
      QoreStringNode *n = dynamic_cast<QoreStringNode *>(l->retrieve_entry(i));
      if (n)
	 return (char *)n->getBuffer();
   }
   QoreStringNode *err = new QoreStringNode();
   if (lopt)
      err->sprintf("long option '--%s' requires an argument", lopt);
   else
      err->sprintf("short option '-%c' requires an argument", sopt);
   addError(h, err);
   return 0;
}

void QoreGetOpt::processLongArg(const char *arg, QoreListNode *l, class QoreHashNode *h, unsigned &i, bool modify)
{
   const char *opt;
   char *val;

   // get a copy of the argument
   QoreString vstr(arg);
   arg = vstr.getBuffer();

   // see if there is an assignment character
   char *tok = (char *)strchr(arg, '=');
   if (tok)
   {
      (*tok) = '\0';
      opt = arg;
      val = tok + 1;
   }
   else
   {  
      opt = arg;
      val = 0;
   }
   // find option
   class QoreGetOptNode *w = find(opt);
   if (!w)
   {
      QoreStringNode *err = new QoreStringNode();
      err->sprintf("unknown long option '--%s'", opt);
      addError(h, err);
      return;
   }
   bool do_modify = false;
   // if we need a value and there isn't one, then try to get the next argument in the list
   if (w->argtype && !val && (w->option & QGO_OPT_MANDATORY))
   {
      val = (char *)getNextArgument(l, h, i, opt, '\0');
      if (val && modify)
	 do_modify = true;
      if (!val)
	 return;
   }
   doOption(w, h, val);
   if (do_modify)
      l->pop_entry(--i, 0);
}

int QoreGetOpt::processShortArg(const char *arg, QoreListNode *l, class QoreHashNode *h, unsigned &i, int &j, bool modify)
{
   char opt = (arg + j)[0];
   // find option
   class QoreGetOptNode *w = find(opt);
   if (!w)
   {
      QoreStringNode *err = new QoreStringNode();
      err->sprintf("unknown short option '-%c'", opt);
      addError(h, err);
      return 0;
   }
   bool do_modify = false;
   const char *val = 0;
   if (w->argtype != -1)
   {
      if ((j < (signed)(strlen(arg) - 1))
	  && ((w->option & QGO_OPT_MANDATORY) || ((arg + j + 1)[0] == '=')))
      {
	 val = arg + j + 1;
	 if (*val == '=')
	    val++;
	 j = 0;
      }
      else if (w->option & QGO_OPT_MANDATORY)
      {
	 if (!(val = getNextArgument(l, h, i, 0, opt)))
	    return 0;
	 if (modify)
	    do_modify = true;
      }
   }
   doOption(w, h, val);
   if (do_modify)
      l->pop_entry(--i, 0);
   //printd(5, "processShortArg(%c) val=%08p %s returning %d\n", opt, val, val, !j);
   return !j;
}

QoreHashNode *QoreGetOpt::parse(QoreListNode *l, bool modify, ExceptionSink *xsink)
{
   QoreHashNode *h = new QoreHashNode();
   for (unsigned i = 0; i < l->size(); i++)
   {
      //printf("QoreGetOpt::parse() %d/%d\n", i, l->size());
      AbstractQoreNode *n = l->retrieve_entry(i);
      if (!n)
	 continue;
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(n);
      if (!str)
	 continue;

      const char *arg = str->getBuffer();
      if (arg[0] == '-')
      {
	 if (!arg[1])
	    continue;
	 if (arg[1] == '-')
	 {
	    if (!arg[2])
	       break;
	    processLongArg(arg + 2, l, h, i, modify);
	    if (modify)
	    {
	       //printd(5, "parse() opt=%s size=%d\n", arg, l->size()); 
	       l->pop_entry(i--, 0);
	       //printd(5, "parse() popped entry, size=%d\n", l->size());
	    }
	    continue;
	 }
	 int len = strlen(arg);
	 for (int j = 1; j < len; j++)
	    if (processShortArg(arg, l, h, i, j, modify))
	       break;
	 l->pop_entry(i--, 0);
      }
   }
   //printd(5, "QoreGetOpt::parse() returning h=%08p (size %d)\n", h, h->size());
   return h;
}
