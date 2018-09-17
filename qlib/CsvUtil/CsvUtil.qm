# -*- mode: qore; indent-tabs-mode: nil -*-
# @file CsvUtil.qm Qore user module for working with CSV files

/*  CsvUtil.qm Copyright 2012 - 2018 Qore Technologies, s.r.o.

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

# minimum required Qore version
%requires qore >= 0.8.13

%requires Util

# assume local var scope, do not use "$" for vars, members, and method calls
%new-style
%strict-args
%require-types
%enable-all-warnings

module CsvUtil {
    version = "1.6.2";
    desc = "user module for working with CSV files";
    author = "Petr Vanek <petr@yarpen.cz>, David Nichols <david@qore.org>";
    url = "http://qore.org";
    license = "MIT";
}

/* see release notes below for version history
*/

/** @mainpage CsvUtil Module

    @tableofcontents

    @section csvutilintro Introduction to the CsvUtil Module

    The %CsvUtil module provides functionality for parsing CSV-like files.

    To use this module, use \c "%requires CsvUtil" in your code.

    All the public symbols in the module are defined in the CsvUtil namespace

    Currently the module provides the following classes:
    - @ref CsvUtil::AbstractCsvIterator "AbstractCsvIterator": base abstract iterator class for iterating line-based CSV data
    - @ref CsvUtil::AbstractCsvWriter "AbstractCsvWriter": a base class for new CSV writer implementations
    - @ref CsvUtil::CsvIterator "CsvIterator": iterator class allowing for generic CSV input data to be processed line by line on a record basis
    - @ref CsvUtil::CsvWriter "CsvWriter": generic stream-based CSV writer

    Furthermore, the following specialized classes are implemented based on the above and are provided for convenience and backwards-compatibility:
    - @ref CsvUtil::CsvDataIterator "CsvDataIterator": iterator class allowing for CSV string data to be processed line by line on a record basis
    - @ref CsvUtil::CsvFileIterator "CsvFileIterator": iterator class allowing for CSV files to be processed line by line on a record basis
    - @ref CsvUtil::CsvFileWriter "CsvFileWriter": CSV file writer
    - @ref CsvUtil::CsvStringWriter "CsvStringWriter": CSV in-memory writer

    Note that the @ref CsvUtil::CsvIterator "CsvIterator" class can be used to parse arbitrary text data; the field separator character can be specified in the @ref CsvUtil::CsvIterator::constructor() "constructor", as well as the quote character and end of line sequence.  See the @ref CsvUtil::CsvIterator::constructor() "constructor documentation" for more information.

    <b>Examples:</b>
    @code{.py}
#!/usr/bin/env qore

%new-style
%strict-args
%require-types
%enable-all-warnings

%requires CsvUtil

FileInputStream input("example-file.csv");
CsvIterator i(input);
FileOutputStream output("example-file-copy.csv");
CsvWriter writer(output, ("headers": ("cc", "serno", "desc", "received")));

while (i.next()) {
    printf("%d: %y\n", i.index(), i.getValue());
    writer.writeLine(i.getValue());
}
    @endcode

    If \c "example-file.csv" is:
    @verbatim
UK,1234567890,"Sony, Xperia S",31052012
UK,1234567891,"Sony, Xperia S",31052012
UK,1234567892,"Sony, Xperia S",31052012
UK,1234567893,"Sony, Xperia S",31052012
    @endverbatim

    The data is read verbatim, each value is returned as a string, header names are generated numerically; the output is:
    @verbatim
1: {0: "UK", 1: "1234567890", 2: "Sony, Xperia S", 3: "31052012"}
2: {0: "UK", 1: "1234567891", 2: "Sony, Xperia S", 3: "31052012"}
3: {0: "UK", 1: "1234567892", 2: "Sony, Xperia S", 3: "31052012"}
4: {0: "UK", 1: "1234567893", 2: "Sony, Xperia S", 3: "31052012"}
    @endverbatim

    Also the \c "example-file-copy.csv" will contain data from the original file formatted as CSV.

    @anchor complex_example
    If header names are provided and field types are specified, the output looks different:
    @code{.py}
#!/usr/bin/env qore

%new-style
%strict-args
%require-types
%enable-all-warnings

%requires CsvUtil

FileInputStream input("example-file.csv");
CsvIterator i(input, NOTHING, ("headers": ("cc", "serno", "desc", "received"), "fields": ("serno": "int", "received": ("type": "date", "format": "DDMMYYYY"))));
while (i.next())
    printf("%d: %y\n", i.index(), i.getValue());
    @endcode

    Now the hash keys in each record returned are those given in the @ref CsvUtil::CsvFileIterator::constructor() "constructor", and the fields \c "serno" and \c "received" are given other data types; this produces:
    @verbatim
1: {cc: "UK", serno: 1234567890, desc: "Sony, Xperia S", received: 2012-05-31 00:00:00 Thu +02:00 (CEST)}
2: {cc: "UK", serno: 1234567891, desc: "Sony, Xperia S", received: 2012-05-31 00:00:00 Thu +02:00 (CEST)}
3: {cc: "UK", serno: 1234567892, desc: "Sony, Xperia S", received: 2012-05-31 00:00:00 Thu +02:00 (CEST)}
4: {cc: "UK", serno: 1234567893, desc: "Sony, Xperia S", received: 2012-05-31 00:00:00 Thu +02:00 (CEST)}
    @endverbatim

    Use the \c "header_lines" and \c "header_names" @ref abstractcsviterator_options "options" to automatically read the header names from the file if present.  Use the \c "fields" @ref abstractcsviterator_options "option" to describe the fields and perform transformations on the data read.  For more information, see the @ref CsvUtil::CsvFileIterator "CsvFileIterator" class.

    @section csvutil_multitype Multi-type support

    The %CsvUtil module supports multi-type records, meaning that different input lines may have different structures, and input record type
    resolution is performed at run-time using predefined rules.
    %CsvUtil classes have two contructors, a backwards-compatible constructor variant, which accepts field definition in options and supports only a single record type, and a new constructor avariant, which ccepts field specifications and options passed as separate parameters.

    When used with multi-type definitions, the @ref CsvUtil::AbstractCsvIterator::getValue() method returns
    records in an extended form.  In this case, the return value of @ref CsvUtil::AbstractCsvIterator::getValue()
    is a hash with the following keys:
    - \c "type": the resolved record type name
    - \c "record": a hash of record values

    As this is not backwards compatible, this mode is enabled using the \c "extended_record" option.

    If no record type resolution rules or logic is defined, then record types are resolved automatically based on their
    unique number of fields.  If the number of fields is not unique (i.e. two or more records have the same number of
    fields), then a rule must exist to resolve the record type.

    Typically the value of a particular field determines the record type, however even multiple fields could be used.  Record type detection configuration is supplied by the \c "value" (field value equality test) or \c "regex" (regular expression test) keys in the
    @ref abstractcsviterator_option_field_hash "field specification hash" for the record in question.  If multiple fields in a record
    definintion have \c "value" or \c "regex" keys, then all fields must match the input data in order for the input
    line to match the record.

    The above record type resolution logic is executed in
    @ref CsvUtil::AbstractCsvIterator::identifyTypeImpl() "AbstractCsvIterator::identifyTypeImpl()",
    which executes any \c "regex" or \c "value" tests on the input line in the order of the field definitions in the
    @ref abstractcsviterator_option_field_hash "record description hash".

    Record type resolution is performed as follow:
    - \c "value": Matches the full value of the field; if an integer \c "value" value is used, then integer comparisons are done, otherwise string comparisons are performed.
    - \c "regex": Matches the full value of the field with regular expression.

    When there are no record-matching keys in the field hashes for any record and the input record field number are
    not unique, then
    @ref CsvUtil::AbstractCsvIterator::identifyTypeImpl() "AbstractCsvIterator::identifyTypeImpl()"
    must be overridden in a subclass to provide custom record matching logic.

    @note
    - It is an error to have both \c "regex" and \c "value" keys in a @ref abstractcsviterator_option_field_hash "field specification hash"
    - If multiple fields have configuration for input line matching (i.e. \c "regex" and \c "value" keys), then all
      fields with this configuration must match for the record to be matched
    - since version 1.5.

    @code{.py}
#!/usr/bin/env qore

%new-style
%strict-args
%require-types
%enable-all-warnings

%requires CsvUtil

const spec = (
    "header": (
        "type": ("type": "int", "value": 1),
        "invoice_no": "string",
        "date": ("type": "date", "format": "YYYYMMDD"),
    ),
    "item": (
        "type": ("type": "int", "value": 2),
        "item_no": "string",
        "item": "string",
        "pcs": "int",
        "price": "number",
    ),
    "footer": (
        "type": ("type": "int", "value": 3),
        "total": "number",
    ),
);

const opts = {};

FileInputStream input("example-multi-file.csv");
CsvIterator i(input, NOTHING, spec, ("extended_record": True) + opts);

FileOutputStream output("example-multi-file-copy.csv");
CsvWriter writer(output, spec, ("write-headers": False, "optimal-quotes" : True, "quote_escape" : "\"") + opts);
while (i.next()) {
    printf("%d: %y\n", i.index(), i.getValue());
    writer.writeLine(i.getValue());
}
    @endcode


    If \c "example-multi-file.csv" is:
    @verbatim
1,2016-01,20160401
2,123,"Sony, Xperia S",1,100
2,124,"Nokia, Lumia",2,150
3,250
    @endverbatim

    The data is read verbatim, each value is returned as a string, header names are generated numerically; the output is:
    @verbatim
1: {type: "header", record: {type: 1, invoice_no: "2016-01", date: 2016-04-01 00:00:00.000000 Fri +02:00 (CEST)}}
2: {type: "item", record: {type: 2, item_no: "123", item: "Sony, Xperia S", pcs: 1, price: 100}}
3: {type: "item", record: {type: 2, item_no: "124", item: "Nokia, Lumia", pcs: 2, price: 150}}
4: {type: "footer", record: {type: 3, total: 250}}
    @endverbatim

    Also the \c "example-multi-file-copy.csv" will contain data from the original file formatted as CSV.

    @section csvutil_relnotes Release Notes

    @subsection csvutil_v1_6_2 Version 1.6.2
    - implemented the \c number_format option to allow numbers with alternative decimal separators to be parsed
      and generated (<a href="https://github.com/qorelanguage/qore/issues/2806">issue 2806</a>)

    @subsection csvutil_v1_6_1 Version 1.6.1
    - implemented the @ref CsvUtil::csvutil_set_global_compat_force_empty_string() function and the
    \c compat_force_empty_string CSV parsing option to force "*string" fields with no
    value to return an empty string when parsing rather than @ref nothing for backwards compatibility
    with very early versions of CsvUtil (<a href="https://github.com/qorelanguage/qore/issues/2476">issue 2476</a>)

    @subsection csvutil_v1_6 Version 1.6
    - added support for streams; the following stream-based classes have been added:
      - @ref CsvUtil::CsvIterator "CsvIterator": provides a more generic interface than @ref CsvUtil::CsvDataIterator "CsvDataIterator" and @ref CsvUtil::CsvFileIterator "CsvFileIterator"
      - @ref CsvUtil::CsvWriter "CsvWriter": provides a more generic interface than @ref CsvUtil::CsvStringWriter "CsvStringWriter" and @ref CsvUtil::CsvFileWriter "CsvFileWriter"
    - fixed a bug in an error message validating input data (<a href="https://github.com/qorelanguage/qore/issues/1062">issue 1062</a>)
    - improved \a fields option documentation and added an exception when detected headers do not match the \a fields option (<a href="https://github.com/qorelanguage/qore/issues/2179">issue 2179</a>)

    @subsection csvutil_v1_5_1 Version 1.5.1
    - fixed a bug in @ref CsvUtil::AbstractCsvIterator::identifyTypeImpl() "AbstractCsvIterator::identifyTypeImpl()" generating an error message (<a href="https://github.com/qorelanguage/qore/issues/1355">issue 1355</a>)

    @subsection csvutil_v1_5 Version 1.5
    - fixed a bug handling the global option \c "eol"
    - converted to new-style
    - if "headers" are not given in the @ref CsvUtil::AbstractCsvWriter::constructor() but "fields" are, then set the headers from the field descriptions automatically
    - added write() methods returning the generated strings to the @ref CsvUtil::CsvStringWriter class for API compatibility with the corresponding FixedLengthDataWriter methods
    - implemented support for @ref Qore::SQL::SQLStatement "SQLStatement" as an iterator source for @ref CsvUtil::AbstractCsvWriter::write()
    - implemented the \c "datamap" and \c "info_log" options for CSV generation
    - implemented options with underscores to replace options with dashes:
      - @ref CsvUtil::AbstractCsvWriter
        - \c "date-format" is now \c "date_format"
        - \c "optimal-quotes" is now \c "optimal_quotes"
        - \c "verify-columns" is now \c "verify_columns"
        - \c "write-headers" is now \c "write_headers"
      - @ref CsvUtil::AbstractCsvIterator
        - \c "ignore-empty" is now \c "ignore_empty"
        - \c "ignore-whitespace" is now \c "ignore_whitespace"
        - \c "header-names" is now \c "header_names"
        - \c "header-lines" is now \c "header_lines"
        - \c "verify-columns" is now \c "verify_columns"
    - implemented multi-type line support
    - two constructors for backward CsvUtil compatability and similarity with FixedLengthUtil API
    - implemented new options \c "header_reorder"; @ref CsvUtil::AbstractCsvIterator \c "date_format",\c "extended_record"
    - implemented options for field specification: \c "index", \c "default", \c "header", \c "value", \c "regex"
    - fixed a UTC bug for default date 1970-01-01Z

    @subsection csvutil_v1_4 Version 1.4
    - fixed the \c "format" field option when used with \c "*date" field types
    - implemented the \c "tolwr" parser option
    - changed the default field type when parsing and generating CSV files from \c "string" to \c "*string"

    @subsection csvutil_v1_3 Version 1.3
    - added the \c "write-headers" option to @ref CsvUtil::AbstractCsvWriter and subclasses to enable headers to be suppressed
    - added the \c "optimal-quotes" option to @ref CsvUtil::AbstractCsvWriter and subclasses to enable more efficient csv output (now the default output option); to revert back to the previous behavior (where all fields are quoted regardless of data type or content), set to @ref Qore::False "False" in the constructor

    @subsection csvutil_v1_2 Version 1.2
    - fixed @ref CsvUtil::CsvDataIterator::next() when header_lines > 0 and working with empty input data
    - implemented support for the \c "*int", \c "*float", \c "*number", and \c "*date" types
    - implemented support for allowing subclasses of @ref CsvUtil::CsvFileIterator to implement support for other custom types
    - fixed \c "date" type handling with empty input; now returns 1970-01-01Z (use "*date" to map empty input to NOTHING)
    - added the @ref CsvUtil::CsvStringWriter, @ref CsvUtil::AbstractCsvWriter, and @ref CsvUtil::CsvFileWriter classes
    - if "headers" are not given in the @ref CsvUtil::CsvFileIterator::constructor() but "fields" are, then set the headers from the field descriptions automatically

    @subsection csvutil_v1_1 Version 1.1
    - bug fixes to header and fields option processing
    - fixed @ref CsvUtil::CsvFileIterator::index() to return the line index
    - added @ref CsvUtil::CsvFileIterator::lineNumber() to return the current line number in the file

    @subsection csvutil_v1_0 Version 1.0
    - initial version of module
*/

