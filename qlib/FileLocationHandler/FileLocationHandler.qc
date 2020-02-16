# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file FileLocationHandler.qc module for returning file data from a URL-like location string

/*  FileLocationHandler Copyright 2020 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the FileLocationHandler module
public namespace FileLocationHandler {
#! The main class
public class FileLocationHandler {
    private {
        #! The location handler cache; scheme -> handler;
        /** Handlers have the signature: @code{.py} data sub (string scheme, bool text, string location) {} @endcode
        */
        static hash<string, code> cache;

        #! The location handler lock to ensure atomic operations
        static Mutex handler_lock();
    }

    #! Register a new location handler
    /** @param scheme the scheme for the location
        @param handler the handler for the location, must have the signature:
        @code{.py} data sub (string scheme, bool text, string location) {} @endcode

        @throw LOCATION-HANDLER-ERROR the given location handler scheme has already been registered
    */
    static registerHandler(string scheme, code handler) {
        handler_lock.lock();
        on_exit handler_lock.unlock();

        if (cache{scheme}) {
            throw "LOCATION-HANDLER-ERROR", sprintf("a location handler for scheme %y has already been registered",
                scheme);
        }

        cache{scheme} = handler;
    }

    #! Retuns a list of supported schemes
    /** @return a list of supported schemes
    */
    static list<string> getSupportedSchemes() {
        return keys cache;
    }

    #! Returns True if the scheme is supported
    /** @param scheme the scheme to check

        @return True if the scheme is supported
    */
    static bool isSchemeSupported(string scheme) {
        return exists cache{scheme};
    }

    #! Returns string data for the given file location
    /** @param location the location of the file data

        @return the resource data as a string

        @throw LOCATION-ERROR the location string cannotbe parsed; unknown location scheme; invalid location option
    */
    static string getTextFileFromLocation(string location) {
        return FileLocationHandler::getFileFromLocation(True, location);
    }

    #! Returns binary data for the given file location
    /** @param location the location of the file data

        @return the resource data as a binary value

        @throw LOCATION-ERROR the location string cannotbe parsed; unknown location scheme; invalid location option
    */
    static binary getBinaryFileFromLocation(string location) {
        return FileLocationHandler::getFileFromLocation(False, location);
    }

    #! Returns the file data from the given location
    /** @param text a flag if the data should be returned as text or not
        @param location the location string

        @throw LOCATION-ERROR invalid location or empty location string
    */
    static data getFileFromLocation(bool text, string location) {
        if (!location.val()) {
            throw "LOCATION-ERROR", "invalid empty location string";
        }

        *string scheme = (location =~ x/^(\w+):\/\//)[0];
        if (!scheme) {
            return cache.file("file", text, location);
        }
        *code handler = cache{scheme};
        if (!handler) {
            throw "LOCATION-ERROR", sprintf("unknown location scheme %y in location %y; known location schemes: %y", scheme,
                location, keys cache);
        }

        # remove the scheme from the location
        splice location, 0, scheme.length() + 3;

        # run the location handler
        return handler(scheme, text, location);
    }

    #! Helper method to eturn data in the expected format
    /** @param d the file data
        @param text a flag if the data should be returned as a string or not
        @param encoding any encoding for string data

        @return the data in the required format with the expected encoding (if applicable)
    */
    private static data getTextOrBin(data d, bool text, *string encoding) {
        if (text) {
            if (d.typeCode() == NT_BINARY) {
                return d.toString(encoding);
            }
            if (encoding && d.encoding() != encoding) {
                d = convert_encoding(d, encoding);
            }
            return d;
        }
        return d.typeCode() == NT_BINARY
            ? d
            : d.toBinary();
    }

    #! Returns a hash option options from the location string
    private static *hash<auto> getOptions(reference<string> location) {
        *string opt_str = (location =~ x/((?:{).*(?:}))$/)[0];
        if (!opt_str) {
            return;
        }
        # remove option string from location string on exit
        on_exit {
            splice location, -opt_str.size();
        }
        auto v = parse_to_qore_value(opt_str);
        if (v.typeCode() != NT_HASH) {
            if (!v.strp()) {
                throw "LOCATION-ERROR", sprintf("option value in %y cannot be parsed", location);
            }
            return {v: True};
        }
        return v;
    }

    #! Returns a string tagged with the given encoding, if any
    private static string forceEncoding(string str, *string encoding) {
        if (encoding && encoding.lwr() != str.encoding().lwr()) {
            return force_encoding(str, encoding);
        }
        return str;
    }
}
}
