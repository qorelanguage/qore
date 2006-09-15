
$funcs = 
    ( 
      "get_days" :
      ( "desc" : "Returns an integer value representing the days value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),
      "get_hours" :
      ( "desc" : "Returns an integer value representing the hours value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),
      "get_midnight" :
      ( "desc" : "Returns a date/time value representing midnight on the date passed (strips the time from the date passed and returns the new value)",
	"ret" : "Date",
	"args" : "date" ),

      "get_milliseconds" :
      ( "desc" : "Returns an integer value representing the milliseconds value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),
      "get_minutes" :
      ( "desc" : "Returns an integer value representing the minutes value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),
      "get_months" :
      ( "desc" : "Returns an integer value representing the months value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),
      "get_seconds" :
      ( "desc" : "Returns an integer value representing the seconds value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),
      "get_years" :
      ( "desc" : "Returns an integer value representing the years value of the date passed (can be either a relative or absolute date).",
	"ret" : "Integer",
	"args" : "date" ),

      "getDateFromISOWeek" :
      ( "desc" : "Retuns an absolute date value for the ISO-8601 calendar week information passed (year, week number, optional: day)",
	"long" : "Retuns an absolute date value for the ISO-8601 calendar week information passed (year, week number, optional: weekday, where 1=Monday, 7=Sunday).  If the weekday is omitted, Monday (1) is assumed.",
	"ret" : "Date",
	"args" : ("integer", "integer", "optional:integer" ) ),

      "getDayOfWeek" :
      ( "desc" : "Returns an integer representing the day of the week for the absolute date passed (0=Sunday, 6=Saturday)",
	"ret" : "Integer",
	"args" : "date" ),

      "getDayNumber" :
      ( "desc" : "Returns an integer representing the ordinal day number in the year for the absolute date passed",
	"ret" : "Integer",
	"args" : "date" ),

      "getISODayOfWeek" :
      ( "desc" : "Returns an integer representing the ISO-8601 day of the week for the absolute date passed (1=Monday, 7=Sunday)",
	"ret" : "Integer",
	"args" : "date" ),

      "getISOWeekHash" :
      ( "desc" : "Returns a hash representing the ISO-8601 calendar week information for the absolute date passed (hash keys: year, week, day)",
	"long" : "Returns a hash representing the ISO-8601 calendar week information for the absolute date passed (hash keys: year, week, day).  Note that the ISO-8601 year does not always correspond with the calendar year at the end and the beginning of every year (for example 2006-01-01 is ISO-8601 calendar week format is: year=2005, week=52, day=7)",	
	"ret" : "Hash",
	"args" : "date" ),

      "getISOWeekString" :
      ( "desc" : "Returns a string representing the ISO-8601 calendar week information for the absolute date passed (ex: 2006-01-01 = \"2005-W52-7\")",
	"long" : "Returns a string representing the ISO-8601 calendar week information for the absolute date passed (ex: 2006-01-01 = \"2005-W52-7\").  Note that the ISO-8601 year does not always correspond with the calendar year at the end and the beginning of every year (for example 2006-01-01 is ISO-8601 calendar week format is: year=2005, week=52, day=7)",
	"ret" : "String",
	"args" : "date" ),

      );

return $funcs;