#! the CsvUtil namespace contains all the objects in the CsvUtil module
public namespace CsvUtil {
    #! Unix end of line character sequence (for new OS X too)
    public const EOL_UNIX = "\n";
    #! MS DOS/Windows end of line character sequence
    public const EOL_WIN = "\r\n";
    #! Old (pre-OSX) Macintosh end of line character sequence
    public const EOL_MACINTOSH = "\r";

    # helper list of end of line values
    const EOLS = (EOL_UNIX, EOL_WIN, EOL_MACINTOSH, );

    #! Record type when non matching any type
    public const CSV_TYPE_UNKNOWN = "<unknown>";
    #! Record type when multi-type is disabled
    public const CSV_TYPE_SINGLE = "<single>";

    #! global option to force "*string" fields with no value to return an empty string when parsing rather than @ref nothing for backwards compatibility with very early versions of CsvUtil
    our bool global_compat_force_empty_string;

    #! sets the \c global_compat_force_empty_string variable to force "*string" fields with no value to return an empty string when parsing rather than @ref nothing for backwards compatibility with very early versions of CsvUtil
    public sub csvutil_set_global_compat_force_empty_string(softbool val) {
        global_compat_force_empty_string = val;
    }

    #! The CsvFileIterator class allows CSV files to be iterated on a record basis
    /** The class is deprecated as @ref CsvIterator provides more flexibility.
        @see
        - @ref CsvUtil::CsvIterator "CsvIterator" for a stream-based class providing the same functionality as this class in a more generic way
        - @ref abstractcsviterator_options
        - @ref abstractcsviterator_option_field_types
        - @ref abstractcsviterator_option_field_hash
     */
    public class CsvFileIterator inherits CsvUtil::AbstractCsvIterator {
        private {
            #! the path of the file being iterated
            string m_file_path;
        }

