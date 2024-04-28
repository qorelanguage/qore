/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QoreLoggerEvent.h LoggerEvent class definition */
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

#ifndef _QORE_MODULE_LOGGER_LOGGEREVENT_H

#define _QORE_MODULE_LOGGER_LOGGEREVENT_H

class QoreLoggerEvent : public AbstractPrivateData {
public:
    DLLLOCAL QoreLoggerEvent(QoreObject* logger, const QoreStringNode* categoryName, const QoreObject* level,
            const QoreStringNode* message, const QoreListNode* msg_args, const QoreHashNode* location_info, int tid,
            DateTimeNode* now, const QoreHashNode* throwable)
            : fqcn(new QoreStringNode(logger->getClassName())), logger(logger->objectRefSelf()),
            categoryName(categoryName->stringRefSelf()), level(level->objectRefSelf()),
            messageFmt(message->stringRefSelf()), messageArgs(msg_args ? msg_args->listRefSelf() : nullptr),
            threadId(tid), timeStamp(now), locationInfo(location_info ? location_info->hashRefSelf() : nullptr),
            throwableInfo(throwable ? throwable->hashRefSelf() : nullptr) {
        assert(timeStamp);
    }

    DLLLOCAL QoreLoggerEvent(QoreStringNode* fqcn, const QoreStringNode* categoryName, const QoreObject* level,
            const QoreStringNode* message, const QoreListNode* msg_args, const QoreHashNode* location_info, int tid,
            DateTimeNode* now, const QoreHashNode* throwable)
            : fqcn(fqcn), categoryName(categoryName->stringRefSelf()), level(level->objectRefSelf()),
            messageFmt(message->stringRefSelf()), messageArgs(msg_args ? msg_args->listRefSelf() : nullptr),
            threadId(tid), timeStamp(now), locationInfo(location_info ? location_info->hashRefSelf() : nullptr),
            throwableInfo(throwable ? throwable->hashRefSelf() : nullptr) {
        assert(timeStamp);
    }

    //! Returns the full qualified classname
    QoreStringNode* getFullQualifiedClassname() const;

    //! Returns the location information for this logging event
    QoreHashNode* getLocationInfo() const;

    //! Returns the level of this event
    QoreObject* getLevel() const;

    //! Returns the logger which created the event
    QoreObject* getLogger() const;

    //! Returns the thread id which is related to event
    int getThreadId() const;

    //! Returns the category name
    QoreStringNode* getCategoryName() const;

    using AbstractPrivateData::deref;
    DLLLOCAL virtual void deref(ExceptionSink* xsink) {
        if (ROdereference()) {
            if (logger) {
                logger->deref(xsink);
            }
            if (level) {
                level->deref(xsink);
            }
            if (messageArgs) {
                messageArgs->deref(xsink);
            }
            if (locationInfo) {
                locationInfo->deref(xsink);
            }
            if (throwableInfo) {
                throwableInfo->deref(xsink);
            }
            delete this;
        }
    }

    //! Returns the event timestamp as an @ref absolute_dates "absolute date/time value"
    DLLLOCAL DateTimeNode* getTimeStamp() const;

    //! Returns a relative timestamp for the event
    /**
        Returns a @ref relative_dates "relative date/time value" for the amount of time passed
        from the beginning of execution to the time when the event was constructed.

        @see @ref getStartTime()
    */
    DLLLOCAL DateTimeNode* getRelativeTime() const;

    //! Returns throwable info, if any
    DLLLOCAL QoreHashNode* getThrowableInfo() const;

    //! Generates a globally unique integer identifier and associates it to the event
    DLLLOCAL int getUniqueId() const;

    //! Returns the string message for the logging event
    DLLLOCAL QoreStringNode* getMessage(ExceptionSink* xsink) const;

    //! Returns the time when the application/logger started to calculate relative time
    DLLLOCAL static DateTimeNode* getStartTime();

    //! Sets the starting time for relative time
    DLLLOCAL static void setStartTime(const DateTimeNode* time);

    DLLLOCAL static void init();

protected:
    //! Fully Qualified Class Name of the calling category class.
    SimpleRefHolder<QoreStringNode> fqcn;

    //! Logger reference
    QoreObject* logger = nullptr;

    //! The category (logger) name.
    SimpleRefHolder<QoreStringNode> categoryName;

    //! Level of the logging event.
    QoreObject* level = nullptr;

    //! The application supplied message of logging event (not rendered)
    SimpleRefHolder<QoreStringNode> messageFmt;

    //! arguments to be rendered
    QoreListNode* messageArgs = nullptr;

    //! The application supplied message rendered through the rendering mechanism.
    mutable SimpleRefHolder<QoreStringNode> renderedMessage;

    //! related thread id
    int threadId;

    //! event time stamp
    SimpleRefHolder<DateTimeNode> timeStamp;

    //! location information where the logging was performed.
    QoreHashNode* locationInfo = nullptr;

    //! internal representation of throwable
    QoreHashNode* throwableInfo = nullptr;

    //! unique id
    int uniqueId = sequence.next();

    //! Atomicity lock for the rendered message
    mutable QoreThreadLock m0;

    //! unique id generator
    static Sequence sequence;

    //! origin stamp to calculate relative time
    static SimpleRefHolder<DateTimeNode> startTime;

    //! Global atomicity lock
    static QoreThreadLock m;

    //! Render the log message
    DLLLOCAL void renderMessageLocked(ExceptionSink* xsink) const;
};

#endif
