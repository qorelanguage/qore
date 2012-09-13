/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_number_private.h

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

#ifndef _QORE_QORE_NUMBER_PRIVATE_H

#define _QORE_QORE_NUMBER_PRIVATE_H

#define QORE_DEFAULT_PREC 128
#define QORE_MAX_PREC 8192
// round to nearest (roundTiesToEven in IEEE 754-2008)
#define QORE_MPFR_RND MPFR_RNDN
// MPFR_RNDA

// for binary operations on MPFR data
typedef int (*q_mpfr_binary_func_t)(mpfr_t, const mpfr_t, const mpfr_t, mpfr_rnd_t);
// for unary operations on MPFR data
typedef int (*q_mpfr_unary_func_t)(mpfr_t, const mpfr_t, mpfr_rnd_t);
// for unary operations on MPFR data without a rounding argument
typedef int (*q_mpfr_unary_nr_func_t)(mpfr_t, const mpfr_t);

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

   DLLLOCAL void checkPrec(const mpfr_t r) {
      mpfr_prec_t p = mpfr_get_prec(r);
      if (p > mpfr_get_prec(num))
         mpfr_prec_round(num, p, QORE_MPFR_RND);
   }

   DLLLOCAL static void checkFlags(ExceptionSink* xsink) {
      if (mpfr_divby0_p()) {
         mpfr_clear_divby0();
         xsink->raiseException("DIVISION-BY-ZERO", "division by zero error in numeric operatior");
      }
      if (mpfr_erangeflag_p()) {
         mpfr_clear_erangeflag();
         xsink->raiseException("INVALID-NUMERIC-OPERATION", "invalid numeric operation attempted");
      }
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

   DLLLOCAL int sign() const {
      return mpfr_sgn(num);
   }

   DLLLOCAL void getAsString(QoreString& str) const {
      // first check for zero
      if (zero()) {
         str.concat("0");
         return;
      }

#ifdef DO_MPFR_PRINTF
#define QORE_MPFR_BUFSIZE 512
      str.allocate(str.size() + QORE_MPFR_BUFSIZE);
      int len = mpfr_sprintf((char*)(str.getBuffer() + str.size()), "%Re", num);
      if (len > 0) {
         str.terminate(str.size() + len);
         //str.concat('n');
      }
      else
         str.concat("<error>");
#else
      mpfr_exp_t exp;
      char* buf = mpfr_get_str(0, &exp, 10, 0, num, QORE_MPFR_RND);
      if (!buf) {
         str.concat("<number error>");
         return;
      }

      // if it's a regular number, then format accordingly
      if (number()) {
         int sgn = sign();
         qore_size_t len = str.size() + (sgn < 0 ? 1 : 0);
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
            if ((qore_size_t)exp > rlen)
               str.insertch('0', str.size(), exp - rlen);
            else if ((qore_size_t)exp < rlen)
               str.insertch('.', len + exp, 1);
         }
         //str.concat('n');
      }
      else
         str.concat(buf);

      mpfr_free_str(buf);
#endif
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

   DLLLOCAL qore_number_private* doBinary(q_mpfr_binary_func_t func, const qore_number_private& r, ExceptionSink* xsink = 0) const {
      mpfr_prec_t prec = QORE_MAX(mpfr_get_prec(num), mpfr_get_prec(r.num));
      qore_number_private* p = new qore_number_private(prec);
      func(p->num, num, r.num, QORE_MPFR_RND);
      if (xsink)
         checkFlags(xsink);

      return p;
   }

   DLLLOCAL qore_number_private* doPlus(const qore_number_private& r) const {
      return doBinary(mpfr_add, r);
   }

   DLLLOCAL qore_number_private* doMinus(const qore_number_private& r) const {
      return doBinary(mpfr_sub, r);
   }

   DLLLOCAL qore_number_private* doMultiply(const qore_number_private& r) const {
      return doBinary(mpfr_mul, r);
   }

   //! add the argument to this value and return the result
   DLLLOCAL qore_number_private* doDivideBy(const qore_number_private& r, ExceptionSink* xsink) const {
      return doBinary(mpfr_div, r, xsink);
   }

   DLLLOCAL qore_number_private* doUnary(q_mpfr_unary_func_t func, ExceptionSink* xsink = 0) const {
      qore_number_private* p = new qore_number_private(*this);
      func(p->num, num, QORE_MPFR_RND);
      if (xsink)
         checkFlags(xsink);

      return p;
   }

   DLLLOCAL qore_number_private* negate() const {
      return doUnary(mpfr_neg);
   }

   DLLLOCAL qore_number_private* absolute() const {
      return doUnary(mpfr_abs);
   }

   DLLLOCAL qore_number_private* doUnaryNR(q_mpfr_unary_nr_func_t func, ExceptionSink* xsink = 0) const {
      qore_number_private* p = new qore_number_private(*this);
      func(p->num, num);
      if (xsink)
         checkFlags(xsink);

      return p;
   }

   DLLLOCAL void inc() {
      //MPFR_DECL_INIT(tmp, mpfr_get_prec(num));
      qore_number_private tmp(mpfr_get_prec(num));
      mpfr_set(tmp,num, num, QORE_MPFR_RND);
      mpfr_add_si(num, tmp.num, 1, QORE_MPFR_RND);
   }

   DLLLOCAL void dec() {
      //MPFR_DECL_INIT(tmp, mpfr_get_prec(num));
      qore_number_private tmp(mpfr_get_prec(num));
      mpfr_set(tmp.num, num, QORE_MPFR_RND);
      mpfr_sub_si(num, tmp.num, 1, QORE_MPFR_RND);
   }

   DLLLOCAL void doBinaryInplace(q_mpfr_binary_func_t func, const qore_number_private& r, ExceptionSink* xsink = 0) {
      checkPrec(r.num);
      //MPFR_DECL_INIT(tmp, mpfr_get_prec(num));
      qore_number_private tmp(mpfr_get_prec(num));
      mpfr_set(tmp.num, num, QORE_MPFR_RND);
      func(num, tmp.num, r.num, QORE_MPFR_RND);
      if (xsink)
         checkFlags(xsink);
   }

   DLLLOCAL void plusEquals(const qore_number_private& r) {
      doBinaryInplace(mpfr_add, r);
   }

   DLLLOCAL void minusEquals(const qore_number_private& r) {
      doBinaryInplace(mpfr_sub, r);
   }

   DLLLOCAL void multiplyEquals(const qore_number_private& r) {
      doBinaryInplace(mpfr_mul, r);
   }

   DLLLOCAL void divideEquals(const qore_number_private& r) {
      assert(!r.zero());
      doBinaryInplace(mpfr_div, r);
   }

   // static accessor methods
   DLLLOCAL static void inc(QoreNumberNode& n) {
      n.priv->inc();
   }

   DLLLOCAL static void dec(QoreNumberNode& n) {
      n.priv->dec();
   }

   DLLLOCAL static void plusEquals(QoreNumberNode& n, const QoreNumberNode& r) {
      n.priv->plusEquals(*r.priv);
   }

   DLLLOCAL static void minusEquals(QoreNumberNode& n, const QoreNumberNode& r) {
      n.priv->minusEquals(*r.priv);
   }

   DLLLOCAL static void multiplyEquals(QoreNumberNode& n, const QoreNumberNode& r) {
      n.priv->multiplyEquals(*r.priv);
   }

   DLLLOCAL static void divideEquals(QoreNumberNode& n, const QoreNumberNode& r) {
      n.priv->divideEquals(*r.priv);
   }

   DLLLOCAL static QoreNumberNode* doUnary(const QoreNumberNode& n, q_mpfr_unary_func_t func, ExceptionSink* xsink = 0) {
      qore_number_private* p = n.priv->doUnary(func, xsink);
      return p ? new QoreNumberNode(p) : 0;
   }

   DLLLOCAL static QoreNumberNode* doBinary(const QoreNumberNode& n, q_mpfr_binary_func_t func, const QoreNumberNode& r, ExceptionSink* xsink = 0) {
      qore_number_private* p = n.priv->doBinary(func, *r.priv, xsink);
      return p ? new QoreNumberNode(p) : 0;
   }

   DLLLOCAL static QoreNumberNode* doUnaryNR(const QoreNumberNode& n, q_mpfr_unary_nr_func_t func, ExceptionSink* xsink = 0) {
      qore_number_private* p = n.priv->doUnaryNR(func, xsink);
      return p ? new QoreNumberNode(p) : 0;
   }

   DLLLOCAL static QoreNumberNode* getNaNumber() {
      return new QoreNumberNode(new qore_number_private("@NaN@"));
   }

   DLLLOCAL static QoreNumberNode* getInfinity() {
      return new QoreNumberNode(new qore_number_private("@Inf@"));
   }
};

#endif
