# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore FilesystemConnection class definition

/*  FilesystemConnection.qc Copyright 2016 - 2019 Qore Technologies, s.r.o.

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
    #! class for filesystem connections; returns an object of type @ref Qore::Dir; this can be used to monitor filesystem free space or to ensure a network filesystem is mounted, for example
    /** supports the following option:
        - \c "readonly": does not mark the connection as down if the directory is read-only; use for example if the connection should point to a read-only base directory; interface directories should normally be writable
    */
    public class FilesystemConnection inherits AbstractConnection {
        public {
            #! FilesystemConnection object connection options
            const Options = {
                "readonly": True,
            };

            #! FilesystemConnection object connection option list
            const OptionList = Options.keys();
        }

        #! DEPRECATED: creates the FilesystemConnection object
        /** @param name the name of the connection
            @param desc connection description
            @param url connection URL
            @param monitor monitoring flag
            @param opts connection options
            @param n_urlh broken down URL hash (as returned by @ref Qore::parse_url())

            @deprecated since Qore 0.9
         */
        deprecated
        constructor(string name, string desc, string url, bool monitor, *hash opts, hash n_urlh)
            : AbstractConnection(name, desc, url, {"monitor": monitor}, opts ?? {}) {
            constructorInit();
        }

        #! creates the FilesystemConnection object
        /** @param name the name of the connection
            @param description connection description
            @param url connection URL (potentially with password info)
            @param attributes various attributes. See below
            @param options connection options

            See @ref AbstractConnection::constructor() for \c attributes and \c options reference.
         */
        constructor(string name, string description, string url, hash attributes = {}, hash options = {})
            : AbstractConnection(name, description, url, attributes, options) {
            constructorInit();
        }

        private:internal constructorInit() {
            # the URL hash is parsed with parse_url() which will put a string in the
            # "host" key if it does not look like an absolute path, in which case "host"
            # needs to be prepended back to the path
            if (urlh.host)
                urlh.path = (remove urlh.host) + urlh.path;

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
        /** @param connect if @ref Qore::True "True", then the target directory is checked for existence and if it's writable, if either check fails, an exception is thrown
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
                    throw "CONNECTION-ERROR", sprintf("connection %y (type %y url %y) points to non-existent, inaccessible, or read-only directory %y", name, urlh.protocol, url, urlh.path);
            }
            return dir;
        }

        #! returns a hash that can be used to contruct the object dynamically
        /** @since %ConnectionProvider 1.1
        */
        private hash<ConnectionConstructorInfo> getConstructorInfoImpl() {
            string my_code;
            if (urlh.path.val()) {
                my_code = sprintf("obj.chdir(%y); ", urlh.path);
            }
            if (!opts.readonly) {
                string err_msg = sprintf("connection %y (type %y url %y) points to non-existent, inaccessible, or read-only directory %y", name, urlh.protocol, url, urlh.path);
                my_code += sprintf("if (connect && (!is_writeable(%y) || !obj.exists())) {throw \"CONNECTION-ERROR\", %y;};", urlh.path, err_msg);
            }

            return new hash<ConnectionConstructorInfo>((
                "class_name": "Dir",
                "post_processing": my_code,
            ));
        }

        #! gets options
        /** @return returns a hash with the following supported option:
        - \c "readonly": if @ref Qore::True "True", does not mark the connection as down if the directory is read-only; use for example if the connection should point to a read-only base directory; interface directories should normally be writable (default: @ref Qore::False "False")
         */
        hash getOptions() {
            return Options;
        }

        #! DEPRECATED: static constructor
        /** @deprecated since Qore 0.9 in favor of new constructor and Qore::Reflection
         */
        deprecated
        static FilesystemConnection make(string name, string desc, string url, bool monitor, *hash opts, hash urlh) {
            hash attributes = {
                "monitor": monitor,
            };

            return new FilesystemConnection(name, desc, url, attributes, opts ?? {});
        }
    }
} # ConnectionProvider namespace
