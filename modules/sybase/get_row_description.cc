/*
  get_row_description.cc

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

#include <assert.h>
#include <ctpublic.h>

#include "get_row_description.h"
#include "command.h"
#include "common_constants.h"

//------------------------------------------------------------------------------
std::vector<CS_DATAFMT> get_row_description(command& cmd, unsigned columns_count, ExceptionSink* xsink)
{
  typedef std::vector<CS_DATAFMT> result_t;
  result_t result;

  for (unsigned i = 0; i < columns_count; ++i) {
     CS_DATAFMT datafmt;
     memset(&datafmt, 0, sizeof(datafmt));
     
     CS_RETCODE err = ct_describe(cmd(), i + 1, &datafmt);
     if (err != CS_SUCCEED) {
	xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_describe() failed with error %d", (int)err);
	return result_t();
     }
     datafmt.count = 1; // fetch just single row per every ct_fetch()

     //printd(5, "bind(): name=%s type=%d usertype=%d\n", datafmt.name, datafmt.datatype, datafmt.usertype);

     switch (datafmt.datatype) {
	// we map DECIMAL types are strings so we have no conversion to do
	case CS_DECIMAL_TYPE:
	case CS_NUMERIC_TYPE: 
	   datafmt.maxlength = 50;

	case CS_UNICHAR_TYPE:
	   datafmt.datatype = CS_CHAR_TYPE;

	case CS_LONGCHAR_TYPE:
	case CS_VARCHAR_TYPE:
	case CS_TEXT_TYPE:
	   datafmt.format = CS_FMT_NULLTERM;
	   break;

	   // freetds only works with CS_FMT_PADNULL with CS_CHAR columns it seems
	   // however this is also compatible with read sybase libs
	case CS_CHAR_TYPE:
	   datafmt.format = CS_FMT_PADNULL;
	   break;

#ifdef CS_BIGINT_TYPE
	case CS_BIGINT_TYPE:
	   break;
#endif

/*
	case CS_DECIMAL_TYPE:
	case CS_NUMERIC_TYPE: 
	   datafmt.precision = DefaultNumericPrecision;
	   datafmt.scale = DefaultNumericScale;
	   datafmt.format = CS_FMT_UNUSED;
	   break;
*/

#ifdef FREETDS
	   // FreeTDS seems to return DECIMAL types as FLOAT for some reason
	case CS_FLOAT_TYPE:
	   // can't find a defined USER_TYPE_* for 26
	   if (datafmt.usertype == 26)
	   {
	      datafmt.maxlength = 50;
	      datafmt.datatype = CS_CHAR_TYPE;
	      datafmt.format = CS_FMT_NULLTERM;
	      break;
	   }
#endif

	default:
	   datafmt.format = CS_FMT_UNUSED;
	   break;
     }

     result.push_back(datafmt); 
  }
  return result;
}

// EOF

