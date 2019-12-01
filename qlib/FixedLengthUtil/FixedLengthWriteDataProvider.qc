# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore FixedLengthWriteDataProvider class definition

/*  FixedLengthWriteDataProvider.qc Copyright 2012 - 2019 Qore Technologies, s.r.o.

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
            "constructor_options": ConstructorOptions,
        };

        #! Constructor options
        const ConstructorOptions = {
            "path": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the path to the fixed-length data; mutually exclusive with \"stream\"",
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
            "opts": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "fixed-length generation options",
            },
        };
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
            writer = new FixedLengthFileWriter(copts.path, copts.spec, copts.opts);
            return;
        }
        if (copts.stream) {
            writer = new FixedLengthWriter(copts.stream, copts.spec, copts.opts);
            return;
        }
        # create a dummy stream output object
        writer = new FixedLengthWriter(new StringOutputStream(), copts.spec, copts.opts);
    }

    #! Returns the description of the record type, if any
    *hash<string, AbstractDataField> getRecordType() {
        return writer.getRecordType();
    }

    #! Creates the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        writer.write(rec);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
} # FixedLengthUtil namespace
