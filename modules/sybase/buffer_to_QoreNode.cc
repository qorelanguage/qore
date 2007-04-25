/*
  buffer_to_QoreNode.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

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
#include <qore/config.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/minitest.hpp>
#include <qore/QoreString.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/DateTime.h>
#include <qore/BinaryObject.h>

#include <assert.h>
#include <memory>
#include <stdlib.h>

#include "buffer_to_QoreNode.h"
#include "command.h"
#include "conversions.h"

//------------------------------------------------------------------------------
QoreNode* buffer_to_QoreNode(command& cmd, const CS_DATAFMT& datafmt, const output_value_buffer& buffer, QoreEncoding* encoding, ExceptionSink* xsink)
{
  if (buffer.indicator == -1) { // SQL NULL
    return new QoreNode(NT_NULL);
  }

  switch (datafmt.datatype) {
    case CS_LONGCHAR_TYPE:
    case CS_VARCHAR_TYPE:
    case CS_TEXT_TYPE:
    case CS_CHAR_TYPE: // varchar
    {
//printf("#### ct_fetch() read a STRING\n");
      CS_CHAR* value = (CS_CHAR*)(buffer.value);
      QoreString* s = new QoreString(value, buffer.value_len, encoding);
      return new QoreNode(s);
    }

    case CS_VARBINARY_TYPE:
    case CS_BINARY_TYPE:
    case CS_LONGBINARY_TYPE:
    case CS_IMAGE_TYPE:
    {
//printf("### ct_fetch() read a BINARY\n");
      CS_BINARY* value = (CS_BINARY*)(buffer.value);
      int size = buffer.value_len;
      void* block = malloc(size);
      if (!block) {
        xsink->outOfMemory();
        return 0;
      }
      memcpy(block, value, size);
      BinaryObject* bin = new BinaryObject(block, size);
      return new QoreNode(bin);
    }

    case CS_TINYINT_TYPE:
    {
//printf("### ct_fetch() read a TINYINT\n");
      CS_TINYINT* value = (CS_TINYINT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_SMALLINT_TYPE:
    {
//printf("### ct_fetch() read a SMALLINT\n");
      CS_SMALLINT* value = (CS_SMALLINT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_INT_TYPE:
    {
      CS_INT* value = (CS_INT*)(buffer.value);
//printf("### ct_fetch() read a INT value %d\n", (int)*value);
      return new QoreNode((int64)*value);
    }

    case CS_REAL_TYPE:
    {
//printf("### ct_fetch() read a REAL\n");
      CS_REAL* value = (CS_REAL*)(buffer.value);
      return new QoreNode((double)*value);
    }

    case CS_FLOAT_TYPE:
    {
//printf("### ct_fetch() read a FLOAT\n");
      CS_FLOAT* value = (CS_FLOAT*)(buffer.value);
      return new QoreNode((double)*value);
    }

    case CS_BIT_TYPE:
    {
//printf("### ct_fetch() read a BIT\n");
      CS_BIT* value = (CS_BIT*)(buffer.value);
      return new QoreNode(*value != 0);
    }

    case CS_DATETIME_TYPE:
    {
//printf("### ct_fetch() read a DATETIME\n");
      CS_DATETIME* value = (CS_DATETIME*)(buffer.value);
      DateTime* dt = DATETIME_to_DateTime(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        if (dt) delete dt;
        return 0;
      }
      return new QoreNode(dt);
    }
    case CS_DATETIME4_TYPE:
    {
//printf("### ct_fetch() read a DATETIME4\n");
      CS_DATETIME4* value = (CS_DATETIME4*)(buffer.value);
      DateTime* dt = DATETIME4_to_DateTime(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        if (dt) delete dt;
        return 0;
      }
      return new QoreNode(dt);
    }

    case CS_MONEY_TYPE:
    {
//printf("#### ct_fetch() read a MONEY\n");
      CS_MONEY* value = (CS_MONEY*)(buffer.value);
      double d = MONEY_to_double(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    case CS_MONEY4_TYPE:
    {
//printf("#### ct_fetch() read a MONEY4\n");
      CS_MONEY4* value = (CS_MONEY4*)(buffer.value);
      double d = MONEY4_to_double(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    case CS_NUMERIC_TYPE:
    {
//printf("### ct_fetch() read a NUMERIC\n");
      CS_NUMERIC* value = (CS_NUMERIC*)(buffer.value);
      double d = NUMERIC_to_double(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    case CS_DECIMAL_TYPE:
    {
//printf("### ct_fetch() read a DECIMAL\n");
      CS_DECIMAL* value = (CS_DECIMAL*)(buffer.value);
      double d = DECIMAL_to_double(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    default:
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Unknown data type %d", (int)datafmt.datatype);
      return 0;
  } // switch
}

// EOF


