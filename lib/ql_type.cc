/*
  type.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/intern/ql_type.h>

static class QoreNode *f_boolean(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return boolean_false();

   if (p0->getAsInt())
      return boolean_true();
   else
      return boolean_false();
}

static class QoreNode *f_int(const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p0 ? p0->getAsBigInt() : 0);
}

static class QoreNode *f_float(const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   return new QoreFloatNode(p0 ? p0->getAsFloat() : 0.0);
}

static class QoreNode *f_string(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeValueHelper str(get_param(params, 0));
   return str.takeReferencedValue();
}

static class QoreNode *f_binary(const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return new BinaryNode();

   if (p0->type == NT_BINARY)
      return p0->RefSelf();

   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(p0);
      if (str)
	 return new BinaryNode(strdup(str->getBuffer()), str->strlen());
   }

   // convert to string and make binary object
   QoreStringValueHelper t(p0);
   TempString str(t.giveString());
   int len = str->strlen();
   return new BinaryNode(str->giveBuffer(), len);
}

static class QoreNode *f_date(const QoreListNode *params, ExceptionSink *xsink)
{
   DateTimeNodeValueHelper date(get_param(params, 0));
   return date.takeReferencedValue();
}

static class QoreNode *f_list(const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreListNode *l = new QoreListNode();
   if (num_params(params) > 1)
      l->push(params->eval(xsink));
   else
   {
      class QoreNode *p0 = get_param(params, 0);
      if (!is_nothing(p0))
	 l->push(p0->eval(xsink));
   }
   return l;
}

static class QoreNode *f_hash(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreHashNode();
}

static class QoreNode *f_type(const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!p)
      return new QoreStringNode(NT_NOTHING->getName());
   else
      return new QoreStringNode(p->getTypeName());
}

void init_type_functions()
{
   builtinFunctions.add("boolean", f_boolean);
   builtinFunctions.add("int", f_int);
   builtinFunctions.add("float", f_float);
   builtinFunctions.add("string", f_string);
   builtinFunctions.add("date", f_date);
   builtinFunctions.add("binary", f_binary);
   builtinFunctions.add("list", f_list);
   builtinFunctions.add("hash", f_hash);
   builtinFunctions.add("type", f_type);
   builtinFunctions.add("typename", f_type);
}
