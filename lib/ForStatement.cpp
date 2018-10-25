/*
    ForStatement.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    Note that the Qore library is released under a choice of three open-source
    licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
    information.
*/

#include <qore/Qore.h>
#include "qore/intern/ForStatement.h"
#include "qore/intern/StatementBlock.h"

ForStatement::ForStatement(int start_line, int end_line, QoreValue a, QoreValue c, QoreValue i, StatementBlock* cd) : AbstractStatement(start_line, end_line), assignment(a), cond(c), iterator(i), code(cd) {
}

ForStatement::~ForStatement() {
    assignment.discard(nullptr);
    cond.discard(nullptr);
    iterator.discard(nullptr);
    delete code;
    delete lvars;
}

int ForStatement::execImpl(QoreValue& return_value, ExceptionSink *xsink) {
    // instantiate local variables
    LVListInstantiator lvi(lvars, xsink);

    // evaluate assignment expression and discard results if any
    if (assignment) {
        ValueEvalRefHolder tmp(assignment, xsink);
        if (*xsink) {
            return 0;
        }
    }

    int rc = 0;

    // execute "for" body
    while (!*xsink) {
        // check conditional expression, exit "for" loop if condition is
        // false
        if (cond) {
            ValueEvalRefHolder val(cond, xsink);
            if (*xsink || !val->getAsBool()) {
                break;
            }
        }

        // otherwise, execute "for" body
        if (code) {
            rc = code->execImpl(return_value, xsink);
            if (*xsink || rc == RC_BREAK) {
                rc = 0;
                break;
            }

            if (rc == RC_RETURN) {
                break;
            }
            else if (rc == RC_CONTINUE) {
                rc = 0;
            }
        }

        // evaluate iterator expression and discard results if any
        if (iterator) {
            ValueEvalRefHolder tmp(iterator, xsink);
            if (*xsink) {
                break;
            }
        }
    }

    return rc;
}

int ForStatement::parseInitImpl(LocalVar *oflag, int pflag) {
    int lvids = 0;

    // turn off top-level flag for statement vars
    pflag &= (~PF_TOP_LEVEL);

    const QoreTypeInfo* argTypeInfo = nullptr;
    if (assignment) {
        parse_init_value(assignment, oflag, pflag | PF_RETURN_VALUE_IGNORED, lvids, argTypeInfo);
        // enable optimizations when return value is ignored for operator expressions
        ignore_return_value(assignment);
    }
    if (cond) {
        argTypeInfo = nullptr;
        parse_init_value(cond, oflag, pflag, lvids, argTypeInfo);
        // FIXME: raise a parse warning if cond cannot be converted to a bool (i.e. always false)
    }
    if (iterator) {
        argTypeInfo = nullptr;
        parse_init_value(iterator, oflag, pflag | PF_RETURN_VALUE_IGNORED, lvids, argTypeInfo);
        // enable optimizations when return value is ignored for operator expressions
        ignore_return_value(iterator);
    }
    if (code) {
        code->parseInitImpl(oflag, pflag | PF_BREAK_OK | PF_CONTINUE_OK);
    }

    // save local variables
    if (lvids) {
        lvars = new LVList(lvids);
    }

    return 0;
}

void ForStatement::parseCommit(QoreProgram* pgm) {
    AbstractStatement::parseCommit(pgm);
    if (code) {
        code->parseCommit(pgm);
    }
}

