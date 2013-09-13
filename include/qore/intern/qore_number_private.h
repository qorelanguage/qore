/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_number_private.h

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

// the number of consecutive trailing 0 or 9 digits that will be rounded in string output
#define QORE_MPFR_ROUND_THRESHOLD 9
// the number of consecutive trailing 0 or 9 digits that will be rounded in string output if there are 2 trailing non-0/9 digits
#define QORE_MPFR_ROUND_THRESHOLD_2 15

#define QORE_DEFAULT_PREC 128
#define QORE_MAX_PREC 8192
#ifndef HAVE_MPFR_RNDN
#define MPFR_RNDN GMP_RNDN
#endif
// round to nearest (roundTiesToEven in IEEE 754-2008)
#define QORE_MPFR_RND MPFR_RNDN
// MPFR_RNDA

#ifndef HAVE_MPFR_EXP_T
typedef mp_exp_t mpfr_exp_t;
#endif

#ifdef HAVE_MPFR_SPRINTF
#define QORE_MPFR_SPRINTF_ARG 'R'
#else
#define QORE_MPFR_SPRINTF_ARG 'L'
#endif

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

   DLLLOCAL void setPrec(mpfr_prec_t prec) {
      if (prec > QORE_MAX_PREC)
         prec = QORE_MAX_PREC;
      mpfr_prec_round(num, prec, QORE_MPFR_RND);
   }

   DLLLOCAL static void do_divide_by_zero(ExceptionSink* xsink) {
      xsink->raiseException("DIVISION-BY-ZERO", "division by zero error in numeric operatior");
   }

   DLLLOCAL static void checkFlags(ExceptionSink* xsink) {
#ifdef HAVE_MPFR_DIVBY0
      if (mpfr_divby0_p()) {
         mpfr_clear_divby0();
         do_divide_by_zero(xsink);
      }
#endif
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

   DLLLOCAL qore_number_private(const char* str) : qore_number_private_intern(QORE_MAX(QORE_DEFAULT_PREC, strlen(str)*5)) {
      // see if number has an exponent and increase the number's precision if necessary
      const char* p = strchrs(str, "eE");
      if (p) {
         int exp = abs(atoi(p + 1));
         mpfr_prec_t np = exp * 5;
         if (np > getPrec())
            setPrec(np);
      }
      if (!str[0])
         mpfr_set_sj(num, 0, QORE_MPFR_RND);
      else
         mpfr_set_str(num, str, 10, QORE_MPFR_RND);
   }

   DLLLOCAL qore_number_private(const char* str, unsigned prec) : qore_number_private_intern(QORE_MAX(QORE_DEFAULT_PREC, prec)) {
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

#ifdef HAVE_MPFR_REGULAR
   // regular and not zero
   DLLLOCAL bool regular() const {
      return (bool)mpfr_regular_p(num);
   }
#endif

   DLLLOCAL int sign() const {
      return mpfr_sgn(num);
   }

   DLLLOCAL void sprintf(QoreString& str, const char* fmt) const {
#ifdef HAVE_MPFR_SPRINTF
      //printd(5, "qore_number_private::sprintf() fmt: '%s'\n", fmt);
      int len = mpfr_snprintf(0, 0, fmt, num);
      if (!len)
         return;
      if (len < 0) {
         numError(str);
         return;
      }
      str.allocate(str.size() + len + 1);
      mpfr_sprintf((char*)(str.getBuffer() + str.size()), fmt, num);
      str.terminate(str.size() + len);
#else
      // if there is no mpfr_sprintf, then we convert to a long double and output the number
      long double ld = mpfr_get_ld(num, QORE_MPFR_RND);
      int len = ::snprintf(0, 0, fmt, ld);
      if (len <= 0)
         return;
      str.allocate(str.size() + len + 1);
      ::sprintf((char*)(str.getBuffer() + str.size()), fmt, ld);
      str.terminate(str.size() + len);
#endif
   }

   DLLLOCAL void getScientificString(QoreString& str, bool round = true) const {
#ifdef HAVE_MPFR_SPRINTF
      sprintf(str, "%Re");
#else
      sprintf(str, "%Le");
#endif
      if (round) {
         qore_offset_t i = str.find('.');
         if (i != -1) {
            qore_offset_t e = str.rfind('e');
            if (e != -1)
               applyRoundingHeuristic(str, i, e);
         }
      }
   }

   DLLLOCAL void getAsString(QoreString& str, bool round = true) const;

   DLLLOCAL void toString(QoreString& str, int fmt = QORE_NF_DEFAULT) const {
      bool raw = !(fmt & QORE_NF_RAW);
      if (fmt & QORE_NF_SCIENTIFIC)
         getScientificString(str, raw);
      else
         getAsString(str, raw);
   }

   DLLLOCAL int format(QoreString& str, const QoreString& fmt, ExceptionSink* xsink) {
      getAsString(str, false);
      return formatNumberString(str, fmt, xsink);
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

   DLLLOCAL qore_number_private* doDivideBy(const qore_number_private& r, ExceptionSink* xsink) const {
#ifndef HAVE_MPFR_DIVBY0
      if (r.zero()) {
         do_divide_by_zero(xsink);
         return 0;
      }
#endif
      return doBinary(mpfr_div, r, xsink);
   }

   DLLLOCAL qore_number_private* doUnary(q_mpfr_unary_func_t func, ExceptionSink* xsink = 0) const {
      qore_number_private* p = new qore_number_private(*this);
      func(p->num, num, QORE_MPFR_RND);
      if (xsink)
         checkFlags(xsink);

      return p;
   }

   DLLLOCAL void negateInPlace() {
      mpfr_neg(num, num, QORE_MPFR_RND);
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

   DLLLOCAL mpfr_prec_t getPrec() const {
      return mpfr_get_prec(num);
   }

   DLLLOCAL void inc() {
      // some compilers (sun/oracle pro c++ notably) do not support arrays with a variable size
      // if not, we can't use the stack for the temporary variable and have to use a dynamically-allocated one
#ifdef HAVE_LOCAL_VARIADIC_ARRAYS
      MPFR_DECL_INIT(tmp, mpfr_get_prec(num));
      mpfr_set(tmp, num, QORE_MPFR_RND);
      mpfr_add_si(num, tmp, 1, QORE_MPFR_RND);
#else
      qore_number_private tmp(mpfr_get_prec(num));
      mpfr_set(tmp.num, num, QORE_MPFR_RND);
      mpfr_add_si(num, tmp.num, 1, QORE_MPFR_RND);
#endif
   }

   DLLLOCAL void dec() {
      // some compilers (sun/oracle pro c++ notably) do not support arrays with a variable size
      // if not, we can't use the stack for the temporary variable and have to use a dynamically-allocated one
#ifdef HAVE_LOCAL_VARIADIC_ARRAYS
      MPFR_DECL_INIT(tmp, mpfr_get_prec(num));
      mpfr_set(tmp, num, QORE_MPFR_RND);
      mpfr_sub_si(num, tmp, 1, QORE_MPFR_RND);
#else
      qore_number_private tmp(mpfr_get_prec(num));
      mpfr_set(tmp.num, num, QORE_MPFR_RND);
      mpfr_sub_si(num, tmp.num, 1, QORE_MPFR_RND);
#endif
   }

   DLLLOCAL void doBinaryInplace(q_mpfr_binary_func_t func, const qore_number_private& r, ExceptionSink* xsink = 0) {
      checkPrec(r.num);
      // some compilers (sun/oracle pro c++ notably) do not support arrays with a variable size
      // if not, we can't use the stack for the temporary variable and have to use a dynamically-allocated one
#ifdef HAVE_LOCAL_VARIADIC_ARRAYS
      MPFR_DECL_INIT(tmp, mpfr_get_prec(num));
      mpfr_set(tmp, num, QORE_MPFR_RND);
      func(num, tmp, r.num, QORE_MPFR_RND);
#else
      qore_number_private tmp(mpfr_get_prec(num));
      mpfr_set(tmp.num, num, QORE_MPFR_RND);
      func(num, tmp.num, r.num, QORE_MPFR_RND);
#endif
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

   DLLLOCAL static void negateInPlace(QoreNumberNode& n) {
      n.priv->negateInPlace();
   }

   DLLLOCAL static int formatNumberString(QoreString& num, const QoreString& fmt, ExceptionSink* xsink);

   DLLLOCAL static void numError(QoreString& str) {
      str.concat("<number error>");
   }

   // try to remove noise from the binary -> decimal conversion process in insignificant digits
   DLLLOCAL static void applyRoundingHeuristic(QoreString& str, qore_size_t dp, qore_size_t last);

   // returns number of digits inserted
   DLLLOCAL static int roundUp(QoreString& str, qore_offset_t pos);

   // static accessor methods
   DLLLOCAL static void sprintf(const QoreNumberNode& n, QoreString& str, const char* fmt) {
      n.priv->sprintf(str, fmt);
   }

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

   DLLLOCAL static QoreNumberNode* getPi() {
      qore_number_private* p = new qore_number_private(0ll);
      mpfr_const_pi(p->num, QORE_MPFR_RND);
      return new QoreNumberNode(p);
   }

   DLLLOCAL static qore_number_private* get(const QoreNumberNode& n) {
      return n.priv;
   }
};

#endif
