# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ElasticSearchRecordIterator class definition

/** ElasticSearchRecordIterator.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the ElasticSearchDataProvider module
public namespace ElasticSearchDataProvider {
#! Defines the record iterator class for Table-based iterators
public class ElasticSearchRecordIterator inherits DataProvider::AbstractDataProviderRecordIterator {
    public {
    }

    private:internal {
        #! The REST client object for API calls
        RestClient::RestClient rest;

        #! current index name
        string name;

        #! Index hash
        hash<auto> index;

        #! The record type for the object
        hash<string, DataProvider::AbstractDataField> record_type;

        #! record iterator
        Qore::ListHashIterator i;

        #! Query match options
        const MatchOptions = ("operator",);
    }

    #! creates the iterator
    /** @param rest the REST client connection
        @param name the name of the index
        @param index the index definition hash
        @param record_type record type information
        @param where_cond the where clause for the query, if any
        @param search_options search options; assumed to have already been processed for validity before this call
    */
    constructor(RestClient::RestClient rest, string name, hash<auto> index,
            hash<string, DataProvider::AbstractDataField> record_type, *hash<auto> where_cond,
            *hash<auto> search_options) {
        self.rest = rest;
        self.name = name;
        self.index = index;
        self.record_type = record_type;

        string uri = sprintf("%s/_search", name);

        if (search_options) {
            uri = ElasticSearchIndexTableDataProvider::getQueryOptions(uri, search_options,
                ElasticSearchIndexTableDataProvider::SearchQueryOptions);
        }

        # make query
        *hash<auto> query_body = makeQuery(where_cond, search_options);
        hash<auto> info;
        try {
            i = new ListHashIterator((map $1{"_id", "_score"} + $1._source,
                rest.post(uri, query_body, \info).body.hits.hits));
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = {
                "query": query_body,
            } + info{"response-code", "response-body"};
            rethrow ex.err, ex.desc, ex_arg;
        }
    }

    #! Returns @ref True if the iterator is valid
    /**
        @return @ref True if the iterator is valid
    */
    bool valid() {
        return i.valid();
    }

    #! Increments the row pointer when retrieving rows from a select statement
    /**
        @return @ref True if there is a row to retrieve, @ref False if not (no more rows to be retrieved)

        @note Exceptions could be thrown by the DBI driver when the statement is executed; see the relevant DBI driver
        docs for more information
    */
    bool next() {
        return i.next();
    }

    #! Returns a single record if the iterator is valid
    /** @throw INVALID-ITERATOR the iterator is not pointing at a valid element
    */
    hash<auto> getValue() {
        hash<auto> rv = i.getValue();
        /*
        # convert JSON strings to native Qore date/time values
        map rv{$1} = date(rv{$1}), keys record_info.date_time_fields, rv{$1}.val();
        # convert JSON strings to native boolean values
        map rv{$1} = rv{$1} == "true" ? True : False, keys record_info.bool_fields, rv{$1}.val();
        # convert JSON strings to native int values
        map rv{$1} = rv{$1}.toInt(), keys record_info.int_fields, rv{$1}.val();
        */
        return rv;
    }

    #! Returns the value of the given field in the current row, if the iterator is valid
    /** @param key the name of the field

        @return the value of the given field in the current row, if the iterator is valid

        @throw FIELD-ERROR invalid or unknown field name
    */
    auto memberGate(string key) {
        hash<auto> row = getValue();
        auto rv = row{key};
        if (!exists rv && !row.hasKey(key)) {
            throw "FIELD-ERROR", sprintf("the current record does not have field %y; valid fields: %y", key,
                keys row);
        }
        return rv;
    }

    #! Returns the query body
    *hash<auto> makeQuery(*hash<auto> where_cond, *hash<auto> search_options) {
        if (!where_cond) {
            return;
        }
        return {
            "query": {
                "match": map {$1.key: {
                    "query": $1.value
                } + search_options{MatchOptions}}, where_cond.pairIterator(),
            },
        };
    }

    #! Returns the record description, if available
    /** @return the record type of the iterator
    */
    *hash<string, DataProvider::AbstractDataField> getRecordType() {
        return record_type;
    }
}
}
