/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qpp.cpp

  Qore Pre-Processor

  Qore Programming Language
  
  Copyright 2003 - 2011 David Nichols
  
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

// only static definitions can be used from the Qore headers - and no compiled code because the library is not available
#include <qore/Qore.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <map>

std::string pn;

// code attribute type
typedef unsigned int attr_t;

typedef std::vector<std::string> strlist_t;
typedef std::map<std::string, std::string> strmap_t;
typedef std::map<std::string, attr_t> amap_t;

// code attribute map
static amap_t amap;

// parameter structure
struct Param {
   std::string type, 
      name,
      val;

   Param(const std::string &t, const std::string &n, const std::string &v) : type(t), name(n), val(v) {
   }
};

// parameter list
typedef std::vector<Param> paramlist_t;

enum LogLevel {
   LL_CRITICAL = 0,
   LL_IMPORANT = 1,
   LL_INFO     = 2,
   LL_DETAIL   = 3,
   LL_DEBUG    = 4,
};

LogLevel LL = LL_DEBUG;

// code attribute bitfield defines
#define QCA_NONE         0
#define QCA_PUBLIC       (1 << 0)
#define QCA_PRIVATE      (1 << 1)
#define QCA_STATIC       (1 << 2)
#define QCA_SYNCHRONIZED (1 << 3)

#define BUFSIZE 1024

void log(LogLevel ll, const char *fmt, ...) {
   if (ll > LL)
      return;

   va_list args;

   char buf[BUFSIZE];

   va_start(args, fmt);
   int rc = vsnprintf(buf, BUFSIZE, fmt, args);
   va_end(args);

   if (rc < 0) {
      fprintf(stderr, "output error in vsnprintf(%d, %s, ...)\n", ll, fmt);
      return;
   }
   if (rc > BUFSIZE)
      buf[BUFSIZE - 1] = '\0';

   fputs(buf, stdout);
   fflush(stdout);
}

void error(const char *fmt, ...) {
   va_list args;

   char buf[BUFSIZE];

   va_start(args, fmt);
   int rc = vsnprintf(buf, BUFSIZE, fmt, args);
   va_end(args);

   if (rc < 0) {
      fprintf(stderr, "output error in vsnprintf(%s, ...)\n", fmt);
      return;
   }
   if (rc > BUFSIZE)
      buf[BUFSIZE - 1] = '\0';

   fputs(buf, stderr);
   fflush(stderr);
}

void getStringList(strlist_t &l, const std::string &str, char separator = ',') {
   size_t start = 0;
   while (true) {
      size_t sep = str.find(separator, start);
      if (sep == std::string::npos)
         break;
      l.push_back(std::string(str, start, sep - start));
      start = sep + 1;
   }
   l.push_back(std::string(str, start));

   //for (unsigned i = 0; i < l.size(); ++i)
   //   printf("DBG: list %u/%lu: %s\n", i, l.size(), l[i].c_str());
}

class AbstractElement {
protected:

public:
   virtual ~AbstractElement() {
   }

   virtual void serializeCpp(FILE *fp) const = 0;
   virtual void serializeDox(FILE *fp) const = 0;
};

class TextElement : public AbstractElement {
protected:
   std::string buf;

public:
   TextElement(const std::string &n_buf) : buf(n_buf) {
      //log(LL_DEBUG, "TextElement::TextElement() str=%s", n_buf.c_str());
   }

   virtual void serializeCpp(FILE *fp) const {
      //fputs(buf.c_str(), fp);
   }

   virtual void serializeDox(FILE *fp) const {
      fputs(buf.c_str(), fp);
   }   
};

class Method {
protected:
   std::string name, docs, return_type, code;
   attr_t attr;
   paramlist_t params;
   bool rt_int, has_return;

   void serializeCppConstructor(FILE *fp, const char *cname) const {
      fprintf(fp, "static void %s_constructor(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {\n", cname);
      fputs(code.c_str(), fp);
      fputs("\n}\n\n", fp);
   }

