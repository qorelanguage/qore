# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ServiceNowRestDataProviderFactory class definition

/** ServiceNowRestDataProviderFactory.qc Copyright 2022 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the ServiceNowRestDataProvider module
public namespace ServiceNowRestDataProvider {
#! The ServiceNowRest data provider factory
public class ServiceNowRestDataProviderFactory inherits DataProvider::AbstractDataProviderFactory {
    private {
        static Class cls = new Class("ServiceNowRestDataProvider");
    }

    #! Returns the name of the data provider factory
    string getName() {
        return "servicenowrest";
    }

    #! Returns static provider information
    /** @note the \c name and \c children attributes are not returned as they are dynamic attributes
    */
    hash<DataProvider::DataProviderInfo> getInfo() {
        return ServiceNowRestDataProvider::ProviderInfo;
    }

    #! Returns the class for the data provider object
    Class getClass() {
        return cls;
    }
}
}
