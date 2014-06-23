/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_TimeZone.h

  Qore Programming Language

  Copyright (C) 2006 - 2014 QoreTechnologies
  
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

#ifndef _QORE_CLASS_TIMEZONE_H
#define _QORE_CLASS_TIMEZONE_H

DLLEXPORT extern qore_classid_t CID_TIMEZONE;
DLLLOCAL extern QoreClass *QC_TIMEZONE;
DLLLOCAL QoreClass *initTimeZoneClass(QoreNamespace& ns);

#include <qore/AbstractPrivateData.h>

class TimeZoneData : public AbstractPrivateData {
protected:
   const AbstractQoreZoneInfo *zone;

public:
   DLLLOCAL TimeZoneData(const AbstractQoreZoneInfo *n_zone) : zone(n_zone) {
   }
   DLLLOCAL TimeZoneData(const TimeZoneData &z) : zone(z.zone) {
   }
   DLLLOCAL const AbstractQoreZoneInfo *operator->() const {
      return zone;
   }
   DLLLOCAL const AbstractQoreZoneInfo *operator*() const {
      return zone;
   }
   DLLLOCAL const AbstractQoreZoneInfo *get() const {
      return zone;
   }
};

#endif
