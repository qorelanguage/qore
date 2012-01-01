/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_TimeZone.h

  Qore Programming Language

  Copyright (C) 2006 - 2011 QoreTechnologies
  
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
