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
/// \brief Defines the StringBuilder helper class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_INTERN_CORE_STRINGBUILDER_H_
#define INCLUDE_QORE_INTERN_CORE_STRINGBUILDER_H_

#include <sstream>
#include <string>
#include "qore/common.h"

namespace qore {

/**
 * \brief Helper class for formatting strings.
 * Allows inline creation of an std::string instance while using formatting constructs with the << operator.
 * \par Example:
 * \code
 * std::string s = StringBuilder() << "the value of i is " << i;
 * \endcode
 */
class StringBuilder {

public:
   /**
    * \brief Default constructor.
    */
   DLLLOCAL StringBuilder() = default;

   /**
    * \brief Default destructor.
    */
   DLLLOCAL ~StringBuilder() = default;

   /**
    * \brief Implicit conversion to std::string.
    */
   DLLLOCAL operator std::string() const {
      return stream.str();
   }

   /**
    * \brief Appends a value to the string.
    * \tparam T the type of the value
    * \param v the value to append
    * \return *this for chaining
    */
   template<typename T>
   DLLLOCAL StringBuilder &operator<<(const T &v) {
      stream << v;
      return *this;
   }

private:
   StringBuilder(const StringBuilder &) = delete;
   StringBuilder(StringBuilder &&) = delete;
   StringBuilder &operator=(const StringBuilder &) = delete;
   StringBuilder &operator=(StringBuilder &&) = delete;

private:
   std::ostringstream stream;
};

} // namespace qore

#endif // INCLUDE_QORE_INTERN_CORE_STRINGBUILDER_H_
