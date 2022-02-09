# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file AbstractDataProviderFactory.qc provides the abstract definition for a data provider factory

/** AbstractDataProviderFactory.qc Copyright 2019 - 2022 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider factory class
public class AbstractDataProviderFactory {
    public {}

    #! Creates an object from the given constructor options
    AbstractDataProvider create(*hash<auto> options) {
        ConstructorMethodVariant con;
        try {
            con = getClass().getConstructorVariant(AutoHashOrNothingType);
        } catch (hash<ExceptionInfo> ex) {
            error("DATA-PROVIDER-ERROR", "data provider does not support a constructor from an option hash");
        }
        return con.call(options);
    }

    #! Returns static provider information as data; no objects are returned
    /** @param with_type_info the raw info with objects describing the types

        @note the \c name and \c children attributes are not returned as they are dynamic attributes
    */
    hash<auto> getInfoAsData(*bool with_type_info) {
        return DataProvider::getInfoAsData(getInfo(), with_type_info);
    }

    #! thrown an exception
    private error(string err, string fmt) {
        throw err, sprintf("data provider factory %y (%y): %s", getName(), self.className(), vsprintf(fmt, argv));
    }

    #! Returns the name of the data provider factory
    abstract string getName();

    #! Returns static provider information
    /** @note the \c name and \c children attributes are not returned as they are dynamic attributes
    */
    abstract hash<DataProviderInfo> getInfo();

    #! Returns the class for the data provider object
    abstract Class getClass();
}
}
