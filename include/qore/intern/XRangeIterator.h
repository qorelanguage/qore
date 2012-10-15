/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  XRangeIterator.h

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

#ifndef _QORE_XRANGEITERATOR_H

#define _QORE_XRANGEEITERATOR_H

// the c++ object. See QC_XRangeIterator.qpp for docs.
class XRangeIterator : public QoreIteratorBase {

protected:
   int64 m_start;
   int64 m_stop;
   int64 m_step;

   int64 m_position;

   bool m_increasing;
   bool m_valid;

   ExceptionSink *m_xsink;

public:
   DLLLOCAL XRangeIterator(int64 start, int64 stop, int64 step, ExceptionSink *xsink)
   	   : QoreIteratorBase(),
   	     m_start(start),
   	     m_stop(stop),
   	     m_step(step),
   	     m_position(-1),
   	     m_increasing(start<stop),
   	     m_valid(false),
   	     m_xsink(xsink)
   {
	   if (step < 1) {
		   m_xsink->raiseException("XRANGEITERATOR-ERROR", "Value of the 'step' argument has to be greater than 0; currently=%d", step);
	   }
   }

   DLLLOCAL bool next() {
       m_position++;
       bool ret;

       if (m_increasing) {
           ret = calculateCurrent() <= m_stop;
       }
       else {
           ret = calculateCurrent() >= m_stop;
       }

       if (ret)
           m_valid = true;

       return ret;
   }

   DLLLOCAL AbstractQoreNode* getValue() {
       if (!m_valid) {
           m_xsink->raiseException("INVALID-ITERATOR", "XRangeIterator is invalid. Position=%d", m_position);
           return 0;
       }
       return new QoreBigIntNode(calculateCurrent());
   }

   DLLLOCAL void reset() {
	   m_position = -1;
	   m_valid = false;
   }

   DLLLOCAL virtual const char* getName() const { return "XRangeIterator"; }

private:

   int calculateCurrent() {
	   if (m_increasing) {
		   return m_start + (m_position * m_step);
	   }
	   else {
		   return m_start - (m_position * m_step);
	   }
   }

};

#endif // _QORE_XRANGEITERATOR_H
