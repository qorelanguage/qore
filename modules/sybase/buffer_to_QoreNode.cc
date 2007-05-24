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

#include <qore/Qore.h>
#include <qore/minitest.hpp>

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
     return null();
  }
  //printd(5, "buffer_to_QoreNode() encoding=%s name=%s type=%d format=%d usertype=%d value_len=%d\n", encoding->getCode(), datafmt.name, datafmt.datatype, datafmt.format, datafmt.usertype, buffer.value_len);

  switch (datafmt.datatype) {
     case CS_CHAR_TYPE: // varchar
     {
	CS_CHAR* value = (CS_CHAR*)(buffer.value);
	QoreString *s;
	// sometimes the strings are not null terminated, both with sybase and freetds
	if (!memchr(value, '\0', buffer.value_len))
	{
	   s = new QoreString(value, buffer.value_len - 1, encoding);
	   printd(5, "buffer_to_QoreNode() oops, no null in %s='%s'\n", datafmt.name, s->getBuffer());
	}
	else
	   s = new QoreString(value, encoding);
	s->trim_trailing_blanks();
	//printd(5, "name=%s vlen=%d strlen=%d len=%d str='%s'\n", datafmt.name, buffer.value_len, s->strlen(), s->length(), s->getBuffer());
	return new QoreNode(s);
     }

     case CS_LONGCHAR_TYPE:
     case CS_VARCHAR_TYPE:
     case CS_TEXT_TYPE:
     {
	CS_CHAR* value = (CS_CHAR*)(buffer.value);

	QoreString *s;
	// see if we need to strip trailing newlines (could not find a defined USER_TYPE_* for this!)
	if (datafmt.usertype == 1)
	{
#ifdef FREETDS_x
	   // sometimes freetds values are not coming with null termination for some reason
	   // see if there is a null value
	   if (!memchr(value, '\0', buffer.value_len))
	   {
	      s = new QoreString(value, buffer.value_len - 1, encoding);
	      printd(5, "buffer_to_QoreNode() no null in %s='%s'\n", datafmt.name, s->getBuffer());
	   }
	   else
	      s = new QoreString(value, encoding);
#else
	   s = new QoreString(value, encoding);
#endif
	   s->trim_trailing_blanks();
	}
	else
	   s = new QoreString(value, buffer.value_len - 1, encoding);
	printd(5, "name=%s type=%d strlen=%d vallen=%d len=%d str='%s'\n", datafmt.name, datafmt.datatype, buffer.value_len, s->strlen(), s->length(), s->getBuffer());
	return new QoreNode(s);
     }

     case CS_VARBINARY_TYPE:
     case CS_BINARY_TYPE:
     case CS_LONGBINARY_TYPE:
     case CS_IMAGE_TYPE:
     {
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
      CS_TINYINT* value = (CS_TINYINT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_SMALLINT_TYPE:
    {
      CS_SMALLINT* value = (CS_SMALLINT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_INT_TYPE:
    {
      CS_INT* value = (CS_INT*)(buffer.value);
      return new QoreNode((int64)*value);
    }

    case CS_REAL_TYPE:
    {
      CS_REAL* value = (CS_REAL*)(buffer.value);
      return new QoreNode((double)*value);
    }

    case CS_FLOAT_TYPE:
    {
      CS_FLOAT* value = (CS_FLOAT*)(buffer.value);
      return new QoreNode((double)*value);
    }

    case CS_BIT_TYPE:
    {
      CS_BIT* value = (CS_BIT*)(buffer.value);
      return new QoreNode(*value != 0);
    }

    case CS_DATETIME_TYPE:
    {
      CS_DATETIME* value = (CS_DATETIME*)(buffer.value);

       // NOTE: can't find a USER_* define for 38!
       if (datafmt.usertype == 38)
	  return new QoreNode(TIME_to_DateTime(*value));

      return new QoreNode(DATETIME_to_DateTime(*value));
    }
    case CS_DATETIME4_TYPE:
    {
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
      CS_MONEY* value = (CS_MONEY*)(buffer.value);
      double d = MONEY_to_double(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    case CS_MONEY4_TYPE:
    {
      CS_MONEY4* value = (CS_MONEY4*)(buffer.value);
      double d = MONEY4_to_double(cmd.getConnection(), *value, xsink);
      if (xsink->isException()) {
        return 0;
      }
      return new QoreNode(d);
    }

    case CS_NUMERIC_TYPE:
    case CS_DECIMAL_TYPE:
    {
      CS_DECIMAL* value = (CS_DECIMAL*)(buffer.value);
      QoreString *str = DECIMAL_to_string(cmd.getConnection(), *value, xsink);
      return str ? new QoreNode(str) : 0;
    }

    default:
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Unknown data type %d", (int)datafmt.datatype);
      return 0;
  } // switch
}

// EOF


