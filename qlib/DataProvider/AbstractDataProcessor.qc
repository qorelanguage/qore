# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProcessor class definition

/** AbstractDataProcessor.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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
#! Defines an abstract class for accepting data and outputting optionally transformed or filtered data
public class AbstractDataProcessor {
    #! Returns the expected type of data to be submitted, if available
    /** @return the expected type of data to be submitted, if available

        @note Calls getExpectedTypeImpl() to provide the return value
    */
    AbstractDataProviderType getExpectedType() {
        return getExpectedTypeImpl() ?? AbstractDataProviderType::anyType;
    }

    #! Returns the type of data that will be returned, if available
    /** @return the type of data that will be returned, if available

        @note Calls getReturnTypeImpl() to provide the return value
    */
    AbstractDataProviderType getReturnType() {
        return getReturnTypeImpl() ?? AbstractDataProviderType::anyType;
    }

    #! Returns @ref True if the data processor supports bulk operation
    /** @return @ref True if the data processor supports bulk operation

        @note Calls supportsBulkApiImpl() to return the answer
    */
    bool supportsBulkApi() {
        return supportsBulkApiImpl();
    }

    #! Submits the data for processing
    /** @param _data the data to process

        @throw DPE-SKIP-DATA throw this exception to tell the
        @ref DataProvider::DataProviderPipeline "DataProviderPipeline" skip processing the data for the rest of the
        queue

        @note
        - Calls submitImpl() on the data to do the actual processing
        - Accept and return type information is not enforced in this method; it must be enforced in submitImpl()
    */
    auto submit(auto _data) {
        return submitImpl(_data);
    }

    #! Returns the expected type of data to be submitted, if available
    /** This base class method returns @ref nothing; reimplement in subclasses to provide a type
    */
    private *AbstractDataProviderType getExpectedTypeImpl() {
    }

    #! Returns the type of data that will be returned, if available
    /** This base class method returns @ref nothing; reimplement in subclasses to provide a type
    */
    private *AbstractDataProviderType getReturnTypeImpl() {
    }

    #! Submits the data for processing
    /** @param _data the data to process

        @throw DPE-SKIP-DATA throw this exception to tell the
        @ref DataProvider::DataProviderPipeline "DataProviderPipeline" to skip processing the data for the rest of the
        queue
    */
    private abstract auto submitImpl(auto _data);

    #! Returns @ref True if the data processor supports bulk operation
    /** @return @ref True if the data processor supports bulk operation
    */
    private abstract bool supportsBulkApiImpl();
}
}
