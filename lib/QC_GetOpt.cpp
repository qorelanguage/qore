/*
  QC_GetOpt.cpp

  Qore Programming Language
  
  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QC_GetOpt.h>

#include <string.h>

qore_classid_t CID_GETOPT;

static inline int process_type(const char *key, int &attributes, char *opt, qore_type_t &at, ExceptionSink *xsink) {
   assert(at == -1);
   const char *type_name = 0;
   // get type
   switch (*opt) {
      case 's':
	 at = NT_STRING;
	 type_name = QoreStringNode::getStaticTypeName();
	 break;
      case 'i':
	 at = NT_INT;
	 type_name = QoreBigIntNode::getStaticTypeName();
	 break;
      case 'f':
	 at = NT_FLOAT;
	 type_name = QoreFloatNode::getStaticTypeName();
	 break;
      case 'b':
	 at = NT_BOOLEAN;
	 type_name = QoreBoolNode::getStaticTypeName();
	 break;
      case 'd':
	 at = NT_DATE;
	 type_name = DateTimeNode::getStaticTypeName();
	 break;
      case 'h':
	 at = NT_HASH;
	 type_name = QoreHashNode::getStaticTypeName();
	 break;
      case '@':
	 at = NT_STRING;
	 attributes |= QGO_OPT_LIST;
	 type_name = QoreStringNode::getStaticTypeName();
	 break;
      case '+':
	 at = NT_INT;
	 attributes |= QGO_OPT_ADDITIVE;
	 type_name = QoreBigIntNode::getStaticTypeName();
	 break;
   }
   if (at == -1) {
      xsink->raiseException("GETOPT-OPTION-ERROR", "type '%c' for key '%s' is unknown", *opt, key);
      return -1;
   }
   if (!opt[1])
      return 0;

   if (opt[2]) {
      xsink->raiseException("GETOPT-OPTION-ERROR", "invalid attributes in option '%s'", key);
      return -1;
   }

   // process modifiers
   if (opt[1] == '@') {
      if (attributes & QGO_OPT_LIST) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "list attribute doubled in option key '%s'", key);
	 return -1;
      }
      if (attributes & QGO_OPT_ADDITIVE) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' cannot have both additive and list attributes turned on", key);
	 return -1;
      }
      attributes |= QGO_OPT_LIST;
      return 0;
   }
   if (opt[1] == '+') {
      if (attributes & QGO_OPT_ADDITIVE) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "additive attribute doubled in option key '%s'", key);
	 return -1;
      }
      if (attributes & QGO_OPT_LIST) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' cannot have both additive and list attributes turned on", key);
	 return -1;
      }
      if (at != NT_INT && at != NT_FLOAT) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "additive attributes for type '%s' are not supported (option '%s')", type_name, key);
	 return -1;
      }
      attributes |= QGO_OPT_ADDITIVE;
      return 0;
   }

   xsink->raiseException("GETOPT-OPTION-ERROR", "unknown modifier '%c' in option '%s'", opt[1], key);
   return -1;
}

static void GETOPT_constructor(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreHashNode *p0 = test_hash_param(args, 0);
   if (!p0){
      xsink->raiseException("GETOPT-PARAMETER-ERROR", "expecting hash as first argument to GetOpt::constructor()");
      return;
   }

   GetOpt *g = new GetOpt();

   ConstHashIterator hi(p0);
   QoreString vstr;
   while (hi.next()) {
      const char *k = hi.getKey();
      if (!strcmp(k, "_ERRORS_")) {
	 xsink->raiseException("GETOPT-PARAMETER-ERROR", "option key '%s' is reserved for errors in the output hash", k);
	 break;
      }

      const AbstractQoreNode *v = hi.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(v);
      if (!str) {
	 xsink->raiseException("GETOPT-PARAMETER-ERROR", "value of option key '%s' is not a string (%s)", k, v ? v->getTypeName() : "NOTHING");
	 break;
      }
      
      qore_type_t at = -1;
      char *long_opt = 0, short_opt = '\0';
      int attributes = QGO_OPT_NONE;

      // reset buffer
      vstr.clear();
      vstr.concat(str->getBuffer());
      char *val = (char *)vstr.getBuffer();

      // get data type, if any
      char *tok = strchrs(val, "=:");
      if (tok) {
	 if (tok[1] && process_type(k, attributes, tok + 1, at, xsink))
	    break;
	 if ((*tok) == '=')
	    attributes |= QGO_OPT_MANDATORY;
	 else if (attributes & QGO_OPT_LIST) {
	    xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' takes a list and therefore must have mandatory arguments", k);
	    break;
	 }
	 
	 (*tok) = '\0';
      }
      // get option names
      if (!val[0]) {
	 //printd(5, "making exception key='%s' tok=%08p val=%08p val='%s'\n", k, tok, val, val);
	 xsink->raiseException("GETOPT-PARAMETER-ERROR", "value of option key '%s' has no option specifiers", k);
	 break;
      }
      tok = strchr(val, ',');
      if (tok) {
	 if (tok == (val + 1)) {
	    short_opt = val[0];
	    long_opt = val + 2;
	 }
	 else if (tok - val == (signed)(strlen(val) - 2)) {
	    (*tok) = 0;
	    short_opt = tok[1];
	    long_opt = val;
	 }
	 else { // if the comma is not in the second or second-to-last position, then it's an error
	    xsink->raiseException("GETOPT-OPTION-ERROR", "user options can only be specified with one short option and one long option, however two long options were given for key '%s' (%s)", k, val);
	    break;
	 }
      }
      else if (val[1])
	 long_opt = val;
      else
	 short_opt = val[0];
      int rc = g->add(k, short_opt, long_opt, at, attributes);
      if (rc == QGO_ERR_DUP_SHORT_OPT) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "short option '%c' was duplicated in key '%s'", short_opt, k);
	 break;
      }
      if (rc == QGO_ERR_DUP_LONG_OPT) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "long option '%s' was duplicated in key '%s'", long_opt, k);
	 break;
      }
      if (rc == QGO_ERR_DUP_NAME) {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' was duplicated", k);
	 break;
      }
   }
   if (!xsink->isException())
      self->setPrivate(CID_GETOPT, g);
   else
      g->deref();
}

static void GETOPT_copy(QoreObject *self, QoreObject *old, GetOpt *g, ExceptionSink *xsink) {
   xsink->raiseException("GETOPT-COPY-ERROR", "copying GetOpt objects is not supported");
}

static QoreHashNode *GETOPT_parse_ref(QoreObject *self, GetOpt *g, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(r, const ReferenceNode, args, 0);

   AutoVLock vl(xsink);
   QoreTypeSafeReferenceHelper ref(r, vl, xsink);
   if (!ref)
      return 0;

   if (ref.getType() != NT_LIST)
      return new QoreHashNode;

   QoreListNode *l = reinterpret_cast<QoreListNode *>(ref.getUnique(xsink));
   if (*xsink)
      return 0;

   return g->parse(l, true, xsink);
}

static QoreHashNode *GETOPT_parse_list(QoreObject *self, GetOpt *g, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(l, const QoreListNode, args, 0);
   
   return g->parse(const_cast<QoreListNode *>(l), false, xsink);
}

static AbstractQoreNode *GETOPT_parse2_intern(ReferenceHolder<QoreHashNode> &rv, ExceptionSink *xsink) {
   assert(rv);

   // check for _ERRORS_ key
   const QoreListNode *l = reinterpret_cast<const QoreListNode *>(rv->getKeyValue("_ERRORS_"));
   if (!l)
      return rv.release();

   const QoreStringNode *err = reinterpret_cast<const QoreStringNode *>(l->retrieve_entry(0));
   xsink->raiseException("GETOPT-ERROR", err->stringRefSelf());
   return 0;
}

static AbstractQoreNode *GETOPT_parse2_ref(QoreObject *self, GetOpt *g, const QoreListNode *args, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> rv(GETOPT_parse_ref(self, g, args, xsink), xsink);
   if (*xsink)
      return 0;

   return GETOPT_parse2_intern(rv, xsink);
}

static AbstractQoreNode *GETOPT_parse2_list(QoreObject *self, GetOpt *g, const QoreListNode *args, ExceptionSink *xsink) {
   ReferenceHolder<QoreHashNode> rv(GETOPT_parse_ref(self, g, args, xsink), xsink);
   if (*xsink)
      return 0;

   return GETOPT_parse2_intern(rv, xsink);
}

QoreClass *initGetOptClass() {
   QORE_TRACE("initGetOptClass()");

   QoreClass *QC_GETOPT = new QoreClass("GetOpt");
   CID_GETOPT = QC_GETOPT->getID();
   QC_GETOPT->setConstructor(GETOPT_constructor);
   QC_GETOPT->setCopy((q_copy_t)GETOPT_copy);

   // default for GetOpt::parse() with incorrect arguments is to do nothing
   QC_GETOPT->addMethodExtended("parse",         (q_method_t)class_noop);
   QC_GETOPT->addMethodExtended("parse",         (q_method_t)GETOPT_parse_ref, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, referenceTypeInfo, QORE_PARAM_NO_ARG);
   QC_GETOPT->addMethodExtended("parse",         (q_method_t)GETOPT_parse_list, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   // default for GetOpt::parse2() with incorrect arguments is to do nothing
   QC_GETOPT->addMethodExtended("parse2",        (q_method_t)class_noop);
   QC_GETOPT->addMethodExtended("parse2",        (q_method_t)GETOPT_parse2_ref, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, referenceTypeInfo, QORE_PARAM_NO_ARG);
   QC_GETOPT->addMethodExtended("parse2",        (q_method_t)GETOPT_parse2_list, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, listTypeInfo, QORE_PARAM_NO_ARG);

   return QC_GETOPT;
}
