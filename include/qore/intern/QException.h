/*
  QException.h

  Qore Programming language

  Copyright (C) 2007 Qore Technologies

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef QORE_EXCEPTION_H_
#define QORE_EXCEPTION_H_

#include <qore/support.h>
#include <string>


class QException
{
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

// EOF