        #! Creates the CsvFileIterator in single-type mode with the path of the file to read and an option hash
        /** @param path the path to the CSV file to read
            @param opts a hash of optional options; see @ref abstractcsviterator_options for more information

            @throw ABSTRACTCSVITERATOR-ERROR invalid or unknown option; invalid data type for option; \c "header_names" is @ref Qore::True "True" and \c "header_lines" is 0 or \c "headers" is also present; unknown field type
         */
        constructor(string path, *hash opts) : AbstractCsvIterator(new InputStreamLineIterator(new FileInputStream(path), opts.encoding, opts.eol), opts) {
            m_file_path = path;
        }

        #! Creates the CsvFileIterator in multi-type mode with the path of the file to read and optionally an option hash
        /** @param path the path to the CSV file to read
            @param spec a hash of field and type definition; see @ref abstractcsviterator_option_field_hash for more information
            @param opts a hash of optional options; see @ref abstractcsviterator_options for more information
         */
        constructor(string path, hash spec, hash opts) : AbstractCsvIterator(new InputStreamLineIterator(new FileInputStream(path), opts.encoding, opts.eol), spec, opts) {
            m_file_path = path;
        }

        #! calls AbstractCsvIterator::memberGate()
        auto memberGate(string name) {
            return AbstractCsvIterator::memberGate(name);
        }

