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

#include <qore/Qore.h>
#include <qore/minitest.hpp>
#include <qore/intern/QException.h>

#include <assert.h>
#include <string.h>
#include <vector>

//------------------------------------------------------------------------------
// pImpl for the main visible class
class QException::QExceptionImpl
{
  const char* m_type;
  std::string m_problem;
  std::string m_cause;
  std::string m_solution;
  std::vector<std::string> m_details;
  int m_counter;
public:
  //----------------------------------------------------------------------------
  QExceptionImpl(const char* exception_type, const char* problem,
    const char* cause, const char* solution, const char* details)
  : m_type(exception_type ? exception_type : "<ERROR - no exception type (like MODULE-FUNCTION-INCIDENT) specified>"),    
    m_counter(1)
  {
    if (!problem || !problem[0]) {
      assert(false);
      m_problem = "<ERROR - 'problem' field not specified>"; 
    } else {
      m_problem = problem;
    }
    if (cause && cause[0]) {
      m_cause = cause;
    }
    if (solution && solution[0]) {
      m_solution = solution;
    }
  }
  //----------------------------------------------------------------------------
  void add_reference() {
    assert(m_counter > 0);
    ++m_counter;
  }
  void release() {
    assert(m_counter > 0);
    if (m_counter <= 1) {
      delete this; 
    }
  }
  //----------------------------------------------------------------------------
  void add_details(const char* details) {
    if (details && details[0]) {
      m_details.push_back(std::string(details));
    }
  }
  //----------------------------------------------------------------------------
  const char* get_type() const { return m_type; }
  const char* get_problem() const { return m_problem.c_str(); }
  const char* get_cause() const { return m_cause.c_str(); }
  const char* get_solution() const { return m_solution.c_str(); }
  const std::vector<std::string>& get_details() const { return m_details; }
};

//------------------------------------------------------------------------------
QException::QException(const char* exception_type, const char* problem, 
  const char* cause, const char* solution, const char* details)
: pImpl(new QExceptionImpl(exception_type, problem, cause, solution, details))
{
}

//------------------------------------------------------------------------------
QException::~QException()
{
  pImpl->release();
}

//------------------------------------------------------------------------------
QException::QException(const QException& other)
: pImpl(other.pImpl)
{
  pImpl->add_reference();
}

//------------------------------------------------------------------------------
QException& QException::operator=(const QException& other)
{
  if (pImpl != other.pImpl) {
    pImpl->release();
    pImpl = other.pImpl;
    pImpl->add_reference();
  }
  return *this;
}

//------------------------------------------------------------------------------
void QException::addDetails(const char* details)
{
  pImpl->add_details(details);
}

//------------------------------------------------------------------------------
const char* QException:getType() const
{
  return pImpl->get_type();
}

//------------------------------------------------------------------------------
const char* QException::getProblem() const
{
  return pImpl->get_problem();
}

//------------------------------------------------------------------------------
const char* pImpl->getCause() const
{
  return pImpl->get_cause();
}

//------------------------------------------------------------------------------
const char* pImpl::getSolution() const
{
  return pImpl->get_solution();
}

//------------------------------------------------------------------------------
std::string getCompleteDescription() const
{
  const char* problem = pImpl->get_problem();
  const char* cause = pImpl->get_cause();
  const char* solution = pImpl->get_solution();
  std::string result("PROBLEM: ");
  result += problem;
  if (*result.rend() != '\n') {
    result += "\n";
  }
  if (cause && cause[0]) {
    result += "CAUSE: ";
    result += cause;
    if (*result.rend() != '\n') {
      result += "\n";
    }
  }
  if (solution && solution[0]) {
    result += "SOLUTION: ";
    result += solution;
    if (*result.rend() != '\n') {
      result += "\n";
    }
  }
  return result;
}

//------------------------------------------------------------------------------
std::string QException::getAllDetails() const
{
  const std::vector<std::string>& details = pImpl->get_details();
  std::string result;
  result.reserve(details.size() * 500); // guess
  for (int i = details.size() - 1; i >= 0; --i) {
    const char* s = details[i].c_str();
    char aux[10];
    sprintf(aux, "%d. ", details.size() - i);
    result += (const char*)aux;
    result += s;
    if (*result.rend() != '\n') { 
      result += "\n";
    }
  }
  return result;
}

#ifdef DEBUG_TESTS
#  include "tests/QException_tests.cc"
#endif

// EOF

