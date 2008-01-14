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

static class QoreNode *f_boolean(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return boolean_false();

   if (p0->getAsInt())
      return boolean_true();
   else
      return boolean_false();
}

static class QoreNode *f_int(const QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return zero();
   return new QoreNode(p0->getAsBigInt());
}

static class QoreNode *f_float(const QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return zero_float();
   return new QoreNode(p0->getAsFloat());
}

static class QoreNode *f_string(const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNodeValueHelper str(get_param(params, 0));
   return str.takeReferencedValue();
}

static class QoreNode *f_binary(const QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return new QoreNode(new BinaryObject());

   if (p0->type == NT_BINARY)
      return p0->RefSelf();

   if (p0->type == NT_STRING) {
      QoreStringNode *str = reinterpret_cast<QoreStringNode *>(p0);
      return new QoreNode(new BinaryObject(strdup(str->getBuffer()), str->strlen()));
   }

   // convert to string and make binary object
   QoreStringValueHelper t(p0);
   TempString str(t.giveString());
   int len = str->strlen();
   return new QoreNode(new BinaryObject(str->giveBuffer(), len));
}

static class QoreNode *f_date(const QoreNode *params, ExceptionSink *xsink)
{
   DateTimeNodeValueHelper date(get_param(params, 0));
   return date.takeReferencedValue();
}

static class QoreNode *f_list(const QoreNode *params, ExceptionSink *xsink)
{
   class QoreList *l = new QoreList();
   if (num_params(params) > 1)
      l->push(params->eval(xsink));
   else
   {
      class QoreNode *p0 = get_param(params, 0);
      if (!is_nothing(p0))
	 l->push(p0->eval(xsink));
   }
   return new QoreNode(l);
}

static class QoreNode *f_hash(const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreHash());
}

static class QoreNode *f_type(const QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!p)
      return new QoreStringNode(NT_NOTHING->getName());
   else
      return new QoreStringNode(p->type->getName());
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
