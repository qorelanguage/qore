# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore HttpBasedConnection class definition

/*  HttpBasedConnection.qc Copyright 2016 - 2022 Qore Technologies, s.r.o.

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

#! The ConnectionProvider namespace
/** All public declarations in the %ConnectionProvider module are defined in this namespace
*/
public namespace ConnectionProvider {
#! base class for HTTP-based connections that need their URLs rewritten to create the real connection object
/** Example of the usage is in \c RestConnection where the scheme \c rest:// is internally used as \c http://
*/
public class HttpBasedConnection inherits AbstractConnectionWithInfo {
    public {
        #! A string containing the real URL using a http/https scheme
        string real_url;
    }

    #! Creates the connection object
    /** @param name the name of the connection
        @param desc connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes. See below
        @param options connection options

        See @ref AbstractConnection::constructor() for \c attributes and \c options reference.

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string desc, string url, hash<auto> attributes, hash<auto> options)
            : AbstractConnectionWithInfo(name, desc, url, attributes, options) {
        string scheme = (url =~ x/^([^:]+)/)[0];
        bool ssl = (scheme =~ /s$/);
        string targ = "http" + (ssl ? "s" : "");
        real_url = regex_subst(url, "^" + scheme, targ);
    }

    #! returns @ref True, since this connection type supports the %Qore @ref Qore::Socket "Socket-based" polling API
    /** @return @ref True

        @since ConnectionProvider 1.7
    */
    bool supportsPollingApi() {
        return True;
    }

    #! Called to start the connection if the connection supports the polling API
    /**
        @return a socket poll operation object that will allow the connection goal to be reached with polling

        @see supportsPollingApi()

        @since ConnectionProvider 1.7
    */
    Qore::AbstractPollOperation startPollConnect() {
        return getPollImpl().startPollConnect();
    }
}
} # ConnectionProvider namespace
