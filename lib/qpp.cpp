/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qpp.cpp

  Qore Pre-Processor

  Qore Programming Language
  
  Copyright 2003 - 2012 David Nichols
  
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

// cannot include Qore.h with the Oracle Sun Studio compiler
// when not linking with libqore as link errors result
#include <qore/common.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include "getopt_long.h"
#endif

#include <string>
#include <vector>
#include <map>
#include <set>

const char usage_str[] = "usage: %s [options] <input file(s)...>\n" \
   " -d, --dox-output=arg   doxygen output file name\n" \
   " -h, --help             this help text\n" \
   " -o, --output=arg       cpp output file name\n" \
   " -t, --table=arg        process the given file for doxygen tables (|!...)\n" \
   " -v, --verbose          increases verbosity level\n";

static const option pgm_opts[] = {
   {"dox-output", required_argument, NULL, 'd'},
   {"help", no_argument, NULL, 'h'},
   {"output", required_argument, NULL, 'o'},
   {"table", required_argument, NULL, 't'},
   {"verbose", optional_argument, NULL, 'v'},
   {NULL, 0, NULL, 0}
};

enum LogLevel {
   LL_CRITICAL  = 0,
   LL_IMPORTANT = 1,
   LL_INFO      = 2,
   LL_DETAIL    = 3,
   LL_DEBUG     = 4
};

static struct qpp_opts {
   std::string output_fn;
   std::string dox_fn;
   std::string table_fn;
   int verbose;
   qpp_opts() : verbose(LL_INFO) {
   }
} opts;

// program name
std::string pn;

// extra c++ initialization string
std::string initcode;

// code attribute type
typedef unsigned int attr_t;

typedef std::vector<std::string> strlist_t;
typedef std::map<std::string, std::string> strmap_t;
typedef std::map<std::string, attr_t> amap_t;
typedef std::set<std::string> strset_t;

// for qppval values
strmap_t qppval;

// for tracking enumerated variants
typedef std::map<std::string, unsigned> vmap_t;

// code attribute map
static amap_t amap;

// qore type to qore c++ typeinfo map
static strmap_t tmap;

// qore value to c++ value map
static strmap_t valmap;

// set of types indicating objects
static strset_t oset;

// parameter structure
struct Param {
   std::string type,  // param type
      name,           // param name
      val,            // param default value
      qore;           // param Qore class name (for objects)

   Param(const std::string &t, const std::string &n, const std::string &v, const std::string &q) : type(t), name(n), val(v), qore(q) {
   }
};

// parameter list
typedef std::vector<Param> paramlist_t;

// code attribute bitfield defines
#define QCA_NONE            0
#define QCA_PUBLIC          (1 << 0)
#define QCA_PRIVATE         (1 << 1)
#define QCA_STATIC          (1 << 2)
#define QCA_SYNCHRONIZED    (1 << 3)
#define QCA_USES_EXTRA_ARGS (1 << 4)

#define BUFSIZE 1024

static int my_vsprintf(std::string &buf, const char *fmt, va_list args) {
   if (buf.size() < BUFSIZE)
      buf.resize(BUFSIZE);

   // copy formatted string to priv->buffer
   int i = ::vsnprintf((char*)buf.c_str(), buf.size(), fmt, args);

#ifdef HPUX
   // vsnprintf failed but didn't tell us how big the priv->buffer should be
   if (i < 0) {
      buf.resize(buf.size() << 1);
      return -1;
   }
#endif
   bool err = i >= (int)buf.size();
   buf.resize(i + 1);
   return err ? -1 : 0;
}

