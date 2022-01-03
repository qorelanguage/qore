# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file FileLocationHandlerFile.qc module for returning file data from a URL-like location string

/*  FileLocationHandlerFile Copyright 2022 Qore Technologies, s.r.o.

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
%requires qore >= 0.9.4
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
# strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

%requires Util

#! Contains all public definitions in the FileLocationHandler module
public namespace FileLocationHandler {
#! The class for handling file locations
class FileLocationHandlerFile inherits FileLocationHandler {
    public {
        #! Valid file location options
        /** supported options:
            - \c encoding: the output string encoding for text files
            - \c max_file_len: the maximum number of bytes to read in the file (default: 10 MB)
        */
        const FileLocationOpts = ("encoding", "max_file_len");

        #! Default maximum file length to read (10 MB)
        const MaxFileLen = 10 * 1024 * 1024;
    }

    #! Returns the file data from the given file location
    /** @param scheme the original scheme string in the location
        @param text a flag if the data should be returned as text or not
        @param location the location string

        @throw LOCATION-ERROR invalid file location options
    */
    static data handler(string scheme, bool text, string location) {
        *hash<auto> opts = FileLocationHandler::getOptions(\location);
        # process file options
        if (opts && (*hash<auto> erropts = opts - FileLocationOpts)) {
            throw "LOCATION-ERROR", sprintf("file location %y has unsupported option(s): %y; supported "
                "options: %y", location, keys erropts, FileLocationOpts);
        }
        if (!exists opts.max_file_len) {
            opts.max_file_len = MaxFileLen;
        }

        return text
            ? ReadOnlyFile::readTextFile(substitute_env_vars(location), opts.encoding,
                opts.max_file_len)
            : ReadOnlyFile::readBinaryFile(substitute_env_vars(location), opts.max_file_len);
    }
}
}