        #! Returns the character encoding for the file
        string getEncoding() {
            return cast<InputStreamLineIterator>(lineIterator).getEncoding();
        }

        #! Returns the file path/name used to open the file
        string getFileName() {
            return m_file_path;
        }

        #! Returns a @ref Qore::StatInfo "StatInfo" hash of the underlying file
        hash<Qore::StatInfo> hstat() {
            return Qore::hstat(m_file_path);
        }

        #! Returns a @ref stat_list "stat list" of the underlying file
        list stat() {
            return Qore::stat(m_file_path);
        }
    } # CsvFileIterator class

    #! The CsvDataIterator class allows arbitrary CSV string data to be iterated on a record basis
    /** The class is deprecated as @ref CsvIterator provides more flexibility.
        @see
        - @ref CsvUtil::CsvIterator "CsvIterator" for a stream-based class providing the same functionality as this class in a more generic way
        - @ref abstractcsviterator_options
        - @ref abstractcsviterator_option_field_types
        - @ref abstractcsviterator_option_field_hash
     */
    public class CsvDataIterator inherits CsvUtil::AbstractCsvIterator {

        #! Creates the CsvDataIterator with the input data and optionally an option hash
        /** @param data the input data to iterate
            @param opts a hash of optional options; see @ref abstractcsviterator_options for more information

            @throw ABSTRACTCSVITERATOR-ERROR invalid or unknown option; invalid data type for option; \c "header_names" is @ref Qore::True "True" and \c "header_lines" is 0 or \c "headers" is also present; unknown field type
         */
        constructor(string data, *hash opts) : AbstractCsvIterator(new DataLineIterator(data, opts.eol), opts) {
        }

        #! Creates the CsvDataIterator in multi-type mode with the path of the file to read and an option hash
        /** @param data the input data to iterate
            @param spec a hash of field and type definition; see @ref abstractcsviterator_option_field_hash for more information
            @param opts a hash of optional options; see @ref abstractcsviterator_options for more information
         */
        constructor(string data, hash spec, hash opts) : AbstractCsvIterator(new DataLineIterator(data, opts.eol), spec, opts) {
        }

        auto memberGate(string name) {
            return AbstractCsvIterator::memberGate(name);
        }

    }

