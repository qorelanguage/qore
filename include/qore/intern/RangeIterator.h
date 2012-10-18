/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  RangeIterator.h

  Qore Programming Language

  Copyright 2003 - 2012 Qore Technologies s r.o.

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

#ifndef _QORE_RANGEITERATOR_H

#define _QORE_RANGEEITERATOR_H

// the c++ object. See QC_RangeIterator.qpp for docs.
class RangeIterator : public QoreIteratorBase {
private:
   int64 m_start;
   int64 m_stop;
   int64 m_step;

   int64 m_position;

   bool m_increasing;
   bool m_valid;

public:
   DLLLOCAL RangeIterator(int64 start, int64 stop, int64 step, ExceptionSink* xsink)
      : QoreIteratorBase(),
        m_start(start),
        m_stop(stop),
        m_step(step),
        m_position(-1),
        m_increasing(start<stop),
        m_valid(false) {
      if (step < 1) {
         xsink->raiseException("RANGEITERATOR-ERROR", "Value of the 'step' argument has to be greater than 0 (value passed: %d)", step);
      }
   }

   DLLLOCAL RangeIterator(const RangeIterator& old)
      : m_start(old.m_start), m_stop(old.m_stop), m_step(old.m_step),
        m_position(old.m_position), m_increasing(old.m_increasing),
        m_valid(old.m_valid) {
   }

   DLLLOCAL bool next() {
      ++m_position;
      m_valid = m_increasing ? (calculateCurrent() <= m_stop) : (calculateCurrent() >= m_stop);
      if (!m_valid)
         m_position = -1;
      return m_valid;
   }

   DLLLOCAL AbstractQoreNode* getValue(ExceptionSink *xsink) {
      if (!m_valid) {
         xsink->raiseException("INVALID-ITERATOR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return 0;
      }
      return new QoreBigIntNode(calculateCurrent());
   }

   DLLLOCAL void reset() {
      m_position = -1;
      m_valid = false;
   }

   DLLLOCAL virtual const char* getName() const { return "RangeIterator"; }

private:
   DLLLOCAL int64 calculateCurrent() {
      if (m_increasing) {
         return m_start + (m_position * m_step);
      }
      else {
         return m_start - (m_position * m_step);
      }
   }
};

#endif // _QORE_RANGEITERATOR_H
