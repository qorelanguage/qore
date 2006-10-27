/*
  modules/Tuxedo/QoreTuxedoTypedBuffer.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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
#include <qore/Exception.h>
#include <qore/charset.h>
#include <qore/minitest.hpp>

#include <atmi.h>

#include "QoreTuxedoTypedBuffer.h"

//------------------------------------------------------------------------------
QoreTuxedoTypedBuffer::QoreTuxedoTypedBuffer()
: buffer(0), size(0), string_encoding(QCS_DEFAULT)
{
}

//-----------------------------------------------------------------------------
QoreTuxedoTypedBuffer::~QoreTuxedoTypedBuffer()
{
  clear();
}

//-----------------------------------------------------------------------------
void QoreTuxedoTypedBuffer::clear()
{
  if (buffer) {
    tpfree(buffer);
    buffer = 0;
  }
  if (size) size = 0;
}

//-----------------------------------------------------------------------------
void QoreTuxedoTypedBuffer::setStringEncoding(QoreEncoding *enc)
{
  string_encoding = enc;
}

//-----------------------------------------------------------------------------
void QoreTuxedoTypedBuffer::setBinary(BinaryObject* bin, char* type, char* subtype, ExceptionSink* xsink)
{
  clear();
  
  void* dt = bin->getPtr();
  unsigned sz = bin->size();
  if (!sz) {
    return;
  }
  buffer = tpalloc(type, subtype, sz);
  if (!buffer) {
    xsink->raiseException("TuxedoTypedBuffer::setBinary()", "tpalloc() failed with error code %d.", tperrno);
    return;
  }

  size = sz;
  memcpy(buffer, dt, sz);
}

//-----------------------------------------------------------------------------
void QoreTuxedoTypedBuffer::setString(char* str, char* type, char* subtype, ExceptionSink* xsink)
{
  if (!str) str = "";
  int sz = strlen(str) + 1;
  buffer = tpalloc(type, subtype, sz);
  if (!buffer) {
    xsink->raiseException("TuxedoTypedBuffer::setString()", "tpalloc() failed with error code %d.", tperrno);
    return;
  }

  strcpy(buffer, str);
  size = sz;
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // test instantiantiaon
  QoreTuxedoTypedBuffer buff;
}

TEST()
{
  // test string
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;
  
  buff.setString("abc", "STRING", 0, &xsink);
  assert(!xsink.isException());
  buff.clear();  
  buff.setString(0, "STRING", 0, &xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test string encoding
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;
  
  buff.setStringEncoding(QCS_ISO_8859_10);

  char s[] = "abc";
  s[1] = 0xFF;

  buff.setString(s, "STRING", 0, &xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test binary
  QoreTuxedoTypedBuffer buff;
  ExceptionSink xsink;

  std::auto_ptr<BinaryObject> bin(new BinaryObject(strdup("abcd"), 5));
  buff.setBinary(bin.get(), "CARRAY", 0, &xsink);
  assert(!xsink.isException());
}
#endif

// EOF

