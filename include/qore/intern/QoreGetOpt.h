/*
  QoreGetOpt.h

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

#ifndef _QORE_QOREGETOPT_H

#define _QORE_QOREGETOPT_H

#include <qore/common.h>
#include <qore/safe_dslist>

#include <map>

#define QGO_ERR_DUP_SHORT_OPT -1
#define QGO_ERR_DUP_LONG_OPT  -2
#define QGO_ERR_DUP_NAME      -3
#define QGO_ERR_NO_NAME       -4
#define QGO_ERR_NO_OPTION     -5

#define QGO_OPT_NONE      0
#define QGO_OPT_ADDITIVE  1
#define QGO_OPT_LIST      2
#define QGO_OPT_MANDATORY 4

#define QGO_OPT_LIST_OR_ADD (QGO_OPT_ADDITIVE|QGO_OPT_LIST)

class QoreGetOptNode {
   public:
      char *name;
      char short_opt;
      char *long_opt;
      qore_type_t argtype;
      int option;

      DLLLOCAL QoreGetOptNode(const char *n, char so, char *lo, qore_type_t at = -1, int o = QGO_OPT_NONE);
      DLLLOCAL ~QoreGetOptNode();
};

typedef std::map<char *, QoreGetOptNode *, class ltstr> getopt_long_map_t;
typedef std::map<char, QoreGetOptNode *, class ltchar> getopt_short_map_t;
typedef safe_dslist<QoreGetOptNode *> getopt_node_list_t;

class QoreGetOpt {
   private:
      getopt_long_map_t long_map;
      getopt_short_map_t short_map;
      getopt_node_list_t node_list;
      
      DLLLOCAL static class AbstractQoreNode *parseDate(const char *val);
      DLLLOCAL void processLongArg(const char *arg, class QoreListNode *l, class QoreHashNode *h, unsigned &i, bool modify);
      DLLLOCAL int processShortArg(const char *arg, class QoreListNode *l, class QoreHashNode *h, unsigned &i, int &j, bool modify);
      DLLLOCAL QoreGetOptNode *find(const char *opt) const;
      DLLLOCAL class QoreGetOptNode *find(char opt) const;
      DLLLOCAL void doOption(class QoreGetOptNode *n, class QoreHashNode *h, const char *val);
      DLLLOCAL char *getNextArgument(class QoreListNode *l, class QoreHashNode *h, unsigned &i, const char *lopt, char sopt);

   public:
      DLLLOCAL QoreGetOpt();
      DLLLOCAL ~QoreGetOpt();
      // returns 0 for OK
      DLLLOCAL int add(const char *name, char short_opt, char *long_opt, qore_type_t argtype = -1, int option = QGO_OPT_NONE);
      DLLLOCAL class QoreHashNode *parse(class QoreListNode *l, bool ml, class ExceptionSink *xsink);
};

#endif // _QORE_QOREGETOPT_H
