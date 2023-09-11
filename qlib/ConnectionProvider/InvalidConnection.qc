# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore InvalidConnection class definition

/*  InvalidConnection.qc Copyright 2016 - 2023 Qore Technologies, s.r.o.

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
%strict-args

#! the ConnectionProvider namespace. All classes used in the ConnectionProvider module should be inside this namespace
public namespace ConnectionProvider {
#! this class is used to mark invalid connections so they can be loaded and reported as invalid
public class InvalidConnection inherits AbstractConnection {
    public {
        #! Error message provided by constructor's \c attributes.error
        string error = "unknown error";
    }

    #! DEPRECATED: creates the invalid connection object
    /** @deprecated since Qore 0.9
    */
    deprecated
    constructor(string n_name, string n_desc, string n_url, *hash n_opts, string n_error, hash urlh)
            : AbstractConnection(n_name, n_desc, n_url, {"monitor": False}, n_opts ?? {}) {
        error = n_error;
    }

    #! creates the invalid connection object
    /** @param name the name of the connection
        @param description connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes. See below
        @param options connection options

        See @ref AbstractConnection::constructor() for \c attributes and \c options reference.

        @par Additional Attributes

            - \c error a custom error string
    */
    constructor(string name, string description, string url, hash<auto> attributes = {}, hash<auto> options = {})
            : AbstractConnection(name, description, url, attributes, options) {
        if (attributes.error.val()) {
            self.error = attributes.error;
        }
    }

    #! Parse the URL to a hash
    /** @param url a string with url
        @return hash untyped, depends on the parsing method

        The base implementation calls @ref Qore::parse_url().

        Reimplement \c parseUrl() if your connection uses different URL scheme.
        Like eg. Qore::Datasource connection string.
    */
    private hash<UrlInfo> parseUrl(string url) {
        try {
            return Qore::parse_url(url);
        } catch (hash<ExceptionInfo> ex) {
            return <UrlInfo>{};
        }
    }

    #! throws an exception because the object is invalid
    private object getImpl(bool connect = True, *hash<auto> rtopts) {
        throw "INVALID-CONNECTION", sprintf("connection %y (type %y url %y) is invalid and therefore could not "
            "be loaded: %s", name, urlh.protocol, url, error);
    }

    #! returns \c "invalid"
    string getType() {
        return "invalid";
    }
}
} # ConnectionProvider namespace