   void serializeCppDestructor(FILE *fp, const char *cname, const char *arg) const {
      fprintf(fp, "static void %s_destructor(QoreObject *self, %s, ExceptionSink *xsink) {\n", cname, arg);
      fputs(code.c_str(), fp);
      fputs("\n}\n\n", fp);
   }

   void serializeCppCopy(FILE *fp, const char *cname, const char *arg) const {
      fprintf(fp, "static void %s_copy(QoreObject *self, QoreObject *old, %s, ExceptionSink *xsink) {\n", cname, arg);
      fputs(code.c_str(), fp);
      fputs("\n}\n\n", fp);
   }

   void serializeCppConstructorBinding(FILE *fp, const char *UC) const {
   }

   void serializeCppDestructorBinding(FILE *fp, const char *UC) const {
   }

   void serializeCppCopyBinding(FILE *fp, const char *UC) const {
   }

   void serializeArgs(FILE *fp) const {
      for (unsigned i = 0; i < params.size(); ++i) {
         const Param &p = params[i];
         if (p.type == "int" || p.type == "softint" || p.type == "timeout") {
            fprintf(fp, "   int64 %s = HARD_QORE_INT(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "float" || p.type == "softfloat") {
            fprintf(fp, "   double %s = HARD_QORE_FLOAT(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "bool" || p.type == "softbool") {
            fprintf(fp, "   bool %s = HARD_QORE_BOOL(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "string" || p.type == "softstring") {
            fprintf(fp, "   QoreStringNode* %s = HARD_QORE_STRING(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "date" || p.type == "softdate") {
            fprintf(fp, "   DateTimeNode* %s = HARD_QORE_DATE(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "binary") {
            fprintf(fp, "   BinaryNode* %s = HARD_QORE_BINARY(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "list" || p.type == "softlist") {
            fprintf(fp, "   QoreListNode* %s = HARD_QORE_LIST(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "hash") {
            fprintf(fp, "   QoreHashNode* %s = HARD_QORE_HASH(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "reference") {
            fprintf(fp, "   ReferenceNode* %s = HARD_QORE_REF(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "object") {
            fprintf(fp, "   QoreObject* %s = HARD_QORE_HASH(args, %d);\n", p.name.c_str(), i);
            return;
         }
         if (p.type == "any") {
            fprintf(fp, "   AbstractQoreNode* %s = get_param(args, %d);\n", p.name.c_str(), i);
            return;
         }

         log(LL_CRITICAL, "don't know how to handle argument type '%s' (for arg %s)\n", p.type.c_str(), p.name.c_str());
         assert(false);
      }
   }

   void serializeQorePrototypeComment(FILE *fp, const char *cname) const {
      fprintf(fp, "// %s %s::%s(", return_type.empty() ? "nothing" : return_type.c_str(), cname, name.c_str());
      for (unsigned i = 0; i < params.size(); ++i) {
         const Param &p = params[i];
         fprintf(fp, "%s %s", p.type.c_str(), p.name.c_str());
         if (!p.val.empty())
            fprintf(fp, " = %s", p.val.c_str());
      }
      fputs(") {}\n", fp);
   }

public:
   Method(const std::string &n_name, attr_t n_attr, const paramlist_t &n_params, const std::string &n_docs, const std::string &n_return_type, const std::string &n_code) : name(n_name), docs(n_docs), return_type(n_return_type), code(n_code), attr(n_attr), params(n_params), rt_int(return_type == "int" || return_type == "softint" ? true : false), has_return(false) {
      // check if there is a return statement on the last line
      size_t i = code.rfind('\n');
      if (i == std::string::npos)
         i = 0;

      has_return = code.find("return", i) != std::string::npos;

      std::string tmp(code, i);
      log(LL_DEBUG, "Method::Method() has_return: %d '%s'\n", has_return, tmp.c_str());
   }

   const char *getName() const {
      return name.c_str();
   }

   virtual void serializeCppMethod(FILE *fp, const char *cname, const char *arg) const {
      if (name == "constructor") {
         serializeCppConstructor(fp, cname);
         return;
      }
      if (name == "destructor") {
         serializeCppDestructor(fp, cname, arg);
         return;
      }
      if (name == "copy") {
         serializeCppCopy(fp, cname, arg);
         return;
      }

      serializeQorePrototypeComment(fp, cname);

      fprintf(fp, "static %s %s_%s(QoreObject *self, %s, const QoreListNode *args, ExceptionSink *xsink) {\n", rt_int ? "int64" : "AbstractQoreNode*", cname, name.c_str(), arg);
      serializeArgs(fp);
      fputs(code.c_str(), fp);

      if (!has_return)
         fprintf(fp, "\n   return 0;");

      fputs("\n}\n\n", fp);
   }

   virtual void serializeCppBinding(FILE *fp, const char *cname, const char *UC) const {
      if (name == "constructor") {
         serializeCppConstructorBinding(fp, UC);
         return;
      }
      if (name == "destructor") {
         serializeCppDestructorBinding(fp, UC);
         return;
      }
      if (name == "copy") {
         serializeCppCopyBinding(fp, UC);
         return;
      }

      fputs("   ", fp);
      serializeQorePrototypeComment(fp, UC);

      //fprintf("   %s\n");
      //xxx;
   }

   virtual void serializeDox(FILE *fp) const {
      assert(false);
   }
};

void toupper(std::string &str) {
   for (unsigned i = 0; i < str.size(); ++i)
      str[i] = toupper(str[i]);
}

void trim(std::string &str) {
   while (str[0] == ' ')
      str.erase(0, 1);
   while (true) {
      size_t len = str.size();
      if (!len || str[len - 1] != ' ')
         break;
      str.erase(len - 1);
   }
}

class ClassElement : public AbstractElement {
protected:
   typedef std::multimap<std::string, Method*> mmap_t;

   std::string name,
      doc, 
      arg;
   strlist_t deptypes, 
      dom;              // functional domains
   mmap_t mmap;         // method map
   int64 flags;
   bool valid, 
      upm;              // unset public member flag

   void addElement(strlist_t &l, const std::string &str, size_t start, size_t end = std::string::npos) {
      std::string se(str, start, end);
      trim(se);
      l.push_back(se);
   }

public:
   ClassElement(const std::string &n_name, const strmap_t &props, const std::string &n_doc) : name(n_name), doc(n_doc), valid(true), upm(false) {
      log(LL_INFO, "registering class '%s'\n", name.c_str());

      // process properties
      for (strmap_t::const_iterator i = props.begin(), e = props.end(); i != e; ++i) {
         //log(LL_DEBUG, "+ prop: '%s': '%s'\n", i->first.c_str(), i->second.c_str());

         // parse domain
         if (i->first == "dom") {
            getStringList(dom, i->second);
            for (strlist_t::iterator di = dom.begin(), e = dom.end(); di != e; ++di)
               toupper(*di);
            continue;
         }

         if (i->first == "arg") {
            arg = i->second;
            log(LL_DEBUG, "+ arg: %s\n", arg.c_str());
            continue;
         }

         if (i->first == "flags") {
            strlist_t l;
            getStringList(l, i->second);
            
            for (unsigned i = 0; i < l.size(); ++i) {
               if (l[i] == "unsetPublicMemberFlag") {
                  upm = true;
                  continue;
               }
               error("class '%s' has unknown flag: '%s'\n", name.c_str(), l[i].c_str());
               valid = false;
            }
            continue;
         }

         if (i->first == "types") {
            getStringList(deptypes, i->second);
            continue;
         }

         error("+ prop: '%s': '%s' - unknown property '%s'\n", i->first.c_str(), i->second.c_str(), i->first.c_str());
         valid = false;
      }

      if (arg.empty()) {
         valid = false;
         error("class '%s' has no 'arg' property\n", name.c_str());
      }
   }

   virtual ~ClassElement() {
      for (mmap_t::iterator i = mmap.begin(), e = mmap.end(); i != e; ++i) {
         delete i->second;
      }
   }

   operator bool() const {
      return valid;
   }

   const char *getName() const {
      return name.c_str();
   }

   void addMethod(const std::string &mname, attr_t attr, const std::string &return_type, const paramlist_t &params, const std::string &code, const std::string &doc) {
      log(LL_DETAIL, "adding method %s%s'%s'::'%s'()\n", return_type.c_str(), return_type.empty() ? "" : " ", name.c_str(), mname.c_str());

      Method *m = new Method(mname, attr, params, doc, return_type, code);
      mmap.insert(mmap_t::value_type(mname, m));
   }

   virtual void serializeCpp(FILE *fp) const {
      fprintf(fp, "/* Qore class %s */\n\n", name.c_str());

      std::string UC;
      for (unsigned i = 0; i < name.size(); ++i)
         UC += toupper(name[i]);

      fprintf(fp, "qore_classid_t CID_%s;\nQoreClass *QC_%s;\n\n", UC.c_str(), UC.c_str());

      for (mmap_t::const_iterator i = mmap.begin(), e = mmap.end(); i != e; ++i) {
         i->second->serializeCppMethod(fp, name.c_str(), arg.c_str());
      }

      fprintf(fp, "QoreClass* init%sClass() {\n   QC_%s = new QoreClass(\"%s\"", name.c_str(), UC.c_str(), name.c_str());
      for (strlist_t::const_iterator i = dom.begin(), e = dom.end(); i != e; ++i) {
         fprintf(fp, ", QDOM_%s", (*i).c_str());
      }
      fprintf(fp, ");\n   CID_%s = QC_%s->getID();\n\n", UC.c_str(), UC.c_str());

      for (mmap_t::const_iterator i = mmap.begin(), e = mmap.end(); i != e; ++i) {
         i->second->serializeCppBinding(fp, name.c_str(), UC.c_str());
      }

      fputs("}\n", fp);
   }

   virtual void serializeDox(FILE *fp) const {
      assert(false);
   }   
};

typedef std::map<std::string, ClassElement *> cmap_t;

typedef std::vector<AbstractElement *> source_t;

class Code {
protected:
   const char *fileName;
   std::string cppFileName, 
      doxFileName;
   unsigned lineNumber;
   source_t source;
   cmap_t cmap;
   bool valid;

   int getDoxComment(std::string &str, FILE *fp) {
      std::string buf;
      if (readLine(buf, fp)) {
         error("%s:%d: premature EOF reading doxygen comment\n", fileName, lineNumber);
         return -1;
      }

      if (strncmp(buf.c_str(), "/**", 3)) {
         error("%s:%d: missing block comment marker '/**' in line following //! comment (str=%s buf=%s)\n", fileName, lineNumber, str.c_str(), buf.c_str());
         return -1;
      }

      str += buf;

      int lc = 0;

      while (true) {
         int c = fgetc(fp);
         if (c == EOF) {
            error("%s:%d: premature EOF reading doxygen comment\n", fileName, lineNumber);
            return -1;
         }

         str += c;

         if (lc && c == '/')
            break;

         lc = (c == '*');

         if (c == '\n')
            ++lineNumber;
      }

      // now read to EOL
      readLine(str, fp);

      return 0;
   }

   int readLine(std::string &str, FILE *fp) {
      while (true) {
         int c = fgetc(fp);
         if (c == EOF)
            return -1;

         str += c;
         if (c == '\n') {
            ++lineNumber;
            break;
         }
      }
      return 0;
   }

   int readUntilClose(std::string &str, FILE *fp) {
      int quote = 0;

      // curly bracket count
      unsigned bc = 1;

      bool backquote = false;
      
      while (true) {
         int c = fgetc(fp);
         if (c == EOF) {
            error("%s: premature EOF on line %d\n", fileName, lineNumber);
            return -1;
         }

         str += c;

         if (backquote == true) {
            backquote = false;
            continue;
         }

         if (c == '\n') {
            ++lineNumber;
            continue;
         }

         if (c == '"' || c == '\'') {
            if (quote == c)
               quote = 0;
            else if (!quote)
               quote = c;
            continue;
         }

         if (quote) {
            if (c == '\\')
               backquote = true;
            continue;
         }

         if (c == '{') {
            ++bc;
            continue;
         }

         if (c == '}') {
            if (!--bc)
               break;
         }
      }

      return 0;
   }

   void checkBuf(std::string &buf) {
      if (!buf.empty()) {
         bool ws = true;
         for (unsigned i = 0, e = buf.size(); i < e; ++i)
            if (buf[i] != '\n') {
               ws = false;
               break;
            }
         if (ws) {
            buf.clear();
            return;
         }

         source.push_back(new TextElement(buf));
         buf.clear();
      }
   }

   int missingValueError(const std::string &propstr) const {
      error("%s:%d: missing '=' or value in class property string '%s'\n", fileName, lineNumber, propstr.c_str());
      return -1;
   }

   int addElement(const std::string &propstr, strmap_t &props, size_t start, size_t end) const {
      size_t eq = propstr.find('=', start);
      if (eq == std::string::npos || eq >= end)
         return missingValueError(propstr);
      while (start < eq && propstr[start] == ' ')
         ++start;
      if (start == eq) {
         error("%s:%d: missing property name in class property string '%s'\n", fileName, lineNumber, propstr.c_str());
         return -1;
      }
      size_t tend = end;
      while (tend > eq && propstr[tend] == ' ')
         --tend;

      if (tend == eq)
         return missingValueError(propstr);

      std::string key(propstr, start, eq - start);
      std::string value(propstr, eq + 1, end - eq -1);
      props[key] = value;
      return 0;
   }

   int parseProperties(std::string &propstr, strmap_t &props) const {
      size_t start = 0;
      while (true) {
         size_t end = propstr.find(';', start);
         if (end == std::string::npos)
            break;
         if (addElement(propstr, props, start, end))
            return -1;
         start = end + 1;
      }

      return addElement(propstr, props, start, propstr.size());
   }

   int parse() {
      FILE *fp = fopen(fileName, "r");
      if (!fp) {
	 error("%s: %s\n", fileName, strerror(errno));
         return -1;
      }

      lineNumber = 1;

      int rc = 0;

      std::string str;
      std::string buf;
      while (true) {
         str.clear();
         readLine(str, fp);

         //log(LL_DEBUG, "%d: %s", lineNumber, str.c_str());

         if (str.empty())
            break;

         if (!strncmp(str.c_str(), "//!", 3)) {
            if (getDoxComment(str, fp)) {
               rc = -1;
               break;
            }

            std::string sc;
            if (readLine(sc, fp)) {
               error("%s:%d: premature EOF reading code signature line\n", fileName, lineNumber);
               rc = -1;
               break;
            }

            //log(LL_DEBUG, "SC=%s", sc.c_str());

            if (!strncmp(sc.c_str(), "qclass ", 7)) {
               const char *p = sc.c_str() + 7;
               while (*p && *p == ' ')
                  ++p;
               if (!*p) {
                  error("%s:%d: premature EOF reading class header line\n", fileName, lineNumber);
                  rc = -1;
                  break;
               }
               const char *p1 = p;
               while (*p1 && *p1 != ' ')
                  ++p1;

               std::string cn(p, p1 - p);

               // get class properties
               p = strchr(sc.c_str(), '[');
               p1 = p ? strchr(p + 1, ']') : 0;
               if (!p1) {
                  error("%s:%d: missing class properties ('[...]') reading class header line\n", fileName, lineNumber);
                  rc = -1;
                  break;
               }

               std::string propstr(p + 1, p1 - p - 1);

               // parse properties
               strmap_t props;
               if (parseProperties(propstr, props)) {
                  rc = -1;
                  break;
               }

               ClassElement *ce = new ClassElement(cn, props, str);
               cmap[cn] = ce;
               checkBuf(buf);
               source.push_back(ce);
               // mark code as invalid if class element is invalid
               if (!*ce)
                  valid = false;
               continue;
            }

            if (strstr(sc.c_str(), "::") && strchr(sc.c_str(), '{')) {
               if (parseMethod(sc, fp, str)) {
                  valid = false;
                  break;
               }
               continue;
            }
         }

         buf += str;
      }

      checkBuf(buf);

      fclose(fp);

      return rc;
   }

   int parseMethod(std::string &sc, FILE *fp, std::string &doc) {
      if (readUntilClose(sc, fp)) {
         error("%s:%d: premature EOF reading method definition\n", fileName, lineNumber);
         return -1;
      }

      size_t i = sc.find("::");
      assert(i != std::string::npos);

      // save position of the '::'
      size_t p = i;

      if (!i) {
         error("%s:%d: missing class name in method definition", fileName, lineNumber);
         return -1;
      }

      // find beginning of class name
      while (i && sc[i - 1] != ' ')
         --i;

      // get class name
      std::string cn(sc, i, p - i);

      cmap_t::iterator ci = cmap.find(cn);
      if (ci == cmap.end()) {
         error("%s:%d: reference to undefined class '%s'\n", fileName, lineNumber, cn.c_str());
         return -1;
      }

      // method attributes
      attr_t attr = QCA_NONE;
      // return type
      std::string return_type;

      if (i) {
         strlist_t pre;
         // get return type and flags
         std::string pstr(sc, 0, i - 1);
         //log(LL_DEBUG, "PSTR='%s'\n", pstr.c_str());
         getStringList(pre, pstr);

         for (unsigned xi = 0; xi < pre.size(); ++xi) {
            //printf("DBG: METHOD list %u/%lu: %s\n", xi, pre.size(), pre[xi].c_str());
            amap_t::iterator ai = amap.find(pre[xi]);
            if (ai == amap.end()) {
               if (!return_type.empty()) {
                  error("%s:%d: multiple return types or unknown code attribute '%s' given\n", fileName, lineNumber, pre[xi].c_str());
                  return -1;
               }
               return_type = pre[xi];
            }
            else
               attr |= ai->second;
         }
      }

      // get method name
      p += 2;
      i = p;
      while (i < sc.size() && sc[i] != ' ' && sc[i] != '(')
         ++i;

      if (i == sc.size()) {
         error("%s:%d: premature EOL reading method definition in class %s\n", fileName, lineNumber, cn.c_str());
         return -1;
      }

      std::string mn(sc, p, i - p);

      if (attr & QCA_PUBLIC && attr & QCA_PRIVATE) {
         error("%s:%d: %s::%s() declared both public and private\n", fileName, lineNumber, cn.c_str(), mn.c_str());
         return -1;
      }

      p = sc.find('(', i);
      if (p == std::string::npos) {
         error("%s:%d: premature EOL reading parameters for %s::%s()\n", fileName, lineNumber, cn.c_str(), mn.c_str());
         return -1;
      }
      ++p;
      
      i = sc.find(')', p);
      if (i == std::string::npos) {
         error("%s:%d: premature EOL reading parameters for %s::%s()\n", fileName, lineNumber, cn.c_str(), mn.c_str());
         return -1;
      }

      size_t cs = sc.find('{', i + 1);
      if (cs == std::string::npos) {
         error("%s:%d: premature EOL looking for open brace for %s::%s()\n", fileName, lineNumber, cn.c_str(), mn.c_str());
         return -1;
      }

      // get params
      paramlist_t params;
      if (i != p + 1) {
         std::string pstr(sc, p , i - p);
         trim(pstr);
         if (!pstr.empty()) {
            strlist_t pl;
            getStringList(pl, pstr);

            for (unsigned xi = 0; xi < pl.size(); ++xi) {
               trim(pl[xi]);
               i = pl[xi].find(' ');
               if (i == std::string::npos) {
                  error("%s:%d: %s::%s(): cannot find type for parameter '%s'\n", fileName, lineNumber, cn.c_str(), mn.c_str(), pl[xi].c_str());
                  return -1;
               }
               std::string type(pl[xi], 0, i);
               std::string param(pl[xi], i + 1);
               trim(type);
               trim(param);
               
               std::string val;
               // see if there is a default value
               i = param.find('=');
               if (i != std::string::npos) {
                  val.assign(param, i + 1, std::string::npos);
                  param.erase(i);
                  trim(val);
                  trim(param);
               }

               log(LL_DEBUG, "+ %s::%s() param %d type '%s' name '%s' default value '%s'\n", cn.c_str(), mn.c_str(), xi, type.c_str(), param.c_str(), val.c_str());
               params.push_back(Param(type, param, val));
            }
         }
      }
      sc.erase(0, cs + 1);
      bool eol = false;
      while (sc[0] == ' ' || sc[0] == '\n') {
         if (!eol && sc[0] == '\n')
            eol = true;
         else if (eol && sc[0] == ' ')
            break;
         sc.erase(0, 1);
      }

      // remove trailing '}' and newlines
      size_t len = sc.size();
      assert(sc[len - 1] == '}');
      do {
         sc.erase(--len);
      } while (len && sc[len - 1] == ' ' || sc[len - 1] == '\n');

      ci->second->addMethod(mn, attr, return_type, params, sc, doc);
      return 0;
   }

public:
   Code(const char *fn) : fileName(fn), lineNumber(0), valid(true) {
      cppFileName = "t.cpp";
      doxFileName = "dox.cpp";
      parse();
   }

   ~Code() {
      for (source_t::iterator i = source.begin(), e = source.end(); i != e; ++i)
	 delete *i;
   }

   int serializeCpp() const {
      FILE *fp = fopen(cppFileName.c_str(), "w");
      if (!fp) {
	 error("%s: %s\n", cppFileName.c_str(), strerror(errno));
         return -1;
      }

      for (source_t::const_iterator i = source.begin(), e = source.end(); i != e; ++i)
	 (*i)->serializeCpp(fp);

      return 0;
   }

   int serializeDox() const {
      FILE *fp = fopen(doxFileName.c_str(), "w");
      if (!fp) {
	 error("%s: %s\n", doxFileName.c_str(), strerror(errno));
         return -1;
      }

      for (source_t::const_iterator i = source.begin(), e = source.end(); i != e; ++i)
	 (*i)->serializeDox(fp);

      return 0;
   }

   operator bool() const {
      return valid;
   }
};

void init() {
   // initialize attribute map
   amap["public"] = QCA_PUBLIC;
   amap["private"] = QCA_PRIVATE;
   amap["static"] = QCA_STATIC;
   amap["synchronized"] = QCA_SYNCHRONIZED;
}

void usage() {
   printf("syntax: %s <filename>\n", pn.c_str());
}

int main(int argc, char *argv[]) {
   pn = basename(argv[0]);

   if (argc < 2) {
      usage();
      exit(1);
   }

   // initialize static reference data
   init();

   Code code(argv[1]);
   if (!code) {
      error("please correct the errors above and try again\n");
      exit(1);
   }

   // create cpp output file
   code.serializeCpp();

   return 0;
}
