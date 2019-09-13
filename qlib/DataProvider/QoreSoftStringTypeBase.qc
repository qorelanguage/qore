# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreSoftStringTypeBase class definition

/** QoreSoftStringTypeBase.qc Copyright 2019 Qore Technologies, s.r.o.

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

# minimum required Qore version
%requires qore >= 0.9.4
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

%requires reflection

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes a data type based on softstring with a target encoding
public class QoreSoftStringTypeBase inherits QoreDataType {
    private {
        #! the target encoding for the string when converting
        string encoding;
    }

    #! creates the object with the given target encoding
    constructor(Type type, string encoding = "UTF-8") : QoreDataType(type) {
        setEncoding(encoding);
    }

    #! sets the target encoding
    setEncoding(string encoding) {
        # make sure the encoding is recognized and the internal target encoding
        self.encoding = string("", encoding).encoding();
    }

    #! returns the target encoding
    string getEncoding() {
        return encoding;
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        if (exists value && value !== NULL) {
            if ((value.typeCode() != NT_STRING) || (value.encoding() != encoding)) {
                return string(value, encoding);
            }
            return value;
        }
        return type.acceptsValue(value);
    }
}
}
