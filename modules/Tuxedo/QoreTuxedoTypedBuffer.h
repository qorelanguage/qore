#ifndef QORE_TUXEDO_TYPED_BUFFER_IMPL_H_
#define QORE_TUXEDO_TYPED_BUFFER_IMPL_H_

/*
  modules/Tuxedo/QoreTuxedoTypedBuffer.h

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 QoreTechnologies

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Object.h>

class ExceptionSink;
class BinaryObject;
class QoreEncoding;

//------------------------------------------------------------------------------
class QoreTuxedoTypedBuffer : public ReferenceObject
{
public:
  char* buffer;
  long size;
  QoreEncoding* string_encoding;

  QoreTuxedoTypedBuffer();
  ~QoreTuxedoTypedBuffer(); 

  void clear();
  void setStringEncoding(QoreEncoding *enc);
  void setBinary(BinaryObject* bin, char* type, char* subtype, ExceptionSink* xsink);
  void setString(char* str, char* type, char* subtype, ExceptionSink* xsink);
  // TBD - add FML -> FML32 conversion, xml <-> fml[32]

  void deref() { 
    if (ROdereference()) {
      delete this;
    }
  }
};

#endif

// EOF

