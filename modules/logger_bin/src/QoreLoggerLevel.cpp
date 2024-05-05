/* -*- mode: c++; indent-tabs-mode: nil -*- */
/** @file QoreLoggerLevel.cpp LoggerLevel class definition */
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

QoreObject* QoreLoggerLevel::LevelAll;
QoreObject* QoreLoggerLevel::LevelTrace;
QoreObject* QoreLoggerLevel::LevelDebug;
QoreObject* QoreLoggerLevel::LevelDetail;
QoreObject* QoreLoggerLevel::LevelInfo;
QoreObject* QoreLoggerLevel::LevelWarn;
QoreObject* QoreLoggerLevel::LevelError;
QoreObject* QoreLoggerLevel::LevelFatal;
QoreObject* QoreLoggerLevel::LevelOff;

QoreLoggerLevel::lsmap_t QoreLoggerLevel::lsmap;
QoreLoggerLevel::limap_t QoreLoggerLevel::limap;

//! Gets level code value
int64 QoreLoggerLevel::getValue() const {
    return levelCode;
}

//! Gets level string
QoreStringNode* QoreLoggerLevel::getStr() const {
    return levelStr->stringRefSelf();
}

//! Compares logger levels
bool QoreLoggerLevel::isGreaterOrEqual(const QoreLoggerLevel* other) const {
    return levelCode >= other->levelCode;
}

//! Compares two logger levels
bool QoreLoggerLevel::isEqual(const QoreLoggerLevel* other) const {
    return levelCode == other->levelCode;
}

//! Converts the input argument to a level
/**
    Return value is either instantiated or used the existing one if there is any.

    @param level the input level
    @param default_level value to return if conversion is not possible.

    @throw LOGGER-ERROR if defaultLevel is nothing and level not found
*/
QoreObject* QoreLoggerLevel::getLevel(ExceptionSink* xsink, int64 level, const QoreObject* default_level) {
    limap_t::const_iterator i = limap.find(level);
    if (i != limap.end()) {
        return i->second->objectRefSelf();
    }
    if (default_level) {
        return default_level->objectRefSelf();
    }
    xsink->raiseException("LOGGER-ERROR", new QoreStringNodeMaker("Cannot get valid level for integer level code "
        "%lld", level));
    return nullptr;
}

//! Converts the input argument to a level.
/**
    Return value is either instantiated or used the existing one if there is any.

    @param level_str the string input level
    @param default_level value to return if conversion is not possible.
    @throw LOGGER-ERROR if default_level is nothing and level_str not found
*/
QoreObject* QoreLoggerLevel::getLevel(ExceptionSink* xsink, const QoreStringNode* level_str,
        const QoreObject* default_level) {
    TempEncodingHelper l(level_str, QCS_DEFAULT, xsink);
    if (*xsink) {
        return nullptr;
    }
    lsmap_t::const_iterator i = lsmap.find(l->c_str());
    if (i != lsmap.end()) {
        return i->second->objectRefSelf();
    }
    if (default_level) {
        return default_level->objectRefSelf();
    }
    xsink->raiseException("LOGGER-ERROR", new QoreStringNodeMaker("Cannot get valid level for level \"%s\"",
        l->c_str()));
    return nullptr;
}

//! Returns closest lower logger level
/**
    @param level the logger level
    @return \c Loggerlevel or \c NOTHING if there is no lower level

    @throw LOGGER-ERROR if level not found
*/
QoreObject* QoreLoggerLevel::getNextLowerLevel(int64 level) {
    limap_t::const_iterator i = limap.find(level);
    if (i != limap.begin()) {
        --i;
        return i->second->objectRefSelf();
    }
    return nullptr;
}

//! Returns closest higher logger level
/**
    @param level the logger level
    @return \c Loggerlevel or \c NOTHING if there is no higher level

    @throw LOGGER-ERROR if level not found
*/
QoreObject* QoreLoggerLevel::getNextHigherLevel(int64 level) {
    limap_t::const_iterator i = limap.find(level);
    if (i != limap.end()) {
        ++i;
        if (i != limap.end()) {
            return i->second->objectRefSelf();
        }
    }
    return nullptr;
}

