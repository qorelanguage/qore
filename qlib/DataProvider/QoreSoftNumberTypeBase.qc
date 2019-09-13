# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreSoftNumberTypeBase class definition

/** QoreSoftNumberTypeBase.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! describes a data type based on softnumber with validation for parsing strings
public class QoreSoftNumberTypeBase inherits QoreDataType {
    private {
        #! the format string to convert strings to numbers
        *string format;
    }

    #! creates the object with no format string
    private constructor(Type type) : QoreDataType(type) {
    }

    #! creates the object with a format string
    constructor(Type type, string format) : QoreDataType(type) {
        self.format = format;
    }

    #! sets or clears the format string
    setFormat(*string format) {
        self.format = format;
    }

    #! returns the format string
    *string getFormat() {
        return format;
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        # try to convert from a string
        if (value.typeCode() == NT_STRING) {
            if (format) {
                return parse_number(value, format);
            }
            if (value !~ /^([-+])?[0-9]+(\.[0-9]+)?+$/)
                throw "INVALID-NUMBER", sprintf("value %y cannot be parsed by type \"number\"; the input data contains "
                    "non-numeric text", value);
            return number(value);
        }
        return type.acceptsValue(value);
    }
}
}
