/*
  QoreNumberNode.cpp
  
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

#include <qore/Qore.h>

#define QORE_DEFAULT_PREC 128
#define QORE_MAX_PREC 8192
// round to nearest (roundTiesToEven in IEEE 754-2008)
#define QORE_MPFR_RND MPFR_RNDN
// MPFR_RNDA

struct qore_number_private_intern {
   mpfr_t num;

   DLLLOCAL qore_number_private_intern() {
      mpfr_init2(num, QORE_DEFAULT_PREC);
   }

   DLLLOCAL qore_number_private_intern(mpfr_prec_t prec) {
      if (prec > QORE_MAX_PREC)
         prec = QORE_MAX_PREC;
      mpfr_init2(num, prec);
   }

   DLLLOCAL ~qore_number_private_intern() {
      mpfr_clear(num);
   }
};

struct qore_number_private : public qore_number_private_intern {
   DLLLOCAL explicit qore_number_private(mpfr_prec_t prec) : qore_number_private_intern(prec) {
   }

   DLLLOCAL qore_number_private(double f) {
      mpfr_set_d(num, f, QORE_MPFR_RND);
   }

   DLLLOCAL qore_number_private(int64 i) {
      mpfr_set_sj(num, i, QORE_MPFR_RND);
   }

   DLLLOCAL qore_number_private(const char* str) : qore_number_private_intern(QORE_MAX(QORE_DEFAULT_PREC, strlen(str)*10)) {
      mpfr_set_str(num, str, 10, QORE_MPFR_RND);
   }

   DLLLOCAL qore_number_private(const qore_number_private& old) : qore_number_private_intern(mpfr_get_prec(old.num)) {
      mpfr_set(num, old.num, QORE_MPFR_RND);
   }

   DLLLOCAL double getAsFloat() const {
      return mpfr_get_d(num, QORE_MPFR_RND);
   }

   DLLLOCAL int64 getAsBigInt() const {
      return mpfr_get_sj(num, QORE_MPFR_RND);
   }

   DLLLOCAL bool getAsBool() const {
      return !zero();
   }

   DLLLOCAL bool zero() const {
      return (bool)mpfr_zero_p(num);
   }

   DLLLOCAL bool nan() const {
      return (bool)mpfr_nan_p(num);
   }

   DLLLOCAL bool inf() const {
      return (bool)mpfr_inf_p(num);
   }

   DLLLOCAL bool number() const {
      return (bool)mpfr_number_p(num);
   }

   // regular and not zero
   DLLLOCAL bool regular() const {
      return (bool)mpfr_regular_p(num);
   }

   DLLLOCAL void getAsString(QoreString& str) const {
      // first check if it's zero
      if (zero()) {
         str.concat("0n");
         return;
      }
      mpfr_exp_t exp;
      char* buf = mpfr_get_str(0, &exp, 10, 0, num, QORE_MPFR_RND);
      if (!buf) {
         str.concat("<number error>");
         return;
      }

      // if it's a regular number, then format accordingly
      if (number()) {
         qore_size_t len = str.size();
         //printd(0, "QoreNumberNode::getAsString() this: %p '%s' exp "QLLD" len: "QLLD"\n", this, buf, exp, len);

         str.concat(buf);
         // trim the trailing zeros off the end
         str.trim_trailing('0');
         if (exp <= 0) {
            exp = -exp;
            str.insert("0.", len);
            if (exp)
               str.insertch('0', len + 2, exp);
         }
         else {
            // get remaining length of string (how many characters were added)
            qore_size_t rlen = str.size() - len;

            //printd(0, "QoreNumberNode::getAsString() this: %p str: '%s' rlen: "QLLD"\n", this, str.getBuffer(), rlen);

            // assert that we have added at least 1 character
            assert(rlen > 0);
            if (exp > rlen)
               str.insertch('0', str.size(), exp - rlen);
            else if (exp < rlen)
               str.insertch('.', len + exp, 1);
         }
         str.concat('n');
      }

      mpfr_free_str(buf);
   }

   DLLLOCAL int compare(const qore_number_private& right) const {
      return mpfr_cmp(num, right.num);
   }

   DLLLOCAL int compare(double right) const {
      return mpfr_cmp_d(num, right);
   }

   DLLLOCAL int compare(int64 right) const {
      MPFR_DECL_INIT(r, QORE_DEFAULT_PREC);
      mpfr_set_sj(r, right, QORE_MPFR_RND);
      return mpfr_cmp(num, r);
   }

   DLLLOCAL qore_number_private* doPlus(const qore_number_private& r) const {
      mpfr_prec_t prec = QORE_MAX(mpfr_get_prec(num), mpfr_get_prec(r.num));
      qore_number_private* p = new qore_number_private(prec);
      mpfr_add(p->num, num, r.num, QORE_MPFR_RND);
      return p;
   }
};

QoreNumberNode::QoreNumberNode(struct qore_number_private* p) : SimpleValueQoreNode(NT_NUMBER), priv(p) {
}

QoreNumberNode::QoreNumberNode(const AbstractQoreNode* n) : SimpleValueQoreNode(NT_NUMBER), priv(0) {
   qore_type_t t = get_node_type(n);
   if (t == NT_NUMBER) {
      priv = new qore_number_private(*reinterpret_cast<const QoreNumberNode*>(n)->priv);
      return;
   }

   if (t == NT_FLOAT) {
      priv = new qore_number_private(reinterpret_cast<const QoreFloatNode*>(n)->f);
      return;
   }

   if (t == NT_STRING) {
      priv = new qore_number_private(reinterpret_cast<const QoreStringNode*>(n)->getBuffer());
      return;
   }

   if (t == NT_INT || (t > QORE_NUM_TYPES && dynamic_cast<const QoreBigIntNode*>(n))) {
      priv = new qore_number_private(reinterpret_cast<const QoreBigIntNode*>(n)->val);
      return;
   }

   if (t != NT_BOOLEAN
       && t != NT_DATE
       && t != NT_NULL) {
      priv = new qore_number_private(0ll);
      return;
   }

   priv = new qore_number_private(n->getAsFloat());
}

QoreNumberNode::QoreNumberNode(double f) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(f)) {
}

QoreNumberNode::QoreNumberNode(int64 i) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(i)) {
}

QoreNumberNode::QoreNumberNode(const char* str) : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(str)) {
}

QoreNumberNode::QoreNumberNode() : SimpleValueQoreNode(NT_NUMBER), priv(new qore_number_private(0ll)) {
}

QoreNumberNode::QoreNumberNode(const QoreNumberNode& old) : SimpleValueQoreNode(old), priv(new qore_number_private(*old.priv)) {
}

QoreNumberNode::~QoreNumberNode() {
   delete priv;
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString* QoreNumberNode::getStringRepresentation(bool& del) const {
   del = true;
   QoreString* str = new QoreString;
   priv->getAsString(*str);
   return str;
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void QoreNumberNode::getStringRepresentation(QoreString& str) const {
   priv->getAsString(str);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreNumberNode::getDateTimeRepresentation(bool& del) const {
   del = true;
   double f = priv->getAsFloat();
   return DateTime::makeAbsoluteLocal(currentTZ(), (int64)f, (int)((f - (float)((int)f)) * 1000000));
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void QoreNumberNode::getDateTimeRepresentation(DateTime& dt) const {
   double f = priv->getAsFloat();
   dt.setLocalDate(currentTZ(), (int64)f, (int)((f - (float)((int)f)) * 1000000));
}

bool QoreNumberNode::getAsBoolImpl() const {
   return priv->getAsBool();
}

int QoreNumberNode::getAsIntImpl() const {
   return priv->getAsBigInt();
}

int64 QoreNumberNode::getAsBigIntImpl() const {
   return priv->getAsBigInt();
}

double QoreNumberNode::getAsFloatImpl() const {
   return priv->getAsFloat();
}

int QoreNumberNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   getStringRepresentation(str);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *QoreNumberNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   return getStringRepresentation(del);
}

AbstractQoreNode* QoreNumberNode::realCopy() const {
   return new QoreNumberNode(*this);
}

// the type passed must always be equal to the current type
bool QoreNumberNode::is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   if (v->getType() == NT_NUMBER)
      return !priv->compare(*reinterpret_cast<const QoreNumberNode*>(v)->priv);
   if (v->getType() == NT_INT || dynamic_cast<const QoreBigIntNode*>(v))
      return !priv->compare(reinterpret_cast<const QoreBigIntNode*>(v)->val);

   return !priv->compare(v->getAsFloat());
}

bool QoreNumberNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   if (v->getType() != NT_NUMBER)
      return false;
   const QoreNumberNode* n = reinterpret_cast<const QoreNumberNode*>(v);
   return !priv->compare(*n->priv);
}

// returns the type name as a c string
const char* QoreNumberNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode* QoreNumberNode::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = numberTypeInfo;
   return this;
}

bool QoreNumberNode::zero() const {
   return priv->zero();
}

QoreNumberNode* QoreNumberNode::doPlus(const QoreNumberNode* right) const {
   return new QoreNumberNode(priv->doPlus(*right->priv));
}
