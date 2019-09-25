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
#! describes a data type based on softdate with validation for string parsing and time zone support
public class QoreSoftDateTypeBase inherits QoreDataType {
    private {
        #! supported options
        const SupportedOptions = {
            "date.format": <DataProviderTypeOptionInfo>{
                "type": Type::String,
                "desc": "the format string for converting strings to dates",
            },
            "date.input_timezone": <DataProviderTypeOptionInfo>{
                "type": "object<TimeZone>",
                "desc": "the time zone for external data when writing to the type and converting strings to dates",
            },
            "date.output_timezone": <DataProviderTypeOptionInfo>{
                "type": "object<TimeZone>",
                "desc": "the time zone to use when writing to the type",
            },
        };
    }

    #! creates the object with no format string
    private constructor(Type type, *hash<auto> options) : QoreDataType(type, options) {
        # translations are performed on date types if there is an output_timezone
        if (options.output_timezone) {
            remove base_type_hash{NT_DATE};
        }
        # set the input timezone option unconditionally
        if (!options."date.input_timezone") {
            self.options."date.input_timezone" = TimeZone::get();
        }
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        if (exists value && value !== NULL) {
            # try to convert from the source type
            if (value.typeCode() !== NT_DATE) {
                value = options."date.input_timezone".date(value, options."date.format");
            }

            # convert to the output TimeZone if necessary
            if (options."date.output_timezone") {
                value = options."date.output_timezone".date(value);
            }
            return value;
        }
        return type.acceptsValue(value);
    }

    #! returns supported options
    *hash<string, hash<DataProviderTypeOptionInfo>> getSupportedOptions() {
        return QoreDataType::getSupportedOptions() + SupportedOptions;
    }
}
}
