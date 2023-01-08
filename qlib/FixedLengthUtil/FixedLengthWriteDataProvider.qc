# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore FixedLengthWriteDataProvider class definition

/*  FixedLengthWriteDataProvider.qc Copyright 2012 - 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the FixedLengthUtil module
public namespace FixedLengthUtil {
#! Provides a data provider for reading fixed-length files
public class FixedLengthWriteDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "FixedLengthWriteDataProvider",
            "supports_create": True,
            "has_record": True,
            "constructor_options": ConstructorOptions,
        };

        #! Constructor options
        const ConstructorOptions = {
            "path": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the path to the fixed-length data; mutually exclusive with \"stream\"; the output location "
                    "is handled by a call to FileLocationHandler::getOutputStreamForLocation()",
            },
            "stream": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("OutputStream")),
                "desc": "an output stream for fixed-length data; mutually exclusive with \"path\"",
            },
            "spec": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "the fixed-length record specification hash",
                "required": True,
            },
            "date_format": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the default global date format for `date` fields",
            },
            "encoding": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the encoding for strings",
            },
            "eol": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the end of line characters",
            },
            "file_flags": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftIntType),
                "desc": "additional writer file open flags; `O_WRONLY | O_CREAT` is used by default; use `O_EXCL` to "
                    "ensure not to overwrite the target or `O_TRUNC` to replace any existing file or `O_APPEND` to "
                    "append to an existing file",
            },
            "ignore_empty": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "if `true` then empty lines are ignored",
            },
            "number_format": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the default global number format for `float` or `number` fields",
            },
            "timezone": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "a string giving a time zone region name or an integer offset in seconds east of UTC",
            },
            "truncate": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftBoolType),
                "desc": "controls whether to truncate an output field value if its bigger than its specified "
                    "`length`; the default is `false`",
            },
            "tab2space": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(SoftIntType),
                "desc": "Controls whether to replace tabs with spaces and its value determines how many spaces to "
                    "output in place of one tab character",
            },
        };

        #! write option list
        const FixedLengthWriteOptionList = keys (ConstructorOptions - ("path", "stream", "spec"));
    }

    private {
        #! the writer object
        FixedLengthAbstractWriter writer;
    }

    #! Creates the FixedLengthWriteDataProvider with the output path and optionally an option hash
    /** @param path The output file to write
        @param specs Fixed-length line specification; see @ref fixedlengthspec for more information
        @param opts Global options; see @ref fixedlengthglobals for valid values

        @throw FIXED-LENGTH-UTIL-PADDING-ERROR record padding error
    */
    constructor(string path, hash<auto> specs, *hash<auto> opts) {
        writer = new FixedLengthFileWriter(path, specs, opts);
    }

    #! Creates the FixedLengthWriteDataProvider with the output data stream where data will be writeen and an option hash
    /** @param stream the output stream for writing the data
        @param specs Fixed-length line specification; see @ref fixedlengthspec for more information
        @param opts Global options; see @ref fixedlengthglobals for valid values

        @throw FIXED-LENGTH-UTIL-PADDING-ERROR record padding error
    */
    constructor(OutputStream stream, hash<auto> specs, *hash<auto> opts) {
        writer = new FixedLengthWriter(stream, specs, opts);
    }

    #! Returns the object name
    string getName() {
        return writer instanceof FixedLengthFileWriter
            ? (cast<FixedLengthFileWriter>(writer).getFileName() ?? sprintf("file %s", self.uniqueHash()))
            : sprintf("stream %s", self.uniqueHash());
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.path) {
            if (copts.stream) {
                error("CONSTRUCTOR-ERROR", "cannot provide both \"stream\" and \"path\" options; use either one or the other");
            }
            writer = new FixedLengthWriter(FileLocationHandler::getOutputStreamForLocation(copts.path), copts.spec,
                copts{FixedLengthWriteOptionList});
            return;
        }
        if (copts.stream) {
            writer = new FixedLengthWriter(copts.stream, copts.spec, copts{FixedLengthWriteOptionList});
            return;
        }
        # create a dummy stream output object
        writer = new FixedLengthWriter(new StringOutputStream(), copts.spec, copts{FixedLengthWriteOptionList});
    }

    #! Returns the description of the record type, if any
    private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        return writer.getRecordType();
    }

    #! Creates the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        writer.write(rec);
        return rec;
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
} # FixedLengthUtil namespace
