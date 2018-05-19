/*
    QoreException.cpp

    Qore programming language exception handling support

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
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreHashNodeIntern.h"

#include <qore/safe_dslist>

#include <assert.h>

#define Q_MAX_EXCEPTIONS 10

void QoreException::del(ExceptionSink *xsink) {
    if (callStack) {
        callStack->deref(xsink);
#ifdef DEBUG
        callStack = nullptr;
#endif
    }
    err.discard(xsink);
    desc.discard(xsink);
    arg.discard(xsink);
    if (next)
        next->del(xsink);

    delete this;
}

QoreHashNode* QoreException::makeExceptionObject() {
    QORE_TRACE("makeExceptionObject()");

    QoreHashNode* h = new QoreHashNode(hashdeclExceptionInfo, nullptr);
    auto ph = qore_hash_private::get(*h);

    ph->setKeyValueIntern("type", new QoreStringNode(type == ET_USER ? "User" : "System"));
    ph->setKeyValueIntern("file", new QoreStringNode(file));
    ph->setKeyValueIntern("line", start_line);
    ph->setKeyValueIntern("endline", end_line);
    ph->setKeyValueIntern("source", new QoreStringNode(source));
    ph->setKeyValueIntern("offset", offset);
    ph->setKeyValueIntern("callstack", callStack->refSelf());
    if (err) {
        ph->setKeyValueIntern("err", err.refSelf());
    }
    if (desc) {
        ph->setKeyValueIntern("desc", desc.refSelf());
    }
    if (arg) {
        ph->setKeyValueIntern("arg", arg.refSelf());
    }

    // add chained exceptions with this "chain reaction" call
    if (next)
        ph->setKeyValueIntern("next", next->makeExceptionObject());

    return h;
}

QoreHashNode *QoreException::makeExceptionObjectAndDelete(ExceptionSink *xsink) {
   QORE_TRACE("makeExceptionObjectAndDelete()");
   QoreHashNode* rv = makeExceptionObject();
   del(xsink);

   return rv;
}

void QoreException::addStackInfo(QoreValue n) {
   callStack->push(n, nullptr);
}

// static member function
void ExceptionSink::defaultExceptionHandler(QoreException* e) {
    ExceptionSink xsink;

    QoreString nstr;
    {
        DateTime now;
        now.setNow();
        now.format(nstr, "YYYY-MM-DD HH:mm:SS.xx Dy Z (z)");
    }

    unsigned ecnt = 0;

    while (e) {
        //printd(5, "ExceptionSink::defaultExceptionHandler() cs size=%d\n", cs->size());
        printe("unhandled QORE %s exception thrown in TID %d at %s", e->type == ET_USER ? "User" : "System", gettid(), nstr.getBuffer());

        QoreListNode *cs = e->callStack;
        bool found = false;
        if (cs->size()) {
            // find first non-rethrow element
            unsigned i = 0;

            QoreHashNode *h;
            while (true) {
                h = cs->retrieveEntry(i).get<QoreHashNode>();
                assert(h);
                if (h->getKeyValue("typecode").getAsBigInt() != CT_RETHROW)
                    break;
                i++;
                if (i == cs->size())
                    break;
            }

            if (i < cs->size()) {
                found = true;
                QoreStringNode *func = h->getKeyValue("function").get<QoreStringNode>();
                QoreStringNode *type = h->getKeyValue("type").get<QoreStringNode>();

                printe(" in %s() (%s:%d", func->getBuffer(), e->file.c_str(), e->start_line);

                if (e->start_line == e->end_line) {
                    if (!e->source.empty())
                        printe(", source %s:%d", e->source.c_str(), e->start_line + e->offset);
                }
                else {
                    printe("-%d", e->end_line);
                    if (!e->source.empty())
                        printe(", source %s:%d-%d", e->source.c_str(), e->start_line + e->offset, e->end_line + e->offset);
                }
                printe(", %s code)\n", type->getBuffer());
            }
        }

        if (!found) {
            if (!e->file.empty()) {
                printe(" at %s:", e->file.c_str());
                if (e->start_line == e->end_line) {
                    if (!e->start_line) {
                        printe("<init>");
                        if (!e->source.empty())
                            printe(" (source %s)", e->source.c_str());
                    }
                    else {
                        printe("%d", e->start_line);
                        if (!e->source.empty())
                            printe(" (source %s:%d)", e->source.c_str(), e->start_line + e->offset);
                    }
                }
                else {
                    printe("%d-%d", e->start_line, e->end_line);
                    if (!e->source.empty())
                        printe(" (source %s:%d-%d)", e->source.c_str(), e->start_line + e->offset, e->end_line + e->offset);
                }
            }
            else if (e->start_line) {
                if (e->start_line == e->end_line) {
                if (!e->start_line)
                    printe(" at <init>");
                else
                    printe(" on line %d", e->start_line);
                }
                else
                printe(" on lines %d through %d", e->start_line, e->end_line);
            }
            printe("\n");
        }

        if (e->type == ET_SYSTEM) {
            QoreStringNode* err = e->err.get<QoreStringNode>();
            QoreStringNode* desc = e->desc.get<QoreStringNode>();
            printe("%s: %s\n", err->c_str(), desc->c_str());
        }
        else {
            bool hdr = false;
            if (!e->err.isNothing()) {
                if (e->err.getType() == NT_STRING) {
                    QoreStringNode *err = e->err.get<QoreStringNode>();
                    printe("%s", err->c_str());
                }
                else {
                    QoreNodeAsStringHelper str(e->err, FMT_NORMAL, &xsink);
                    printe("EXCEPTION: %s", str->c_str());
                    hdr = true;
                }
            }
            else
                printe("EXCEPTION");

            if (!e->desc.isNothing()) {
                if (e->desc.getType() == NT_STRING) {
                    QoreStringNode *desc = e->desc.get<QoreStringNode>();
                    printe("%s%s", hdr ? ", desc: " : ": ", desc->c_str());
                }
                else {
                    QoreNodeAsStringHelper str(e->desc, FMT_NORMAL, &xsink);
                    printe(", desc: %s", str->c_str());
                    if (!hdr)
                        hdr = true;
                }
            }

            if (!e->arg.isNothing()) {
                if (e->arg.getType() == NT_STRING) {
                    QoreStringNode *arg = e->arg.get<QoreStringNode>();
                    printe("%s%s", hdr ? ", arg: " : ": ", arg->c_str());
                }
                else {
                    QoreNodeAsStringHelper str(e->arg, FMT_NORMAL, &xsink);
                    printe(", arg: %s", str->c_str());
                }
            }
            printe("\n");
        }

        if (cs->size()) {
            printe("call stack:\n");
            for (unsigned i = 0; i < cs->size(); i++) {
                int pos = cs->size() - i;
                QoreHashNode* h = cs->retrieveEntry(i).get<QoreHashNode>();
                QoreStringNode* strtype = h->getKeyValue("type").get<QoreStringNode>();
                const char* type = strtype->getBuffer();
                int typecode = (int)h->getKeyValue("typecode").getAsBigInt();
                if (!strcmp(type, "new-thread"))
                    printe(" %2d: *thread start*\n", pos);
                else {
                    QoreStringNode* fn = h->getKeyValue("file").get<QoreStringNode>();
                    const char* fns = fn && !fn->empty() ? fn->getBuffer() : 0;
                    int start_line = (int)h->getKeyValue("line").getAsBigInt();
                    int end_line = (int)h->getKeyValue("endline").getAsBigInt();

                    QoreStringNode* src = h->getKeyValue("source").get<QoreStringNode>();
                    const char* srcs = src && !src->empty() ? src->getBuffer() : 0;
                    int offset = (int)h->getKeyValue("offset").getAsBigInt();

                    printe(" %2d: ", pos);

                    if (typecode == CT_RETHROW) {
                        printe("RETHROW at ");
                        if (fn) {
                            printe("%s:", fn->getBuffer());
                        }
                        else
                            printe("line");
                        printe("%d", start_line);
                        if (srcs)
                            printe(" (source %s:%d)", srcs, offset + start_line);
                    }
                    else {
                        QoreStringNode* fs = h->getKeyValue("function").get<QoreStringNode>();
                        printe("%s() (", fs->getBuffer());
                        if (fns) {
                            if (start_line == end_line) {
                                if (!start_line)
                                    printe("%s:<init>", fns);
                                else {
                                    printe("%s:%d", fns, start_line);
                                    if (srcs)
                                        printe(" (source %s:%d)", srcs, start_line + offset);
                                }
                            }
                            else {
                                printe("%s:%d-%d", fns, start_line, end_line);
                                if (srcs)
                                    printe(" (source %s:%d-%d)", srcs, start_line + offset, end_line + offset);
                            }
                        }
                        else {
                            if (start_line == end_line) {
                                if (!start_line)
                                    printe("<init>");
                                else
                                    printe("line %d", start_line);
                            }
                            else
                                printe("line %d - %d", start_line, end_line);
                        }
                        printe(", %s code)", type);
                    }
                    printe("\n");
                }
            }
        }
        e = e->next;
        if (e) {
            ++ecnt;
            if (ecnt == Q_MAX_EXCEPTIONS) {
                printe("*** maximum exception count reached (%d); suppressing further output\n", ecnt);
                break;
            }
            printe("chained exception:\n");
        }
    }
}

// static member function
void ExceptionSink::defaultWarningHandler(QoreException *e) {
   ExceptionSink xsink;

   while (e) {
      printe("warning encountered ");

      if (!e->file.empty()) {
         printe("at %s:", e->file.c_str());
         if (e->start_line == e->end_line) {
            if (!e->start_line) {
               printe("<init>");
               if (!e->source.empty())
                  printe(" (source %s)", e->source.c_str());
            }
            else {
               printe("%d", e->start_line);
               if (!e->source.empty())
                  printe(" (source %s:%d)", e->source.c_str(), e->start_line + e->offset);
            }
         }
         else {
            printe("%d-%d", e->start_line, e->end_line);
            if (!e->source.empty())
               printe(" (source %s:%d-%d)", e->source.c_str(), e->start_line + e->offset, e->end_line + e->offset);
         }
      }
      else if (e->start_line) {
         if (e->start_line == e->end_line) {
            if (!e->start_line)
               printe("at <init>");
            else
               printe("on line %d", e->start_line);
         }
         else
            printe("on line %d-%d", e->start_line, e->end_line);
      }
      printe("\n");

      QoreStringNode* err  = e->err.get<QoreStringNode>();
      QoreStringNode* desc = e->desc.get<QoreStringNode>();

      printe("%s: %s\n", err->c_str(), desc->c_str());

      e = e->next;
      if (e)
         printe("next warning:\n");
   }
}

const char* QoreException::getType(qore_call_t type) {
    switch (type) {
        case CT_USER:
            return "user";
        case CT_BUILTIN:
            return "builtin";
        case CT_RETHROW:
            return "rethrow";
        default:
            break;
    }
    assert(false);
    return 0;
}

// static function
QoreHashNode* QoreException::getStackHash(const QoreCallStackElement& cse) {
    QoreHashNode* h = new QoreHashNode;

    qore_hash_private* ph = qore_hash_private::get(*h);

    assert(!cse.code.empty());
    ph->setKeyValueIntern("function", new QoreStringNode(cse.code));
    ph->setKeyValueIntern("line",     cse.start_line);
    ph->setKeyValueIntern("endline",  cse.end_line);
    ph->setKeyValueIntern("file",     !cse.label.empty() ? new QoreStringNode(cse.label) : QoreValue());
    ph->setKeyValueIntern("source",   !cse.source.empty() ? new QoreStringNode(cse.source) : QoreValue());
    ph->setKeyValueIntern("offset",   cse.offset);
    ph->setKeyValueIntern("typecode", cse.type);
    ph->setKeyValueIntern("type",     new QoreStringNode(getType(cse.type)));

    return h;
}

// static function
QoreHashNode *QoreException::getStackHash(int type, const char *class_name, const char *code, const QoreProgramLocation& loc) {
    QoreHashNode* h = new QoreHashNode;

    qore_hash_private* ph = qore_hash_private::get(*h);

    QoreStringNode *str = new QoreStringNode;
    if (class_name)
        str->sprintf("%s::", class_name);
    str->concat(code);

    //printd(5, "QoreException::getStackHash() %s at %s:%d-%d src: %s+%d\n", str->getBuffer(), loc.getFile() ? loc.getFile() : "n/a", loc.start_line, loc.end_line, loc.getSource() ? loc.getSource() : "n/a", loc.offset);

    ph->setKeyValueIntern("function", str);
    ph->setKeyValueIntern("line",     loc.start_line);
    ph->setKeyValueIntern("endline",  loc.end_line);
    ph->setKeyValueIntern("file",     loc.getFile() ? new QoreStringNode(loc.getFile()) : QoreValue());
    ph->setKeyValueIntern("source",   loc.getSource() ? new QoreStringNode(loc.getSource()) : QoreValue());
    ph->setKeyValueIntern("offset",   loc.offset);
    ph->setKeyValueIntern("typecode", type);
    ph->setKeyValueIntern("type",     new QoreStringNode(getType((qore_call_t)type)));

    return h;
}

DLLLOCAL ParseExceptionSink::~ParseExceptionSink() {
   if (xsink)
      qore_program_private::addParseException(getProgram(), xsink);
}
