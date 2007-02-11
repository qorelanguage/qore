/*
  lib/ql_xml.cc

  Qore JSON (JavaScript Object Notation) functions

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
#include <qore/ql_json.h>

#include <ctype.h>
#include <stdlib.h>

// RFC 4627 JSON specification
// qore only supports JSON with UTF-8 

// returns 0 for OK
static int cmp_rest_token(char *&p, char *tok)
{
   p++;
   while (*tok)
      if ((*(p++)) != (*(tok++)))
	 return -1;
   if (!*p || *p == ',' || *p == ']' || *p == '}')
      return 0;
   if (isblank(*p) || (*p) == '\r' || (*p) == '\n')
   {
      p++;
      return 0;
   }
   return -1;
} 

static void skip_whitespace(char *&buf, int &line_number)
{
   while (*buf)
   {
      if (isblank(*buf) || (*buf) == '\r')
      {
	 buf++;
	 continue;
      }
      if ((*buf) == '\n')
      {
	 line_number++;
	 buf++;
	 continue;
      }
      break;
   }
}

// '"' has already been read and the buffer is set to this character
static class QoreString *getJSONStringToken(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // increment buffer to first character of string
   buf++;
   class QoreString *str = new QoreString(enc);
   while (*buf)
   {
      if (*buf == '"')
      {
	 buf++;
	 return str;
      }
      if (*buf == '\\')
      {
	 buf++;
	 if (*buf == '"' || *buf == '/' || *buf == '\\')
	 {
	    str->concat(*buf);
	    buf++;
	    continue;
	 }
	 if (*buf == 'b')
	    str->concat('\b');
	 else if (*buf == 'f')
	    str->concat('\f');
	 else if (*buf == 'n')
	    str->concat('\n');
	 else if (*buf == 'r')
	    str->concat('\r');
	 else if (*buf == 't')
	    str->concat('\t');
	 else if (*buf == 'u') // expect a unicode character specification
	 {
	    ++buf;
	    // check for 4 hex digits
	    if (isxdigit(*buf) && *(buf + 1) && isxdigit(*(buf + 1)) 
		&& *(buf + 2) && isxdigit(*(buf + 2)) 
		&& *(buf + 3) && isxdigit(*(buf + 3)))
	    {
	       char unicode[5];
	       strncpy(unicode, buf, 4);
	       unicode[4] = '\0';
	       unsigned code = strtoul(unicode, NULL, 16);
	       if (str->concatUnicode(code, xsink))
		  break;
	       buf += 3;
	    }
	    else
	       str->concat("\\u");
	 }
	 else // otherwise just concatenate the characters
	 {
	    str->concat('\\');
	    str->concat(*buf);
	 }
	 buf++;
	 continue;
      }
      if (*buf == '\n')
	 line_number++;
      str->concat(*buf);
      buf++;
   }
   delete str;
   xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON string", line_number);
   return NULL;
}

static class QoreNode *getJSONValue(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink);

// '{' has already been read and the buffer is set to this character
static class QoreNode *getJSONObject(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // increment buffer to first character of object description
   buf++;
   class Hash *h = new Hash();

   // get either string or '}'
   skip_whitespace(buf, line_number);
      
   if (*buf == '}')
   {
      buf++;
      return new QoreNode(h);
   }

   while (*buf)
   {
      if (*buf != '"')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 if (h->size())
	    xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting '\"' for key string)", line_number);
	 else
	    xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting '\" or '}'')", line_number);
	 break;
      }

      // get key
      class QoreString *str = getJSONStringToken(buf, line_number, enc, xsink);
      if (!str)
	 break;

      //printd(5, "getJSONObject() key=%s\n", str->getBuffer());
      
      skip_whitespace(buf, line_number);
      if (*buf != ':')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting ':')", line_number);
	 break;
      }
      buf++;
      skip_whitespace(buf, line_number);

      // get value
      class QoreNode *val = getJSONValue(buf, line_number, enc, xsink);
      if (!val)
      {
	 if (!xsink->isException())
	    xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON object (expecting JSON value for key '%s')", line_number, str->getBuffer());
	 delete str;
	 break;
      }
      h->setKeyValue(str, val, xsink);
      delete str;

      skip_whitespace(buf, line_number);
      if (*buf == '}')
      {
	 buf++;
	 return new QoreNode(h);
      }

      if (*buf != ',')
      {
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting ',' or '}')", line_number);
	 break;
      }
      buf++;
      
      skip_whitespace(buf, line_number);

   }
   h->derefAndDelete(NULL);
   return NULL;
}

// '[' has already been read and the buffer is set to this character
static class QoreNode *getJSONArray(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // increment buffer to first character of array description
   buf++;
   class List *l = new List();

   skip_whitespace(buf, line_number);
   if (*buf == ']')
   {
      *buf++;
      return new QoreNode(l);
   }

   while (*buf)
   {
      //printd(5, "before getJSONValue() buf=%s\n", buf);
      class QoreNode *val = getJSONValue(buf, line_number, enc, xsink);
      if (!val)
      {
	 if (!xsink->isException())
	    xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON array (expecting JSON value)", line_number);
	 break;
      }
      //printd(5, "after getJSONValue() buf=%s\n", buf);
      l->push(val);

      skip_whitespace(buf, line_number);
      if (*buf == ']')
      {
	 buf++;
	 return new QoreNode(l);
      }

      if (*buf != ',')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON array (expecting ',' or ']')", line_number);
	 break;
      }
      buf++;
      
      skip_whitespace(buf, line_number);
   }
   l->derefAndDelete(NULL);
   return NULL;
}

static class QoreNode *getJSONValue(char *&buf, int &line_number, class QoreEncoding *enc, class ExceptionSink *xsink)
{
   // skip whitespace
   skip_whitespace(buf, line_number);
   if (!*buf)
      return NULL;

   // can expect: 't'rue, 'f'alse, '{', '[', '"'string...", integer, '.'digits
   if (*buf == '{')
      return getJSONObject(buf, line_number, enc, xsink);

   if (*buf == '[')
      return getJSONArray(buf, line_number, enc, xsink);

   if (*buf == '"')
   {
      class QoreString *str = getJSONStringToken(buf, line_number, enc, xsink);
      return str ? new QoreNode(str) : NULL;
   }

   // FIXME: implement parsing of JSON exponents
   if (isdigit(*buf) || (*buf) == '.')
   {
      // temporarily use a QoreString
      QoreString str;
      bool has_dot;
      if (*buf == '.')
      {
	 // add a leading zero
	 str.concat("0.");
	 has_dot = true;
      }
      else
      {
	 str.concat(*buf);
	 has_dot = false;
      }
      buf++;
      while (*buf)
      {
	 if (*buf == '.')
	 {
	    if (has_dot)
	    {
	       xsink->raiseException("JSON-PARSE-ERROR", "unexpected '.' in floating point number (too many '.' characters)");
	       return NULL;
	    }
	    has_dot = true;
	 }
	 // if another token follows then break but do not increment buffer position
	 else if (*buf == ',' || *buf == '}' || *buf == ']')
	    break;
	 // if whitespace follows then increment buffer position and break
	 else if (isblank(*buf) || (*buf) == '\r')
	 {
	    buf++;
	    break;
	 }
	 // if a newline follows then  increment buffer position and line number and break
	 else if ((*buf) == '\n')
	 {
	    buf++;
	    line_number++;
	    break;
	 }
	 else if (!isdigit(*buf))
	 {
	    xsink->raiseException("JSON-PARSE-ERROR", "unexpected character in number");
	    return NULL;
	 }
	 str.concat(*buf);
	 buf++;
      }
      if (has_dot)
	 return new QoreNode(atof(str.getBuffer()));
      return new QoreNode(strtoll(str.getBuffer(), NULL, 10));
   }
   
   if ((*buf) == 't')
   {
      if (!cmp_rest_token(buf, "rue"))
	 return boolean_true();
      goto error;
   }
   if ((*buf) == 'f')
   {
      if (!cmp_rest_token(buf, "alse"))
	 return boolean_false();
      goto error;
   }
   if ((*buf) == 'n')
   {
      if (!cmp_rest_token(buf, "ull"))
	 return nothing();
      goto error;
   }
   
  error:
   //printd(5, "buf=%s\n", buf);

   xsink->raiseException("JSON-PARSE-ERROR", "invalid input at line %d; unable to parse JSON value", line_number);
   return NULL;
}

#define JSF_THRESHOLD 20

static int doJSONValue(class QoreString *str, class QoreNode *v, int format, class ExceptionSink *xsink)
{
   if (is_nothing(v))
   {
      str->concat("null");
      return 0;
   }
   if (v->type == NT_LIST)
   {
      str->concat("[ ");
      ListIterator li(v->val.list);
      QoreString tmp(str->getEncoding());
      while (li.next())
      {
	 bool ind = tmp.strlen() > JSF_THRESHOLD;
	 tmp.terminate(0);
	 if (doJSONValue(&tmp, li.getValue(), format == -1 ? format : format + 2, xsink))
	    return -1;

	 if (format != -1 && (ind || tmp.strlen() > JSF_THRESHOLD))
	 {
	    str->concat('\n');
	    str->addch(' ', format + 2);
	 }
	 str->sprintf("%s", tmp.getBuffer());

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
      QoreString tmp(str->getEncoding());
      while (hi.next())
      {
	 bool ind = tmp.strlen() > JSF_THRESHOLD;
	 tmp.terminate(0);
	 if (doJSONValue(&tmp, hi.getValue(), format == -1 ? format : format + 2, xsink))
	    return -1;

	 if (format != -1 && (ind || tmp.strlen() > JSF_THRESHOLD))
	 {
	    str->concat('\n');
	    str->addch(' ', format + 2);
	 }
	 str->sprintf("\"%s\" : %s", hi.getKey(), tmp.getBuffer());
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
      str->concatEscape(t, '"', '\\', xsink);
      if (xsink->isException())
      {
	 if (t != v->val.String)
	    delete t;
	 delete str;
	 return -1;
      }
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
      ccs = QCS_UTF8;

   class QoreString *str = new QoreString(ccs);
   return doJSONValue(str, val, -1, xsink) ? NULL : new QoreNode(str);
}

static class QoreNode *f_makeFormattedJSONString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *val, *pcs;

   tracein("f_makeFormattedJSONString()");
   val = get_param(params, 0);

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_UTF8;

   class QoreString *str = new QoreString(ccs);
   return doJSONValue(str, val, 0, xsink) ? NULL : new QoreNode(str);
}

class QoreNode *parseJSONValue(class QoreString *str, class ExceptionSink *xsink)
{
   int line_number = 1;
   char *buf = str->getBuffer();
   class QoreNode *rv = getJSONValue(buf, line_number, str->getEncoding(), xsink);
   if (rv && *buf)
   {
      // check for excess text after JSON data
      skip_whitespace(buf, line_number);
      if (*buf)
      {
	 xsink->raiseException("JSON-PARSE-ERROR", "extra text after JSON data on line %d", line_number);
	 rv->deref(xsink);
	 rv = NULL;
      }
   }
   return rv;
}

static class QoreNode *f_parseJSON(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
       return NULL;
 
   return parseJSONValue(p0->val.String, xsink);
}

class QoreString *makeJSONRPC11RequestStringArgs(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-JSONRPC11-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return NULL;
   }

   class QoreNode *p1 = get_param(params, 1);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first
   str->concat("{ \"version\" : \"1.1\", \"method\" : ");
   if (doJSONValue(str, p0, -1, xsink))
      return NULL;

   // params key should come last
   str->concat(", \"params\" : ");
   if (p1)
   {
      if (doJSONValue(str, p1, -1, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat(" }");
   return str;
}

class QoreString *makeJSONRPC11RequestString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-JSONRPC11-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return NULL;
   }

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first
   str->concat("{ \"version\" : \"1.1\", \"method\" : ");
   if (doJSONValue(str, p0, -1, xsink))
      return NULL;

   // params key should come last
   str->concat(", \"params\" : ");
   if (num_params(params) > 1)
   {
      ReferenceHolder<QoreNode> new_params(new QoreNode(params->val.list->copyListFrom(1)));

      if (doJSONValue(str, *new_params, -1, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat(" }");
   return str;
}

// syntax: makeJSONRPCRequestString(method, version, id, params)
static class QoreNode *f_makeJSONRPCRequestString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-JSONRPC-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return NULL;
   }

   class QoreNode *p1, *p2, *p3;
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first if present
   if (p1)
   {
      str->concat("{ \"version\" : ");
      if (doJSONValue(str, p1, -1, xsink))
	 return NULL;
      str->concat(", ");
   }
   else
      str->concat("{ ");

   str->concat("\"method\" : ");
   if (doJSONValue(str, p0, -1, xsink))
      return NULL;

   if (p2)
   {
      str->concat(", \"id\" : ");
      if (doJSONValue(str, p2, -1, xsink))
	 return NULL;
   }

   // params key should come last
   str->concat(", \"params\" : ");
   if (p3)
   {
      if (doJSONValue(str, p3, -1, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat(" }");
   return new QoreNode(str);
}

// syntax: makeFormattedJSONRPCRequestString(method, version, id, params)
static class QoreNode *f_makeFormattedJSONRPCRequestString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-JSONRPC-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return NULL;
   }

   class QoreNode *p1, *p2, *p3;
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first if present
   if (p1)
   {
      str->concat("{\n  \"version\" : ");
      if (doJSONValue(str, p1, 2, xsink))
	 return NULL;
      str->concat(",\n  ");
   }
   else
      str->concat("{\n  ");

   str->concat("\"method\" : ");
   if (doJSONValue(str, p0, 2, xsink))
      return NULL;

   if (p2)
   {
      str->concat(",\n  \"id\" : ");
      if (doJSONValue(str, p2, 2, xsink))
	 return NULL;
   }

   // params key should come last
   str->concat(",\n  \"params\" : ");
   if (p3)
   {
      if (doJSONValue(str, p3, 2, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat("\n}");
   return new QoreNode(str);
}

// syntax: makeJSONRPCResponseString(version, id, response)
static class QoreNode *f_makeJSONRPCResponseString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first if present
   if (p0)
   {
      str->concat("{ \"version\" : ");
      if (doJSONValue(str, p0, -1, xsink))
	 return NULL;
      str->concat(", ");
   }
   else
      str->concat("{ ");

   if (p1)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(str, p1, -1, xsink))
	 return NULL;
      str->concat(", ");
   }

   // result key should come last
   str->concat("\"result\" : ");
   if (p2)
   {
      if (doJSONValue(str, p2, -1, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat(" }");
   return new QoreNode(str);
}

// syntax: makeFormattedJSONRPCResponseString(version, id, response)
static class QoreNode *f_makeFormattedJSONRPCResponseString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first if present
   if (p0)
   {
      str->concat("{\n  \"version\" : ");
      if (doJSONValue(str, p0, 2, xsink))
	 return NULL;
      str->concat(",\n  ");
   }
   else
      str->concat("{\n  ");

   if (p1)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(str, p1, 2, xsink))
	 return NULL;
      str->concat(",\n  ");
   }

   // result key should come last
   str->concat("\"result\" : ");
   if (p2)
   {
      if (doJSONValue(str, p2, 2, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat("\n}");
   return new QoreNode(str);
}

// syntax: makeJSONRPCErrorString(version, id, response)
static class QoreNode *f_makeJSONRPCErrorString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first if present
   if (p0)
   {
      str->concat("{ \"version\" : ");
      if (doJSONValue(str, p0, -1, xsink))
	 return NULL;
      str->concat(", ");
   }
   else
      str->concat("{ ");

   if (p1)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(str, p1, -1, xsink))
	 return NULL;
      str->concat(", ");
   }

   // error key should come last
   str->concat("\"error\" : ");
   if (p2)
   {
      if (doJSONValue(str, p2, -1, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat(" }");
   return new QoreNode(str);
}

// syntax: makeFormattedJSONRPCErrorString(version, id, response)
static class QoreNode *f_makeFormattedJSONRPCErrorString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreString *str = new QoreString(QCS_UTF8);

   // write version key first if present
   if (p0)
   {
      str->concat("{\n  \"version\" : ");
      if (doJSONValue(str, p0, 2, xsink))
	 return NULL;
      str->concat(",\n  ");
   }
   else
      str->concat("{\n  ");

   if (p1)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(str, p1, 2, xsink))
	 return NULL;
      str->concat(",\n  ");
   }

   // error key should come last
   str->concat("\"error\" : ");
   if (p2)
   {
      if (doJSONValue(str, p2, 2, xsink))
	 return NULL;
   }
   else
      str->concat("null");
   str->concat("\n}");
   return new QoreNode(str);
}

// syntax: makeJSONRPC11ErrorString(code, message, id, error)
static class QoreNode *f_makeJSONRPC11ErrorString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p;
   p = get_param(params, 0);
   int code = p ? p->getAsInt() : 0;
   if (code < 100 || code > 999)
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error code (first argument) must be between 100 and 999 inclusive (value passed: %d)", code);
      return NULL;
   }

   p = test_param(params, NT_STRING, 1);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error message string not passed as second argument)");
      return NULL;
   }
   QoreString *mess = p->val.String;

   QoreString *str = new QoreString(QCS_UTF8);
   str->concat("{ \"version\" : \"1.1\", ");

   // get optional "id" value
   p = get_param(params, 2);
   if (p)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(str, p, -1, xsink))
	 return NULL;
      str->concat(", ");
   }
   
   str->sprintf("\"error\" : { \"name\" : \"JSONRPCError\", \"code\" : %d, \"message\" : \"", code);
   // concat here so character encodings can be automatically converted if necessary
   str->concatEscape(mess, '"', '\\', xsink);
   if (xsink->isException())
   {
      delete str;
      return NULL;
   }

   str->concat('\"');

   // get optional "error" value
   p = get_param(params, 3);
   if (p)
   {
      str->concat(", \"error\" : ");
      if (doJSONValue(str, p, -1, xsink))
	 return NULL;
   }
   str->concat(" } }");
   return new QoreNode(str);
}

// syntax: makeFormattedJSONRPC11ErrorString(code, message, id, error)
static class QoreNode *f_makeFormattedJSONRPC11ErrorString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p;
   p = get_param(params, 0);
   int code = p ? p->getAsInt() : 0;
   if (code < 100 || code > 999)
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error code (first argument) must be between 100 and 999 inclusive (value passed: %d)", code);
      return NULL;
   }

   p = test_param(params, NT_STRING, 1);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error message string not passed as second argument)");
      return NULL;
   }
   QoreString *mess = p->val.String;

   QoreString *str = new QoreString(QCS_UTF8);
   str->sprintf("{\n  \"version\" : \"1.1\",\n  ");

   // get optional "id" value
   p = get_param(params, 2);
   if (p)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(str, p, -1, xsink))
	 return NULL;
      str->concat(",\n  ");
   }

   str->sprintf("\"error\" :\n  {\n    \"name\" : \"JSONRPCError\",\n    \"code\" : %d,\n    \"message\" : \"", code);
   // concat here so character encodings can be automatically converted if necessary
   str->concatEscape(mess, '"', '\\', xsink);
   if (xsink->isException())
   {
      delete str;
      return NULL;
   }

   str->concat('\"');

   // get optional "error" value
   p = get_param(params, 3);
   if (p)
   {
      str->concat(",\n    \"error\" : ");
      if (doJSONValue(str, p, 4, xsink))
	 return NULL;
   }
   str->concat("\n  }\n}");
   return new QoreNode(str);
}

void init_json_functions()
{
   builtinFunctions.add("makeJSONString",                      f_makeJSONString);
   builtinFunctions.add("makeFormattedJSONString",             f_makeFormattedJSONString);
   builtinFunctions.add("parseJSON",                           f_parseJSON);

   builtinFunctions.add("makeJSONRPCRequestString",            f_makeJSONRPCRequestString);
   builtinFunctions.add("makeFormattedJSONRPCRequestString",   f_makeFormattedJSONRPCRequestString);
   builtinFunctions.add("makeJSONRPCResponseString",           f_makeJSONRPCResponseString);
   builtinFunctions.add("makeFormattedJSONRPCResponseString",  f_makeFormattedJSONRPCResponseString);
   builtinFunctions.add("makeJSONRPCErrorString",              f_makeJSONRPCErrorString);
   builtinFunctions.add("makeFormattedJSONRPCErrorString",     f_makeFormattedJSONRPCErrorString);
   builtinFunctions.add("makeJSONRPC11ErrorString",            f_makeJSONRPC11ErrorString);
   builtinFunctions.add("makeFormattedJSONRPC11ErrorString",   f_makeFormattedJSONRPC11ErrorString);
}
