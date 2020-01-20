# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file FixedLengthReadDataProvider.qc Qore user module for working with files with fixed lines length

/*  FixedLengthReadDataProvider Copyright 2015 - 2020 Qore Technologies, s.r.o.

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

%require-types
%enable-all-warnings
%new-style

#! The FixedLengthUtil namespace contains all the definitions in the FixedLengthUtil module
public namespace FixedLengthUtil {
#! Provides a data provider for reading fixed-length files
public class FixedLengthReadDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "FixedLengthReadDataProvider",
            "supports_read": True,
            "has_record": True,
            "constructor_options": ConstructorOptions,
        };

        #! Constructor options
        const ConstructorOptions = {
            "path": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the path to the fixed-length data; mutually exclusive with \"stream\"",
            },
            "stream": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("InputStream")),
                "desc": "the input stream for fixed-length data; mutually exclusive with \"path\"",
            },
            "spec": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "the fixed-length record specification hash",
                "required": True,
            },
            "opts": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "fixed-length parsing options",
            },
        };
    }

    private {
        #! The iterator object
        FixedLengthAbstractIterator i;
    }

    #! Creates the FixedLengthReadDataProvider with the input data path, the record specifications, and optionally an option hash
    /** @param path the input file to iterate
        @param spec Fixed-length line specification; see @ref fixedlengthspec for more information
        @param opts a hash of options; see @ref fixedlengthglobals for more information

        @throw FIXED-LENGTH-UTIL-INVALID-SPEC invalid record specification; invalid type or missing field length
    */
    constructor(string path, hash<auto> spec, *hash<auto> opts) {
        i = new FixedLengthFileIterator(path, spec, opts);
    }

    #! Creates the FixedLengthReadDataProvider with the string input data stream, the record specifications, and optionally an option hash
    /** @param stream the string input stream to iterate
        @param spec Fixed-length line specification; see @ref fixedlengthspec for more information
        @param opts a hash of options; see @ref fixedlengthglobals for more information

        @throw FIXED-LENGTH-UTIL-INVALID-SPEC invalid record specification; invalid type or missing field length
    */
    constructor(StringInputStream stream, hash<auto> spec, *hash<auto> opts) {
        i = new FixedLengthIterator(stream, spec, opts);
    }

    #! Creates the FixedLengthReadDataProvider from a binary input data stream, the record specifications, and optionally an option hash
    /** @param stream the input stream to iterate
        @param encoding the encoding of the string data in the binary input stream
        @param spec Fixed-length line specification; see @ref fixedlengthspec for more information
        @param opts a hash of options; see @ref fixedlengthglobals for more information

        @throw FIXED-LENGTH-UTIL-INVALID-SPEC invalid record specification; invalid type or missing field length
    */
    constructor(InputStream stream, *string encoding, hash<auto> spec, *hash<auto> opts) {
        i = new FixedLengthIterator(stream, encoding, spec, opts);
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.path) {
            if (copts.stream) {
                error("CONSTRUCTOR-ERROR", "cannot provide both \"stream\" and \"path\" options; use either one or the other");
            }
            i = new FixedLengthFileIterator(copts.path, copts.spec, copts.opts);
            return;
        }
        if (copts.stream) {
            i = new FixedLengthIterator(copts.stream, copts.spec, copts.opts);
            return;
        }
        # create a dummy interator with no input
        i = new FixedLengthIterator(new StringInputStream(""), copts.spec, copts.opts);
    }

    #! Returns the object name
    string getName() {
        return i instanceof FixedLengthFileIterator
            ? cast<FixedLengthFileIterator>(i).getFileName()
            : sprintf("stream %s", self.uniqueHash());
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        return new DefaultRecordIterator(i, where_cond, search_options);
    }

    #! Returns the description of the record type, if any
    private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        return i.getRecordType();
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