    #! The AbstractCsvWriter class provides a parent for all CSV writers
    /**
        Any inherited class must provide concrete implementations for the following abstract method, where the real physical write action must be implemented.

        @code{.py} private writeRawLine(list values) @endcode

        @section csvwriter_options AbstractCsvWriter Constructor Option Hash Overview
        The AbstractCsvWriter class constructor takes an optional hash with possible keys given in the following table.
        Note that key names are case-sensitive, and data types are soft (conversions are made when possible).

        <b>AbstractCsvWriter Options</b>
        |!Option|!Data Type|!Description
        |\c "block"|@ref int_type "int"|the block size when generating output based on a @ref Qore::SQL::SQLStatement "SQLStatement" source (default: \c 1000)
        |\c "date_format"|@ref string_type "string"|the default mask for date values formatting
        |\c "encoding"|@ref string_type "string"|the @ref character_encoding "character encoding" for the file (and for tagging string data read); if the value of this key is not a string then it will be ignored
        |\c "eol"|@ref string_type "string"|the end of line character(s) (default: auto-detect); if the value of this key is not a string then it will be ignored
        |\c "info_log"|@ref code_type "code"|a call reference / closure for informational logging when performing bulk generation with write(Qore::SQL::SQLStatement); must take a single string argument
        |\c "optimal_quotes"|@ref bool_type "bool"|set to @ref Qore::False "False" to disable optimal quoting; when optimal quoting is disabled, all fields are quoted regardless of type or content, when it is enabled, then fields are quoted only if they require quoting (i.e. they contain a quote or separator character); the default is @ref Qore::True "True"
        |\c "quote"|@ref string_type "string"|the field quote character (default: \c '\"')
        |\c "quote_escape"|@ref string_type "string"|the escape character(s) used for \c "quote" (default: \c '\\')
        |\c "separator"|@ref string_type "string"|the string separating the fields in the file (default: \c ",")
        |\c "verify_columns"|@ref bool_type "bool"|if @ref Qore::True "True" (the default is @ref Qore::False "False") then if a line is parsed with a different column count than other lines, a \c CSVFILEWRITER-DATA-ERROR exception is thrown
        |\c "write_headers"|@ref bool_type "bool"|set to @ref Qore::False "False" to suppress the output of headers; the default is @ref Qore::True "True", meaning to output headers if they are present. The value is @ref Qore::False "False" for multi-type lines.

        <b>AbstractCsvIterator Single-type-only Options </b>
        |!Option|!Data Type|!Description
        |\c "datamap"|@ref hash_type "hash"|a hash mapping actual data key names to the output field names, for use in case the data field names differ; does not have to include every data or output key; keys not present will be assumed to be mapped 1:1
        |\c "fields"|@ref hash_type "hash"|describes the data to be output; see @ref abstractcsviterator_option_field_hash "option field hash" and @ref abstractcsviterator_option_field_types "option field types" for more information on the possible values of this option; if no field type is given, the default is \c "*string"
        |\c "headers"|@ref list_type "list" of @ref string_type "strings"|list of header / column names for the data iterated.
        |\c "header_reorder"|@ref bool_type "bool"|if @ref Qore::True "True" (default value) then if \"headers\" are provided by options then fields are reordered to follow headers. It has effect on expected field order as passed to write function via list. If @ref Qore::False "False" then fields not yet specified are pushed at the end of field definition.

        @note the following options with dashes in their names are supported for backwards-compatibility:
        - \c "date-format"
        - \c "optimal-quotes"
        - \c "verify-columns"
        - \c "write-headers"
     */
    public class AbstractCsvWriter inherits private CsvHelper {
        private {
            #! valid options for the object (a hash for quick lookups of valid keys)
            const Options = (
                "block": C_OPT1|C_OPT2,
                "datamap": C_OPT1,
                "date_format": C_OPT1|C_OPT2,
                "date-format": C_OPT1|C_OPT2,
                "encoding": C_OPT1|C_OPT2,
                "eol": C_OPT1|C_OPT2,
                "fields": C_OPT1,
                "headers": C_OPT1,
                "header_reorder": C_OPT1,
                "info_log": C_OPT1|C_OPT2,
                "number_format": C_OPT1|C_OPT2,
                "optimal_quotes": C_OPT1|C_OPT2,
                "optimal-quotes": C_OPT1|C_OPT2,
                "quote": C_OPT1|C_OPT2,
                "quote_escape": C_OPT1|C_OPT2,
                "separator": C_OPT1|C_OPT2,
                "verify_columns": C_OPT1|C_OPT2,
                "verify-columns": C_OPT1|C_OPT2,
                "write_headers": C_OPT1|C_OPT2,
                "write-headers": C_OPT1|C_OPT2,
                );

            #! output file character encoding
            string encoding;

            #! field separator
            string separator = ",";

            #! field content delimiter
            string quote = "\"";

            #! quote escape character
            string m_quoteEscapeChar = "\\";

            #! end of line sequence
            string eol = EOL_UNIX;

            #! verify the column count for every row; if a row does not match, then throw a \c CSVFILEITERATOR-DATA-ERROR exception
            bool checkElementCounts = False;

            #! the latest line number
            int lineNo = 0;

            #! block size for bulk DML
            int block = 1000;

            #! base template for value format
            string baseTemplate;

            #! this flag determines if any stored headers are output
            bool write_headers = True;

            #! stores the optimal quotes option
            bool optimal_quotes = True;

            #! a closure/call reference for informational logging when using write(SQLStatement)
            *code info_log;

            #! mapping output field by name
            hash m_out_by_name;

            #! mapping output field by index
            hash m_out_by_idx;
        }

        #! Creates the AbstractCsvWriter in single-type mode
        /**
            @param n_errname a string to construct child class error message. For example: value 'FOO' will result in exception names 'CSVFOOWRITER-ERROR'
            @param n_opts @ref csvwriter_options

            @throw CSV<errname>ITER-ERROR in the case of incorrect options
         */
        constructor(string n_errname, *hash n_opts): CsvHelper(sprintf("CSV%sWRITER-ERROR", n_errname)) {

            processCommonOptions(n_opts, C_OPT1);

            # set headers automatically from field names if not set
            if (n_opts.fields && !n_opts.headers)
                n_opts.headers = n_opts.fields.keys();

            m_specs = getSpec1(n_opts.fields);

            if (n_opts.datamap) {
                if (n_opts.datamap.typeCode() != NT_HASH)
                    throw errname, sprintf("expecting a hash value to option %y; got %y (type %s) instead", n_opts.datamap.key, n_opts.datamap, n_opts.datamap.type());
                # ensure that all the values are strings
                foreach hash h in (n_opts.datamap.pairIterator()) {
                    if (h.value.typeCode() != NT_STRING)
                        throw errname, sprintf("\"datamap\" key %y has a non-string value; got type %y", h.key, h.value.type());
                }

                string k = m_specs.firstKey();
                foreach hash h in (n_opts.datamap.pairIterator()) {
                    if (!exists m_specs{k}{h.key}) {
                        m_specs{k}{h.key} = ("type": "*string", "idx": m_specs{k}.size());
                    }
                    m_specs{k}{h.key}.header = tolwr ? h.value.lwr() : h.value;
                }
            }
            if (n_opts.headers) {
                adjustFieldsFromHeaders(m_specs.firstKey(), n_opts.headers);
            }
            processSpec();
        }

        #! Creates the AbstractCsvWriter in single-type mode
        /**

            @param spec a hash of field and type definition; see @ref abstractcsviterator_option_field_hash for more information
            @param n_errname a string to construct child class error message. For example: value 'FOO' will result in exception names 'CSVFOOWRITER-ERROR'
            @param n_opts @ref csvwriter_options

            @throw CSV<errname>WRITER-ERROR in the case of incorrect options
         */
        constructor(string n_errname, hash spec, hash n_opts): CsvHelper(sprintf("CSV%sWRITER-ERROR", n_errname)) {
            m_specs = getSpec2(spec);
            processCommonOptions(n_opts, C_OPT2);
            processSpec();
        }

