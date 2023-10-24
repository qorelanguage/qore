# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore CsvReadDataProvider class definition

/*  CsvReadDataProvider.qc Copyright 2012 - 2023 Qore Technologies, s.r.o.

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
*/

#! the CsvUtil namespace. All classes used in the CsvUtil module should be inside this namespace
public namespace CsvUtil {
#! Provides a data provider for reading CSV files
/** Supports generic record search operators and options
*/
public class CsvReadDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "CsvReadDataProvider",
            "supports_read": True,
            "has_record": True,
            "supports_search_expressions": True,
            "constructor_options": ConstructorOptions,
            "search_options": GenericRecordSearchOptions,
            "expressions": AbstractDataProvider::GenericExpressions,
        };

        #! Constructor options
        const ConstructorOptions = {
            "path": <DataProviderOptionInfo>{
                "display_name": "CSV Input Path",
                "short_desc": "The path to the input file",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The location of the CSV data; mutually exclusive with \"stream\"; resolved with a call to "
                    "[FileLocationHandler::getBinaryStreamFromLocation()](https://qoretechnologies.com/manual/qorus/"
                    "current/qore/modules/FileLocationHandler/html/"
                    "class_file_location_handler_1_1_file_location_handler.html#a29c2338efc901a6b8f24542505e2a094)",
            },
            "stream": <DataProviderOptionInfo>{
                "display_name": "Input Stream",
                "short_desc": "An input stream object for CSV data",
                "type": AbstractDataProviderType::get(new Type("InputStream")),
                "desc": "An input stream for CSV data; mutually exclusive with \"path\"",
            },
            "date_format": <DataProviderOptionInfo>{
                "display_name": "Date Format",
                "short_desc": "The default mask for date value input parsing",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The default date format for parsing `date` fields; see "
                    "[Date Mask Format](https://qoretechnologies.com/manual/qorus/current/qore/lang/html/"
                    "group__date__and__time__functions.html#date_mask) for more information on this option "
                    "(ex: `DD/MM/YYYY HH:mm:SS`)",
                "example_value": "DD/MM/YYYY HH:mm:SS",
            },
            "encoding": <DataProviderOptionInfo>{
                "display_name": "Input Character Encoding",
                "short_desc": "The Character encoding for the input file",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Character encoding for the input file (and output data)",
                "default_value": "UTF-8",
            },
            "eol": <DataProviderOptionInfo>{
                "display_name": "End Of Line Chars",
                "short_desc": "The end of line / record character(s)",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The end of line character(s); if not specified, then the end-of-line characters will be "
                    "auto detected",
            },
            "fields": <DataProviderOptionInfo>{
                "display_name": "Field Descriptions",
                "short_desc": "A hash describing the CSV fields on input",
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "The keys are field names as given by the `header_names` or `headers` options (in case "
                    "neither of these options are used, then field names are numbers starting with `0`) and the "
                    "values are "
                    "[field types](https://qoretechnologies.com/manual/qorus/current/qore/modules/CsvUtil/html/"
                    "class_csv_util_1_1_abstract_csv_iterator.html#abstractcsviterator_option_field_types) "
                    "or a hash with the following keys:\n"
                    "- `type`: a [field type string](https://qoretechnologies.com/manual/qorus/current/qore/modules/"
                        "CsvUtil/html/class_csv_util_1_1_abstract_csv_iterator.html"
                        "#abstractcsviterator_option_field_types)\n"
                    "- `format`: when ysed with type `date`, a [date format](https://qoretechnologies.com/manual/"
                        "qorus/current/qore/lang/html/group__date__and__time__functions.html#date_formatting), when "
                        "used with type `int`, `float`, or `number`, it is a number format string with 1 to 3 "
                        "fields as follows: *<thousands_separator>*[*<decimal_separator>*[*<decimals>*]]; ex: "
                        "`\".,3\"` (= `.` for the thousands separator, `,` for the decimal separator, and 3 digits "
                        "after the decimal place)\n"
                    "- `timezone`: used only with the `date` type; this value is passed to "
                        "[TimeZone::constructor()](https://qoretechnologies.com/manual/qorus/current/qore/lang/html/"
                        "class_qore_1_1_time_zone.html#ab77972801cee2c9d116a9631dfd631d9), "
                        "and the resulting time zone is used to parse the date (this value overrides any default "
                        "time zone for the object; use only in case date/time values from different time zones are "
                        "present in different columns of the same file)\n\n"
                    "Note that setting this value also sets `headers` if not set automatically with `header_names`; "
                    "if no field type is given, the default is `*string`; note as well that invalid field names "
                    "given in this option are ignored",
            },
            "header_lines": <DataProviderOptionInfo>{
                "display_name": "Number of Header Lines",
                "short_desc": "The number of headers lines in the file",
                "type": AbstractDataProviderType::get(SoftIntType),
                "desc": "The number of headers lines in the file",
            },
            "header_names": <DataProviderOptionInfo>{
                "display_name": "Automatically Detect Headers",
                "short_desc": "If true then headers will be automatically detected from the first header row",
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` then the object will parse the header names from the first header row, in this "
                    "case if `header_lines` is not set explicitly, it will be assumed to be `1`",
            },
            "header_reorder": <DataProviderOptionInfo>{
                "display_name": "Enforce Header Order",
                "short_desc": "If true, then the input data will be reordered in the order of `headers`",
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "If `true` (the default) then if `headers` are provided, then data fields are reordered to "
                    "follow headers",
                "default_value": True,
            },
            "headers": <DataProviderOptionInfo>{
                "display_name": "Header Names",
                "short_desc": "List of header / column names for the data iterated",
                "type": AbstractDataProviderType::get(new Type("softlist<string>")),
                "desc": "List of header / column names for the data iterated; if this is present, then "
                    "`header_names` must be `false`",
            },
            "ignore_empty": <DataProviderOptionInfo>{
                "display_name": "Ignore Empty Lines",
                "short_desc": "If true, empty lines will be ignored",
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "If `true`, empty lines will be ignored",
            },
            "ignore_whitespace": <DataProviderOptionInfo>{
                "display_name": "Ignore Leading and Trailing Whitespace",
                "short_desc": "If true, leading and trailing whitespace will be stripped from non-quoted fields",
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "If `true`, leading and trailing whitespace will be stripped from non-quoted fields",
            },
            "number_format": <DataProviderOptionInfo>{
                "display_name": "Number Format",
                "short_desc": "The default number format for parsing int, float, and number fields",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The default format for `int`, `float`, and `number` fields as a string giving the thousands "
                    "separator character followed by the decimal separator character (ex: `.,` for "
                    "continental-European-style numbers)",
                "example_value": ".,",
            },
            "quote": <DataProviderOptionInfo>{
                "display_name": "Quote Char(s)",
                "short_desc": "The quote character(s)",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The field quote character (default: `\"`)",
                "default_value": "\"",
            },
            "separator": <DataProviderOptionInfo>{
                "display_name": "Field Separator",
                "short_desc": "The field separator character(s)",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the string separating the fields in the file (default: `,`)",
                "default_value": ",",
            },
            "timezone": <DataProviderOptionInfo>{
                "display_name": "Timezone Region",
                "short_desc": "The timezone region to use when parsing dates",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The timezone region to use when parsing dates (ex: `Europe/Prague`); see "
                    "[TZ indentifier](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones) for valid values",
                "example_value": "Europe/Prague",
            },
            "tolwr": <DataProviderOptionInfo>{
                "display_name": "Convert Headers to Lower Case",
                "short_desc": "Convert all header names to lower case",
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "If `true` (default `false`), all header names will be converted to lower case letters",
                "default_value": False,
            },
            "verify_columns": <DataProviderOptionInfo>{
                "display_name": "Verify Columns",
                "short_desc": "Verifies input data to ensure it has the expected structure",
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` (default `false`), if a line is parsed with a different column or field count "
                    "than other lines, a `CSVFILEITERATOR-DATA-ERROR` exception is raised\n\n"
                    "Note that enabling this option will incurr a performance penalty",
                "default_value": False,
            },
        };

        #! CSV iteration option list
        const CsvIterationOptionList = keys (ConstructorOptions - ("path", "stream"));
    }

    private {
        #! the iterator object
        AbstractCsvIterator i;
    }

    #! Creates the CsvReadDataProvider with the input data path and optionally an option hash
    /** @param path the input file to iterate
        @param opts a hash of optional options; see @ref abstractcsviterator_options for more information

        @throw ABSTRACTCSVITERATOR-ERROR invalid or unknown option; invalid data type for option; \c "header_names"
        is @ref True "True" and \c "headers" is also present; unknown field type
    */
    constructor(string path, *hash<auto> opts) {
        i = new CsvFileIterator(path, opts);
    }

    #! Creates the CsvReadDataProvider with the input data stream and optionally an option hash
    /** @param stream the input stream to iterate
        @param opts a hash of optional options; see @ref abstractcsviterator_options for more information

        @throw ABSTRACTCSVITERATOR-ERROR invalid or unknown option; invalid data type for option; \c "header_names"
        is @ref True "True" and \c "headers" is also present; unknown field type
    */
    constructor(InputStream stream, *hash<auto> opts) {
        i = new CsvIterator(stream, opts.encoding, opts);
    }

    #! Returns the object name
    string getName() {
        return i instanceof CsvFileIterator
            ? i.getFileName()
            : sprintf("stream %s", self.uniqueHash());
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.path) {
            if (copts.stream) {
                error("CONSTRUCTOR-ERROR", "cannot provide both \"stream\" and \"path\" options; use either one or "
                    "the other");
            }
            i = new CsvIterator(FileLocationHandler::getBinaryStreamFromLocation(copts.path),
                copts.encoding, copts{CsvIterationOptionList} - "encoding");
            return;
        }
        if (copts.stream) {
            i = new CsvIterator(copts.stream, copts.encoding, copts{CsvIterationOptionList});
            return;
        }
        # create a dummy interator with no input
        i = new CsvIterator(new StringInputStream(""), copts.encoding, copts{CsvIterationOptionList});
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        return new DefaultRecordIterator(i, where_cond, search_options, getRecordType());
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<DataProviderExpression> where_cond,
            *hash<auto> search_options) {
        return new DefaultRecordIterator(i, where_cond, search_options, getRecordType());
    }

    #! Returns the description of the record type, if any
    private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        if (!i.valid()) {
            i.peek();
        }
        return i.getRecordType();
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
} # CsvUtil namespace
