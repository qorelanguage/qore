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

#include <string>
#include <vector>
#include <map>

std::string pn;

typedef std::vector<std::string> strlist_t;
typedef std::map<std::string, std::string> strmap_t;
typedef std::map<std::string, int64, ltstrcase> dmap_t;

// domain map
static dmap_t dmap;

enum LogLevel {
   LL_CRITICAL = 0,
   LL_IMPORANT = 1,
   LL_INFO     = 2,
   LL_DETAIL   = 3,
   LL_DEBUG    = 4,
};

LogLevel LL = LL_DEBUG;

#define BUFSIZE 1024

int64 get_domain(const std::string &dom) {
   dmap_t::const_iterator i = dmap.find(dom);
   return i == dmap.end() ? -1 : i->second;
}

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
      fprintf(fp, buf.c_str());
   }

   virtual void serializeDox(FILE *fp) const {
      fprintf(fp, buf.c_str());
   }   
};

class Argument {
public:
   std::string type, name;
};

class Method {
protected:
   std::string name, docs, return_type, code;
   strlist_t args;

public:
   const char *getName() const {
      return name.c_str();
   }
};

class ClassElement : public AbstractElement {
protected:
   std::string name,
      doc, 
      arg;
   strlist_t deptypes;
   int64 domain, 
      flags;
   bool valid, 
      upm;              // unset public member flag

   int getStringList(strlist_t &l, const std::string &str) {
      xxx
   }

public:
   ClassElement(const std::string &n_name, const strmap_t &props, const std::string &n_doc) : name(n_name), doc(n_doc), valid(true), upm(false) {
      log(LL_INFO, "registering class '%s'\n", name.c_str());

      // process properties
      for (strmap_t::const_iterator i = props.begin(), e = props.end(); i != e; ++i) {
         //log(LL_DEBUG, "+ prop: '%s': '%s'\n", i->first.c_str(), i->second.c_str());

         // parse domain
         if (i->first == "dom") {
            domain = get_domain(i->second);
            if (domain == -1) {
               error("class '%s' has invalid domain '%s'\n", name.c_str(), i->second.c_str());
               valid = false;
            }
            else
               log(LL_DEBUG, "+ domain: %s: %d\n", i->second.c_str(), (int)domain);
            continue;
         }

         if (i->first == "arg") {
            arg = i->second;
            log(LL_DEBUG, "+ arg: %s\n", arg.c_str());
            continue;
         }

         if (i->first == "flags") {
         }

         error("+ prop: '%s': '%s' - unknown property '%s'\n", i->first.c_str(), i->second.c_str(), i->first.c_str());
         valid = false;
      }

      if (arg.empty()) {
         valid = false;
         error("class '%s' has no 'arg' property\n", name.c_str());
      }
   }

   operator bool() const {
      return valid;
   }

   const char *getName() const {
      return name.c_str();
   }

   int addMethod(std::string &mname, std::string &code, std::string &doc) {
      log(LL_DETAIL, "adding method %s::%s()\n", name.c_str(), mname.c_str());
      return 0;
   }

   virtual void serializeCpp(FILE *fp) const {
      assert(false);
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
   unsigned lineNumber;
   source_t source;
   cmap_t cmap;

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
   
public:
   Code() : fileName(0), lineNumber(0) {
   }

   ~Code() {
      for (source_t::iterator i = source.begin(), e = source.end(); i != e; ++i)
	 delete *i;
   }

   int parse(const char *f) {
      FILE *fp = fopen(f, "r");
      if (!fp) {
	 error("%s: %s\n", f, strerror(errno));
         return -1;
      }

      fileName = f;
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
               continue;
            }

            const char *p;
            if ((p = strstr(sc.c_str(), "::")) && strchr(sc.c_str(), '{')) {
               if (readUntilClose(sc, fp)) {
                  error("%s:%d: premature EOF reading method definition\n", fileName, lineNumber);
                  rc = -1;
                  break;
               }
               
               p = strstr(sc.c_str(), "::");

               // get class name
               const char *p1 = p;
               while (p1 != sc.c_str()) {
                  if (*p1 == ' ') {
                     ++p1;
                     break;
                  }
                  --p1;
               }
               std::string cn(p1, p - p1);
               cmap_t::iterator i = cmap.find(cn);
               if (i == cmap.end()) {
                  error("%s:%d: reference to undefined class '%s'\n", fileName, lineNumber, cn.c_str());
                  rc = -1;
                  break;
               }

               // get method name
               p += 2;
               p1 = p + 1;
               while (*p1 && *p1 != ' ' && *p1 != '(')
                  ++p1;
               if (!*p1) {
                  error("%s:%d: premature EOL reading method definition\n", fileName, lineNumber);
                  rc = -1;
                  break;
               }
               std::string mn(p, p1 - p);

               i->second->addMethod(mn, sc, buf);
               continue;
            }
         }

         buf += str;
      }

      checkBuf(buf);

      fclose(fp);
      lineNumber = 0;
      fileName = 0;

      return rc;
   }

   void serializeCpp(FILE *fp) const {
      for (source_t::const_iterator i = source.begin(), e = source.end(); i != e; ++i)
	 (*i)->serializeCpp(fp);
   }

   void serializeDox(FILE *fp) const {
      for (source_t::const_iterator i = source.begin(), e = source.end(); i != e; ++i)
	 (*i)->serializeDox(fp);
   }
};

void init() {
   dmap["process"] = QDOM_PROCESS;
   dmap["network"] = QDOM_NETWORK;
   dmap["external_process"] = QDOM_EXTERNAL_PROCESS;
   dmap["filesystem"] = QDOM_FILESYSTEM;
   dmap["thread_class"] = QDOM_THREAD_CLASS;
   dmap["thread_control"] = QDOM_THREAD_CONTROL;
   dmap["database"] = QDOM_DATABASE;
   dmap["gui"] = QDOM_GUI;
   dmap["terminal_io"] = QDOM_TERMINAL_IO;
   dmap["external_info"] = QDOM_EXTERNAL_INFO;
   dmap["thread_info"] = QDOM_THREAD_INFO;
   dmap["locale_control"] = QDOM_LOCALE_CONTROL;
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

   Code code;
   code.parse(argv[1]);

   return 0;
}
