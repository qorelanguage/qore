/*
  Number.h

  Qore Arbitrary-precision numeric support

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols
*/

#ifndef _QORE_TYPES_NUMBER_H

#define _QORE_TYPES_NUMBER_H

#include <qore/common.h>
QoreNode *number_convert_to(QoreNode *node, int type);
QoreNode *defNumber();

class QoreNode *number_op_lt(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_gt(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_log_eq(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_ne(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_le(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_ge(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_cmp(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_minus(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_plus(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_multiply(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_divide(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_unary_minus(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);
class QoreNode *number_op_log_not(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);

int number_compare(class QoreNode *left, class QoreNode *right, ExceptionSink *xsink);

#endif
