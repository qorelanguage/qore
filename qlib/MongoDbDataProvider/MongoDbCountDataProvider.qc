# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MongoDbCountDataProvider class definition

/** MongoDbCountDataProvider.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace MongoDbDataProvider {
#! MongoDB count data provider - counts the documents matching a filter
public class MongoDbCountDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "MongoDbCountDataProvider",
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new MongoDbCountRequestDataType();

        #! Response type
        const ResponseType = new MongoDbCountResponseDataType();
    }

    private {
        #! The MongoDB client
        MongoClient client;

        #! The database
        MongoDatabase db;

        #! The collection
        MongoCollection coll;
    }

    #! Creates the object from a client, database and collection
    constructor(MongoClient client, MongoDatabase db, MongoCollection coll) {
        self.client = client;
        self.db = db;
        self.coll = coll;
    }

    #! Returns the data provider name
    string getName() {
        return "count";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "MongoDB count data provider - counts the documents matching a filter";
    }

    #! Returns the request type
    private *AbstractDataProviderType getRequestTypeImpl() {
        return RequestType;
    }

    #! Returns the response type
    private *AbstractDataProviderType getResponseTypeImpl() {
        return ResponseType;
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Counts the matching documents
    /** @param req the request hash
        @param request_options the request options

        @return a hash with the number of matching documents
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        return {
            "count": coll.countDocuments(req.filter),
        };
    }
}

#! Count request data type
public class MongoDbCountRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "filter": {
                "display_name": "Filter",
                "short_desc": "The query filter selecting the documents to count",
                "type": HashOrNothingType,
                "desc": "The query filter selecting the documents to count; every document in the "
                    "collection is counted when this is empty or `NOTHING`.\n\n"
                    "This is the same filter **Find Documents** takes, so a count is the cheap way "
                    "to ask how large a result would be before asking for it.",
                "example_value": {"status": "active"},
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("MongoDbCountRequest") {
        addQoreFields(Fields);
    }
}

#! Count response data type
public class MongoDbCountResponseDataType inherits HashDataType {
    public {
        const Fields = {
            "count": {
                "display_name": "Count",
                "short_desc": "The number of matching documents",
                "type": IntType,
                "desc": "The number of documents matching the filter.",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("MongoDbCountResponse") {
        addQoreFields(Fields);
    }
}
}
