/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QoreLoggerPattern.h LoggerPattern class definition */
/*
    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

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

#ifndef _QORE_MODULE_LOGGER_LOGGERPATTERN_H

#define _QORE_MODULE_LOGGER_LOGGERPATTERN_H

#include <regex>

//! Escape char
#define ESCAPE_STR "%"
#define ESCAPE_CHAR ESCAPE_STR[0]

//! Token extraction pattern
#define TOKEN_EXTRACT_PATTERN "^([-0-9\\.]*)([a-z]+)(\\{[^\\}]*\\})?"

#define TOKEN_PATTERN_1 "^(-?)([0-9]+)\\.([0-9]+)$"
#define TOKEN_PATTERN_2 "^(-?)([0-9]+)$"
#define TOKEN_PATTERN_3 "^.([0-9]+)$"
#define TOKEN_PATTERN_4 "^\\{([^\\}]*)\\}$"
#define TOKEN_PATTERN_5 "^[-0-9\\.]*[a-z]+(\\{[^\\}]*\\})?"

// forward references
class QoreLoggerLayoutPattern;
class QoreLoggerEvent;

class QoreLoggerPattern : public AbstractPrivateData {
public:
    DLLLOCAL QoreLoggerPattern(QoreObject* self) : self(self) {
    }

    DLLLOCAL virtual ~QoreLoggerPattern() {
    }

    DLLLOCAL int setPattern(const QoreStringNode* pattern, ExceptionSink* xsink);

    using AbstractPrivateData::deref;
    DLLLOCAL virtual void deref(ExceptionSink* xsink) {
        if (ROdereference()) {
            if (parsedPattern) {
                parsedPattern->deref(xsink);
            }
            delete this;
        }
    }

    DLLLOCAL QoreStringNode* getPattern() const {
        return origPattern ? origPattern->stringRefSelf() : new QoreStringNode();
    }

    DLLLOCAL QoreListNode* getParsedPattern() const {
        return parsedPattern ? parsedPattern->listRefSelf() : new QoreListNode(autoTypeInfo);
    }

    DLLLOCAL QoreStringNode* format(const QoreValue data, QoreLoggerLayoutPattern* llp, ExceptionSink* xsink) const;

protected:
    //! parsed pattern; list elements may be strings or hashes
    QoreListNode* parsedPattern = nullptr;

    //! Atomicity lock
    mutable QoreThreadLock m;

    //! The QoreObject this private data is associated with
    QoreObject* self;

    DLLLOCAL QoreValue callResolveField(QoreLoggerLayoutPattern* llp, QoreObject* event, QoreLoggerEvent* ev,
            const QoreValue& data, const QoreStringNode* key, const QoreStringNode* option,
            ExceptionSink* xsink) const;

private:
    //! pattern
    SimpleRefHolder<QoreStringNode> origPattern;

    //! Token extraction regexes
    static QoreRegexInterface te;
    static QoreRegexInterface t1;
    static QoreRegexInterface t2;
    static QoreRegexInterface t3;
    static QoreRegexInterface t4;
    static QoreRegexSubstInterface t5;
};

#endif
