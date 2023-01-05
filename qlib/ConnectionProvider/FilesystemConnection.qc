# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore FilesystemConnection class definition

/*  FilesystemConnection.qc Copyright 2016 - 2023 Qore Technologies, s.r.o.

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

#! the ConnectionProvider namespace. All classes used in the ConnectionProvider module should be inside this namespace
public namespace ConnectionProvider {
#! class for filesystem connections; returns an object of type @ref Qore::Dir
/** this can be used to monitor filesystem free space or to ensure a network filesystem is mounted, for example

    supports the following option:
    - \c "readonly": does not mark the connection as down if the directory is read-only; use for example if the
      connection should point to a read-only base directory; interface directories should normally be writable

    @note the path will be subject to environment variable substitutions
*/
public class FilesystemConnection inherits AbstractConnectionWithInfo {
    public {
        #! Connection entry info
        const ConnectionScheme = <ConnectionSchemeInfo>{
            "cls": Class::forName("FilesystemConnection"),
            "options": {
                "readonly": <ConnectionOptionInfo>{
                    "type": "bool",
                    "desc": "does not mark the connection as down if the directory is read only; use for example "
                        "if the connection should point to a read-only base directory; interface directories "
                        "should normally be writable",
                    "default_value": False,
                },
            },
        };
    }

    #! creates the FilesystemConnection object
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
        # the URL hash is parsed with parse_url() which will put a string in the
        # "host" key if it does not look like an absolute path, in which case "host"
        # needs to be prepended back to the path
        if (urlh.host)
            urlh.path = (remove urlh.host) + urlh.path;

        urlh.path = substitute_env_vars(urlh.path);

        if (!absolute_path(urlh.path)) {
%ifdef Windows
            # here we have to guess where the "root" is
            urlh.path = "c:\\" + urlh.path;
%else
            urlh.path = "/" + urlh.path;
%endif
        }

        # sanitize / normalize the path
        urlh.path = normalize_dir(urlh.path);
    }

    #! returns \c "dir"
    string getType() {
        return "dir";
    }

    #! returns a @ref Qore::Dir object
    /** @param connect if @ref True "True", then the target directory is checked for existence and if it's writable,
        if either check fails, an exception is thrown
        @param rtopts this connection type does not accept any runtime option, so this parameter is ignored

        @return a @ref Qore::Dir object
    */
    private Dir getImpl(bool connect = True, *hash rtopts) {
        Dir dir();
        if (urlh.path.val()) {
            dir.chdir(urlh.path);
        }
        if (connect) {
            if (!opts.readonly && (!is_writeable(urlh.path) || !dir.exists()))
                throw "CONNECTION-ERROR", sprintf("connection %y (type %y url %y) points to non-existent, "
                    "inaccessible, or read-only directory %y", name, urlh.protocol, url, urlh.path);
        }
        return dir;
    }

    #! Returns the ConnectionSchemeInfo hash for this object
    private hash<ConnectionSchemeInfo> getConnectionSchemeInfoImpl() {
        return ConnectionScheme;
    }
}
} # ConnectionProvider namespace
