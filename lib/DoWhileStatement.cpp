/*
    DoWhileStatement.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2023 Qore Technologies, s.r.o.

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
#include "qore/intern/DoWhileStatement.h"
#include "qore/intern/StatementBlock.h"

int DoWhileStatement::execImpl(QoreValue& return_value, ExceptionSink* xsink) {
    // instantiate local variables
    LVListInstantiator lvi(lvars, xsink);

    int rc = 0;

    while (true) {
        if (code) {
            rc = code->execImpl(return_value, xsink);
            if (*xsink || rc == RC_BREAK) {
                rc = 0;
                break;
            }
            if (rc == RC_RETURN) {
                break;
            }
            if (rc == RC_CONTINUE) {
                rc = 0;
            }
        }

        ValueEvalRefHolder val(cond, xsink);
        if (*xsink) {
            break;
        }

        if (!val->getAsBool()) {
            break;
        }
    }

    return rc;
}

/* do ... while statements can have variables local to the statement
 * however, it doesn't do much good :-) */
int DoWhileStatement::parseInitImpl(QoreParseContext& parse_context) {
    // turn off top-level flag for statement vars
    QoreParseContextFlagHelper fh(parse_context);
    fh.unsetFlags(PF_TOP_LEVEL);

    // saves local variables after parsing
    QoreParseContextLvarHelper lh(parse_context, lvars);

    int err = 0;

    if (code) {
        QoreParseContextFlagHelper fh0(parse_context);
        fh0.setFlags(PF_BREAK_OK | PF_CONTINUE_OK);

        err = code->parseInitImpl(parse_context);
    }
    if (cond) {
        parse_context.typeInfo = nullptr;
        if (parse_init_value(cond, parse_context) && !err) {
            err = -1;
        }
        // FIXME: raise a parse warning if cond cannot be converted to a bool (i.e. always false)
    }

    return err;
}
