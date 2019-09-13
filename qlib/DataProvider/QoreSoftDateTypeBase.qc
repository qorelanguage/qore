# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreSoftDateTypeBase class definition

/** QoreSoftDateTypeBase.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! hash for date type options
public hashdecl QoreSoftDateOptionInfo {
    #! the format string to convert strings to dates
    *string format;

    #! the input timezone for converting strings to dates
    TimeZone input_timezone = TimeZone::get();

    #! the output timezone for ensuring that output dates are correct
    *TimeZone output_timezone;
}

#! describes a data type based on softdate with validation for string parsing and time zone support
public class QoreSoftDateTypeBase inherits QoreDataType {
    private {
        #! the format string to convert strings to dates
        *string format;

        #! the input timezone for converting strings to dates
        TimeZone input_timezone;

        #! the output timezone for ensuring that output dates are correct
        *TimeZone output_timezone;
    }

    #! creates the object with no format string
    private constructor(Type type, *hash<QoreSoftDateOptionInfo> opts) : QoreDataType(type) {
        self += opts;
    }

    #! sets or clears the format string
    setFormat(*string format) {
        self.format = format;
    }

    #! returns the format string
    *string getFormat() {
        return format;
    }

    #! sets or clears the input timezone
    setInputTimeZone(*TimeZone input_timezone) {
        self.input_timezone = input_timezone;
    }

    #! returns the input timezone
    *TimeZone getInputTimeZone() {
        return input_timezone;
    }

    #! sets or clears the input timezone
    setOutputTimeZone(*TimeZone output_timezone) {
        self.output_timezone = output_timezone;
    }

    #! returns the output timezone
    *TimeZone getOutputTimeZone() {
        return output_timezone;
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        if (exists value & value != NULL) {
            # try to convert from the source type
            if (value.typeCode() !== NT_DATE) {
                value = input_timezone.date(value, format);
            }

            # convert to the output TimeZone if necessary
            if (output_timezone) {
                value = output_timezone.date(value);
            }
            return value;
        }
        return type.acceptsValue(value);
    }
}
}
