# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreListDataType class definition

/** QoreListDataType.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes a data type based on a hash
public class QoreListDataType inherits QoreDataType {
    #! creates the object
    constructor(Type type, *hash<auto> options, *hash<auto> tags) : QoreDataType(type, options, tags) {
        init(type.getName());
    }

    #! creates the object
    constructor(string name, Type type, *hash<auto> options, *hash<auto> tags) : QoreDataType(type, options, tags) {
        init(name);
    }

    #! common constructor initialization
    private init(string name) {
        if (type.getBaseTypeCode() != NT_LIST) {
            throw "TYPE-ERROR", sprintf("cannot create a %s object from type %y", self.className(),
                type.getName());
        }

        if (!self.name.val()) {
            self.name = name;
        }
    }
}
}
