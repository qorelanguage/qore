#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

/*  Copyright (C) 2014 - 2023 Qore Technologies, sro

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

%new-style
%enable-all-warnings
%require-types
%strict-args

# allow child Program objects to have more liberal restrictions than the parent
%no-child-restrictions

%append-module-path ../../qlib
%requires ../../qlib/Util.qm

%exec-class Test

class Test {
    public {
    }

    private {
        int rc = 0;
        *string rstr;
    }

    constructor() {
        doDir(get_script_dir());
        if (rc)
            exit(rc);
    }

    doDir(string dname) {
%ifdef Windows
        dname =~ s/[\\\/]+$//g;
%else
        dname =~ s/\/+$//g;
%endif
        Dir d();
        if (!d.chdir(dname))
            throw "DIR-ERROR", sprintf("directory %y does not exist", dname);

        map doDir(dname + DirSep + $1), d.listDirs();
        map doFile(dname + DirSep + $1), d.listFiles("\\.qtest\$");
    }

    doFile(string fname) {
        printf("running %s\n", fname);

        Program pgm = getTestProgram(fname);
        # save current directory and restore afterwards
        string cwd = getcwd();
        on_exit chdir(cwd);

        pgm.run();
    }

    private Program getTestProgram(string fname) {
        # bare-refs flag
        bool br = False;
        # parse options
        int po = PO_ALLOW_INJECTION|PO_NO_CHILD_PO_RESTRICTIONS;
        # read in file
        string fd = getFile(fname, \br, \po);

        Program pgm(po);
        pgm.setScriptPath(fname);

        # parse the code
        try {
            pgm.parse(fd, fname);
        }
        catch (hash ex) {
            print(fd);
            rethrow;
        }

        return pgm;
    }

    private string getFile(string fname, reference br, reference po) {
        FileLineIterator i(fname);

        string str;

        bool fl;

        while (i.next()) {
            string line = i.getValue();

            if (!fl) {
                fl = True;
                if (line =~ /(\\|\s)qr\s*$/) {
                    po |= PO_NEW_STYLE;
                    br = True;
                }
            }

            if (!br && (line =~ /^%allow-bare-refs/ || line =~ /^%new-style/))
                br = True;

            str += line + "\n";
        }

        return str;
    }
}