static void error(const char *fmt, ...) {
   std::string buf;
   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = my_vsprintf(buf, fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   
   fprintf(stderr, "%s: ERROR: ", pn.c_str());
   fputs(buf.c_str(), stderr);
   //fflush(stderr);
}

static void warning(const char *fmt, ...) {
   std::string buf;
   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = my_vsprintf(buf, fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   
   fprintf(stderr, "%s: WARNING: ", pn.c_str());
   fputs(buf.c_str(), stderr);
   //fflush(stderr);
}

static void log(LogLevel ll, const char *fmt, ...) {
   if (ll > opts.verbose)
      return;

   std::string buf;
   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = my_vsprintf(buf, fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   
   fprintf(stderr, "%s: ", pn.c_str());
   if (ll == LL_DEBUG)
      fputs("DEBUG: ", stdout);
   fputs(buf.c_str(), stdout);
   fflush(stdout);
}

void toupper(std::string &str) {
   for (unsigned i = 0; i < str.size(); ++i)
      str[i] = toupper(str[i]);
}

static bool whitespace(int c) {
   return c == ' ' || c == '\n' || c == '\t';
}

static void trim(std::string &str) {
   while (whitespace(str[0]))
      str.erase(0, 1);
   while (true) {
      size_t len = str.size();
      if (!len || !whitespace(str[len - 1]))
         break;
      str.erase(len - 1);
   }
}

static int read_line(unsigned& lineNumber, std::string &str, FILE *fp) {
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

static int get_dox_comment(const char* fileName, unsigned& lineNumber, std::string& str, FILE* fp, bool already_started = false) {
   if (!already_started) {
      std::string buf;
      if (read_line(lineNumber, buf, fp)) {
         error("%s:%d: premature EOF reading doxygen comment\n", fileName, lineNumber);
         return -1;
      }
      
      if (buf.compare(0, 3, "/**")) {
         error("%s:%d: missing block comment marker '/**' in line following //! comment (str='%s' buf='%s')\n", fileName, lineNumber, str.c_str(), buf.c_str());
         return -1;
      }

      str += buf;
   }

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
   read_line(lineNumber, str, fp);

   return 0;
}

static void get_string_list(strlist_t &l, const std::string &str, char separator = ',') {
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

int parse_attributes(const char* fileName, unsigned& lineNumber, attr_t& attr, std::string& return_type, std::string& sc, size_t i) {
   strlist_t pre;
   // get return type and flags
   std::string pstr(sc, 0, i - 1);
   //log(LL_DEBUG, "PSTR='%s'\n", pstr.c_str());
   get_string_list(pre, pstr, ' ');

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

   if (attr & QCA_PUBLIC && attr & QCA_PRIVATE) {
      error("%s:%d: declared both public and private\n", fileName, lineNumber);
      return -1;
   }

   return 0;
}

static int missing_value_error(const char* fileName, unsigned lineNumber, const std::string &propstr) {
   error("%s:%d: missing '=' or value in property string '%s'\n", fileName, lineNumber, propstr.c_str());
   return -1;
}

static int add_element(const char* fileName, unsigned lineNumber, const std::string &propstr, strmap_t &props, size_t start, size_t end) {
   size_t eq = propstr.find('=', start);
   if (eq == std::string::npos || eq >= end)
      return missing_value_error(fileName, lineNumber, propstr);
   while (start < eq && propstr[start] == ' ')
      ++start;
   if (start == eq) {
      error("%s:%d: missing property name in property string '%s'\n", fileName, lineNumber, propstr.c_str());
      return -1;
   }
   size_t tend = end;
   while (tend > eq && propstr[tend] == ' ')
      --tend;

   if (tend == eq)
      return missing_value_error(fileName, lineNumber, propstr);

   std::string key(propstr, start, eq - start);
   std::string value(propstr, eq + 1, end - eq -1);
   props[key] = value;
   return 0;
}

int parse_properties(const char* fileName, unsigned lineNumber, std::string &propstr, strmap_t &props) {
   size_t start = 0;
   while (true) {
      size_t end = propstr.find(';', start);
      if (end == std::string::npos)
         break;
      if (add_element(fileName, lineNumber, propstr, props, start, end))
         return -1;
      start = end + 1;
   }

   return add_element(fileName, lineNumber, propstr, props, start, propstr.size());
}

int parse_params_and_flags(const char* fileName, unsigned& lineNumber, strmap_t& flags, paramlist_t& params, attr_t& attr, std::string& sc, size_t p, const std::string& dn) {
   size_t i = sc.find(')', p);
   if (i == std::string::npos) {
      error("%s:%d: premature EOL reading parameters for %s()\n", fileName, lineNumber, dn.c_str());
      return -1;
   }

   size_t cs = sc.find('{', i + 1);
   if (cs == std::string::npos) {
      error("%s:%d: premature EOL looking for open brace for %s()\n", fileName, lineNumber, dn.c_str());
      return -1;
   }

   // get flags if any
   if (cs - i > 2) {
      std::string fstr(sc, i + 1, cs - i - 2);
      size_t f = fstr.find('[');
      if (f != std::string::npos) {
         fstr.erase(0, f + 1);
         size_t fe = fstr.find(']');
         if (fe == std::string::npos) {
            error("%s:%d: premature EOL looking for ']' in flags for %s()\n", fileName, lineNumber, dn.c_str());
            return -1;
         }
         fstr.erase(fe);

         log(LL_DEBUG, "fstr='%s'\n", fstr.c_str());

         // parse properties
         if (parse_properties(fileName, lineNumber, fstr, flags))
            return -1;
      }
   }

   // get params
   if (i != p + 1) {
      std::string pstr(sc, p , i - p);
      trim(pstr);
      if (!pstr.empty()) {
         strlist_t pl;
         get_string_list(pl, pstr);

         for (unsigned xi = 0; xi < pl.size(); ++xi) {
            trim(pl[xi]);

            // handle special case arg "..." - which is just for documentation
            if (pl[xi] == "...") {
               params.push_back(Param(pl[xi], pl[xi], "", ""));
               attr |= QCA_USES_EXTRA_ARGS;
               continue;
            }

            i = pl[xi].find(' ');
            if (i == std::string::npos) {
               error("%s:%d: %s(): cannot find type for parameter '%s'\n", fileName, lineNumber, dn.c_str(), pl[xi].c_str());
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

            std::string qore;

            // see if there's a Qore class name
            if (type[type.size() - 1] == ']') {
               i = type.find('[');
               if (i == std::string::npos) {
                  error("%s:%d: %s(): cannot find open square bracket '[' in type name '%s' in parameter '%s'\n", fileName, lineNumber, dn.c_str(), type.c_str(), param.c_str());
                  return -1;
               }
               type.erase(type.size() - 1);
               qore = type.c_str() + i + 1;
               type.erase(i);                  
            }

            log(LL_DEBUG, "+ %s() param %d type '%s' name '%s' default value '%s'\n", dn.c_str(), xi, type.c_str(), param.c_str(), val.c_str());
            params.push_back(Param(type, param, val, qore));
         }
      }
   }

   // erase function/method header
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
   } while (len && (sc[len - 1] == ' ' || sc[len - 1] == '\n'));

   return 0;
}

static void get_type_name(std::string &t, const std::string &type) {
   size_t cp = type.rfind("::");
   if (cp == std::string::npos)
      t = type;
   else
      t.assign(type, cp + 2, -1);
}

// takes into account quotes and @code @endcode
static void get_string_list2(strlist_t &l, const std::string &str, char separator = ',') {
   size_t start = 0,
      p = 0;
   int quote = 0;
   bool code = false;

   while (p < str.size()) {
      int c = str[p++];

      if (c == '@') {
         if (code) {
            if (!str.compare(p, 7, "endcode")) {
               code = false;
               p += 7;
            }
            continue;
         }
         if (!str.compare(p, 4, "code")) {
            code = true;
            p += 4;
         }
         continue;
      }
      if (code)
         continue;

      if (c == '\'' || c == '\"') {
         if (quote == c)
            quote = 0;
         else if (!quote)
            quote = c;
         continue;
      }
      if (quote)
         continue;

      if (c == separator) {
         l.push_back(std::string(str, start, p - start - 1));
         start = p;
      }
   }
   l.push_back(std::string(str, start));

   //for (unsigned i = 0; i < l.size(); ++i)
   //   printf("DBG: list %u/%lu: %s\n", i, l.size(), l[i].c_str());
}

static int get_qore_type(const std::string &qt, std::string &cppt) {
   if (qt.empty()) {
      cppt = "nothingTypeInfo";
      return 0;
   }
      
   strmap_t::iterator i = tmap.find(qt);
   if (i == tmap.end()) {
      // assume a Qore object of the given class
      std::string qc;
      get_type_name(qc, qt);
      bool on = qt[0] == '*';
      if (on)
         qc.erase(0, 1);
      toupper(qc);
      qc = "QC_" + qc;
      qc += on ? "->getOrNothingTypeInfo()" : "->getTypeInfo()";
      log(LL_DEBUG, "registering object return type '%s': '%s'\n", qt.c_str(), qc.c_str());
      oset.insert(qt);
      tmap[qt] = qc;
      cppt = tmap[qt];
   }
   else
      cppt = i->second;
   return 0;
}

static bool is_object(const std::string &qt) {
   return !qt.empty() && (oset.find(qt) != oset.end() || tmap.find(qt) == tmap.end());
}

int read_until_close(const char* fileName, unsigned& lineNumber, std::string& str, FILE* fp, int cchar = '}') {
   int quote = 0;

   // curly bracket count
   unsigned bc = 1;

   bool backquote = false;
   bool line_comment = false;
   // current character
   int c = 0;
   // last character read
   int last;

   while (true) {
      last = c;
      c = fgetc(fp);
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
         line_comment = false;
         continue;
      }
         
      if (line_comment)
         continue;

      if (c == '/' && last == '/') {
         line_comment = true;
         continue;
      }

      if (c == '"' || c == '\'') {
         log(LL_DEBUG, "found %c on line %d (quote: %c %d)\n", c, lineNumber, quote ? quote : 'x', quote);
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

      if (cchar == '}' && c == '{') {
         log(LL_DEBUG, "found '{' on line %d (bc: %d)\n", lineNumber, bc);
         ++bc;
         continue;
      }

      if (c == cchar) {
         log(LL_DEBUG, "found '%c' on line %d (bc: %d)\n", cchar, lineNumber, bc);
         if (!--bc)
            break;
      }
   }

   return 0;
}

static void add_init_code(FILE* fp) {
   if (initcode.empty())
      return;

   fprintf(fp, "\n   // initialization code added by qppinit\n   %s", initcode.c_str());
   initcode.clear();
}

#define T_INT    0
#define T_FLOAT  1
#define T_STRING 2
#define T_HASH   3
#define T_BIN    4
#define T_OTHER  5
static int get_val_type(const std::string &str) {
   if (!str.empty()) {
      if ((str[0] == '"' && str[str.size() - 1] == '"') || (str[0] == '\'' && str[str.size() - 1] == '\''))
         return T_STRING;
      if (str[0] == '(' && str[str.size() - 1] == ')')
         return T_HASH;
      if (str[0] == '<' && str[str.size() - 1] == '>')
         return T_BIN;
   }

   bool pint = false,  // has integers
      pucus = false,   // has uppercase and/or underscore chars
      pdec = false,    // has a decimal point
      poth = false;    // has other chars

   size_t start = 0;
   if (str[0] == '-') {
      start = 1;
      pint = true;
   }
   for (size_t i = start, e = str.size(); i != e; ++i) {
      char c = str[i];
      if (isdigit(c))
         pint = true;
      else if (isupper(c) || c == '_')
         pucus = true;
      else if (c == '.')
         pdec = true;
      else
         poth = true;
   }

   if (poth)
      return T_OTHER;
   if (pdec)
      return pint && !pucus ? T_FLOAT : T_OTHER;
   if (pucus || pint)
      return T_INT;
   return T_OTHER;
}

static int get_qore_value(const std::string &qv, std::string &v, const char *cname = 0, const char** prefix = 0) {
   {
      strmap_t::iterator i = valmap.find(qv);
      if (i != valmap.end()) {
         v = i->second;
         return 0;
      }
   }
   
   int t = get_val_type(qv);
   switch (t) {
      case T_INT: {
         v = "new QoreBigIntNode(";
         v += qv;
         v += ")";
         return 0;
      }
      case T_FLOAT: {
         v = "new QoreFloatNode(";
         v += qv;
         v += ")";
         return 0;
      }
      case T_STRING: {
         v = "new QoreStringNode(";
         v += qv;
         v += ")";
         return 0;
      }
      case T_BIN: {
         if (!cname) {
            error("cannot create a hash for a non-constant value\n");
            return -1;
         }

         assert(prefix);
         (*prefix) = "bin";

         v = "BinaryNode* bin_";
         v += cname;
         v += " = new BinaryNode;\n   bin_";
         v += cname;
         v += "->append(\"";

         for (unsigned i = 1; i < qv.size() - 1; ++i) {
            v += "\\x";
            v += qv[i];
            v += qv[++i];
         }
         v += "\", ";
         char buf[20];
         sprintf(buf, "%ld", qv.size() / 2 - 1);
         v += buf;
         v += ");\n";
         return 0;
      }
      case T_HASH: {
         if (!cname) {
            error("cannot create a hash for a non-constant value\n");
            return -1;
         }
         assert(prefix);
         (*prefix) = "hash";

         v = "QoreHashNode* hash_";
         v += cname;
         v += " = new QoreHashNode;\n";

         std::string hv(qv, 1, qv.size() -2);

         strlist_t l;
         get_string_list2(l, hv, ',');
         
         for (unsigned i = 0; i < l.size(); ++i) {
            std::string& str = l[i];
            trim(str);
            if (str.empty())
               continue;
            size_t sep = str.find(':');
            if (sep == std::string::npos) {
               error("cannot find ':' in hash line: '%s' (line %d)\n", str.c_str(), i);
               return -1;
            }
            std::string key(str, 0, sep);
            trim(key);
            if (key.empty()) {
               error("hash key is empty\n");
               return -1;
            }
         
            if (key[0] == '"') {
               if (key[key.size() - 1] != '"') {
                  error("missing end quote in hash key '%s'\n", key.c_str());
                  return -1;
               }
            }
            else {
               key.insert(0, "MAKE_STRING_FROM_SYMBOL(");
               key += ")";
            }

            std::string value(str, sep + 1);
            trim(value);
            std::string qoreval;
            if (get_qore_value(value, qoreval))
               return -1;

            v += "   hash_";
            v += cname;
            v += "->setKeyValue(" + key + ", " + qoreval + ", 0);\n";
         }
         return 0;
      }
   }

   {
      strmap_t::iterator i = qppval.find(qv);
      if (i != qppval.end()) {
         v = i->second;
         return 0;
      }
   }

   error("could not match qore value '%s' to a c++ value\n", qv.c_str());
   return -1;
}

static void doRow(strlist_t &sl, std::string &tstr) {
   tstr += "   <tr>\n";
   for (unsigned k = 0; k < sl.size(); ++k) {
      tstr += "      ";
      if (sl[k][0] == '!') {
         sl[k].erase(0, 1);
         tstr += "<td class=\"qore\"><b>" + sl[k] + "</b></td>\n";
      }
      else
         tstr += "<td>" + sl[k] + "</td>\n";
   }
   tstr += "   </tr>\n";
}

static int serialize_dox_comment(FILE* fp, std::string &buf) {
   size_t start = 0;
   while (true) {
      size_t i = buf.find("|!", start);
      log(LL_DEBUG, "TextElement::serializeDox() looking for |! i: %d\n", i);
      if (i == std::string::npos)
         break;

      size_t start = i;

      size_t end;

      // find end of line
      size_t j = buf.find('\n', i + 2);
      log(LL_DEBUG, "TextElement::serializeDox() looking for first EOL j: %d\n", j);
      if (j == std::string::npos)
         break;

      end = j;

      std::string tstr = "@htmlonly <style><!-- td.qore { background-color: #5b9409; color: white; } --></style> @endhtmlonly\n<table>";

      std::string str(buf, i + 1, j - i - 1);

      while (true) {
         strlist_t sl;
         get_string_list2(sl, str, '|');

         doRow(sl, tstr);
         i = j + 1;
            
         // find next EOL
         j = buf.find('\n', i);
         if (j == std::string::npos)
            break;

         str.assign(buf, i, j - i);

         // find start of next row, if any
         size_t k = str.find('|');
         if (k == std::string::npos)
            break;

         str.erase(0, k + 1);
         end = j;
      }

      tstr += "</table>\n";

      buf.replace(start, end - start, tstr);
   }
   fputs(buf.c_str(), fp);
   return 0;
}

class ConstantGroupElement {
protected:
   std::string name,
      doc,
      value;

public:
   ConstantGroupElement(const std::string& n, const std::string& d, const std::string& v) : name(n), doc(d), value(v) {
   }

   int serializeCpp(FILE *fp) {
      // should never be called
      assert(false);
      return 0;
   }

   int serializeCppBinding(FILE *fp) const {
      std::string qv;
      const char* prefix = 0;
      if (get_qore_value(value, qv, name.c_str(), &prefix))
         return -1;
      if (prefix)
         fprintf(fp, "   %s\n   qorens.addConstant(\"%s\", %s_%s);\n", qv.c_str(), name.c_str(), prefix, name.c_str());
      else
         fprintf(fp, "   qorens.addConstant(\"%s\", %s);\n", name.c_str(), qv.c_str());
      return 0;
   }

   int serializeDox(FILE *fp) {
      fputs(doc.c_str(), fp);
      fprintf(fp, "   const %s = %s;\n", name.c_str(), value.c_str());
      return 0;
   }
};

class TextGroupElement {
protected:
   std::string text;

public:
   TextGroupElement(const std::string &t) : text(t) {
   }

   int serializeCpp(FILE *fp) const {
      fputs(text.c_str(), fp);
      return 0;
   }

   int serializeCppBinding(FILE *fp) const {
      fputs(text.c_str(), fp);
      return 0;
   }

   int serializeDox(FILE *fp) const {
      fputs(text.c_str(), fp);
      return 0;
   }
};

class CodeBase {
protected:
   enum ReturnType { RT_ANY = 0, RT_INT = 1, RT_BOOL = 2, RT_OBJ = 3, RT_FLOAT = 4 };

   std::string name,   // name
      vname,           // variant name
      docs,            // docs
      return_type,     // return type
      code,            // c++ code
      dom,             // domain
      flags;           // flags
   attr_t attr;
   paramlist_t params;
   ReturnType rt;
   unsigned line;
   bool has_return,
      doconly;         // only for documentation

   void serializeArgs(FILE *fp, const char *cname = 0, bool rv = true) const {
      for (unsigned i = 0; i < params.size(); ++i) {
         const Param &p = params[i];

         if (!p.qore.empty()) {
            // skip args that are only there for documentation (otherwise we get unused variable warnings)
            if (p.qore == "doc")
               continue;

            //log(LL_DEBUG, "found object class '%s' param name '%s'\n", p.type.c_str(), p.name.c_str());
            //HARD_QORE_OBJ_DATA(cert, QoreSSLCertificate, args, 0, CID_SSLCERTIFICATE, "Socket::setCertificate()", "SSLCertificate", xsink);
            std::string cid;
            get_type_name(cid, p.type);
            toupper(cid);            
            fprintf(fp, "   HARD_QORE_OBJ_DATA(%s, %s, args, %d, CID_%s, \"%s%s%s()\", \"%s\", xsink);\n   if (*xsink)\n      return%s;\n",
                    p.name.c_str(), p.qore.c_str(), i, cid.c_str(), cname ? cname : "", cname ? "::" : "", name.c_str(), p.type.c_str(), rv ? " 0" : "");
            continue;
         }

         if (p.type == "int" || p.type == "softint" || p.type == "timeout") {
            fprintf(fp, "   int64 %s = HARD_QORE_INT(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "float" || p.type == "softfloat") {
            fprintf(fp, "   double %s = HARD_QORE_FLOAT(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "bool" || p.type == "softbool") {
            fprintf(fp, "   bool %s = HARD_QORE_BOOL(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "string" || p.type == "softstring") {
            fprintf(fp, "   const QoreStringNode* %s = HARD_QORE_STRING(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*string" || p.type == "*softstring") {
            fprintf(fp, "   const QoreStringNode* %s = reinterpret_cast<const QoreStringNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "date" || p.type == "softdate") {
            fprintf(fp, "   const DateTimeNode* %s = HARD_QORE_DATE(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*date" || p.type == "*softdate") {
            fprintf(fp, "   const DateTimeNode* %s = reinterpret_cast<const DateTimeNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "binary") {
            fprintf(fp, "   const BinaryNode* %s = HARD_QORE_BINARY(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*binary") {
            fprintf(fp, "   const BinaryNode* %s = reinterpret_cast<const BinaryNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "list" || p.type == "softlist") {
            fprintf(fp, "   const QoreListNode* %s = HARD_QORE_LIST(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*list" || p.type == "*softlist") {
            fprintf(fp, "   const QoreListNode* %s = reinterpret_cast<const QoreListNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "hash") {
            fprintf(fp, "   const QoreHashNode* %s = HARD_QORE_HASH(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*hash") {
            fprintf(fp, "   const QoreHashNode* %s = reinterpret_cast<const QoreHashNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "reference") {
            fprintf(fp, "   const ReferenceNode* %s = HARD_QORE_REF(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*reference") {
            fprintf(fp, "   const ReferenceNode* %s = reinterpret_cast<const ReferenceNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "object") {
            fprintf(fp, "   QoreObject* %s = HARD_QORE_OBJECT(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "*object") {
            fprintf(fp, "   QoreObject* %s = const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(get_param(args, %d)));\n", p.name.c_str(), i);
            continue;
         }
         if (p.type == "code" || p.type == "*code") {
            fprintf(fp, "   const ResolvedCallReferenceNode* %s = reinterpret_cast<const ResolvedCallReferenceNode*>(get_param(args, %d));\n", p.name.c_str(), i);
            continue;
         }

         if (p.type == "any" || p.type == "data") {
            fprintf(fp, "   const AbstractQoreNode* %s = get_param(args, %d);\n", p.name.c_str(), i);
            continue;
         }
         // skip "..." arg which is just for documentation
         if (p.type == "...")
            continue;

         log(LL_CRITICAL, "don't know how to handle argument type '%s' (for arg %s)\n", p.type.c_str(), p.name.c_str());
         assert(false);
      }
   }

   void serializeQoreParams(FILE *fp) const {
      for (unsigned i = 0; i < params.size(); ++i) {
         if (i)
            fputs(", ", fp);
         const Param &p = params[i];
         if (p.type == "...")
            fputs("...", fp);
         else {
            fprintf(fp, "%s %s", p.type.c_str(), p.name.c_str());
            if (!p.val.empty())
               fprintf(fp, " = %s", p.val.c_str());
         }
      }
   }

   int serializeBindingArgs(FILE *fp) const {
      size_t size = params.size();
      if (size && params[size - 1].type == "...")
         --size;

      if (size) {
         fprintf(fp, ", %lu", size);
         for (unsigned i = 0; i < size; ++i) {
            std::string str;
            if (get_qore_type(params[i].type, str))
               return -1;
            fprintf(fp, ", %s, ", str.c_str());
            if (params[i].val.empty())
               fputs("NULL", fp);
            else {
               std::string vs;
               if (get_qore_value(params[i].val, vs))
                  return -1;
               fputs(vs.c_str(), fp);
            }
         }
      }
      return 0;
   }

   void serializeQoreAttrComment(FILE *fp, unsigned indent = 0) const {
      while (indent--)
         fputc(' ', fp);
      fputs("// ", fp);
      if (attr & QCA_SYNCHRONIZED)
         fputs("synchronized ", fp);
      if (attr & QCA_STATIC)
         fputs("static ", fp);
      if (attr & QCA_PRIVATE)
         fputs("private ", fp);
   }

   const char *getReturnType() const {
      switch (rt) {
         case RT_OBJ:
            return "QoreObject*";
         case RT_INT:
            return "int64";
         case RT_BOOL:
            return "bool";
         default:
            return "AbstractQoreNode*";
      }
   }

   static void serializeQoreCppType(FILE *fp, const std::string &tstr) {
      if (tstr == "...")
         return;

      size_t i = tstr.find('*');
      if (i == std::string::npos) {
         fputs(tstr.c_str(), fp);
         return;
      }

      std::string t = tstr;
      t.replace(i, 1, "__7_ ");
      fputs(t.c_str(), fp);
   }

   const char *getFunctionType() const {
      switch (rt) {
         case RT_INT:
            return "q_func_int64_t";
         case RT_BOOL:
            return "q_func_bool_t";
         case RT_FLOAT:
            return "q_func_double_t";
         default:
            return "q_func_t";
      }
   }

public:
   CodeBase(const std::string &n_name, attr_t n_attr, const paramlist_t &n_params, 
            const std::string &n_docs, const std::string &n_return_type, 
            const std::string &n_flags, const std::string &n_dom, const std::string &n_code,
            unsigned vnum, unsigned n_line, bool n_doconly) : name(n_name), vname(name), docs(n_docs), return_type(n_return_type), 
                                                              code(n_code), dom(n_dom), flags(n_flags), attr(n_attr), params(n_params), 
                                                              rt(RT_ANY), line(n_line), has_return(false), doconly(n_doconly) {
      if (return_type == "int" || return_type == "softint")
         rt = RT_INT;
      else if (return_type == "bool" || return_type == "softbool")
         rt = RT_BOOL;
      else if (is_object(return_type))
         rt = RT_OBJ;

      // check if there is a return statement on the last line
      size_t i = code.rfind('\n');
      if (i == std::string::npos)
         i = 0;

      has_return = code.find("return", i) != std::string::npos;

      // assign enumerated variant name if vnum > 0
      if (vnum) {
         vname += '_';
         char buf[20];
         sprintf(buf, "%d", vnum);
         vname += buf;
      }

      if (flags.empty())
         flags = attr & QCA_USES_EXTRA_ARGS ? "QC_USES_EXTRA_ARGS" : "QC_NO_FLAGS";
      else if (attr & QCA_USES_EXTRA_ARGS)
         flags += "|QC_USES_EXTRA_ARGS";

      log(LL_DEBUG, "CodeBase::CodeBase() name %s flags '%s'\n", name.c_str(), flags.c_str());
   }

   const char *getName() const {
      return name.c_str();
   }
};

class FunctionGroupElement : public CodeBase {
protected:
   void serializeQorePrototypeComment(FILE *fp, unsigned indent = 0) const {
      serializeQoreAttrComment(fp, indent);
      fprintf(fp, "%s %s(", return_type.empty() ? "nothing" : return_type.c_str(), name.c_str());
      serializeQoreParams(fp);
      fputs(") {}\n", fp);
   }

public:
   FunctionGroupElement(const std::string &n_name, attr_t n_attr, const paramlist_t &n_params, 
                        const std::string &n_docs, const std::string &n_return_type, 
                        const std::string &n_flags, const std::string &n_dom, const std::string &n_code,
                        unsigned vnum, unsigned n_line, bool n_doconly) : CodeBase(n_name, n_attr, n_params, n_docs, n_return_type, 
                                                                                   n_flags, n_dom, n_code, vnum, n_line, n_doconly) {
   }

   int serializeCpp(FILE *fp) const {
      if (doconly)
         return 0;

      serializeQorePrototypeComment(fp);

      fprintf(fp, "static %s f_%s(const QoreListNode* args, ExceptionSink* xsink) {\n", getReturnType(), vname.c_str());
      serializeArgs(fp);
      fputs(code.c_str(), fp);

      if (!has_return)
         fprintf(fp, "\n   return 0;");

      fputs("\n}\n\n", fp);
      return 0;
   }

   int serializeCppBinding(FILE *fp) const {
      if (doconly)
         return 0;

      fputc('\n', fp);
      serializeQorePrototypeComment(fp, 3);

      // get return type
      std::string cppt;
      if (get_qore_type(return_type, cppt))
         return -1;

      fprintf(fp, "   builtinFunctions.add2(\"%s\", (%s)f_%s, %s, %s, %s", 
              name.c_str(),
              getFunctionType(), 
              vname.c_str(),
              flags.c_str(),
              dom.empty() ? "QDOM_DEFAULT" : dom.c_str(), cppt.c_str());

      if (serializeBindingArgs(fp))
         return -1;

      fputs(");\n", fp);

      return 0;
   }

   int serializeDox(FILE *fp) {
      serialize_dox_comment(fp, docs);
      
      serializeQoreCppType(fp, return_type);
      fprintf(fp, " %s(", name.c_str());

      for (unsigned i = 0; i < params.size(); ++i) {
         if (i)
            fputs(", ", fp);
         const Param &p = params[i];
         serializeQoreCppType(fp, p.type);
         fputc(' ' , fp);
         fputs(p.name.c_str(), fp);
         if (!p.val.empty()) {
            fputs(" = ", fp);
            fputs(p.val.c_str(), fp);
         }
      }
      fputs(");\n\n", fp);

      return 0;
   }
};

typedef std::vector<TextGroupElement*> tlist_raw_t;
class tlist_t : public tlist_raw_t {
public:
   ~tlist_t() {
      clear();
   }

   void clear() {
      for (unsigned i = 0; i < size(); ++i)
         delete at(i);
      tlist_raw_t::clear();
   }
};

typedef std::map<std::string, ConstantGroupElement*> cmap_raw_t;
typedef std::multimap<std::string, FunctionGroupElement*> fmap_raw_t;

template <class T>
class MyMap : public T {
public:
   ~MyMap() {
      clear();
   }

   void clear() {
      for (typename T::iterator i = T::begin(), e = T::end(); i != e; ++i)
         delete i->second;
      T::clear();
   }
};

typedef MyMap<cmap_raw_t> cmap_t;
typedef MyMap<fmap_raw_t> fmap_t;

class Group {
protected:
   cmap_t cmap;     // constant map
   fmap_t fmap;     // function multimap
   tlist_t tlist;   // list of text elements

   // function variant number map
   vmap_t vmap;

   std::string doc,  // doc header for group
      ns;            // namespace

   bool valid;
   const char *fileName;
   unsigned &lineNumber;

   void checkBuf(std::string &buf) {
      if (!buf.empty()) {
         tlist.push_back(new TextGroupElement(buf));
         buf.clear();
      }
   }

   int readUntilOpenGroup(const char* fileName, unsigned& lineNumber, std::string& str, FILE* fp) {
      while (true) {
         std::string line;
         if (read_line(lineNumber, line, fp)) {
            error("%s:%d: premature EOF reading group description\n", fileName, lineNumber);
            valid = false;
            return -1;
         }

         str += line;

         if (!line.compare(0, 4, "//@{"))
            break;
      }
      
      return 0;
   }

   int parseFunction(std::string &code, FILE *fp, std::string &doc) {
      unsigned line = lineNumber;
      if (read_until_close(fileName, lineNumber, code, fp)) {
         error("%s:%d: premature EOF reading method definition starting on line %d\n", fileName, lineNumber, line);
         return -1;
      }

      // find start of parameter list
      size_t i = code.find('(');
      if (i == std::string::npos) {
         error("%s:%d: expecting function definition\n", fileName, lineNumber);
         return -1;
      }

      // save the position of the '('
      size_t p = i;

      // find beginning of function name
      while (i && code[i - 1] != ' ')
         --i;

      // get function name
      std::string fn(code, i, p - i);

      // function attributes
      attr_t attr = QCA_NONE;
      // return type
      std::string return_type;

      if (i && parse_attributes(fileName, lineNumber, attr, return_type, code, i))
         return -1;

      strmap_t flags;
      paramlist_t params;

      if (parse_params_and_flags(fileName, lineNumber, flags, params, attr, code, p + 1, fn))
         return -1;

      std::string cf, dom;
      bool doconly = false;
      // parse flags
      for (strmap_t::const_iterator i = flags.begin(), e = flags.end(); i != e; ++i) {
         //log(LL_DEBUG, "+ method %s::%s() flag '%s': '%s'\n", name.c_str(), mname.c_str(), i->first.c_str(), i->second.c_str());
         if (i->first == "dom")
            dom = i->second;
         else if (i->first == "flags")
            cf = i->second;
         else if (i->first == "doconly")
            doconly = true;
         else {
            error("unknown flag '%s' = '%s' defining function %s()\n", i->first.c_str(), i->second.c_str(), fn.c_str());
            return -1;
         }
      }

      //log(LL_INFO, "+ method %s::%s() attr: 0x%x (static: %d)\n", name.c_str(), mname.c_str(), attr, attr & QCA_STATIC);

      unsigned vnum;
      {
         vmap_t::iterator vi = vmap.find(fn);
         if (vi == vmap.end()) {
            vnum = 0;
            vmap[fn] = 0;
         }
         else
            vnum = ++vi->second;
      }

      fmap.insert(fmap_t::value_type(fn, new FunctionGroupElement(fn, attr, params, doc, return_type, cf, dom, code, vnum, line, doconly)));
      return 0;
   }

public:
   Group(std::string &buf, FILE *fp, const char *fn, unsigned &ln) : valid(true), fileName(fn), lineNumber(ln) {     
      doc = buf;
      if (readUntilOpenGroup(fileName, lineNumber, doc, fp)) {
         error("%s:%d: could not find start of group\n", fileName, lineNumber);
         valid = false;
         return;
      }
      buf.clear();

      while (true) {
         std::string line;
         if (read_line(lineNumber, line, fp)) {
            error("%s:%d: premature EOF reading group\n", fileName, lineNumber);
            valid = false;
            return;
         }

         if (!line.compare(0, 3, "//!")) {
            checkBuf(buf);

            // check for block comment
            std::string cdoc = line;
            line.clear();
            if (read_line(lineNumber, line, fp)) {
               error("%s:%d: premature EOF reading group\n", fileName, lineNumber);
               valid = false;
               return;
            }
            if (!line.compare(0, 3, "/**")) {
               if (get_dox_comment(fileName, lineNumber, line, fp, true)) {
                  valid = false;
                  return;
               }
               cdoc += line;
               line.clear();

               if (read_line(lineNumber, line, fp)) {
                  error("%s:%d: premature EOF reading group\n", fileName, lineNumber);
                  valid = false;
                  return;
               }
            }
            

            if (!line.compare(0, 6, "const ")) {
               checkBuf(buf);

               // trim off trailing newline and spaces
               line.erase(line.size() - 1);
               trim(line);
               if (line[line.size() - 1] != ';') {
                  // re-add newline
                  line += '\n';
                  if (read_until_close(fileName, lineNumber, line, fp, ';')) {
                     error("%s:%d: expecting constant definition; got: %s (%c)\n", fileName, lineNumber, line.c_str(), line[line.size() - 1]);
                     valid = false;
                  }
               }

               line.erase(0, 6);
               // erase trailing ';'
               line.erase(line.size() - 1);
               trim(line);

               size_t i = line.find('=');
               if (i == std::string::npos) {
                  error("%s:%d: invalid constant definition: const %s\n", fileName, lineNumber, line.c_str());
                  valid = false;
                  return;
               }

               std::string name, val;
               name.assign(line, 0, i);
               val.assign(line, i + 1, -1);
               trim(name);
               trim(val);
               if (cmap.find(name) != cmap.end()) {
                  error("%s:%d: constant '%s' defined twice\n", fileName, lineNumber, name.c_str());
                  valid = false;
                  return;
               }

               cmap[name] = new ConstantGroupElement(name, cdoc, val);
               continue;
            }

            if (parseFunction(line, fp, cdoc)) {
               valid = false;
               return;
            }

            continue;
         }

         //log(LL_INFO, "Group line: %s", line.c_str());
         if (!line.compare(0, 4, "//@}"))
             break;

         buf += line;
      }
      checkBuf(buf);

      if (!fmap.empty() && !cmap.empty()) {
         error("group has both constants and functions defined in it; this is invalid\n");
         valid = false;
      }
   }

   operator bool() const {
      return valid;
   }

   int serializeFunctionCpp(FILE *fp) const {
      if (fmap.empty())
         return 0;

      // first serialize text
      for (unsigned i = 0; i < tlist.size(); ++i)
         tlist[i]->serializeCpp(fp);

      // now serialize function group members
      fputc('\n', fp);
      for (fmap_t::const_iterator i = fmap.begin(), e = fmap.end(); i != e; ++i)
         if (i->second->serializeCpp(fp))
            return -1;

      return 0;
   }

   int serializeCppConstantBindings(FILE *fp) const {
      // first serialize text
      for (unsigned i = 0; i < tlist.size(); ++i)
         tlist[i]->serializeCpp(fp);

      if (!cmap.empty()) {
         fputc('\n', fp);
         for (cmap_t::const_iterator i = cmap.begin(), e = cmap.end(); i != e; ++i)
            if (i->second->serializeCppBinding(fp))
               return -1;
      }
      return 0;
   }

   int serializeCppFunctionBindings(FILE *fp) const {
      if (!fmap.empty()) {
         fputc('\n', fp);
         for (fmap_t::const_iterator i = fmap.begin(), e = fmap.end(); i != e; ++i)
            if (i->second->serializeCppBinding(fp))
               return -1;
      }
      return 0;
   }

   int serializeFunctionDox(FILE *fp) {
      if (fmap.empty())
         return 0;

      // serialize group header doc
      fputs(doc.c_str(), fp);

      fputc('\n', fp);
      for (fmap_t::const_iterator i = fmap.begin(), e = fmap.end(); i != e; ++i)
         if (i->second->serializeDox(fp))
            return -1;

      // serialize group trailer
      fputs("//@}\n", fp);

      return 0;
   }

   int serializeConstantDox(FILE *fp, bool needs_prefix) const {
      if (cmap.empty())
         return 0;

      if (needs_prefix) {
         if (ns.empty())
            fputs("\n//! main Qore-language namespace\nnamespace Qore {\n", fp);
         else
            fprintf(fp, "//! %s namespace\nnamespace Qore::%s {\n", ns.c_str(), ns.c_str());
      }

      // serialize group header doc
      fputs(doc.c_str(), fp);

      fputc('\n', fp);
      for (cmap_t::const_iterator i = cmap.begin(), e = cmap.end(); i != e; ++i)
         if (i->second->serializeDox(fp))
            return -1;

      // serialize group trailer
      fputs("//@}\n", fp);

      if (needs_prefix)
         fputs("};\n", fp);

      return 0;
   }

   bool hasFunctions() const {
      return !fmap.empty();
   }

   bool hasConstants() const {
      return !cmap.empty();
   }
};

typedef std::vector<Group*> grouplist_t;
class Groups {
protected:
   grouplist_t grouplist;
   bool has_funcs, has_constants, cccp_done, cdox_done;

public:
   Groups() : has_funcs(false), has_constants(false), cccp_done(false), cdox_done(false) {
   }

   ~Groups() {
      clear();
   }

   int add(Group *g) {
      grouplist.push_back(g);
      if (!has_funcs && g->hasFunctions())
         has_funcs = true;
      else if (!has_constants && g->hasConstants())
         has_constants = true;
      return !(*g);
   }

   bool empty() const {
      return grouplist.empty();
   }

   int serializeFunctionCpp(FILE *fp, const char *rootName) {
      if (!has_funcs)
         return 0;

      for (unsigned i = 0; i < grouplist.size(); ++i)
         if (grouplist[i]->serializeFunctionCpp(fp))
            return -1;

      // now serialize function bindings
      fprintf(fp, "\nvoid init_%s_functions(QoreNamespace& qorens) {\n", rootName);

      add_init_code(fp);

      for (unsigned i = 0; i < grouplist.size(); ++i)
         if (grouplist[i]->serializeCppFunctionBindings(fp))
            return -1;

      // serialize any constants
      if (serializeCppConstantBindings(fp))
         return -1;

      fputs("\n}\n", fp);

      return 0;
   }

   int serializeCppConstantBindings(FILE *fp) {
      if (cccp_done || !has_constants)
         return 0;
      cccp_done = true;

      for (unsigned i = 0; i < grouplist.size(); ++i)
         if (grouplist[i]->serializeCppConstantBindings(fp))
            return -1;
      return 0;
   }

   int serializeFunctionDox(FILE *fp) {
      if (!has_funcs)
         return 0;

      for (unsigned i = 0; i < grouplist.size(); ++i)
         if (grouplist[i]->serializeFunctionDox(fp))
            return -1;

      if (serializeConstantDox(fp, true))
         return -1;

      return 0;
   }

   int serializeConstantDox(FILE *fp, bool needs_prefix = false) {
      if (cdox_done || !has_constants)
         return 0;
      cdox_done = true;

      for (unsigned i = 0; i < grouplist.size(); ++i)
         if (grouplist[i]->serializeConstantDox(fp, needs_prefix))
            return -1;
      return 0;
   }

   void clear() {
      for (unsigned i = 0; i < grouplist.size(); ++i)
         delete grouplist[i];
      grouplist.clear();
   }
};

static Groups groups;

class AbstractElement {
protected:

public:
   virtual ~AbstractElement() {
   }

   virtual int serializeCpp(FILE *fp) = 0;
   virtual int serializeDox(FILE *fp) = 0;
};

class TextElement : public AbstractElement {
protected:
   std::string buf;

public:
   TextElement(const std::string &n_buf) : buf(n_buf) {
      //log(LL_DEBUG, "TextElement::TextElement() str=%s", n_buf.c_str());
   }

   virtual int serializeCpp(FILE *fp) {
      fputs(buf.c_str(), fp);
      return 0;
   }

   virtual int serializeDox(FILE *fp) {
      //return serialize_dox_comment(fp, buf);
      return 0;
   }   
};

class Method : public CodeBase {
protected:
   void serializeCppConstructor(FILE *fp, const char *cname) const {
      serializeQoreConstructorPrototypeComment(fp, cname);
      fprintf(fp, "static void %s_%s(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {\n", cname, vname.c_str());
      serializeArgs(fp, cname, false);
      fputs(code.c_str(), fp);
      fputs("\n}\n\n", fp);
   }

   void serializeCppDestructor(FILE *fp, const char *cname, const char *arg) const {
      serializeQoreDestructorCopyPrototypeComment(fp, cname);
      fprintf(fp, "static void %s_destructor(QoreObject* self, %s, ExceptionSink* xsink) {\n", cname, arg);
      fputs(code.c_str(), fp);
      fputs("\n}\n\n", fp);
   }

   void serializeCppCopy(FILE *fp, const char *cname, const char *arg) const {
      serializeQoreDestructorCopyPrototypeComment(fp, cname);
      fprintf(fp, "static void %s_copy(QoreObject* self, QoreObject* old, %s, ExceptionSink* xsink) {\n", cname, arg);
      fputs(code.c_str(), fp);
      fputs("\n}\n\n", fp);
   }

   int serializeCppConstructorBinding(FILE *fp, const char *cname, const char *UC) const {
      fputc('\n', fp);
      serializeQoreConstructorPrototypeComment(fp, cname, 3);

      fprintf(fp, "   QC_%s->setConstructorExtended(%s_%s, %s, %s, %s", 
              UC, 
              cname, vname.c_str(),
              attr & QCA_PRIVATE ? "true" : "false",
              flags.c_str(),
              dom.empty() ? "QDOM_DEFAULT" : dom.c_str());

      if (serializeBindingArgs(fp))
         return -1;

      fputs(");\n", fp);
      
      return 0;
   }

   int serializeCppDestructorBinding(FILE *fp, const char *cname, const char *UC) const {
      fputc('\n', fp);
      serializeQoreDestructorCopyPrototypeComment(fp, cname, 3);
      fprintf(fp, "   QC_%s->setDestructor((q_destructor_t)%s_destructor);\n", UC, cname);
      return 0;
   }

   int serializeCppCopyBinding(FILE *fp, const char *cname, const char *UC) const {
      fputc('\n', fp);
      serializeQoreDestructorCopyPrototypeComment(fp, cname, 3);
      fprintf(fp, "   QC_%s->setCopy((q_copy_t)%s_copy);\n", UC, cname);
      return 0;
   }

   void serializeQoreDestructorCopyPrototypeComment(FILE *fp, const char *cname, unsigned indent = 0) const {
      serializeQoreAttrComment(fp, indent);
      fprintf(fp, "%s::%s() {}\n", cname, name.c_str());
   }

   void serializeQoreConstructorPrototypeComment(FILE *fp, const char *cname, unsigned indent = 0) const {
      serializeQoreAttrComment(fp, indent);
      fprintf(fp, "%s::constructor(", cname);
      serializeQoreParams(fp);
      fputs(") {}\n", fp);
   }

   void serializeQorePrototypeComment(FILE *fp, const char *cname, unsigned indent = 0) const {
      serializeQoreAttrComment(fp, indent);
      fprintf(fp, "%s %s::%s(", return_type.empty() ? "nothing" : return_type.c_str(), cname, name.c_str());
      serializeQoreParams(fp);
      fputs(") {}\n", fp);
   }

   const char *getMethodType() const {
      switch (rt) {
         case RT_INT:
            return "q_method_int64_t";
         case RT_BOOL:
            return "q_method_bool_t";
         case RT_FLOAT:
            return "q_method_double_t";
         default:
            return "q_method_t";
      }
   }

public:
   Method(const std::string &n_name, attr_t n_attr, const paramlist_t &n_params, 
          const std::string &n_docs, const std::string &n_return_type, 
          const std::string &n_flags, const std::string &n_dom, const std::string &n_code,
          unsigned vnum, unsigned n_line, bool n_doconly) : CodeBase(n_name, n_attr, n_params, n_docs, n_return_type, 
                                                                     n_flags, n_dom, n_code, vnum, n_line, n_doconly) {
   }

   virtual ~Method() {
   }

   void serializeNormalCppMethod(FILE *fp, const char *cname, const char *arg) const {
      assert(!(attr & QCA_STATIC));

      if (doconly)
         return;

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

      fprintf(fp, "static %s %s_%s(QoreObject* self, %s, const QoreListNode* args, ExceptionSink* xsink) {\n", getReturnType(), cname, vname.c_str(), arg);
      serializeArgs(fp, cname);
      fputs(code.c_str(), fp);

      if (!has_return)
         fprintf(fp, "\n   return 0;");

      fputs("\n}\n\n", fp);
   }

   int serializeNormalCppBinding(FILE *fp, const char *cname, const char *UC) const {
      if (doconly)
         return 0;

      if (name == "constructor")
         return serializeCppConstructorBinding(fp, cname, UC);

      if (name == "destructor")
         return serializeCppDestructorBinding(fp, cname, UC);

      if (name == "copy")
         return serializeCppCopyBinding(fp, cname, UC);

      fputc('\n', fp);
      serializeQorePrototypeComment(fp, cname, 3);

      // get return type
      std::string cppt;
      if (get_qore_type(return_type, cppt))
         return -1;

      fprintf(fp, "   QC_%s->addMethodExtended(\"%s\", (%s)%s_%s, %s, %s, %s, %s", UC, 
              name.c_str(),
              getMethodType(), cname, vname.c_str(),
              attr & QCA_PRIVATE ? "true" : "false",
              flags.c_str(),
              dom.empty() ? "QDOM_DEFAULT" : dom.c_str(), cppt.c_str());

      if (serializeBindingArgs(fp))
         return -1;

      fputs(");\n", fp);

      return 0;
   }

   void serializeStaticCppMethod(FILE *fp, const char *cname, const char *arg) const {
      assert(attr & QCA_STATIC);

      if (doconly)
         return;

      serializeQorePrototypeComment(fp, cname);

      fprintf(fp, "static %s static_%s_%s(const QoreListNode* args, ExceptionSink* xsink) {\n", getReturnType(), cname, vname.c_str());
      serializeArgs(fp, cname);
      fputs(code.c_str(), fp);

      if (!has_return)
         fprintf(fp, "\n   return 0;");

      fputs("\n}\n\n", fp);
   }

   int serializeStaticCppBinding(FILE *fp, const char *cname, const char *UC) const {
      assert(attr & QCA_STATIC);

      if (doconly)
         return 0;

      fputc('\n', fp);
      serializeQorePrototypeComment(fp, cname, 3);

      // get return type
      std::string cppt;
      if (get_qore_type(return_type, cppt))
         return -1;

      fprintf(fp, "   QC_%s->addStaticMethodExtended(\"%s\", (%s)static_%s_%s, %s, %s, %s, %s", UC, 
              name.c_str(),
              getFunctionType(), 
              cname, vname.c_str(),
              attr & QCA_PRIVATE ? "true" : "false",
              flags.c_str(),
              dom.empty() ? "QDOM_DEFAULT" : dom.c_str(), cppt.c_str());

      if (serializeBindingArgs(fp))
         return -1;

      fputs(");\n", fp);

      return 0;
   }

   int serializeDox(FILE *fp) {
      if (attr & QCA_PRIVATE)
         fputs("\nprivate:\n", fp);
      else
         fputs("\npublic:\n", fp);

      serialize_dox_comment(fp, docs);
      
      //fputs("   ", fp);
      if (attr & QCA_STATIC)
         fputs("static ", fp);

      serializeQoreCppType(fp, return_type);
      fprintf(fp, " %s(", name.c_str());

      for (unsigned i = 0; i < params.size(); ++i) {
         if (i)
            fputs(", ", fp);
         const Param &p = params[i];
         serializeQoreCppType(fp, p.type);
         fputc(' ' , fp);
         fputs(p.name.c_str(), fp);
      }
      fputs(");\n", fp);
      return 0;
   }
};

class ClassElement : public AbstractElement {
protected:
   typedef std::multimap<std::string, Method*> mmap_t;

   std::string name,    // class name
      doc,              // doc string
      arg,              // argument for non-static methods
      scons,            // system constructor
      ns,               // namespace name
      virt_parent;      // builtin virtual base/parent class
   paramlist_t public_members;  // public members

   strlist_t dom;       // functional domains

   mmap_t normal_mmap,  // normal method map
      static_mmap;      // static method map

   vmap_t normal_vmap,  // normal variant enumeration map
      static_vmap;      // static variant enumation map

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
      log(LL_DETAIL, "parsing Qore class '%s'\n", name.c_str());

      // process properties
      for (strmap_t::const_iterator i = props.begin(), e = props.end(); i != e; ++i) {
         //log(LL_DEBUG, "+ prop: '%s': '%s'\n", i->first.c_str(), i->second.c_str());

         // parse domain
         if (i->first == "dom") {
            get_string_list(dom, i->second);
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
            get_string_list(l, i->second);
            
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

         if (i->first == "public_members") {
            strlist_t pml;
            get_string_list(pml, i->second);

            for (unsigned i = 0; i < pml.size(); ++i) {
               size_t p = pml[i].find(' ');
               if (p == std::string::npos) {
                  error("class '%s' has member without type: '%s'\n", name.c_str(), pml[i].c_str());
                  valid = false;
                  continue;
               }
               std::string type(pml[i], 0, p);
               std::string name(pml[i], p + 1);

               public_members.push_back(Param(type, name, "", ""));
            }
            continue;
         }

         if (i->first == "system_constructor") {
            scons = i->second;
            log(LL_DEBUG, "+ system_constructor: %s\n", scons.c_str());
            continue;
         }

         if (i->first == "ns") {
            ns = i->second;
            log(LL_DEBUG, "+ namespace: %s\n", ns.c_str());
            continue;
         }

         if (i->first == "vparent") {
            virt_parent = i->second;
            log(LL_DEBUG, "+ virtual base class: %s\n", virt_parent.c_str());
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
      for (mmap_t::iterator i = normal_mmap.begin(), e = normal_mmap.end(); i != e; ++i) {
         delete i->second;
      }

      for (mmap_t::iterator i = static_mmap.begin(), e = static_mmap.end(); i != e; ++i) {
         delete i->second;
      }
   }

   operator bool() const {
      return valid;
   }

   const char *getName() const {
      return name.c_str();
   }

   int addMethod(const std::string &mname, attr_t attr, const std::string &return_type, const paramlist_t &params, const strmap_t &flags, const std::string &code, const std::string &doc, unsigned line) {
      log(LL_DETAIL, "adding method %s%s'%s'::'%s'()\n", return_type.c_str(), return_type.empty() ? "" : " ", name.c_str(), mname.c_str());

      std::string cf, dom;
      bool doconly = false;
      // parse flags
      for (strmap_t::const_iterator i = flags.begin(), e = flags.end(); i != e; ++i) {
         //log(LL_DEBUG, "+ method %s::%s() flag '%s': '%s'\n", name.c_str(), mname.c_str(), i->first.c_str(), i->second.c_str());
         if (i->first == "dom")
            dom = i->second;
         else if (i->first == "flags")
            cf = i->second;
         else if (i->first == "doconly")
            doconly = true;
         else {
            error("unknown flag '%s' = '%s' defining method %s::%s()\n", i->first.c_str(), i->second.c_str(), name.c_str(), mname.c_str());
            return -1;
         }
      }

      //log(LL_INFO, "+ method %s::%s() attr: 0x%x (static: %d)\n", name.c_str(), mname.c_str(), attr, attr & QCA_STATIC);

      mmap_t &mmap = (attr & QCA_STATIC) ? static_mmap : normal_mmap;
      vmap_t &vmap = (attr & QCA_STATIC) ? static_vmap : normal_vmap;

      unsigned vnum;
      vmap_t::iterator i = vmap.find(mname);
      if (i == vmap.end()) {
         vnum = 0;
         vmap[mname] = 0;
      }
      else
         vnum = ++i->second;

      Method *m = new Method(mname, attr, params, doc, return_type, cf, dom, code, vnum, line, doconly);
      mmap.insert(mmap_t::value_type(mname, m));
      return 0;
   }

   virtual int serializeCpp(FILE *fp) {
      fprintf(fp, "/* Qore class %s::%s */\n\n", ns.empty() ? "Qore" : ns.c_str(), name.c_str());

      std::string UC;
      for (unsigned i = 0; i < name.size(); ++i)
         UC += toupper(name[i]);

      fprintf(fp, "qore_classid_t CID_%s;\nQoreClass *QC_%s;\n\n", UC.c_str(), UC.c_str());

      for (mmap_t::const_iterator i = normal_mmap.begin(), e = normal_mmap.end(); i != e; ++i) {
         i->second->serializeNormalCppMethod(fp, name.c_str(), arg.c_str());
      }

      for (mmap_t::const_iterator i = static_mmap.begin(), e = static_mmap.end(); i != e; ++i) {
         i->second->serializeStaticCppMethod(fp, name.c_str(), arg.c_str());
      }

      fprintf(fp, "QoreClass* init%sClass(QoreNamespace &qorens) {\n   QC_%s = new QoreClass(\"%s\"", name.c_str(), UC.c_str(), name.c_str());
      for (strlist_t::const_iterator i = dom.begin(), e = dom.end(); i != e; ++i) {
         fprintf(fp, ", QDOM_%s", (*i).c_str());
      }
      fprintf(fp, ");\n   CID_%s = QC_%s->getID();\n", UC.c_str(), UC.c_str());

      if (!virt_parent.empty()) {
         std::string vp;
         get_type_name(vp, virt_parent);
         toupper(vp);
         fprintf(fp, "\n   // set parent class\n   assert(QC_%s);\n   QC_%s->addBuiltinVirtualBaseClass(QC_%s);\n", vp.c_str(), UC.c_str(), vp.c_str());
      }


      add_init_code(fp);

      // set system constructor if any
      if (!scons.empty())
         fprintf(fp, "\n   // set system constructor\n   QC_%s->setSystemConstructor(%s);\n", UC.c_str(), scons.c_str());
      
      // output public members if any
      if (!public_members.empty()) {
         fputs("\n   // public members\n", fp);
         for (paramlist_t::iterator i = public_members.begin(), e = public_members.end(); i != e; ++i) {
            std::string mt;
            get_qore_type((*i).type, mt);
            fprintf(fp, "   QC_%s->addPublicMember(\"%s\", %s);\n", UC.c_str(), (*i).name.c_str(), mt.c_str());
         }
      }

      if (upm)
         fprintf(fp, "\n   QC_%s->unsetPublicMemberFlag();\n", UC.c_str());

      for (mmap_t::const_iterator i = normal_mmap.begin(), e = normal_mmap.end(); i != e; ++i) {
         if (i->second->serializeNormalCppBinding(fp, name.c_str(), UC.c_str())) {
            error("error processing bindings for %s::%s()\n", name.c_str(), i->second->getName());
            valid = false;
            return -1;
         }
      }

      for (mmap_t::const_iterator i = static_mmap.begin(), e = static_mmap.end(); i != e; ++i) {
         if (i->second->serializeStaticCppBinding(fp, name.c_str(), UC.c_str())) {
            error("error processing bindings for static method %s::%s()\n", name.c_str(), i->second->getName());
            valid = false;
            return -1;
         }
      }

      if (groups.serializeCppConstantBindings(fp))
         return -1;

      fprintf(fp, "\n   return QC_%s;\n}\n", UC.c_str());
      return 0;
   }

   virtual int serializeDox(FILE *fp) {
      if (ns.empty())
         fputs("//! main Qore-language namespace\nnamespace Qore {\n", fp);
      else
         fprintf(fp, "//! %s namespace\nnamespace Qore::%s {\n", ns.c_str(), ns.c_str());

      serialize_dox_comment(fp, doc);
      
      fprintf(fp, "class %s", name.c_str());
      if (!virt_parent.empty())
         fprintf(fp, " : public %s", virt_parent.c_str());

      fputs(" {\n", fp);
      
      for (mmap_t::const_iterator i = normal_mmap.begin(), e = normal_mmap.end(); i != e; ++i)
         i->second->serializeDox(fp);

      for (mmap_t::const_iterator i = static_mmap.begin(), e = static_mmap.end(); i != e; ++i)
         i->second->serializeDox(fp);

      fputs("};\n", fp);

      groups.serializeConstantDox(fp);

      fputs("};\n", fp);
      return 0;
   }   
};

typedef std::map<std::string, ClassElement *> cemap_t;

typedef std::vector<AbstractElement *> source_t;

class Code {
protected:
   const char *fileName;
   std::string cppFileName, 
      doxFileName,
      rootName;
   // argument to fopen()
   const char *cpp_open_flag,
      *dox_open_flag;
   unsigned lineNumber;
   source_t source;
   cemap_t cemap;
   bool valid, 
      has_class; // has at least 1 class element

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
         read_line(lineNumber, str, fp);

         log(LL_DEBUG, "%d: %s", lineNumber, str.c_str());

         if (str.empty())
            break;

         if (!str.compare(0, 13, "/** @defgroup")) {
            checkBuf(buf);

            if (groups.add(new Group(str, fp, fileName, lineNumber))) {
               rc = -1;
               break;
            }            
            continue;
         }

         if (!str.compare(0, 3, "//!")) {
            if (!str.compare(0, 14, "//! @qppinit: ")) {
               str.erase(0, 14);
               log(LL_DEBUG, "got qppinit: %s", str.c_str());
               
               initcode += str;
               continue;
            }

            if (!str.compare(0, 13, "//! @qppval: ")) {
               str.erase(0, 13);

               size_t i = str.find('=');
               if (i == std::string::npos) {
                  error("%s:%d: qppval has no '=' sign for key assignment: %s", fileName, lineNumber, str.c_str());
                  rc = -1;
                  break;
               }
               std::string key(str, 0, i);
               trim(key);
               std::string val(str, i + 1);
               trim(val);

               log(LL_DEBUG, "got qppval: '%s' = '%s'\n", key.c_str(), val.c_str());

               if (qppval.find(key) != qppval.end()) {
                  error("%s:%d: qppval '%s' given twice\n", fileName, lineNumber, key.c_str());
                  rc = -1;
                  break;
               }

               qppval[key] = val;
               continue;
            }

            if (get_dox_comment(fileName, lineNumber, str, fp)) {
               rc = -1;
               break;
            }

            std::string sc;
            if (read_line(lineNumber, sc, fp)) {
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
               if (parse_properties(fileName, lineNumber, propstr, props)) {
                  rc = -1;
                  break;
               }

               ClassElement *ce = new ClassElement(cn, props, str);
               cemap[cn] = ce;
               checkBuf(buf);
               source.push_back(ce);
               // mark code as invalid if class element is invalid
               if (!*ce)
                  valid = false;
               has_class = true;
               continue;
            }

            if (strstr(sc.c_str(), "::") && strchr(sc.c_str(), '{')) {
               if (parseMethod(sc, fp, str)) {
                  valid = false;
                  break;
               }
               continue;
            }

            continue;
         }

         buf += str;
      }

      checkBuf(buf);

      fclose(fp);

      return rc;
   }

   int parseMethod(std::string &sc, FILE *fp, std::string &doc) {
      unsigned sl = lineNumber;
      if (read_until_close(fileName, lineNumber, sc, fp)) {
         error("%s:%d: premature EOF reading method definition starting on line %d\n", fileName, lineNumber, sl);
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

      cemap_t::iterator ci = cemap.find(cn);
      if (ci == cemap.end()) {
         error("%s:%d: reference to undefined class '%s'\n", fileName, lineNumber, cn.c_str());
         return -1;
      }

      // method attributes
      attr_t attr = QCA_NONE;
      // return type
      std::string return_type;

      if (i && parse_attributes(fileName, lineNumber, attr, return_type, sc, i))
         return -1;

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

      p = sc.find('(', i);
      if (p == std::string::npos) {
         error("%s:%d: premature EOL reading parameters for %s::%s()\n", fileName, lineNumber, cn.c_str(), mn.c_str());
         return -1;
      }
      ++p;
      
      strmap_t flags;
      paramlist_t params;

      std::string dn(cn);
      dn += "::";
      dn += mn;

      if (parse_params_and_flags(fileName, lineNumber, flags, params, attr, sc, p, dn))
         return -1;

      return ci->second->addMethod(mn, attr, return_type, params, flags, sc, doc, sl);
   }

public:
   Code(const char *fn, const std::string &ofn, const std::string &dfn, 
        bool cpp_append = false, bool dox_append = false) : fileName(fn), 
                                                            cpp_open_flag(cpp_append ? "a" : "w"), 
                                                            dox_open_flag(dox_append ? "a" : "w"), 
                                                            lineNumber(0),
                                                            valid(true), has_class(false) {
      std::string base;
      std::string dir;

      std::string bnc(fn);
      base = basename((char*)bnc.c_str());
      bnc = fn;
      dir = dirname((char*)bnc.c_str());
      
      if (base.size() > 4 && !strcmp(base.c_str() + base.size() - 4, ".qpp"))
         base.erase(base.size() - 4);
      else {
         warning("'%s' does not end in extension '.qpp'\n", fn);
         size_t d = base.rfind('.');
         if (d != std::string::npos)
            base.erase(d);
      }

      cppFileName = !ofn.empty() ? ofn : dir + "/" + base + ".cpp";
      doxFileName = !dfn.empty() ? dfn : dir + "/" + base + ".dox.h";
      if (parse())
         valid = false;

      size_t i = base.find('_');
      if ((i == std::string::npos) || (i == (base.size() - 1)))
         rootName = base;
      else
         rootName.assign(base, i + 1, -1);
   }

   ~Code() {
      for (source_t::iterator i = source.begin(), e = source.end(); i != e; ++i)
	 delete *i;
   }

   int serializeCpp() {
      FILE *fp = fopen(cppFileName.c_str(), cpp_open_flag);
      if (!fp) {
	 error("%s: %s\n", cppFileName.c_str(), strerror(errno));
         return -1;
      }
      log(LL_INFO, "creating cpp file %s -> %s\n", fileName, cppFileName.c_str());

      for (source_t::const_iterator i = source.begin(), e = source.end(); i != e; ++i) {
	 if ((*i)->serializeCpp(fp)) {
            valid = false;
            return -1;
         }
      }
      
      return groups.serializeFunctionCpp(fp, rootName.c_str());
   }

   int serializeDox() {
      FILE *fp = fopen(doxFileName.c_str(), dox_open_flag);
      if (!fp) {
	 error("%s: %s\n", doxFileName.c_str(), strerror(errno));
         return -1;
      }
      log(LL_INFO, "creating dox file %s -> %s\n", fileName, doxFileName.c_str());

      for (source_t::const_iterator i = source.begin(), e = source.end(); i != e; ++i) {
         if ((*i)->serializeDox(fp)) {
            valid = false;
            return -1;
         }
      }

      groups.serializeFunctionDox(fp);

      return 0;
   }

   operator bool() const {
      return valid;
   }
};

int do_table_file() {
   FILE *ifp = fopen(opts.table_fn.c_str(), "r");
   if (!ifp) {
      error("%s: %s\n", opts.table_fn.c_str(), strerror(errno));
      return -1;
   }

   FILE *ofp = fopen(opts.output_fn.c_str(), "w");
   if (!ofp) {
      error("%s: %s\n", opts.output_fn.c_str(), strerror(errno));
      return -1;
   }

   std::string buf;
   while (true) {
      int c = fgetc(ifp);
      if (c == EOF)
         break;
      
      buf += c;
   }
   fclose(ifp);

   serialize_dox_comment(ofp, buf);
   fclose(ofp);

   return 0;
}

void init() {
   // initialize attribute map
   amap["public"] = QCA_PUBLIC;
   amap["private"] = QCA_PRIVATE;
   amap["static"] = QCA_STATIC;
   amap["synchronized"] = QCA_SYNCHRONIZED;

   // initialize qore type to c++ typeinfo map
   tmap["int"] = "bigIntTypeInfo";
   tmap["softint"] = "softBigIntTypeInfo";
   tmap["*int"] = "bigIntOrNothingTypeInfo";
   tmap["*softint"] = "softBigIntOrNothingTypeInfo";

   tmap["float"] = "floatTypeInfo";
   tmap["softfloat"] = "softFloatTypeInfo";
   tmap["*float"] = "floatOrNothingTypeInfo";
   tmap["*softfloat"] = "softFloatOrNothingTypeInfo";

   tmap["bool"] = "boolTypeInfo";
   tmap["softbool"] = "softBoolTypeInfo";
   tmap["*bool"] = "boolOrNothingTypeInfo";
   tmap["*softbool"] = "softBoolOrNothingTypeInfo";

   tmap["string"] = "stringTypeInfo";
   tmap["softstring"] = "softStringTypeInfo";
   tmap["*string"] = "stringOrNothingTypeInfo";
   tmap["*softstring"] = "softStringOrNothingTypeInfo";

   tmap["list"] = "listTypeInfo";
   tmap["softlist"] = "softListTypeInfo";
   tmap["*list"] = "listOrNothingTypeInfo";
   tmap["*softlist"] = "softListOrNothingTypeInfo";

   tmap["date"] = "dateTypeInfo";
   tmap["softdate"] = "softDateTypeInfo";
   tmap["*date"] = "dateOrNothingTypeInfo";
   tmap["*softdate"] = "softDateOrNothingTypeInfo";

   tmap["binary"] = "binaryTypeInfo";
   tmap["*binary"] = "binaryOrNothingTypeInfo";

   tmap["hash"] = "hashTypeInfo";
   tmap["*hash"] = "hashOrNothingTypeInfo";

   tmap["null"] = "nullTypeInfo";
   tmap["*null"] = "nullOrNothingTypeInfo";

   tmap["timeout"] = "timeoutTypeInfo";
   tmap["*timeout"] = "timeoutOrNothingTypeInfo";

   tmap["object"] = "objectTypeInfo";
   tmap["*object"] = "objectOrNothingTypeInfo";

   tmap["code"] = "codeTypeInfo";
   tmap["*code"] = "codeOrNothingTypeInfo";

   tmap["any"] = "anyTypeInfo";
   tmap["nothing"] = "nothingTypeInfo";

   tmap["reference"] = "referenceTypeInfo";
   tmap["*reference"] = "anyTypeInfo";

   tmap["data"] = "dataTypeInfo";
   tmap["*data"] = "dataOrNothingTypeInfo";

   // initialize qore value to c++ value map
   valmap["0"] = "zero()";
   valmap["0.0"] = "zero_float()";
   valmap["binary()"] = "new BinaryNode";
   valmap["QCS_DEFAULT->getCode()"] = "QCS_DEFAULT->getCode()";
   valmap["True"] = "&True";
   valmap["False"] = "&False";
}

void usage() {
   printf(usage_str, pn.c_str());
   exit(1);
}

void process_command_line(int &argc, char **&argv) {
   pn = basename(argv[0]);

   int ch;
   while ((ch = getopt_long(argc, argv, "d:ho:t:v:", pgm_opts, NULL)) != -1) {
      //log(LL_INFO, "ch=%c optarg=%p (%s)\n", ch, optarg, optarg ? optarg : "(null)");

      switch (ch) {
         case 'h':
            usage();

         case 'd':
            opts.dox_fn = optarg;
            break;

         case 'o':
            opts.output_fn = optarg;
            break;

         case 't':
            opts.table_fn = optarg;
            break;

         case 'v':
            if (optarg)
               opts.verbose = strtoll(optarg, NULL, 10);
            else
               ++opts.verbose;
      }
   }

   argc -= optind;
   argv += optind;

   if (!argc) {
      if (opts.table_fn.empty()) {
         error("at least one input file must be given\n");
         usage();
      }
      else if (opts.output_fn.empty()) {
         error("output file name must be given with -t\n");
         usage();
      }
   }
   else if (!opts.table_fn.empty()) {
      error("table file name must not be given along with an input file list\n");
      usage();
   }      
}

int main(int argc, char *argv[]) {
   process_command_line(argc, argv);

   // initialize static reference data
   init();

   for (int i = 0; i < argc; ++i) {
      Code code(argv[i], opts.output_fn, opts.dox_fn, i && !opts.output_fn.empty(), i && !opts.dox_fn.empty());
      if (!code) {
         error("please correct the errors above and try again\n");
         exit(1);
      }
      // create cpp output file
      if (code.serializeCpp())
         return -1;

      // create dox output file
      code.serializeDox();
   }

   if (!opts.table_fn.empty())
      do_table_file();

   return 0;
}
