/*
  Number.h

  Qore Arbitrary-precision numeric support

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
*/

#ifndef _QORE_TYPES_NUMBER_H

#define _QORE_TYPES_NUMBER_H

#include <qore/common.h>
AbstractQoreNode *number_convert_to(AbstractQoreNode *node, int type);
AbstractQoreNode *defNumber();

class AbstractQoreNode *number_op_lt(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_gt(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_log_eq(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_ne(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_le(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_ge(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_cmp(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_minus(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_plus(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_multiply(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_divide(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_unary_minus(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);
class AbstractQoreNode *number_op_log_not(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);

int number_compare(class AbstractQoreNode *left, class AbstractQoreNode *right, ExceptionSink *xsink);

#endif