        #! Process options and set internal variables
        private processCommonOptions(*hash n_opts, int C_OPTx) {
            date_format = 'DD/MM/YYYY hh:mm:SS';  # different default value in writer
            if (n_opts.encoding.typeCode() == NT_STRING) {
                encoding = n_opts.encoding;
                # ensure that the default value for the encoding is used (to handle case + encoding aliases); also validates that
                # the encoding is known and supported
                string str = convert_encoding(encoding, encoding);
                encoding = str.encoding();
            }
            else
                encoding = get_default_encoding();

            foreach hash i in (n_opts.pairIterator()) {
                if (!exists Options.(i.key) && (Options.(i.key) & C_OPTx) == 0)
                    throw errname, sprintf("unknown option %y passed to %s::constructor() (valid options: %y)", i.key, self.className(), (select Options.keys(), (Options.$1 & C_OPTx) != 0));

                # if the option is not set, then ignore it
                if (!exists i.value || i.value === NULL)
                    continue;

                switch (i.key) {
                    case "block": {
                        if (!i.value.intp())
                            throw errname, sprintf("expecting an integer value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        block = i.value.toInt();
                        if (block < 1)
                            throw errname, sprintf("expecting a positive integer value to option %y; got %y instead", i.key, i.value);
                        break;
                    }
                    case "separator": {
                        if (i.value.typeCode() != NT_STRING)
                            throw errname, sprintf("expecting a string value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        separator = i.value;
                        break;
                    }
                    case "quote": {
                        if (i.value.typeCode() != NT_STRING)
                            throw errname, sprintf("expecting a string value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        quote = i.value;
                        break;
                    }
                    case "quote_escape": {
                        if (i.value.typeCode() != NT_STRING) {
                            throw errname, sprintf("expecting a string value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        }
                        m_quoteEscapeChar = i.value;
                        break;
                    }
                    case "eol": {
                        if (i.value.typeCode() != NT_STRING)
                            throw errname, sprintf("expecting a string value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        if (!inlist(i.value, EOLS))
                            throw errname, sprintf("expecting a known end of line value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        eol = i.value;
                        break;
                    }
                    case "date_format":
                    case "date-format": {
                        if (i.value.typeCode() != NT_STRING)
                            throw errname, sprintf("expecting a string value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        date_format = i.value;
                        break;
                    }
                    case "number_format": {
                        if (i.value.typeCode() != NT_STRING)
                            throw errname, sprintf("expecting a string value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        number_format = i.value;
                        break;
                    }
                    case "verify_columns":
                    case "verify-columns": {
                        checkElementCounts = parse_boolean(i.value);
                        break;
                    }
                    case "write_headers":
                    case "write-headers": {
                        write_headers = parse_boolean(i.value);
                        break;
                    }
                    case "optimal_quotes":
                    case "optimal-quotes": {
                        optimal_quotes = parse_boolean(i.value);
                        break;
                    }
                    case "info_log": {
                        if (!i.value.callp())
                            throw errname, sprintf("expecting a callable value to option %y; got %y (type %s) instead", i.key, i.value, i.value.type());
                        info_log = i.value;
                        break;
                    }
                    case "header_reorder": {
                        headerReorder = parse_boolean(i.value);
                        break;
                    }
                }

            }
            # make eg. '"%s",' template
            baseTemplate = sprintf("%s%%s%s%s", quote, quote, separator);
        }

        #! Process specification and set internal variable for mapping
        private processSpec() {
            if (write_headers && isMultiType()) {
                throw errname, sprintf("\"write_headers\" is True but \"headers\" defined as multi-type (%y)", m_specs);
            }
            foreach string k in (m_specs.keyIterator()) {
                list ll;
                if (m_specs{k}) {
                    int i = 0;
                    foreach hash f in (m_specs{k}.pairIterator()) {
                        ll += ("out_idx": f.value.idx ?? 0, "field": f.key, "idx": i);
                        i++;
                    }
                    code sort_func = int sub (hash l, hash r) { return l.out_idx <=> r.out_idx; };
                    ll = sort(ll, sort_func);
                    m_out_by_name{k} = map ($1.field), ll;
                    m_out_by_idx{k} = map ($1.idx), ll;
                }
            }
        }

        #! Write csv headers
        private writeHeaders() {
            if (write_headers && m_specs.size() == 1 && m_specs.firstValue().size() > 0) {
                string type = m_specs.firstKey();
                list values = map (m_specs{type}{$1}.header ?? $1), m_out_by_name{type};
                writeRawLine(values);
            }
        }

        #! Write a line with a list of values; data are checked against column rules
        /**
            @param values a list with values.

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints
         */
        writeLine(list values) {
            writeLine(CSV_TYPE_SINGLE, values);
        }

        #! Write a line with headers-values hash
        /**
            @param values a hash with keys as column headers in single-type format or with hash with \"type\" and \"record\" members for multi-type. The format is automatically detected.

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints
         */
        writeLine(hash values) {
            if (exists values.type && exists values.record) {
                writeLine(values.type, values.record);
            } else {
                writeLine(CSV_TYPE_SINGLE, values);
            }
        }

        #! Write a line with headers-values list
        /**
            @param type record type
            @param values as list of values

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints
         */
        writeLine(string type, list values) {
            lineNo++;
            if (checkElementCounts && m_specs{type} && values.size() != m_specs{type}.size())
                throw errname, sprintf("Line (%d), Header size (%d) and line size (%d) are different (enforced by the \"verify_columns\" option)", lineNo, m_specs{type}.size(), values.size());

            list rawValues = ();
            for(int j = 0; j < values.size(); j++) {
                int i = j;
                #! format date to string by options. Priority: 1) column definition 2) standard format
                string dateFmt = date_format;
                *string numFmt = number_format;
                if (m_out_by_idx{type} && j < m_out_by_idx{type}.size()) {
                    # get correct input value
                    i = m_out_by_idx{type}[j];
                    hash spec = m_specs{type}{m_out_by_name{type}[j]};
                    switch (spec.type) {
                        case "int": {
                            if (int(values[i]) != values[i])
                                throw errname, sprintf("Line (%d), Column (%d) does not contain an integer value %n vs %n", lineNo, j, int(values[i]), values[i]);
                            numFmt = spec.format ?? number_format;
                            break;
                        }
                        case "*int": {
                            if (!values[i].empty() && int(values[i]) != values[i])
                                throw errname, sprintf("Line (%d), Column (%d) does not contain an integer value %n vs %n", lineNo, j, int(values[i]), values[i]);
                            numFmt = spec.format ?? number_format;
                            break;
                        }
                        case "float": {
                            if (float(values[i]) != values[i])
                                throw errname, sprintf("Line (%d), Column (%d) does not contain a float value %n vs %n", lineNo, j, float(values[i]), values[i]);
                            numFmt = spec.format ?? number_format;
                            break;
                        }
                        case "*float": {
                            if (!values[i].empty() && float(values[i]) != values[i])
                                throw errname, sprintf("Line (%d), Column (%d) does not contain a float value %n vs %n", lineNo, j, float(values[i]), values[i]);
                            numFmt = spec.format ?? number_format;
                            break;
                        }
                        case "number": {
                            if (number(values[i]) != values[i])
                                throw errname, sprintf("Line (%d), Column (%d) does not contain a number value %n vs %n", lineNo, j, number(values[i]), values[i]);
                            numFmt = spec.format ?? number_format;
                            break;
                        }
                        case "*number": {
                            if (!values[i].empty() && number(values[i]) != values[i])
                                throw errname, sprintf("Line (%d), Column (%d) does not contain a number value %n vs %n", lineNo, j, number(values[i]), values[i]);
                            numFmt = spec.format ?? number_format;
                            break;
                        }
                        case "date": {
                            if (values[i].typeCode() != NT_DATE)
                                throw errname, sprintf("Line (%d), Column (%d) does not contain a date value (%s), %y", lineNo, j, values[i].type(), values[i]);
                            dateFmt = spec.format ?? date_format;
                            break;
                        }
                        case "*date": {
                            if (!values[i].empty() && values[i].typeCode() != NT_DATE)
                                throw errname, sprintf("Line (%d), Column (%d) does not contain a date value (%s)", lineNo, j, values[i].type());
                            dateFmt = spec.format ?? date_format;
                            break;
                        }
                    }
                }

                # format date as requested
                if (values[i].typeCode() == NT_DATE) {
                    values[i] = format_date(dateFmt, values[i]);
                }
                if (numFmt) {
                    switch (values[i].typeCode()) {
                        case NT_INT: values[i] = values[i].format(numFmt); break;
                        case NT_FLOAT: values[i] = values[i].format(numFmt); break;
                        case NT_NUMBER: values[i] = values[i].format(numFmt); break;
                    }
                }
                push rawValues, values[i];
            }

            writeRawLine(rawValues);
        }

        #! Write a line for a specific record from a hash to the output
        /**
            @param type record type
            @param values a hash of values.

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints
         */
        writeLine(string type, hash values) {
            if (!m_specs{type})
                throw errname, sprintf("Type '%s' not defined header in AbstractCsvWriter instance to bind the hash key", type);
            list line;
            foreach string i in (m_specs{type}.keyIterator()) { # (m_out_by_name{type}) {
                if (!values.hasKey(i))
                    if (exists m_specs{type}{i}.default) {
                        push line, m_specs{type}{i}.default;
                    } else {
                        push line, NOTHING;
                    }
                else {
                    push line, values{i};
                }
            }
            writeLine(type, line);
        }

        #! Stream an iterator into the output
        /**
            @param iterator an iterator to stream data into file

            The iterator has to return @ref list or @ref hash from the @ref Qore::AbstractIterator::getValue() "getValue()" method.

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints or when @ref Qore::AbstractIterator::getValue() "getValue()" does not return list or hash
         */
        write(Qore::AbstractIterator iterator) {
            while (iterator.next()) {
                auto row = iterator.getValue();
                switch (row.typeCode()) {
                    case NT_HASH:
                        writeLine(row);
                        break;
                    case NT_LIST:
                        writeLine(row);
                        break;
                    default:
                        throw errname, sprintf("Iterator's getValue returned %s, expected list or hash", row.type());
                }
            }
        }

        #! Stream an iterator into the output
        /**
            @param iterator an @ref Qore::SQL::SQLStatement "SQLStatement" iterator to stream data into file; @ref Qore::SQL::SQLStatement::fetchColumns() "SQLStatement::fetchColumns()" is used to leverage bulk DML for more efficient SQL I/O

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints

            @note if any \c "info_log" option is set in the constructor; it is used here to log each block of SQL data used to generate the corresponding number of lines; log messages look like: \c "query input generated bulk output lines: 1000"
         */
        write(Qore::SQL::SQLStatement iterator) {
            while (*hash h = iterator.fetchColumns(block)) {
                int n = h.firstValue().lsize();
                if (!n)
                    break;
                map writeLine($1), h.contextIterator();
                if (info_log)
                    info_log(sprintf("query input generated bulk output lines: %d", n));
            }
        }

        #! Stream the contents of the list into the output
        /**
            @param l a list of input data to format as CSV output

            The list has to contain @ref list or @ref hash elements that can be formatted according to the CSV definition

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints or when the list elements have the wrong type or values
         */
        write(list l) {
            write(l.iterator());
        }

        #! This method must be overridden in child classes to provide the output implementation
        abstract private writeRawLine(list values);

        #! Prepare a string (line with EOF) with formatting and escaping.
        /**
            @param values a list with values

            @return string a formatted and escaped line with a trailing EOL string; if an \c encoding option was given to the constructor; the return value always has the given encoding
        */
        private string prepareRawLine(list values) {
            string rv = prepareRawLineIntern(values);
            return encoding && rv.encoding() != encoding ? convert_encoding(rv, encoding) : rv;
        }

        private string prepareRawLineIntern(list values) {
            if (!optimal_quotes) {
                # make full line template
                string template = strmul(baseTemplate, elements values, 1) + eol;
                # escape separators in strings
                list row = map replace(string($1), quote, m_quoteEscapeChar + quote), values;

                return vsprintf(template, row);
            }

            list l = ();
            foreach auto v in (values) {
                if (v.typeCode() == NT_STRING) {
                    if (v.find(quote) >= 0) {
                        v = replace(v, quote, m_quoteEscapeChar + quote);
                        v = sprintf("\"%s\"", v);
                    }
                    else if (v.find(separator) >= 0)
                        v = sprintf("\"%s\"", v);
                    l += v;
                    continue;
                }
                l += string(v);
            }
            return (foldl $1 + separator + $2, l) + eol;
        }

    } # AbstractCsvWriter class

    #! The CsvWriter class for safe CSV data creation
    public class CsvWriter inherits AbstractCsvWriter {
        private {
            #! the output stream for the CSV data
            StreamWriter output;
        }

        #! creates the CsvWriter in single-type mode with the @ref Qore::OutputStream "OutputStream" and an optional option hash
        /**
            @param output the @ref Qore::OutputStream "OutputStream" for the CSV data generated
            @param opts @ref csvwriter_options

            @throw CSVFILEITER-ERROR in the case of incorrect options
        */
        constructor(Qore::OutputStream output, *hash opts) : AbstractCsvWriter("STREAM", opts) {
            # we do not use the encoding option with the StreamWriter constructor() here, because we always generate output data
            # with the output encoding in any case
            self.output = new StreamWriter(output);
            writeHeaders();
        }

        #! creates the CsvWriter in multi-type mode with the @ref Qore::OutputStream "OutputStream" and an option hash
        /**
            @param output the @ref Qore::OutputStream "OutputStream" for the CSV data generated
            @param spec a hash of field and type definition; see @ref abstractcsviterator_option_field_hash for more information
            @param opts @ref csvwriter_options

            @throw CSVFILEITER-ERROR in the case of incorrect options
        */
        constructor(Qore::OutputStream output, hash spec, hash opts) : AbstractCsvWriter("STREAM", spec, opts) {
            # we do not use the encoding option with the StreamWriter constructor() here, because we always generate output data
            # with the output encoding in any case
            self.output = new StreamWriter(output);
            writeHeaders();
        }

        #! renders the line and writes it to the output stream
        private writeRawLine(list values) {
            output.print(prepareRawLine(values));
        }
    }

    #! The CsvFileWriter class for safe CSV file creation
    /**
        @see
        - @ref CsvUtil::CsvWriter "CsvWriter" for a stream-based class providing the same functionality as this class in a more generic way
     */
    public class CsvFileWriter inherits AbstractCsvWriter {
        private {
            #! the file to write
            Qore::File file;
        }

        #! creates the CsvFileWriter in single-type mode with the path of the file to create and an optional option hash
        /**
            @param path a file name (with path optionally) to write
            @param opts @ref csvwriter_options

            The file is created with @ref Qore::O_CREAT "O_CREAT", @ref Qore::O_TRUNC "O_TRUNC", @ref Qore::O_WRONLY "O_WRONLY" and \c 0644 access.

            @throw CSVFILEITER-ERROR in the case of incorrect options
        */
        constructor(string path, *hash opts) : AbstractCsvWriter("FILE", opts) {
            openFile(path);
        }

        #! creates the CsvFileWriter in multi-type mode with the path of the file to create and an optional option hash
        /**
            @param path a file name (with path optionally) to write
            @param spec a hash of field and type definition; see @ref abstractcsviterator_option_field_hash for more information
            @param opts @ref csvwriter_options

            The file is created with @ref Qore::O_CREAT "O_CREAT", @ref Qore::O_TRUNC "O_TRUNC", @ref Qore::O_WRONLY "O_WRONLY" and \c 0644 access.

            @throw CSVFILEITER-ERROR in the case of incorrect options
        */
        constructor(string path, hash spec, hash opts) : AbstractCsvWriter("FILE", spec, opts) {
            openFile(path);
        }

        private openFile(string path) {
            # we do not use the encoding option with the File constructor() here, because we always generate output data
            # with the output encoding in any case
            file = new File();
            file.open2(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
            writeHeaders();
        }

        private writeRawLine(list values) {
            file.write(prepareRawLine(values));
        }
    } # CsvFileWriter

    #! The CsvStringWriter class for in-memory string CSV creation
    /**
        @see
        - @ref CsvUtil::CsvWriter "CsvWriter" for a stream-based class providing the same functionality as this class in a more generic way
     */
    public class CsvStringWriter inherits AbstractCsvWriter {
        private {
            # a csv content
            string content;
        }

        #! creates the CsvStringWriter single-type mode with content in the memory
        /**
            @param opts @ref csvwriter_options

            @throw CSVSTRINGITER-ERROR in the case of incorrect options
        */
        constructor(*hash opts) : AbstractCsvWriter("STRING", opts) {
            initContent();
        }

        #! creates the CsvStringWriter single-type mode with content in the memory
        /**
            @param spec a hash of field and type definitions; see @ref abstractcsviterator_option_field_hash for more information
            @param opts @ref csvwriter_options

            @throw CSVSTRINGITER-ERROR in the case of incorrect options
        */
        constructor(hash spec, hash opts) : AbstractCsvWriter("STRING", spec, opts) {
            initContent();
        }

        private initContent() {
            writeHeaders();
        }

        private writeRawLine(list values) {
            content += prepareRawLine(values);
        }

        #! Stream iterator and return a CSV-formatted output string
        /**
            @param iterator an iterator to stream data

            @return the CSV-formatted output string corresponding to the input data

            The iterator has to contain @ref list or @ref hash as a return value of \c getValue() method.

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints or when \c getValue does not return list or hash
         */
        string write(Qore::AbstractIterator iterator) {
            AbstractCsvWriter::write(iterator);
            return content;
        }

        #! Stream the contents of the list and return CSV-formatted output as a string
        /**
            @param l a list of input data to format as CSV output

            @return the CSV-formatted output string corresponding to the input data

            The list has to contain @ref list or @ref hash elements that can be formatted according to the CSV definition

            @throw CSVFILEWRITER-DATA-ERROR when the data does not fit defined column constraints or when the list elements have the wrong type or values
         */
        string write(list l) {
            AbstractCsvWriter::write(l);
            return content;
        }

        #! Get the current in-memory content as a string
        string getContent() {
            return content;
        }
    } # CsvStringWriter
} # CsvUtil namespace
