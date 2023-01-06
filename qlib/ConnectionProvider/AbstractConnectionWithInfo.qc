# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore AbstractConnection class definition

/*  AbstractConnectionWithInfo.qc Copyright 2016 - 2023 Qore Technologies, s.r.o.

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
#! abstract base class for connections
/** this class can be specialized to provide for user-defined connection types
*/
public class AbstractConnectionWithInfo inherits AbstractConnection {
    #! creates the AbstractConnection object
    /** @param name the name of the connection
        @param description connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes; see below
        @param options connection options

        @par Attributes
        Attributes are special flags for given connection - stored as public attributes of the object instance.
        For now there are:
        - \c enabled (bool), default \c True
        - \c monitor (bool), default \c True
        - \c locked (bool), default \c False
        - \c tags (hash), no default value

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string description, string url, hash<auto> attributes = {}, hash<auto> options = {})
            : AbstractConnection(name, description, url, attributes, options) {
    }

    #! Returns the ConnectionSchemeInfo hash for this object
    hash<ConnectionSchemeInfo> getConnectionSchemeInfo() {
        return getConnectionSchemeInfoImpl();
    }

    #! returns static / initialization options
    *hash<auto> getOptions() {
        return map {$1: True}, keys getConnectionSchemeInfo().options;
    }

    #! returns default options
    *hash<auto> getDefaultOptions() {
        return map {$1.key: $1.value.default_value}, getConnectionSchemeInfo().options.pairIterator(),
            exists $1.value.default_value;
    }

    #! Returns the ConnectionSchemeInfo hash for this object
    private abstract hash<ConnectionSchemeInfo> getConnectionSchemeInfoImpl();
}
}
