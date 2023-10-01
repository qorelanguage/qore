# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore FileDataProviderBase class definition

/** FileDataProviderBase.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the FileDataProvider module
public namespace FileDataProvider {
#! The base file data provider class
public class FileDataProviderBase inherits DataProvider::AbstractDataProvider {
    public {
        #! Constructor options
        const ConstructorOptions = {
            "lock": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."bool",
                "desc": "If `True` and `root` is set, then no files may be accessed outside of `root`",
            },
            "readonly": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."bool",
                "desc": "If `True` then only read operations will be permitted",
            },
            "root": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Any root directory for relative paths",
            },
        };
    }

    private {
        #! Any anchoring dir for filesystem actions
        *string root;

        #! Lock down access to the root?
        bool lock;

        #! Read only flag
        bool readonly;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (options.root.val()) {
            root = normalize_dir(options.root);
        }
        if (options.lock) {
            lock = True;
        }
        if (options.readonly) {
            readonly = True;
        }
    }

    #! Creates the object from the arguments
    constructor(*string root, *bool lock, *bool readonly) {
        if (root.val()) {
            self.root = root;
        }
        if (lock) {
            self.lock = True;
        }
        if (readonly) {
            self.readonly = True;
        }
    }

    #! Check the given path when "lock" and "root" are set
    private checkPath(string path) {
        path = normalize_dir(path, root);
        if (!path.equalPartialPath(root)) {
            throw "SANDBOX-ERROR", sprintf("%y: attempt to access path outside of sandbox %y", path, root);
        }
    }

    #! Checks if the readonly flag is set
    private checkReadOnly(string op) {
        if (readonly) {
            throw "READONLY-ERROR", sprintf("the read-only flag is set; cannot perform %y operation", op);
        }
    }
}
}