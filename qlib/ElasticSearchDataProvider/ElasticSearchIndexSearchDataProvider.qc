# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ElasticSearchDataProvider module definition

/** ElasticSearchIndexSearchDataProvider.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the ElasticSearchDataProvider module
public namespace ElasticSearchDataProvider {
#! The ElasticSearch index search API data provider
/** This class implements an API to search an index using ElasticSearch's index search API

    The `query` API argument takes a data structure that corresponds to ElasticSearch's JSON Query DSL, for example.
*/
public class ElasticSearchIndexSearchDataProvider inherits ElasticSearchDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "search",
            "desc": "ElasticSearch index search API data provider",
            "type": "ElasticSearchIndexSearchDataProvider",
            "constructor_options": ElasticSearchDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new ElasticSearchIndexSearchRequestDataType();

        #! Response type
        const ResponseType = new ElasticSearchAcknowledgedDataType();

        #! Query args
        const QueryArgs = ("name", "allow_no_indices", "allow_partial_search_results", "analyzer", "analyze_wildcard",
            "batched_reduce_size", "ccs_minimize_roundtrips", "default_operator", "df", "docvalue_fields",
            "expand_wildcards", "explain", "from", "ignore_throttled", "ignore_unavailable", "lenient",
            "max_concurrent_shard_requests", "pre_filter_shard_size", "preference", "q", "request_cache",
            "rest_total_hits_as_int", "routing", "scroll", "search_type", "seq_no_primary_term", "size", "sort",
            "_source", "_source_excludes", "_source_includes", "stats", "stored_fields", "suggest_field",
            "suggest_mode", "suggest_size", "suggest_text", "terminate_after", "timeout", "track_scores",
            "track_total_hits", "typed_keys", "version");
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", ElasticSearchDataProvider::ConstructorOptions, options);
        rest = ElasticSearchDataProvider::getRestConnection(options);
    }

    #! Creates the object from a REST connection
    constructor(RestClient::RestClient rest) : ElasticSearchDataProviderBase(rest) {
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("ElasticSearch data index %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string uri_path = sprintf("%s/_search", remove req.index);
        bool q;
        foreach string arg in (QueryArgs) {
            if (!exists req{arg}) {
                continue;
            }
            if (!q) {
                uri_path += "?";
                q = True;
            } else {
                uri_path += "&";
            }
            if (arg =~ /_query$/) {
                arg =~ s/_query$//;
            }
            uri_path += sprintf("%s=%s", arg, remove req{arg});
        }
        return rest.post(uri_path, req).body;
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider
    */
    private *DataProvider::AbstractDataProviderType getRequestTypeImpl() {
        return RequestType;
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() {
        return ResponseType;
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! ElasticSearch index search API request
class ElasticSearchIndexSearchRequestDataType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            # query parameters
            "index": {
                "type": StringType,
                "desc": "The name of the index to search",
            },
            "allow_no_indices": {
                "type": SoftBoolStringType,
                "desc": "If `false` (default `true`), the request returns an error if any wildcard expression, index "
                    "alias, or `_all` value targets only missing or closed indices. This behavior applies even if "
                    "the request targets other open indices. For example, a request targeting `foo*,bar*` returns an "
                    "error if an index starts with `foo` but no index starts with `bar`",
            },
            "allow_partial_search_results": {
                "type": SoftBoolStringType,
                "desc": "If `true`, returns partial results if there are shard request timeouts or shard failures. "
                    "If `false`, returns an error with no partial results.\n\n"
                    "To override the default for this field, set the `search.default_allow_partial_results` cluster "
                    "setting to `false`"
            },
            "analyzer": {
                "type": StringOrNothingType,
                "desc": "Analyzer to use for the query string.\n\n"
                    "This parameter can only be used when the `q` query string parameter is specified",
            },
            "analyze_wildcard": {
                "type": SoftBoolStringType,
                "desc": "If `true` (default `false`), wildcard and prefix queries are analyzed.\n\n"
                    "This parameter can only be used when the `q` query string parameter is specified",
            },
            "batched_reduce_size": {
                "type": IntOrNothingType,
                "desc": "The number of shard results (default `512`) that should be reduced at once on the "
                    "coordinating node. This value should be used as a protection mechanism to reduce the memory "
                    "overhead per search request if the potential number of shards in the request can be large",
            },
            "ccs_minimize_roundtrips": {
                "type": SoftBoolStringType,
                "desc": "If `true` (the detaul), network round-trips between the coordinating node and the remote "
                    "clusters are minimized when executing cross-cluster search (CCS) requests",
            },
            "default_operator": {
                "type": StringOrNothingType,
                "desc": "The default operator for query string query: `AND` or `OR`\n\n"
                    "This parameter can only be used when the `q` query string parameter is specified",
            },
            "df": {
                "type": StringOrNothingType,
                "desc": "Field to use as default where no field prefix is given in the query string.\n\n"
                    "This parameter can only be used when the `q` query string parameter is specified",
            },
            "docvalue_fields_query": {
                "type": StringOrNothingType,
                "desc": "A comma-separated list of fields to return as the docvalue representation of a field for "
                    "each hit",
            },
            "expand_wildcards": {
                "type": StringOrNothingType,
                "desc": "Type of index that wildcard patterns can match. If the request can target data streams, "
                    "this argument determines whether wildcard expressions match hidden data streams. Supports "
                    "comma-separated values, such as open,hidden. Valid values are:\n"
                    "- `all`: Match any data stream or index, including hidden ones\n"
                    "- `open`: (default) Match open, non-hidden indices. Also matches any non-hidden data stream\n"
                    "- `closed`: Match closed, non-hidden indices. Also matches any non-hidden data stream. Data "
                        "streams cannot be closed\n"
                    "- `hidden`: Match hidden data streams and hidden indices. Must be combined with `open`, "
                        "`closed`, or both\n"
                    "- `none`: Wildcard patterns are not accepted",
            },
            "explain_query": {
                "type": SoftBoolStringType,
                "desc": "If `true` (default `false`), returns detailed information about score computation as part "
                    "of a hit",
            },
            "from_query": {
                "type": IntOrNothingType,
                "desc": "Starting document offset. Needs to be non-negative and defaults to 0.\n\n"
                    "By default, you cannot page through more than 10,000 hits using the from and size parameters. "
                    "To page through more hits, use the `search_after` parameter",
            },
            "ignore_throttled": {
                "type": SoftBoolStringType,
                "desc": "If `true` (the default), concrete, expanded or aliased indices will be ignored when frozen",
            },
            "ignore_unavailable": {
                "type": SoftBoolStringType,
                "desc": "If `false` (the default), the request returns an error if it targets a missing or closed "
                    "index",
            },
            "lenient": {
                "type": SoftBoolStringType,
                "desc": "If `true` (default `false`), format-based query failures (such as providing text to a "
                    "numeric field) in the query string will be ignored.\n\n"
                    "This parameter can only be used when the `q` query string parameter is specified",
            },
            "max_concurrent_shard_requests": {
                "type": IntOrNothingType,
                "desc": "Defines the number of concurrent shard requests per node this search executes concurrently "
                    "(default `5`). This value should be used to limit the impact of the search on the cluster in "
                    "order to limit the number of concurrent shard requests",
            },
            "pre_filter_shard_size": {
                "type": IntOrNothingType,
                "desc": "Defines a threshold that enforces a pre-filter roundtrip to prefilter search shards based "
                    "on query rewriting if the number of shards the search request expands to exceeds the threshold. "
                    "This filter roundtrip can limit the number of shards significantly if for instance a shard can "
                    "not match any documents based on its rewrite method ie. if date filters are mandatory to match "
                    "but the shard bounds and the query are disjoint. When unspecified, the pre-filter phase is "
                    "executed if any of these conditions is met:\n"
                    "- The request targets more than 128 shards\n"
                    "- The request targets one or more read-only index\n"
                    "- The primary sort of the query targets an indexed field",
            },
            "preference": {
                "type": StringOrNothingType,
                "desc": "Nodes and shards used for the search. By default, Elasticsearch selects from eligible nodes "
                    "and shards using adaptive replica selection, accounting for allocation awareness.\n\n"
                    "Valid values for preference:\n"
                    "- `_only_local`: Run the search only on shards on the local node\n"
                    "- `_local`: If possible, run the search on shards on the local node. If not, select shards "
                        "using the default method\n"
                    "- `_only_nodes:<node-id>,<node-id>`: Run the search on only the specified nodes IDs. If "
                        "suitable shards exist on more than one selected node, use shards on those nodes using the "
                        "default method. If none of the specified nodes are available, select shards from any "
                        "available node using the default method\n"
                    "- `_prefer_nodes:<node-id>,<node-id>`: If possible, run the search on the specified nodes IDs. "
                        "If not, select shards using the default method.\n"
                    "- `_shards:<shard>,<shard>`: Run the search only on the specified shards. You can combine this "
                        "value with other preference values. However, the `_shards` value must come first. For "
                        "example: `_shards:2,3|_local`\n"
                    "- `<custom-string>`: Any string that does not start with `_`. If the cluster state and selected "
                        "shards do not change, searches using the same `<custom-string>` value are routed to the "
                        "same shards in the same order",
            },
            "q": {
                "type": StringOrNothingType,
                "desc": "Query in the Lucene query string syntax.\n\n"
                    "You can use the `q` parameter to run a query parameter search. Query parameter searches do not "
                    "support the full Elasticsearch Query DSL but are handy for testing.\n\n"
                    "The `q` parameter overrides the query parameter in the request body",
            },
            "request_cache": {
                "type": SoftBoolStringType,
                "desc": "If `true`, the caching of search results is enabled for requests where size is `0`. "
                    "Defaults to index level settings",
            },
            "rest_total_hits_as_int": {
                "type": SoftBoolStringType,
                "desc": "Indicates whether `hits.total` should be rendered as an integer or an object in the rest "
                    "search response (default `false`)",
            },
            "routing": {
                "type": StringOrNothingType,
                "desc": "Value used to route indexing and search operations to a specific shard",
            },
            "scroll": {
                "type": StringOrNothingType,
                "desc": "Period to retain the search context for scrolling.\n\n"
                    "By default, this value cannot exceed `1d` (24 hours). You can change this limit using the "
                    "`search.max_keep_alive` cluster-level setting",
            },
            "search_type": {
                "type": StringOrNothingType,
                "desc": "How distributed term frequencies are calculated for relevance scoring.\n\n"
                    "Valid values for search_type:\n"
                    "- `query_then_fetch`: (Default) Distributed term frequencies are calculated locally for each "
                        "shard running the search. We recommend this option for faster searches with potentially "
                        "less accurate scoring\n"
                    "- `dfs_query_then_fetch`: Distributed term frequencies are calculated globally, using "
                        "information gathered from all shards running the search. While this option increases the "
                        "accuracy of scoring, it adds a round-trip to each shard, which can result in slower "
                        "searches",
            },
            "seq_no_primary_term_query": {
                "type": SoftBoolStringType,
                "desc": "If `true`, returns sequence number and primary term of the last modification of each hit",
            },
            "size_query": {
                "type": IntOrNothingType,
                "desc": "Defines the number of hits to return.\n\n"
                    "By default, you cannot page through more than 10,000 hits using the `from` and `size` "
                        "parameters. To page through more hits, use the `search_after` parameter",
            },
            "sort": {
                "type": StringOrNothingType,
                "desc": "A comma-separated list of `<field>:<direction>` pairs",
            },
            "_source_query": {
                "type": StringOrNothingType,
                "desc": "Indicates which source fields are returned for matching documents. These fields are "
                    "returned in the `hits._source` property of the search response. Defaults to `true`.\n\n"
                    "Valid values for `_source`:\n"
                    "- `true`: The entire document source is returned\n"
                    "- `false`: The document source is not returned\n"
                    "- `<string>`: Comma-separated list of source fields to return. Wildcard (*) patterns are "
                        "supported",
            },
            "_source_excludes": {
                "type": StringOrNothingType,
                "desc": "A comma-separated list of source fields to exclude from the response.\n\n"
                    "You can also use this parameter to exclude fields from the subset specified in "
                    "`_source_includes` query parameter.\n\n"
                    "If the `_source` parameter is `false`, this parameter is ignored",
            },
            "_source_includes": {
                "type": StringOrNothingType,
                "desc": "A comma-separated list of source fields to include in the response.\n\n"
                    "If this parameter is specified, only these source fields are returned. You can exclude fields "
                    "from this subset using the `_source_excludes` query parameter.\n\n"
                    "If the `_source` parameter is `false`, this parameter is ignored",
            },
            "stats_query": {
                "type": StringOrNothingType,
                "desc": "Specific tag of the request for logging and statistical purposes",
            },
            "stored_fields_query": {
                "type": StringOrNothingType,
                "desc": "A comma-separated list of stored fields to return as part of a hit. If no fields are "
                    "specified, no stored fields are included in the response.\n\n"
                    "If this field is specified, the `_source` parameter defaults to `false`. You can pass "
                    "`_source=true` to return both source fields and stored fields in the search response",
            },
            "suggest_field": {
                "type": StringOrNothingType,
                "desc": "Specifies which field to use for suggestions",
            },
            "suggest_mode": {
                "type": StringOrNothingType,
                "desc": "Specifies the suggest mode. Available options:\n"
                    "- `always`\n"
                    "- `missing`\n"
                    "- `popular`\n\n"
                    "This parameter can only be used when the `suggest_field` and `suggest_text` query string "
                    "parameters are specified",
            },
            "suggest_size": {
                "type": IntOrNothingType,
                "desc": "Number of suggestions to return.\n\n"
                    "This parameter can only be used when the `suggest_field` and `suggest_text` query string "
                    "parameters are specified",
            },
            "suggest_text": {
                "type": StringOrNothingType,
                "desc": "The source text for which the suggestions should be returned.\n\n"
                    "This parameter can only be used when the `suggest_field` query string parameter is specified",
            },
            "terminate_after_query": {
                "type": IntOrNothingType,
                "desc": "Maximum number of documents to collect for each shard. If a query reaches this limit, "
                    "Elasticsearch terminates the query early. Elasticsearch collects documents before sorting.\n\n"
                    "Use with caution. Elasticsearch applies this parameter to each shard handling the request. When "
                    "possible, let Elasticsearch perform early termination automatically. Avoid specifying this "
                    "parameter for requests that target data streams with backing indices across multiple data "
                    "tiers.\n\n"
                    "Defaults to `0`, which does not terminate query execution early",
            },
            "timeout_query": {
                "type": StringOrNothingType,
                "desc": "Specifies the period of time to wait for a response from each shard. If no response is "
                    "received before the timeout expires, the request fails and returns an error. Defaults to no "
                    "timeout",
            },
            "track_scores": {
                "type": SoftBoolStringType,
                "desc": "If `true` (default `false`), calculate and return document scores, even if the scores are "
                    "not used for sorting",
            },
            "track_total_hits": {
                "type": StringOrNothingType,
                "desc": "Number of hits matching the query to count accurately (default `\"10000\"`).\n\n"
                    "If `true`, the exact number of hits is returned at the cost of some performance. If `false`, "
                    "the response does not include the total number of hits matching the query",
            },
            "typed_keys": {
                "type": SoftBoolStringType,
                "desc": "If `true` (the default), aggregation and suggester names are be prefixed by their "
                    "respective types in the response",
            },
            "version_query": {
                "type": SoftBoolStringType,
                "desc": "If `true`, returns document version as part of a hit",
            },

            # body parameters
            "docvalue_fields": {
                "type": SoftAutoListOrNothingType,
                "desc": "Array of field patterns. The request returns values for field names matching these patterns "
                    "in the `hits.fields` property of the response.\n\n"
                    "You can specify items in the array as a string or object.\n\n"
                    "Properties of docvalue_fields objects:\n"
                    "- `field`: (Required, string) Wildcard pattern. The request returns doc values for field names "
                        "matching this pattern.\n"
                    "- `format`: (Optional, string) Format in which the doc values are returned.\n\n"
                    "For date fields, you can specify a date date format. For numeric fields fields, you can specify "
                    "a DecimalFormat pattern.\n\n"
                    "For other field data types, this parameter is not supported",
            },
            "fields": {
                "type": SoftAutoListOrNothingType,
                "desc": "Array of field patterns. The request returns values for field names matching these patterns "
                    "in the `hits.fields` property of the response.\n\n"
                    "You can specify items in the array as a string or object.\n\n"
                    "Properties of fields objects:\n"
                    "- `field`: (Required, string) Field to return. Supports wildcards (*)\n"
                    "- `format`: (Optional, string) Format for date and geospatial fields. Other field data types do "
                        "not support this parameter.\n\n"
                    "  `date` and `date_nanos` fields accept a date format.\n\n"
                    "  `geo_point` and `geo_shape` fields accept:\n"
                    "  - `geojson (default)`: GeoJSON\n"
                    "  - `wkt`: Well Known Text\n"
                    "  - `mvt(<spec>)`: Binary Mapbox vector tile. The API returns the tile as a base64-encoded "
                    "    string. `<spec>` has the format `<zoom>/<x>/<y>` with two optional suffixes: "
                    "    `@<extent>` and/or `:<buffer>`. For example, `2/0/1` or `2/0/1@4096:5`.\n\n"
                    "    mvt parameters:\n"
                    "    - `<zoom>`: (Required, integer) Zoom level for the tile. Accepts `0-29`\n"
                    "    - `<x>`: (Required, integer) X coordinate for the tile\n"
                    "    - `<y>`: (Required, integer) Y coordinate for the tile\n"
                    "    - `<extent>`: (Optional, integer) Size, in pixels, of a side of the tile. Vector tiles are "
                    "      square with equal sides. Defaults to `4096`\n"
                    "    - `<buffer>`: (Optional, integer) Size, in pixels, of a clipping buffer outside the tile. "
                    "      This allows renderers to avoid outline artifacts from geometries that extend past the "
                    "      extent of the tile. Defaults to `5`",
            },
            "stored_fields": {
                "type": StringOrNothingType,
                "desc": "A comma-separated list of stored fields to return as part of a hit. If no fields are "
                    "specified, no stored fields are included in the response.\n\n"
                    "If this option is specified, the `_source` parameter defaults to `false`. You can pass "
                    "`_source: true` to return both source fields and stored fields in the search response",
            },
            "explain": {
                "type": BoolOrNothingType,
                "desc": "If `true` (default `false`), returns detailed information about score computation as part "
                    "of a hit",
            },
            "from": {
                "type": IntOrNothingType,
                "desc": "Starting document offset (default `0`). Needs to be non-negative and defaults to `0`.\n\n"
                    "By default, you cannot page through more than 10,000 hits using the from and size parameters. "
                    "To page through more hits, use the `search_after` parameter",
            },
            "indices_boost": {
                "type": SoftAutoListOrNothingType,
                "desc": "Boosts the `_score` of documents from specified indices.\n\n"
                    "Properties of `indices_boost` objects:\n"
                    "- `<index>: <boost-value>`: (Required, float)\n"
                    "  - `<index>` is the name of the index or index alias. Wildcard (*) expressions are supported\n"
                    "  - `<boost-value>` is the factor by which scores are multiplied. A boost value greater than "
                    "    `1.0` increases the score. A boost value between `0` and `1.0` decreases the score",
            },
            "knn": {
                "type": AutoType,
                "desc": "Defines the kNN query to run.\n\n"
                    "Properties of `knn` object:\n"
                    "- `field`: (Required, string) The name of the vector field to search against. Must be a "
                       "`dense_vector` field with indexing enabled\n"
                    "- `filter`: (Optional, Query DSL object) Query to filter the documents that can match. The kNN "
                       "search will return the top k documents that also match this filter. The value can be a "
                       "single query or a list of queries. If filter is not provided, all documents are allowed to "
                       "match.\n"
                    "- `k`: (Required, integer) Number of nearest neighbors to return as top hits. This value must "
                       "be less than `num_candidates`\n"
                    "- `num_candidates`: (Required, integer) The number of nearest neighbor candidates to consider "
                       "per shard. Cannot exceed 10,000. Elasticsearch collects `num_candidates` results from each "
                       "shard, then merges them to find the top k results. Increasing `num_candidates` tends to "
                       "improve the accuracy of the final k results\n"
                    "- `query_vector`: (Optional, array of floats) Query vector. Must have the same number of "
                       "dimensions as the vector field you are searching against\n"
                    "- `query_vector_builder`: (Optional, object) A configuration object indicating how to build a "
                       "`query_vector` before executing the request. You must provide a `query_vector_builder` or "
                       "`query_vector`, but not both\n"
                    "- `similarity`: (Optional, float) The minimum similarity required for a document to be "
                       "considered a match. The similarity value calculated relates to the raw similarity used. Not "
                       "the document score. The matched documents are then scored according to similarity and the "
                       "provided boost is applied\n\n"
                    "  The similarity parameter is the direct vector similarity calculation.\n\n"
                    "  - `l2_norm`: also known as Euclidean, will include documents where the vector is within the "
                    "    dims dimensional hypersphere with radius similarity with origin at query_vector\n"
                    "  - `cosine` & `dot_product`: Only return vectors where the cosine similarity or dot-product "
                    "    are at least the provided similarity",
            },
            "min_score": {
                "type": FloatOrNothingType,
                "desc": "Minimum `_score` for matching documents. Documents with a lower `_score` are not included "
                    "in the search results",
            },
            "pit": {
                "type": AutoHashOrNothingType,
                "desc": "Limits the search to a point in time (PIT). If you provide a `pit`, you cannot specify a "
                    "`<target>` in the request path.\n\n"
                    "Properties of `pit`:\n"
                    "- `id`: (Required*, string) ID for the PIT to search. If you provide a `pit` object, this "
                    "  parameter is required\n"
                    "- `keep_alive`: (Optional, time value) Period of time used to extend the life of the PIT",
            },
            "query": {
                "type": AutoHashOrNothingType,
                "desc": "Specifies the search using the Query DSL",
            },
            "rank": {
                "type": AutoHashOrNothingType,
                "desc": "Defines a method for combining and ranking result sets from either:\n"
                    "- 1 query and 1 or more kNN searches\n"
                    "- 2 or more kNN searches\n\n"
                    "Ranking methods:\n"
                    "- `rrf`: (Optional, object) Sets the ranking method to reciprocal rank fusion (RRF)",
            },
            "runtime_mappings": {
                "type": AutoHashOrNothingType,
                "desc": "Defines one or more runtime fields in the search request. These fields take precedence over "
                    "mapped fields with the same name.\n\n"
                    "Properties of `runtime_mappings` objects:\n"
                    "- `<field-name>`: (Required, object) Configuration for the runtime field. The key is the field "
                        "name.\n\n"
                    "  Properties of <field-name>:\n"
                    "  - `type`: (Required, string) Field type, which can be any of the following:\n"
                    "    - `boolean`\n"
                    "    - `composite`\n"
                    "    - `date`\n"
                    "    - `double`\n"
                    "    - `geo_point`\n"
                    "    - `ip`\n"
                    "    - `keyword`\n"
                    "    - `long`\n"
                    "    - `lookup`\n"
                    "  - `script`: (Optional, string) Painless script executed at query time. The script has access "
                        "to the entire context of a document, including the original `_source` and any mapped fields "
                        "plus their values.\n\n"
                    "    This script must include emit to return calculated values. For example:\n"
                    "    `\"script\": \"emit(doc['@timestamp'].value.dayOfWeekEnum.toString())\"`",
            },
            "seq_no_primary_term": {
                "type": SoftBoolStringType,
                "desc": "If `true`, returns sequence number and primary term of the last modification of each hit",
            },
            "size": {
                "type": IntOrNothingType,
                "desc": "The number of hits to return. Needs to be non-negative and defaults to `10`.\n\n"
                    "By default, you cannot page through more than 10,000 hits using the `from` and `size` "
                    "parameters. To page through more hits, use the `search_after` parameter",
            },
            "_source": {
                "type": AutoType,
                "desc": "Indicates which source fields are returned for matching documents. These fields are "
                    "returned in the `hits._source` property of the search response.\n\n"
                    "Valid values for `_source`:\n"
                    "- `true`: (Boolean - the default) The entire document source is returned\n"
                    "- `false`: (Boolean) The document source is not returned\n"
                    "- `<wildcard_pattern>`: (string or array of strings) Wildcard (*) pattern or array of patterns "
                        "containing source fields to return\n"
                    "- `<object>`: (object) Object containing a list of source fields to include or exclude\n"
                    "  Properties for `<object>`:\n"
                    "  - `excludes`: (string or array of strings) Wildcard (*) pattern or array of patterns "
                        "containing source fields to exclude from the response\n\n"
                    "    You can also use this property to exclude fields from the subset specified in the "
                        "`includes` property\n"
                    "  - `includes`: (string or array of strings) Wildcard (*) pattern or array of patterns "
                        "containing source fields to return\n\n"
                    "    If this property is specified, only these source fields are returned. You can exclude "
                        "fields from this subset using the `excludes` property",
            },
            "stats": {
                "type": new Type("*softlist<string>"),
                "desc": "Stats groups to associate with the search. Each group maintains a statistics aggregation "
                    "for its associated searches",
            },
            "terminate_after": {
                "type": IntOrNothingType,
                "desc": "Maximum number of documents to collect for each shard. If a query reaches "
                    "this limit, Elasticsearch terminates the query early. Elasticsearch collects documents before "
                    "sorting.\n\n"
                    "Use with caution. Elasticsearch applies this parameter to each shard handling the request. When "
                    "possible, let Elasticsearch perform early termination automatically. Avoid specifying this "
                    "parameter for requests that target data streams with backing indices across multiple data "
                    "tiers.\n\n"
                    "Defaults to `0`, which does not terminate query execution early",
            },
            "timeout": {
                "type": StringOrNothingType,
                "desc": "Specifies the period of time to wait for a response from each shard. If no response is "
                    "received before the timeout expires, the request fails and returns an error. Defaults to no "
                    "timeout",
            },
            "version": {
                "type": SoftBoolStringType,
                "desc": "If `true` (default `false`), returns document version as part of a hit",
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}

#! ElasticSearch index search API response
class ElasticSearchIndexSearchResponseDataType inherits DataProvider::HashDataType {
    #! Creates the object
    constructor() {
        setDefaultOtherFieldType(new ElasticSearchIndexDataTypeBase());
    }
}
}