//! Returns an OFF Level
QoreObject* QoreLoggerLevel::getLevelOff() {
    return LevelOff->objectRefSelf();
}

//! Returns a FATAL Level
QoreObject* QoreLoggerLevel::getLevelFatal() {
    return LevelFatal->objectRefSelf();
}

//! Returns an ERROR Level
QoreObject* QoreLoggerLevel::getLevelError() {
    return LevelError->objectRefSelf();
}

//! Returns a WARN Level
QoreObject* QoreLoggerLevel::getLevelWarn() {
    return LevelWarn->objectRefSelf();
}

//! Returns an INFO Level
QoreObject* QoreLoggerLevel::getLevelInfo() {
    return LevelInfo->objectRefSelf();
}

//! Returns a DETAIL Level
QoreObject* QoreLoggerLevel::getLevelDetail() {
    return LevelDetail->objectRefSelf();
}

//! Returns a DEBUG Level
QoreObject* QoreLoggerLevel::getLevelDebug() {
    return LevelDebug->objectRefSelf();
}

//! Returns a TRACE Level
QoreObject* QoreLoggerLevel::getLevelTrace() {
    return LevelTrace->objectRefSelf();
}

//! Returns an ALL Level
QoreObject* QoreLoggerLevel::getLevelAll() {
    return LevelAll->objectRefSelf();
}

void QoreLoggerLevel::init() {
    LevelAll = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_ALL, new QoreStringNode("ALL")));
    LevelTrace = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_TRACE, new QoreStringNode("TRACE")));
    LevelDebug = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_DEBUG, new QoreStringNode("DEBUG")));
    LevelDetail = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_DETAIL, new QoreStringNode("DETAIL")));
    LevelInfo = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_INFO, new QoreStringNode("INFO")));
    LevelWarn = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_WARN, new QoreStringNode("WARN")));
    LevelError = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_ERROR, new QoreStringNode("ERROR")));
    LevelFatal = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_FATAL, new QoreStringNode("FATAL")));
    LevelOff = new QoreObject(QC_LOGGERLEVEL, getProgram(),
        new QoreLoggerLevel(QLL_OFF, new QoreStringNode("OFF")));

    lsmap.insert(lsmap_t::value_type("ALL", LevelAll));
    lsmap.insert(lsmap_t::value_type("TRACE", LevelTrace));
    lsmap.insert(lsmap_t::value_type("DEBUG", LevelDebug));
    lsmap.insert(lsmap_t::value_type("DETAIL", LevelDetail));
    lsmap.insert(lsmap_t::value_type("INFO", LevelInfo));
    lsmap.insert(lsmap_t::value_type("WARN", LevelWarn));
    lsmap.insert(lsmap_t::value_type("ERROR", LevelError));
    lsmap.insert(lsmap_t::value_type("FATAL", LevelFatal));
    lsmap.insert(lsmap_t::value_type("OFF", LevelOff));

    limap.insert(limap_t::value_type(QLL_ALL, LevelAll));
    limap.insert(limap_t::value_type(QLL_TRACE, LevelTrace));
    limap.insert(limap_t::value_type(QLL_DEBUG, LevelDebug));
    limap.insert(limap_t::value_type(QLL_DETAIL, LevelDetail));
    limap.insert(limap_t::value_type(QLL_INFO, LevelInfo));
    limap.insert(limap_t::value_type(QLL_WARN, LevelWarn));
    limap.insert(limap_t::value_type(QLL_ERROR, LevelError));
    limap.insert(limap_t::value_type(QLL_FATAL, LevelFatal));
    limap.insert(limap_t::value_type(QLL_OFF, LevelOff));
}

void QoreLoggerLevel::del() {
    for (auto& i : lsmap) {
        i.second->deref(nullptr);
    }
    lsmap.clear();
    limap.clear();
}