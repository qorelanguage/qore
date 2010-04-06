/*
  lib/ql_json.cc

  Qore JSON (JavaScript QoreObject Notation) functions

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
#include <qore/intern/ql_json.h>

#include <ctype.h>
#include <stdlib.h>

// RFC 4627 JSON specification
// qore only supports JSON with UTF-8 

// returns 0 for OK
static int cmp_rest_token(const char *&p, const char *tok)
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

static void skip_whitespace(const char *&buf, int &line_number)
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
static int getJSONStringToken(QoreString &str, const char *&buf, int &line_number, ExceptionSink *xsink)
{
   // increment buffer to first character of string
   buf++;
   while (*buf)
   {
      if (*buf == '"')
      {
	 buf++;
	 return 0;
      }
      if (*buf == '\\')
      {
	 buf++;
	 if (*buf == '"' || *buf == '/' || *buf == '\\')
	 {
	    str.concat(*buf);
	    buf++;
	    continue;
	 }
	 if (*buf == 'b')
	    str.concat('\b');
	 else if (*buf == 'f')
	    str.concat('\f');
	 else if (*buf == 'n')
	    str.concat('\n');
	 else if (*buf == 'r')
	    str.concat('\r');
	 else if (*buf == 't')
	    str.concat('\t');
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
	       unsigned code = strtoul(unicode, 0, 16);
	       if (str.concatUnicode(code, xsink))
		  break;
	       buf += 3;
	    }
	    else
	       str.concat("\\u");
	 }
	 else // otherwise just concatenate the characters
	 {
	    str.concat('\\');
	    str.concat(*buf);
	 }
	 buf++;
	 continue;
      }
      if (*buf == '\n')
	 line_number++;
      str.concat(*buf);
      buf++;
   }
   xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON string", line_number);
   return -1;
}

static AbstractQoreNode *getJSONValue(const char *&buf, int &line_number, const QoreEncoding *enc, ExceptionSink *xsink);

// '{' has already been read and the buffer is set to this character
static QoreHashNode *getJSONObject(const char *&buf, int &line_number, const QoreEncoding *enc, ExceptionSink *xsink)
{
   // increment buffer to first character of object description
   buf++;
   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);

   // get either string or '}'
   skip_whitespace(buf, line_number);
      
   if (*buf == '}')
   {
      buf++;
      return h.release();
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
      QoreString str(enc);
      if (getJSONStringToken(str, buf, line_number, xsink))
	 break;

      //printd(5, "getJSONObject() key=%s\n", str.getBuffer());
      
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
      AbstractQoreNode *val = getJSONValue(buf, line_number, enc, xsink);
      if (!val)
      {
	 if (!xsink->isException())
	    xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON object (expecting JSON value for key '%s')", line_number, str.getBuffer());
	 break;
      }
      h->setKeyValue(&str, val, xsink);

      skip_whitespace(buf, line_number);
      if (*buf == '}')
      {
	 buf++;
	 return h.release();
      }

      if (*buf != ',')
      {
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON object (expecting ',' or '}')", line_number);
	 break;
      }
      buf++;
      
      skip_whitespace(buf, line_number);

   }
   return 0;
}

// '[' has already been read and the buffer is set to this character
static AbstractQoreNode *getJSONArray(const char *&buf, int &line_number, const QoreEncoding *enc, ExceptionSink *xsink)
{
   // increment buffer to first character of array description
   buf++;
   ReferenceHolder<QoreListNode> l(new QoreListNode(), xsink);

   skip_whitespace(buf, line_number);
   if (*buf == ']')
   {
      *buf++;
      return l.release();
   }

   while (*buf)
   {
      //printd(5, "before getJSONValue() buf=%s\n", buf);
      AbstractQoreNode *val = getJSONValue(buf, line_number, enc, xsink);
      if (!val)
      {
	 if (!xsink->isException())
	    xsink->raiseException("JSON-PARSE-ERROR", "premature end of input at line %d while parsing JSON array (expecting JSON value)", line_number);
	 return 0;
      }
      //printd(5, "after getJSONValue() buf=%s\n", buf);
      l->push(val);

      skip_whitespace(buf, line_number);
      if (*buf == ']')
      {
	 buf++;
	 return l.release();
      }

      if (*buf != ',')
      {
	 //printd(5, "*buf='%c'\n", *buf);
	 xsink->raiseException("JSON-PARSE-ERROR", "unexpected text encountered at line %d while parsing JSON array (expecting ',' or ']')", line_number);
	 return 0;
      }
      buf++;
      
      skip_whitespace(buf, line_number);
   }
   return 0;
}

static AbstractQoreNode *getJSONValue(const char *&buf, int &line_number, const QoreEncoding *enc, ExceptionSink *xsink)
{
   // skip whitespace
   skip_whitespace(buf, line_number);
   if (!*buf)
      return 0;

   // can expect: 't'rue, 'f'alse, '{', '[', '"'string...", integer, '.'digits
   if (*buf == '{')
      return getJSONObject(buf, line_number, enc, xsink);

   if (*buf == '[')
      return getJSONArray(buf, line_number, enc, xsink);

   if (*buf == '"')
   {
      QoreStringNodeHolder str(new QoreStringNode(enc));
      return getJSONStringToken(*(*str), buf, line_number, xsink) ? 0 : str.release();
   }

   // FIXME: implement parsing of JSON exponents
   if (isdigit(*buf) || (*buf) == '.' || (*buf) == '-')
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
	       return 0;
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
	    return 0;
	 }
	 str.concat(*buf);
	 buf++;
      }
      if (has_dot)
	 return new QoreFloatNode(atof(str.getBuffer()));
      return new QoreBigIntNode(strtoll(str.getBuffer(), 0, 10));
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
   return 0;
}

#define JSF_THRESHOLD 20

static int doJSONValue(class QoreString *str, const AbstractQoreNode *v, int format, ExceptionSink *xsink) {
   if (is_nothing(v) || is_null(v)) {
      str->concat("null");
      return 0;
   }

   qore_type_t vtype = v->getType();

   if (vtype == NT_LIST) {
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(v);
      str->concat("[ ");
      ConstListIterator li(l);
      QoreString tmp(str->getEncoding());
      while (li.next()) {
	 bool ind = tmp.strlen() > JSF_THRESHOLD;
	 tmp.clear();
	 if (doJSONValue(&tmp, li.getValue(), format == -1 ? format : format + 2, xsink))
	    return -1;
	 
	 if (format != -1 && (ind || tmp.strlen() > JSF_THRESHOLD)) {
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

   if (vtype == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(v);
      str->concat("{ ");
      ConstHashIterator hi(h);
      QoreString tmp(str->getEncoding());
      while (hi.next()) {
	 bool ind = tmp.strlen() > JSF_THRESHOLD;
	 tmp.clear();
	 if (doJSONValue(&tmp, hi.getValue(), format == -1 ? format : format + 2, xsink))
	    return -1;
	 
	 if (format != -1 && (ind || tmp.strlen() > JSF_THRESHOLD)) {
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

   if (vtype == NT_STRING) {
      const QoreStringNode *vstr = reinterpret_cast<const QoreStringNode *>(v);
      TempEncodingHelper t(vstr, str->getEncoding(), xsink);
      if (*xsink)
	 return -1;
      
      str->concat('"');
      str->concatEscape(*t, '"', '\\', xsink);
      if (*xsink)
	 return -1;
      str->concat('"');
      return 0;
   }

   if (vtype == NT_INT) {
      const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(v);
      str->sprintf("%lld", b->val);
      return 0;
   }

   if (vtype == NT_FLOAT) {
      str->sprintf("%.9g", reinterpret_cast<const QoreFloatNode *>(v)->f);
      return 0;
   }

   if (vtype == NT_BOOLEAN) {
      str->concat(reinterpret_cast<const QoreBoolNode *>(v)->getValue() ? "true" : "false");
      return 0;
   }

   if (vtype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(v);
      // this will be serialized as a string
      str->concat('"');
      date->DateTimeNode::getAsString(*str, 0, 0);
      str->concat('"');
      return 0;
   }
   
   xsink->raiseException("JSON-SERIALIZATION-ERROR", "don't know how to serialize type '%s'", v->getTypeName());
   return -1;
}

static AbstractQoreNode *f_makeJSONString(const QoreListNode *params, ExceptionSink *xsink) {
   QORE_TRACE("f_makeJSONString()");
   const AbstractQoreNode *val = get_param(params, 0);

   const QoreStringNode *pcs;
   const QoreEncoding *ccs;
   if ((pcs = test_string_param(params, 1)))
      ccs = QEM.findCreate(pcs);
   else
      ccs = QCS_UTF8;

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   if (doJSONValue(*str, val, -1, xsink))
      return 0;
   return str.release();
}

static AbstractQoreNode *f_makeFormattedJSONString(const QoreListNode *params, ExceptionSink *xsink) {
   QORE_TRACE("f_makeFormattedJSONString()");
   const AbstractQoreNode *val = get_param(params, 0);

   const QoreStringNode *pcs;
   const QoreEncoding *ccs;
   if ((pcs = test_string_param(params, 1)))
      ccs = QEM.findCreate(pcs);
   else
      ccs = QCS_UTF8;

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   if (doJSONValue(*str, val, 0, xsink))
      return 0;
   return str.release();
}

AbstractQoreNode *parseJSONValue(const QoreString *str, ExceptionSink *xsink) {
   int line_number = 1;
   const char *buf = str->getBuffer();
   AbstractQoreNode *rv = getJSONValue(buf, line_number, str->getEncoding(), xsink);
   if (rv && *buf) {
      // check for excess text after JSON data
      skip_whitespace(buf, line_number);
      if (*buf) {
	 xsink->raiseException("JSON-PARSE-ERROR", "extra text after JSON data on line %d", line_number);
	 rv->deref(xsink);
	 rv = 0;
      }
   }
   return rv;
}

static AbstractQoreNode *f_parseJSON(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
       return 0;
 
   return parseJSONValue(p0, xsink);
}

QoreStringNode *makeJSONRPC11RequestStringArgs(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-JSONRPC11-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return 0;
   }

   const AbstractQoreNode *p1 = get_param(params, 1);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first
   str->concat("{ \"version\" : \"1.1\", \"method\" : ");
   if (doJSONValue(*str, p0, -1, xsink))
      return 0;

   // params key should come last
   str->concat(", \"params\" : ");
   if (!is_nothing(p1)) {
      if (doJSONValue(*str, p1, -1, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat(" }");
   return str.release();
}

QoreStringNode *makeJSONRPC11RequestString(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-JSONRPC11-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first
   str->concat("{ \"version\" : \"1.1\", \"method\" : ");

   if (doJSONValue(*str, p0, -1, xsink))
      return 0;

   // params key should come last
   str->concat(", \"params\" : ");
   if (num_params(params) > 1) {
      ReferenceHolder<QoreListNode> new_params(params->copyListFrom(1), xsink);

      if (doJSONValue(*str, *new_params, -1, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat(" }");

   return str.release();
}

// syntax: makeJSONRPCRequestString(method, version, id, params)
static AbstractQoreNode *f_makeJSONRPCRequestString(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-JSONRPC-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return 0;
   }

   const AbstractQoreNode *p1, *p2, *p3;
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first if present
   if (p1) {
      str->concat("{ \"version\" : ");
      if (doJSONValue(*str, p1, -1, xsink))
	 return 0;
      str->concat(", ");
   }
   else
      str->concat("{ ");

   str->concat("\"method\" : ");
   if (doJSONValue(*str, p0, -1, xsink))
      return 0;

   if (p2) {
      str->concat(", \"id\" : ");
      if (doJSONValue(*str, p2, -1, xsink))
	 return 0;
   }

   // params key should come last
   str->concat(", \"params\" : ");
   if (p3) {
      if (doJSONValue(*str, p3, -1, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat(" }");
   return str.release();
}

// syntax: makeFormattedJSONRPCRequestString(method, version, id, params)
static AbstractQoreNode *f_makeFormattedJSONRPCRequestString(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-JSONRPC-REQUEST-STRING-ERROR", "expecting method name as first parameter");
      return 0;
   }

   const AbstractQoreNode *p1, *p2, *p3;
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first if present
   if (p1) {
      str->concat("{\n  \"version\" : ");
      if (doJSONValue(*str, p1, 2, xsink))
	 return 0;
      str->concat(",\n  ");
   }
   else
      str->concat("{\n  ");

   str->concat("\"method\" : ");
   if (doJSONValue(*str, p0, 2, xsink))
      return 0;

   if (p2) {
      str->concat(",\n  \"id\" : ");
      if (doJSONValue(*str, p2, 2, xsink))
	 return 0;
   }

   // params key should come last
   str->concat(",\n  \"params\" : ");
   if (p3) {
      if (doJSONValue(*str, p3, 2, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat("\n}");
   return str.release();
}

// syntax: makeJSONRPCResponseString(version, id, response)
static AbstractQoreNode *f_makeJSONRPCResponseString(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first if present
   if (p0) {
      str->concat("{ \"version\" : ");
      if (doJSONValue(*str, p0, -1, xsink))
	 return 0;
      str->concat(", ");
   }
   else
      str->concat("{ ");

   if (p1) {
      str->concat("\"id\" : ");
      if (doJSONValue(*str, p1, -1, xsink))
	 return 0;
      str->concat(", ");
   }

   // result key should come last
   str->concat("\"result\" : ");
   if (p2) {
      if (doJSONValue(*str, p2, -1, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat(" }");
   return str.release();
}

// syntax: makeFormattedJSONRPCResponseString(version, id, response)
static AbstractQoreNode *f_makeFormattedJSONRPCResponseString(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first if present
   if (p0) {
      str->concat("{\n  \"version\" : ");
      if (doJSONValue(*str, p0, 2, xsink))
	 return 0;
      str->concat(",\n  ");
   }
   else
      str->concat("{\n  ");

   if (p1) {
      str->concat("\"id\" : ");
      if (doJSONValue(*str, p1, 2, xsink))
	 return 0;
      str->concat(",\n  ");
   }

   // result key should come last
   str->concat("\"result\" : ");
   if (p2) {
      if (doJSONValue(*str, p2, 2, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat("\n}");
   return str.release();
}

// syntax: makeJSONRPCErrorString(version, id, response)
static AbstractQoreNode *f_makeJSONRPCErrorString(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first if present
   if (!is_nothing(p0)) {
      str->concat("{ \"version\" : ");
      if (doJSONValue(*str, p0, -1, xsink))
	 return 0;
      str->concat(", ");
   }
   else
      str->concat("{ ");

   if (!is_nothing(p1)) {
      str->concat("\"id\" : ");
      if (doJSONValue(*str, p1, -1, xsink))
	 return 0;
      str->concat(", ");
   }

   // error key should come last
   str->concat("\"error\" : ");
   if (!is_nothing(p2)) {
      if (doJSONValue(*str, p2, -1, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat(" }");
   return str.release();
}

// syntax: makeFormattedJSONRPCErrorString(version, id, response)
static AbstractQoreNode *f_makeFormattedJSONRPCErrorString(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0, *p1, *p2;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   // write version key first if present
   if (p0) {
      str->concat("{\n  \"version\" : ");
      if (doJSONValue(*str, p0, 2, xsink))
	 return 0;
      str->concat(",\n  ");
   }
   else
      str->concat("{\n  ");

   if (p1) {
      str->concat("\"id\" : ");
      if (doJSONValue(*str, p1, 2, xsink))
	 return 0;
      str->concat(",\n  ");
   }

   // error key should come last
   str->concat("\"error\" : ");
   if (p2) {
      if (doJSONValue(*str, p2, 2, xsink))
	 return 0;
   }
   else
      str->concat("null");
   str->concat("\n}");
   return str.release();
}

// syntax: makeJSONRPC11ErrorString(code, message, id, error)
static AbstractQoreNode *f_makeJSONRPC11ErrorString(const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p;
   p = get_param(params, 0);
   int code = p ? p->getAsInt() : 0;
   if (code < 100 || code > 999) {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error code (first argument) must be between 100 and 999 inclusive (value passed: %d)", code);
      return 0;
   }

   const QoreStringNode *mess = test_string_param(params, 1);
   if (!mess || !mess->strlen())
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error message string not passed as second argument)");
      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));

   str->concat("{ \"version\" : \"1.1\", ");

   // get optional "id" value
   p = get_param(params, 2);
   if (p)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(*str, p, -1, xsink))
	 return 0;
      str->concat(", ");
   }
   
   str->sprintf("\"error\" : { \"name\" : \"JSONRPCError\", \"code\" : %d, \"message\" : \"", code);
   // concat here so character encodings can be automatically converted if necessary
   str->concatEscape(mess, '"', '\\', xsink);
   if (xsink->isException())
      return 0;

   str->concat('\"');

   // get optional "error" value
   p = get_param(params, 3);
   if (p)
   {
      str->concat(", \"error\" : ");
      if (doJSONValue(*str, p, -1, xsink))
	 return 0;
   }
   str->concat(" } }");
   return str.release();
}

// syntax: makeFormattedJSONRPC11ErrorString(code, message, id, error)
static AbstractQoreNode *f_makeFormattedJSONRPC11ErrorString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p;
   p = get_param(params, 0);
   int code = p ? p->getAsInt() : 0;
   if (code < 100 || code > 999)
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error code (first argument) must be between 100 and 999 inclusive (value passed: %d)", code);
      return 0;
   }
   
   const QoreStringNode *mess = test_string_param(params, 1);
   if (!mess || !mess->strlen())
   {
      xsink->raiseException("MAKE-JSONRPC11-ERROR-STRING-ERROR", "error message string not passed as second argument)");
      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(QCS_UTF8));
   str->sprintf("{\n  \"version\" : \"1.1\",\n  ");

   // get optional "id" value
   p = get_param(params, 2);
   if (p)
   {
      str->concat("\"id\" : ");
      if (doJSONValue(*str, p, -1, xsink))
	 return 0;
      str->concat(",\n  ");
   }

   str->sprintf("\"error\" :\n  {\n    \"name\" : \"JSONRPCError\",\n    \"code\" : %d,\n    \"message\" : \"", code);
   // concat here so character encodings can be automatically converted if necessary
   str->concatEscape(mess, '"', '\\', xsink);
   if (xsink->isException())
      return 0;

   str->concat('\"');

   // get optional "error" value
   p = get_param(params, 3);
   if (p)
   {
      str->concat(",\n    \"error\" : ");
      if (doJSONValue(*str, p, 4, xsink))
	 return 0;
   }
   str->concat("\n  }\n}");
   return str.release();
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
