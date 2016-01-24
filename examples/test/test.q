#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

/*  Copyright (C) 2014 - 2015 Qore Technologies, sro

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
%requires ../../qlib/UnitTest.qm
%requires ../../qlib/Util.qm

%exec-class Test

class MyUnitTest inherits UnitTest {
    private {
        *softlist rlist;
        *softlist slist;

        const MyOpts = Opts + (
            "skip": "S,skip=s@",
            );
    }

    constructor(*reference p_argv, hash opts = MyOpts) : UnitTest(\p_argv, opts) {
    }

    processOptions(reference p_argv) {
        rlist = remove p_argv;
        if (m_options.slist && rlist) {
            printf("%s: error: cannot use both \"skip\" and inclusive test options in the same command\n", get_script_name());
            exit(1);
        }
        UnitTest::processOptions(\p_argv);
    }

    bool doFile(string fname) {
        # check for tests to be skipped
        foreach string sstr in (m_options.slist) {
            if (regex(fname, sstr))
                return False;
        }

        # check for tests to be expicitly run
        if (!rlist)
            return True;

        foreach string rstr in (rlist) {
            if (regex(fname, rstr))
                return True;
        }
        return False;
    }
}

class Test {
    public {
    }

    private {
        int rc = 0;
        *string rstr;
    }

    constructor() {
        our MyUnitTest unit(\ARGV);

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
        if (!unit.doFile(fname))
            return;

        unit.printLog(sprintf("running %s", fname));

        Program pgm = getTestProgram(fname);
        pgm.run();
    }

    private Program getTestProgram(string fname) {
        # UnitTest variable name detected from the embedded file
        string vn;
        # bare-refs flag
        bool br = False;
        # uses UnitTest?
        bool ut = False;
        # parse options
        int po = PO_ALLOW_INJECTION|PO_NO_CHILD_PO_RESTRICTIONS;
        # read in file
        string fd = getFile(fname, \vn, \br, \ut, \po);

        Program pgm(po);
        pgm.setScriptPath(fname);
        if (ut) {
            pgm.loadModule("UnitTest");
            pgm.importGlobalVariable("unit");
            # add an alias for the $unit variable if the script uses another variable
            if (vn && vn != "unit") {
                *string d = br ? "" : "\$";
                string nd = sprintf("our UnitTest %s%s = %sunit;", d, vn, d);
                fd = replace(fd, "#XXX_MARKER_XXX", nd);
            }
        }

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

    private string getFile(string fname, reference vn, reference br, reference ut, reference po) {
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

            if (!ut && line =~ /^%requires.*UnitTest/)
                ut = True;

            # replace the UnitTest declaration with a marker
            if (*string v = (line =~ x/(((my|our)\s+)?UnitTest\s+(\$)?([a-zA-Z0-9_]+)\(\))\s*;/)[4]) {
                line =~ s/(((my|our)\s+)?UnitTest\s+(\$)?[a-zA-Z0-9_]+\(\))\s*;/#XXX_MARKER_XXX/;
                vn = v;
            }
            else if (!br && (line =~ /^%allow-bare-refs/ || line =~ /^%new-style/))
                br = True;

            str += line + "\n";
        }

        return str;
    }
}
