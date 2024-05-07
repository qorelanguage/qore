/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QoreLoggerEvent.cpp LoggerEvent class definition */
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

#include "qore_logger.h"
#include "QC_LoggerLevel.h"
#include "QoreLoggerEvent.h"

SimpleRefHolder<DateTimeNode> QoreLoggerEvent::startTime;
Sequence QoreLoggerEvent::sequence(1);
QoreThreadLock QoreLoggerEvent::m;

void QoreLoggerEvent::init() {
    int us;
    int64 seconds = q_epoch_us(us);
    startTime = DateTimeNode::makeAbsolute(currentTZ(), seconds, us);
}

//! Returns the full qualified classname
QoreStringNode* QoreLoggerEvent::getFullQualifiedClassname() const {
    return fqcn ? fqcn->stringRefSelf() : nullptr;
}

//! Returns the location information for this logging event
QoreHashNode* QoreLoggerEvent::getLocationInfo() const {
    return locationInfo ? locationInfo->hashRefSelf() : nullptr;
}

//! Returns the level of this event
QoreObject* QoreLoggerEvent::getLevel() {
    if (!level && lvl) {
        AutoLocker al(m0);
        // check again in the lock
        if (!level && lvl) {
            lvl->ref();
            level = new QoreObject(QC_LOGGERLEVEL, getProgram(), lvl);
        }
    }
    return level ? level->objectRefSelf() : nullptr;
}

//! Returns the logger level object for this event
const QoreLoggerLevel* QoreLoggerEvent::getLoggerLevel() const {
    return lvl;
}

//! Returns the logger which created the event
QoreObject* QoreLoggerEvent::getLogger() const {
    return logger ? logger->objectRefSelf() : nullptr;
}

//! Returns the thread id which is related to event
int QoreLoggerEvent::getThreadId() const {
    return threadId;
}

//! Returns the category name
QoreStringNode* QoreLoggerEvent::getCategoryName() const {
    return categoryName ? categoryName->stringRefSelf() : nullptr;
}

//! Returns the event timestamp as an @ref absolute_dates "absolute date/time value"
DateTimeNode* QoreLoggerEvent::getTimeStamp() const {
    return timeStamp->refSelf();
}

//! Returns a relative timestamp for the event
/**
    Returns a @ref relative_dates "relative date/time value" for the amount of time passed
    from the beginning of execution to the time when the event was constructed.

    @see @ref getStartTime()
*/
DateTimeNode* QoreLoggerEvent::getRelativeTime() const {
    AutoLocker al(m);
    return timeStamp->subtractBy(*startTime);
}

//! Returns throwable info, if any
QoreHashNode* QoreLoggerEvent::getThrowableInfo() const {
    return throwableInfo ? throwableInfo->hashRefSelf() : nullptr;
}

//! Generates a globally unique integer identifier and associates it to the event
int QoreLoggerEvent::getUniqueId() const {
    return uniqueId;
}

//! Returns the string message for the logging event
QoreStringNode* QoreLoggerEvent::getMessage(ExceptionSink* xsink) const {
    if (!renderedMessage) {
        AutoLocker al(m0);
        // check again in the lock
        if (!renderedMessage) {
            renderMessageLocked(xsink);
        }
    }
    assert(renderedMessage);
    return renderedMessage->stringRefSelf();
}

//! Returns the time when the application/logger started to calculate relative time
DateTimeNode* QoreLoggerEvent::getStartTime() {
    AutoLocker al(m);
    return startTime->refSelf();
}

//! Sets the starting time for relative time
void QoreLoggerEvent::setStartTime(const DateTimeNode* time) {
    AutoLocker al(m);
    startTime = time->refSelf();;
}

void QoreLoggerEvent::renderMessageLocked(ExceptionSink* xsink) const {
    // evaluating of callable parameters (via LoggerEventParameter) may be used to get time consuming stuff
    // i.e. they are evaluated only when logging is performed
    ReferenceHolder<QoreListNode> args(new QoreListNode(autoTypeInfo), xsink);
    args->push(messageFmt->stringRefSelf(), xsink);
    if (messageArgs) {
        ConstListIterator i(messageArgs, (ssize_t)offset - 1);
        while (i.next()) {
            const QoreValue arg = i.getValue();
            bool done = false;
            if (arg.getType() == NT_OBJECT) {
                QoreObject* obj = const_cast<QoreObject*>(arg.get<const QoreObject>());
                // FIXME: implement proper hierarchy check with LoggerEventParameter as a base class
                if (!strcmp("LoggerEventParameter", obj->getClassName())) {
                    ExceptionSink xsink2;
                    ValueHolder v(obj->evalMethod("call", nullptr, &xsink2), xsink);
                    if (xsink2) {
                        args->push(xsink2.getExceptionInfo(), xsink);
                    } else {
                        args->push(v.release(), xsink);
                    }
                    done = true;
                }
            }
            if (!done) {
                args->push(arg.refSelf(), xsink);
            }
        }
    }
    // message is considered as printf() format so we process it this way.
    renderedMessage = q_sprintf(*args, 0, 0, xsink);
}