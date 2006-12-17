/*
  lib/ql_xml.cc

  Qore JSON (JavaScript Object Notation) functions

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
#include <qore/QoreNode.h>
#include <qore/Object.h>
#include <qore/support.h>
#include <qore/params.h>
#include <qore/charset.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/ql_json.h>

static int doJSONValue(class QoreString *str, class QoreNode *v, int format, class ExceptionSink *xsink)
{
   if (is_nothing(v))
   {
      str->concat("Null");
      return 0;
   }
   if (v->type == NT_LIST)
   {
      str->concat("[ ");
      ListIterator li(v->val.list);
      while (li.next())
      {
	 if (doJSONValue(str, li.getValue(), format, xsink))
	    return -1;
	 if (!li.last())
	    str->concat(", ");
      }
      str->concat(" ]");
      return 0;
   }
   if (v->type == NT_HASH)
   {
      str->concat("{ ");
      HashIterator hi(v->val.hash);
      while (hi.next())
      {
	 str->sprintf("\"%s\" : ", hi.getKey());
	 if (doJSONValue(str, hi.getValue(), format, xsink))
	    return -1;
	 if (!hi.last())
	    str->concat(", ");
      }
      str->concat(" }");
      return 0;
   }
   if (v->type == NT_STRING)
   {
      // convert encoding if necessary
      QoreString *t;
      if (v->val.String->getEncoding() != str->getEncoding())
      {
	 t = v->val.String->convertEncoding(str->getEncoding(), xsink);
	 if (!t)
	 {
	    delete str;
	    return -1;
	 }
      }
      else
	 t = v->val.String;

      str->concat('"');
      str->concatEscape(t, '"');
      str->concat('"');
      if (t != v->val.String)
	 delete t;
      return 0;
   }
   if (v->type == NT_INT)
   {
      str->sprintf("%lld", v->val.intval);
      return 0;
   }
   if (v->type == NT_FLOAT)
   {
      str->sprintf("%.9g", v->val.floatval);
      return 0;
   }
   if (v->type == NT_BOOLEAN)
   {
      if (v->val.boolval)
	 str->concat("true");
      else
	 str->concat("false");
      return 0;
   }
   if (v->type == NT_DATE)  // this will be serialized as a string
   {
      str->concat('"');
      v->val.date_time->getString(str);
      str->concat('"');
      return 0;
   }
   
   delete str;
   xsink->raiseException("JSON-SERIALIZATION-ERROR", "don't know how to serialize type '%s'", v->type->getName());
   return -1;
}

static class QoreNode *f_makeJSONString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *val, *pcs;

   tracein("f_makeJSONString()");
   val = get_param(params, 0);

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   class QoreString *str = new QoreString(ccs);
   return doJSONValue(str, val, 0, xsink) ? NULL : new QoreNode(str);
}

/*
static class QoreNode *f_makeFormattedJSONString(class QoreNode *params, ExceptionSink *xsink)
{
}
*/

void init_json_functions()
{
   builtinFunctions.add("makeJSONString",        f_makeJSONString);
   //builtinFunctions.add("makeFormattedJSONString",        f_makeFormattedJSONString);
}
