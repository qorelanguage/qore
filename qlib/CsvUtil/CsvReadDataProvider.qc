# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore CsvReadDataProvider class definition

/*  CsvReadDataProvider.qc Copyright 2012 - 2019 Qore Technologies, s.r.o.

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

# assume local var scope, do not use "$" for vars, members, and method calls
%new-style
%strict-args
%require-types
%enable-all-warnings

#! the CsvUtil namespace. All classes used in the CsvUtil module should be inside this namespace
public namespace CsvUtil {
#! Provides a data provider for reading CSV files
public class CsvReadDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "CsvReadDataProvider",
            "supports_read": True,
            "constructor_options": ConstructorOptions,
        };

        #! Constructor options
        const ConstructorOptions = {
            "path": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the path to the CSV data; mutually exclusive with \"stream\"",
            },
            "stream": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("InputStream")),
                "desc": "an input stream for CSV data; mutually exclusive with \"path\"",
            },
            "opts": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "CSV parsing options",
            },
        };
    }

    private {
        #! the iterator object
        AbstractCsvIterator i;
    }

    #! Creates the CsvReadDataProvider with the input data path and optionally an option hash
    /** @param path the input file to iterate
        @param opts a hash of optional options; see @ref abstractcsviterator_options for more information

        @throw ABSTRACTCSVITERATOR-ERROR invalid or unknown option; invalid data type for option; \c "header_names" is @ref True "True" and \c "header_lines" is 0 or \c "headers" is also present; unknown field type
    */
    constructor(string path, *hash<auto> opts) {
        i = new CsvFileIterator(path, opts);
    }

    #! Creates the CsvReadDataProvider with the input data stream and optionally an option hash
    /** @param stream the input stream to iterate
        @param opts a hash of optional options; see @ref abstractcsviterator_options for more information

        @throw ABSTRACTCSVITERATOR-ERROR invalid or unknown option; invalid data type for option; \c "header_names" is @ref True "True" and \c "header_lines" is 0 or \c "headers" is also present; unknown field type
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
                error("CONSTRUCTOR-ERROR", "cannot provide both \"stream\" and \"path\" options; use either one or the other");
            }
            i = new CsvFileIterator(copts.path, copts.opts);
            return;
        }
        if (copts.stream) {
            i = new CsvIterator(copts.stream, copts.opts.encoding, copts.opts);
            return;
        }
        # create a dummy interator with no input
        i = new CsvIterator(new StringInputStream(""), copts.opts.encoding, copts.opts);
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        return new DefaultRecordIterator(i, where_cond, search_options);
    }

    #! Returns the description of the record type, if any
    *hash<string, AbstractDataField> getRecordType() {
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
