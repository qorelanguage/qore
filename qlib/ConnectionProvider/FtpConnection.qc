# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore FtpConnection class definition

/*  FtpConnection.qc Copyright 2016 - 2023 Qore Technologies, s.r.o.

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
#! class for FTP connections; returns @ref Qore::FtpClient objects
/** supports the following runtime options in getImpl():
    - \c "path": overrides the path component in the URL at runtime
    - \c "path_add": appends the given string to the path component of the URL at runtime
*/
public class FtpConnection inherits AbstractConnectionWithInfo {
    public {
        #! Connection entry info
        const ConnectionScheme = <ConnectionSchemeInfo>{
            "cls": Class::forName("FtpConnection"),
        };
    }

    #! creates the FtpConnection object
    /** @param name the name of the connection
        @param description connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes. See below
        @param options connection options

        See @ref AbstractConnection::constructor() for \c attributes and \c options reference.

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string description, string url, hash<auto> attributes = {}, hash<auto> options = {})
        : AbstractConnectionWithInfo(name, description, url, attributes, options) {
    }

    #! returns \c "ftp"
    string getType() {
        return "ftp";
    }

    #! returns runtime options
    /** return a hash with the following keys reflecting support for the corresponding runtime options in getImpl():
        - \c "path": overrides the path component in the URL at runtime
        - \c "path_add": appends the given string to the path component of the URL at runtime
    */
    *hash<auto> getRuntimeOptions() {
        return {
            "path": True,
            "path_add": True,
        };
    }

    #! returns an FtpClientDataProvider object for this connection
    /** @return an FtpClientDataProvider object for this connection

        @see hasDataProvider()
    */
    DataProvider::AbstractDataProvider getDataProvider() {
        # to avoid circular dependencies, this object loads the FtpDataProvider and creates the data provider
        # object dynamically
        load_module("FtpClientDataProvider");
        return create_object("FtpClientDataProvider", get());
    }

    #! returns @ref True, as the connection returns a data provider with the @ref getDataProvider() method
    /** @return @ref True, as the connection returns a data provider with the @ref getDataProvider() method

        @see @ref getDataProvider()
    */
    bool hasDataProvider() {
        return True;
    }

    #! Sets child data provider capabilities
    private setChildCapabilities() {
        children_can_support_apis = True;
    }

    #! returns an @ref Qore::FtpClient object
    /** @param connect if @ref True "True" then a connection is made any if there is any path in the URL a \c CWD operation is performed to change to the target directory
        @param rtopts this connection type does not accept any runtime option, so this parameter is ignored
    */
    private FtpClient getImpl(bool connect = True, *hash<auto> rtopts) {
        # set runtime path
        *string rtpath = rtopts.path ?* urlh.path;
        if (rtopts.path_add.val()) {
            if (rtpath !~ /\/$/ && rtopts.path_add !~ /^\//)
                rtpath += "/";
            rtpath += rtopts.path_add;
        }

        # create the object and perform post processing if necessary
        FtpClient fc(url);
        if (connect) {
            fc.connect();
            if (rtpath.val()) {
                fc.cwd(rtpath);
                fc.path = rtpath;
            }
        }
        return fc;
    }

    #! Returns the ConnectionSchemeInfo hash for this object
    private hash<ConnectionSchemeInfo> getConnectionSchemeInfoImpl() {
        return ConnectionScheme;
    }
}
} # ConnectionProvider namespace
