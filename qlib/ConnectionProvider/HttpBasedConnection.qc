# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore HttpBasedConnection class definition

/*  HttpBasedConnection.qc Copyright 2016 - 2019 Qore Technologies, s.r.o.

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
    #! base class for HTTP-based connections that need their URLs rewritten to create the real connection object
    /** Example of the usage is in the RestConnection. Schema \c rest:// is internally used as \c http://
     */
    public class HttpBasedConnection inherits AbstractConnection {
        public {
            #! A string containing real url. With http(s) internal schema.
            string real_url;
        }

        deprecated
        constructor(string name, string desc, string url, bool monitor, *hash<auto> opts, hash<auto> urlh)
            : AbstractConnection(name, desc, url, {"monitor": monitor}, opts ?? {}) {
            constructorInit();
        }

        #! creates the invalid connection object
        /** @param name the name of the connection
            @param description connection description
            @param url connection URL (potentially with password info)
            @param attributes various attributes. See below
            @param options connection options

            See @ref AbstractConnection::constructor() for \c attributes and \c options reference.
         */
        constructor(string name, string desc, string url, hash<auto> attributes, hash<auto> options)
            : AbstractConnection(name, desc, url, attributes, options) {
            constructorInit();
        }

        # sets an HTTP-based URL for the URL argument
        private:internal constructorInit() {
            string scheme = (url =~ x/^([^:]+)/)[0];
            bool ssl = (scheme =~ /s$/);
            string targ = "http" + (ssl ? "s" : "");
            real_url = regex_subst(url, "^" + scheme, targ);
        }
    }
} # ConnectionProvider namespace
