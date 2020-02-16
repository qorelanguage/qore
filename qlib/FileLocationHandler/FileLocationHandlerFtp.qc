# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file FileLocationHandlerFtp.qc module for returning file data from a URL-like location string

/*  FileLocationHandlerFtp Copyright 2020 Qore Technologies, s.r.o.

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
class FileLocationHandlerFtp inherits FileLocationHandler {
    public {
        #! Valid FTP location options
        /** supported options:
            - \c encoding: the output string encoding for text files
        */
        const FtpClientLocationOpts = (
            "encoding",
        );
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
        if (opts && (*hash<auto> erropts = opts - FtpClientLocationOpts)) {
            throw "LOCATION-ERROR", sprintf("file location %y has unsupported option(s): %y; supported "
                "options: %y", location, keys erropts, FtpClientLocationOpts);
        }

        location = scheme + "://" + location;

        string file;
        hash<UrlInfo> url_info = parse_url(location);
        if (url_info.path) {
            file = basename(url_info.path);
        }
        if (!file) {
            throw "LOCATION-ERROR", sprintf("FTP location %y missing file path", location);
        }
        FtpClient f(location);
        f.connect();
        string dir = dirname(url_info.path);
        if (dir) {
            f.cwd(dir);
        }
        if (text) {
            return FileLocationHandler::forceEncoding(f.getAsString(file), opts.encoding);
        }
        return f.getAsBinary(file);
    }
}
}
