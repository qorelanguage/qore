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
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the location of the CSV data; mutually exclusive with \"stream\"; resolved with a call to "
                    "FileLocationHandler::getBinaryStreamFromLocation()",
            },
            "stream": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("InputStream")),
                "desc": "an input stream for CSV data; mutually exclusive with \"path\"",
            },
            "date_format": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the default date format for `date` fields (ex: `DD/MM/YYYY HH:mm:SS`)",
            },
            "encoding": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "character encoding for the file (and output data)",
            },
            "eol": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the end of line character(s); if not specified, then the end-of-line characters will be "
                    "auto detected",
            },
            "fields": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "the keys are field names as given by the `header_names` or `headers` options (in case "
                    "neither of these options are used, then field names are numbers starting with `0`) and the "
                    "values are either strings (one of `bool`, `int`, `float`, `number`, `string`, `date`, "
                    "optionally prefixed by `*` if the value is optional) or a hash describing the field; also sets "
                    "`headers` if not set automatically with `header_names`; if no field type is given, the default "
                    "is `*string`; note that invalid field names given in this option are ignored",
            },
            "header_lines": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftIntType),
                "desc": "the number of headers lines in the file",
            },
            "header_names": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` then the object will parse the header names from the first header row, in this "
                    "case if `header_lines` is not set explicitly, it will be assumed to be `1`",
            },
            "header_reorder": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` (the default) then if `headers` are provided, then data fields are reordered to "
                    "follow headers",
            },
            "headers": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("softlist<string>")),
                "desc": "list of header / column names for the data iterated; if this is present, then "
                    "`header_names` must be `false`",
            },
            "ignore_empty": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true`, empty lines will be ignored",
            },
            "ignore_whitespace": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true`, leading and trailing whitespace will be stripped from non-quoted fields",
            },
            "number_format": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the default format for `int`, `float`, and `number` fields as a string giving the thousands "
                    "separator character followed by the decimal separator character (ex: `.,` for "
                    "continental-European-style numbers)",
            },
            "quote": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the field quote character (default: `\"`)",
            },
            "separator": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the string separating the fields in the file (default: `,`)",
            },
            "timezone": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the timezone region to use when parsing dates (ex: `Europe/Prague`)",
            },
            "tolwr": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` (default `false`), all header names will be converted to lower case letters",
            },
            "verify_columns": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` (default `false`), if a line is parsed with a different column or field count "
                    "than other lines, a `CSVFILEITERATOR-DATA-ERROR` exception is raised",
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
