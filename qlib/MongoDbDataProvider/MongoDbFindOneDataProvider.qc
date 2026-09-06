# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MongoDbFindOneDataProvider class definition

/** MongoDbFindOneDataProvider.qc Copyright 2026 Qore Technologies, s.r.o.

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
#! MongoDB find-one data provider - returns the first document matching a filter
public class MongoDbFindOneDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "MongoDbFindOneDataProvider",
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new MongoDbFindOneRequestDataType();

        #! Response type
        const ResponseType = new MongoDbFindOneResponseDataType();
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
        return "find-one";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "MongoDB find-one data provider - returns the first document matching a filter";
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

    #! Returns the first matching document
    /** @param req the request hash
        @param request_options the request options

        @return a hash holding the document, and whether one was found at all
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        *hash<auto> doc = coll.findOne(req.filter);
        return {
            "document": doc,
            "found": exists doc,
        };
    }
}

#! Find-one request data type
public class MongoDbFindOneRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "filter": {
                "display_name": "Filter",
                "short_desc": "The query filter selecting the document",
                "type": HashOrNothingType,
                "desc": "The query filter selecting the document; the first document in the "
                    "collection is returned when this is empty or `NOTHING`.\n\n"
                    "Use this rather than **Find Documents** with a limit of 1 when at most one "
                    "document is wanted: the result is the document itself rather than a list "
                    "holding it.",
                "example_value": {"_id": "6789"},
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("MongoDbFindOneRequest") {
        addQoreFields(Fields);
    }
}

#! Find-one response data type
public class MongoDbFindOneResponseDataType inherits HashDataType {
    public {
        const Fields = {
            "document": {
                "display_name": "Document",
                "short_desc": "The matching document",
                "type": HashOrNothingType,
                "desc": "The matching document; missing when nothing matched the filter.",
            },
            "found": {
                "display_name": "Found",
                "short_desc": "Whether a document matched the filter",
                "type": BoolType,
                "desc": "Whether a document matched the filter.  Reported separately so that a "
                    "flow can branch on it without having to test the document for emptiness.",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("MongoDbFindOneResponse") {
        addQoreFields(Fields);
    }
}
}
