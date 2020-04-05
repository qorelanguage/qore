# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProviderRecordProcessor class definition

/** AbstractDataProviderRecordProcessor.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Defines an abstract class for accepting records and outputting a result
public class AbstractDataProviderRecordProcessor {
    #! Returns the expected type of records to be submitted, if available
    /** @return the expected type of records to be submitted, if available

        @note Calls getExpectedRecordTypeImpl() to provide the return value
    */
    *hash<string, AbstractDataField> getExpectedRecordType() {
        return getExpectedRecordTypeImpl();
    }

    #! Returns the type of records that will be returned, if available
    /** @return the type of records that will be returned, if available

        @note Calls getReturnRecordTypeImpl() to provide the return value
    */
    *hash<string, AbstractDataField> getReturnRecordType() {
        return getReturnRecordTypeImpl();
    }

    #! Returns @ref True if the record processor supports bulk operation
    /** @return @ref True if the record processor supports bulk operation

        @note Calls supportsBulkApiImpl() to return the answer
    */
    bool supportsBulkApi() {
        return supportsBulkApiImpl();
    }

    #! Submits the record for processing
    /** @param rec the record to process

        @note Calls submitImpl() on the record to do the actual processing
    */
    hash<auto> submit(hash<auto> rec) {
        return submitImpl(rec);
    }

    #! Returns the expected type of records to be submitted, if available
    /** This base class method returns @ref nothing; reimplement in subclasses to provide a record type
    */
    private *hash<string, AbstractDataField> getExpectedRecordTypeImpl() {
    }

    #! Returns the type of records that will be returned, if available
    /** This base class method returns @ref nothing; reimplement in subclasses to provide a record type
    */
    private *hash<string, AbstractDataField> getReturnRecordTypeImpl() {
    }

    #! Submits the record for processing
    /** @param rec the record to process
    */
    private abstract hash<auto> submitImpl(hash<auto> rec);

    #! Returns @ref True if the record processor supports bulk operation
    /** @return @ref True if the record processor supports bulk operation
    */
    private abstract bool supportsBulkApiImpl();
}
}
