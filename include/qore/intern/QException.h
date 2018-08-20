/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QException.h

  Qore Programming language

  Copyright (C) 2007 - 2015 Qore Technologies

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

#ifndef QORE_EXCEPTION_H_
#define QORE_EXCEPTION_H_

#include <qore/support.h>
#include <string>

class QException {
private:
   class QExceptionImpl;
   QExceptionImpl* pImpl;

public:
   DLLEXPORT QException(
    const char* exception_type, // literal like "DBI-EXCEPTION-SOMETHING"
    const char* problem, 
    const char* cause = 0,
    const char* solution = 0,
    const char* details = 0   // low level details if needed
    );

  DLLEXPORT ~QException();
  DLLEXPORT QException(const QException&);
  DLLEXPORT QException& operator=(const QException&);

  DLLEXPORT void addDetails(const char* details);

  DLLEXPORT const char* getType() const;
  DLLEXPORT const char* getProblem() const;
  DLLEXPORT const char* getCause() const;
  DLLEXPORT const char* getSolution() const;
  DLLEXPORT std::string getCompleteDescription() const; // combines problem + cause + solution
  DLLEXPORT std::string getAllDetails() const; // all details (if any) joined together
};

#endif
