//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2016 Qore Technologies, s.r.o.
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief Defines the Exception class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_INTERN_CORE_EXCEPTION_H_
#define INCLUDE_QORE_INTERN_CORE_EXCEPTION_H_

#include <cassert>
#include <exception>
#include <new>
#include <string>
#include "qore/common.h"

#define CATCH(XSINK, RET)                                                       \
   catch (const qore::Exception &e) {                                           \
      XSINK->raiseException(e.getErr().c_str(), e.getDescription().c_str());    \
      RET;                                                                      \
   } catch (qore::ExceptionWrapper &e) {                                        \
      XSINK->raiseException(e.get());                                           \
      RET;                                                                      \
   } catch (const std::bad_alloc &e) {                                          \
      XSINK->outOfMemory();                                                     \
      RET;                                                                      \
   } catch (const std::exception &e) {                                          \
      XSINK->raiseException("UNEXPECTED-ERROR", e.what());                      \
      RET;                                                                      \
   } catch (...) {                                                              \
      XSINK->raiseException("UNEXPECTED-ERROR", "Unknown error");               \
      RET;                                                                      \
   }

class QoreException;

namespace qore {

/**
 * \brief Basis C++ exception class used by Qore.
 */
class Exception : public virtual std::exception {

public:
   /**
    * \brief Constructor.
    * \param err the error code
    * \param desc the description
    */
   DLLLOCAL Exception(std::string err, std::string desc = "") : err(std::move(err)), description(std::move(desc)) {
   }

   DLLLOCAL Exception(Exception &&) noexcept = default;
   DLLLOCAL Exception &operator=(Exception &&) noexcept = default;

   DLLLOCAL const char* what() const noexcept override {
      return description.c_str();
   }

   DLLLOCAL const std::string &getErr() const {
      return err;
   }

   DLLLOCAL const std::string &getDescription() const {
      return description;
   }

   DLLLOCAL void setDescription(std::string desc) {
      description = std::move(desc);
   }

private:
   Exception(const Exception &) = delete;
   Exception &operator=(const Exception &) = delete;

private:
   std::string err;
   std::string description;
};

class ExceptionWrapper : public virtual std::exception {

public:
   ExceptionWrapper(QoreException *e) : e(e) {
   }

   ~ExceptionWrapper() {
      assert(!e);
   }

   QoreException *get() {
      assert(e);
      QoreException *ee = e;
      e = nullptr;
      return ee;
   }

private:
   QoreException *e;
};

} // namespace qore

#endif // INCLUDE_QORE_INTERN_CORE_EXCEPTION_H_
