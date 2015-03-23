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
%require-our
%enable-all-warnings
# allow child Program objects to have more liberal restrictions than the parent
%no-child-restrictions

%append-module-path ../../qlib
%requires ../../qlib/UnitTest.qm
%requires ../../qlib/Util.qm

%exec-class Test

class Test {
    public {
    }

    private {
        int rc = 0;
    }

    constructor() {
        our UnitTest unit();

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
        if (unit.verbose())
            printf("running %s\n", fname);

        Program pgm = getTestProgram(fname);
        pgm.run();
    }

    private Program getTestProgram(string fname) {
        # UnitTest variable name detected from the embedded file
        string vn;
        # bare-refs flag
        bool br = False;
        # read in file
        string fd = getFile(fname, \vn, \br);

        Program pgm();
        pgm.setScriptPath(fname);
        pgm.loadModule("UnitTest");
        pgm.importGlobalVariable("unit");
        # add an alias for the $unit variable if the script uses another variable
        if (vn && vn != "unit") {
            *string d = br ? "" : "\$";
            string nd = sprintf("our UnitTest %s%s = %sunit;", d, vn, d);
            fd = replace(fd, "#XXX_MARKER_XXX", nd);
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

    private string getFile(string fname, reference vn, reference br) {
        FileLineIterator i(fname);

        string str;

        while (i.next()) {
            string line = i.getValue();

            # replace the UnitTest declaration with a marker
            if (*string v = (line =~ x/(((my|our)\s+)?UnitTest\s+(\$)?([a-zA-Z0-9_]+)\(\))\s*;/)[4]) {
                line =~ s/(((my|our)\s+)?UnitTest\s+(\$)?[a-zA-Z0-9_]+\(\))\s*;/#XXX_MARKER_XXX/;
                vn = v;
            }
            else if (line =~ /^%allow-bare-refs/ || line =~ /^%new-style/)
                br = True;

            str += line + "\n";
        }

        return str;
    }
}
