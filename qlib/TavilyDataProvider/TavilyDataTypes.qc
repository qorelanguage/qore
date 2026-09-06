# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore Tavily data type definitions

/** TavilyDataTypes.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace TavilyDataProvider {
#! How hard Tavily works on a search, named rather than shown as raw tokens
/** Tavily rejects anything else with <tt>"Invalid search depth. Must be 'ultra-fast', 'fast',
    'basic' or 'advanced'."</tt>
*/
public const TavilySearchDepthValues = (
    <AllowedValueInfo>{"value": "ultra-fast", "display_name": "Ultra Fast"},
    <AllowedValueInfo>{"value": "fast",       "display_name": "Fast"},
    <AllowedValueInfo>{"value": "basic",      "display_name": "Basic"},
    <AllowedValueInfo>{"value": "advanced",   "display_name": "Advanced"},
);

#! The corpus a search runs against
public const TavilyTopicValues = (
    <AllowedValueInfo>{"value": "general", "display_name": "General"},
    <AllowedValueInfo>{"value": "news",    "display_name": "News"},
    <AllowedValueInfo>{"value": "finance", "display_name": "Finance"},
);

#! How far back a search reaches
/** Tavily accepts the single-letter aliases as well, but a dropdown offering both spellings of the
    same four choices would only invite the question of how they differ.
*/
public const TavilyTimeRangeValues = (
    <AllowedValueInfo>{"value": "day",   "display_name": "Past Day"},
    <AllowedValueInfo>{"value": "week",  "display_name": "Past Week"},
    <AllowedValueInfo>{"value": "month", "display_name": "Past Month"},
    <AllowedValueInfo>{"value": "year",  "display_name": "Past Year"},
);

#! Whether and how hard Tavily writes an answer over the results
public const TavilyIncludeAnswerValues = (
    <AllowedValueInfo>{"value": "basic",    "display_name": "Basic"},
    <AllowedValueInfo>{"value": "advanced", "display_name": "Advanced"},
);

#! The form the full page text is returned in
public const TavilyRawContentValues = (
    <AllowedValueInfo>{"value": "markdown", "display_name": "Markdown"},
    <AllowedValueInfo>{"value": "text",     "display_name": "Plain Text"},
);

#! Whether the listed domains are the only ones allowed, or merely preferred
public const TavilyDomainsModeValues = (
    <AllowedValueInfo>{"value": "filter", "display_name": "Only These Domains"},
    <AllowedValueInfo>{"value": "boost",  "display_name": "Prefer These Domains"},
);

#! How hard Tavily works to extract a page
public const TavilyExtractDepthValues = (
    <AllowedValueInfo>{"value": "basic",    "display_name": "Basic"},
    <AllowedValueInfo>{"value": "advanced", "display_name": "Advanced"},
);

#! The form extracted page content is returned in
/** \c html_tags is accepted by \c "POST /extract" but not by \c "POST /crawl", which is why the two
    actions do not share one list.
*/
public const TavilyExtractFormatValues = (
    <AllowedValueInfo>{"value": "markdown",  "display_name": "Markdown"},
    <AllowedValueInfo>{"value": "text",      "display_name": "Plain Text"},
    <AllowedValueInfo>{"value": "html_tags", "display_name": "HTML Tags"},
);

#! The form crawled page content is returned in
public const TavilyCrawlFormatValues = (
    <AllowedValueInfo>{"value": "markdown", "display_name": "Markdown"},
    <AllowedValueInfo>{"value": "text",     "display_name": "Plain Text"},
);

#! Which model runs a research task
public const TavilyResearchModelValues = (
    <AllowedValueInfo>{"value": "auto", "display_name": "Automatic"},
    <AllowedValueInfo>{"value": "mini", "display_name": "Mini"},
    <AllowedValueInfo>{"value": "pro",  "display_name": "Pro"},
);

#! How a research report cites its sources
public const TavilyCitationFormatValues = (
    <AllowedValueInfo>{"value": "numbered", "display_name": "Numbered"},
    <AllowedValueInfo>{"value": "mla",      "display_name": "MLA"},
    <AllowedValueInfo>{"value": "apa",      "display_name": "APA"},
    <AllowedValueInfo>{"value": "chicago",  "display_name": "Chicago"},
);

#! How long a research report runs
public const TavilyOutputLengthValues = (
    <AllowedValueInfo>{"value": "short",    "display_name": "Short"},
    <AllowedValueInfo>{"value": "standard", "display_name": "Standard"},
    <AllowedValueInfo>{"value": "long",     "display_name": "Long"},
);

#! The endpoints a usage log entry can name
public const TavilyLogEndpointValues = (
    <AllowedValueInfo>{"value": "search",   "display_name": "Search"},
    <AllowedValueInfo>{"value": "extract",  "display_name": "Extract"},
    <AllowedValueInfo>{"value": "crawl",    "display_name": "Crawl"},
    <AllowedValueInfo>{"value": "map",      "display_name": "Map"},
    <AllowedValueInfo>{"value": "research", "display_name": "Research"},
);

#! Where a research task has got to
public const TavilyResearchStatusValues = (
    <AllowedValueInfo>{"value": "pending",     "display_name": "Pending"},
    <AllowedValueInfo>{"value": "in_progress", "display_name": "In Progress"},
    <AllowedValueInfo>{"value": "completed",   "display_name": "Completed"},
    <AllowedValueInfo>{"value": "failed",      "display_name": "Failed"},
);

#! The credits a request consumed
public class TavilyUsageDataType inherits HashDataType {
    private {
        const Fields = {
            "credits": {"display_name": "Credits",
                "desc": "The credits this request consumed.",
                "type": AbstractDataProviderTypeMap."*softint"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request body for \c "POST /search"
public class TavilySearchRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "query": {
                "display_name": "Query",
                "short_desc": "What to search for, in natural language",
                "desc": "What to search for, in natural language.  Tavily searches on meaning, so a "
                    "question reads better here than a list of keywords.\n\n"
                    "Must be at least 2 characters.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "Qore 3.0 release date",
            },
            "search_depth": {
                "display_name": "Search Depth",
                "short_desc": "How hard Tavily works on the query",
                "desc": "How hard Tavily works on the query, trading latency and credits against "
                    "recall.  `ultra-fast` and `fast` answer in well under a second; `advanced` "
                    "re-ranks and reads more of each page, and is the only depth that makes "
                    "`chunks_per_source` meaningful.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilySearchDepthValues,
            },
            "topic": {
                "display_name": "Topic",
                "short_desc": "The corpus the search runs against",
                "desc": "The corpus the search runs against.  `news` favours recent journalism and "
                    "returns a `published_date` with each result; `finance` favours market and "
                    "company sources.  Narrowing here usually beats adding words to the query.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyTopicValues,
            },
            "max_results": {
                "display_name": "Result Count",
                "short_desc": "How many results to return",
                "desc": "How many results to return, from 0 to 20.  Defaults to 10.",
                "type": AbstractDataProviderTypeMap."*softint",
                "example_value": 5,
            },
            "include_answer": {
                "display_name": "Answer",
                "short_desc": "Have Tavily write an answer over the results",
                "desc": "Have Tavily write an answer over the top results and return it as `answer`, "
                    "rather than leaving the caller to read the results.\n\n"
                    "`basic` is a short answer; `advanced` reads more of each source before writing.  "
                    "`True` is accepted and means `basic`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyIncludeAnswerValues,
            },
            "include_raw_content": {
                "display_name": "Full Page Text",
                "short_desc": "Return the full text of each result, not just the snippet",
                "desc": "Return the full text of each result as `raw_content`, in the form named "
                    "here.  This is what makes a search usable as a retrieval step: without it each "
                    "result carries only a short snippet.  `True` is accepted and means `markdown`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyRawContentValues,
            },
            "chunks_per_source": {
                "display_name": "Chunks per Source",
                "short_desc": "How many content chunks to keep from each source",
                "desc": "How many separately-scored content chunks to keep from each source, from 1 "
                    "to 5, or `auto`.  Only has an effect at `advanced` search depth.  Defaults "
                    "to 3.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "example_value": "3",
            },
            "time_range": {
                "display_name": "Time Range",
                "short_desc": "How far back the search reaches",
                "desc": "How far back the search reaches, relative to now.  Use `start_date` and "
                    "`end_date` instead for a fixed window.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyTimeRangeValues,
            },
            "start_date": {
                "display_name": "Published Since",
                "short_desc": "Return only pages published on or after this date",
                "desc": "Return only pages published on or after this date, as `YYYY-MM-DD`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "example_value": "2026-01-01",
            },
            "end_date": {
                "display_name": "Published Before",
                "short_desc": "Return only pages published on or before this date",
                "desc": "Return only pages published on or before this date, as `YYYY-MM-DD`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "example_value": "2026-09-01",
            },
            "include_domains": {
                "display_name": "Include Domains",
                "short_desc": "Domains to restrict the search to, or prefer",
                "desc": "Domains the results may come from - up to 300.  Whether this restricts the "
                    "search or merely prefers those domains is decided by `include_domains_mode`.",
                "type": AbstractDataProviderTypeMap."*softlist",
                "example_value": ("docs.qore.org",),
            },
            "exclude_domains": {
                "display_name": "Exclude Domains",
                "short_desc": "Domains never to return results from",
                "desc": "Domains never to return results from - up to 150.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "include_domains_mode": {
                "display_name": "Domain Mode",
                "short_desc": "Whether the listed domains are required or merely preferred",
                "desc": "Whether `include_domains` is a hard restriction (`filter`) or a preference "
                    "Tavily weighs alongside relevance (`boost`).  A hard restriction on a small "
                    "list can return nothing at all, which is usually why a domain-scoped search "
                    "comes back empty.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyDomainsModeValues,
            },
            "country": {
                "display_name": "Country",
                "short_desc": "Boost results from this country",
                "desc": "Boost results from this country.  Tavily accepts only the names in this "
                    "list and rejects anything else.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyCountryValues,
            },
            "language": {
                "display_name": "Language",
                "short_desc": "Boost results in this language",
                "desc": "Boost results in this language, as an ISO 639-1 code (`en`, `de`) or an "
                    "English language name.  Set `filter_by_language` to exclude other languages "
                    "rather than merely ranking them lower.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "example_value": "en",
            },
            "filter_by_language": {
                "display_name": "Only This Language",
                "short_desc": "Exclude results not in the chosen language",
                "desc": "Exclude results that are not in `language`, rather than ranking them lower.  "
                    "Has no effect unless `language` is set.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "exact_match": {
                "display_name": "Exact Phrase",
                "short_desc": "Require the quoted phrase in the query to appear verbatim",
                "desc": "Require the quoted phrase in the query to appear verbatim.\n\n"
                    "**The query must contain a quoted phrase** - for example "
                    "`\"John Smith\" CEO` - or Tavily rejects the request.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "auto_parameters": {
                "display_name": "Automatic Parameters",
                "short_desc": "Let Tavily choose the search parameters from the query",
                "desc": "Let Tavily infer the search parameters - depth, topic and the rest - from "
                    "the query itself, and report what it chose as `auto_parameters` in the "
                    "response.  Anything set explicitly still wins.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "safe_search": {
                "display_name": "Safe Search",
                "short_desc": "Filter out adult and explicit results",
                "desc": "Filter out adult and explicit results.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_images": {
                "display_name": "Images",
                "short_desc": "Return images found alongside the results",
                "desc": "Return images found alongside the results, as an `images` list.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_image_descriptions": {
                "display_name": "Image Descriptions",
                "short_desc": "Describe each returned image",
                "desc": "Add a written description to each returned image.  Has no effect unless "
                    "`include_images` is set.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_favicon": {
                "display_name": "Favicons",
                "short_desc": "Return each result's favicon URL",
                "desc": "Return each result's favicon URL, for rendering a source list.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_usage": {
                "display_name": "Credit Usage",
                "short_desc": "Return what this request cost",
                "desc": "Return what this request cost in credits, as `usage`.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "days": {
                "display_name": "Days (legacy)",
                "short_desc": "Superseded by Time Range; restricts news searches to the last N days",
                "desc": "Restricts a `news` search to the last N days.\n\n"
                    "**Superseded by `time_range`**, which applies to every topic; this option is "
                    "still accepted so that existing flows keep working.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! An image returned with a search
public class TavilySearchImageDataType inherits HashDataType {
    private {
        const Fields = {
            "url": {"display_name": "URL",
                "desc": "The image URL.",
                "type": AbstractDataProviderTypeMap."*string"},
            "description": {"display_name": "Description",
                "desc": "What the image shows, when `include_image_descriptions` was set.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! One result of a search
public class TavilySearchResultDataType inherits HashDataType {
    private {
        const Fields = {
            "id": {"display_name": "ID",
                "desc": "Tavily's identifier for this result.",
                "type": AbstractDataProviderTypeMap."*string"},
            "title": {"display_name": "Title",
                "desc": "The page title.",
                "type": AbstractDataProviderTypeMap."*string"},
            "url": {"display_name": "URL",
                "desc": "The page URL.",
                "type": AbstractDataProviderTypeMap."*string"},
            "content": {"display_name": "Content",
                "desc": "The passage of the page that matched, not the whole page.",
                "type": AbstractDataProviderTypeMap."*string"},
            "raw_content": {"display_name": "Full Page Text",
                "desc": "The full page text, when `include_raw_content` was set.",
                "type": AbstractDataProviderTypeMap."*string"},
            "score": {"display_name": "Score",
                "desc": "How relevant Tavily judged this result, from 0 to 1.",
                "type": AbstractDataProviderTypeMap."*float"},
            "published_date": {"display_name": "Published Date",
                "desc": "When the page was published; returned for the `news` topic.",
                "type": AbstractDataProviderTypeMap."*string"},
            "favicon": {"display_name": "Favicon",
                "desc": "The site's favicon URL, when `include_favicon` was set.",
                "type": AbstractDataProviderTypeMap."*string"},
            "images": {"display_name": "Images",
                "desc": "Images found on this page, when `include_images` was set.",
                "type": AbstractDataProviderTypeMap."*list"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "POST /search"
public class TavilySearchResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "query": {"display_name": "Query",
                "desc": "The query that was run.",
                "type": AbstractDataProviderTypeMap."*string"},
            "answer": {"display_name": "Answer",
                "desc": "The answer Tavily wrote over the results, when `include_answer` was set.",
                "type": AbstractDataProviderTypeMap."*string"},
            "results": {"display_name": "Results",
                "desc": "The results, most relevant first.",
                "type": new ListDataType("TavilySearchResultList",
                    new TavilySearchResultDataType(), True)},
            "images": {"display_name": "Images",
                "desc": "Images found alongside the results, when `include_images` was set.",
                "type": new ListDataType("TavilySearchImageList",
                    new TavilySearchImageDataType(), True)},
            "auto_parameters": {"display_name": "Automatic Parameters",
                "desc": "The parameters Tavily chose, when `auto_parameters` was set.",
                "type": AbstractDataProviderTypeMap."*hash"},
            "follow_up_questions": {"display_name": "Follow-Up Questions",
                "desc": "Questions Tavily suggests asking next.",
                "type": AbstractDataProviderTypeMap."*list"},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the request took, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
            "usage": {"display_name": "Credit Usage",
                "desc": "What the request cost, when `include_usage` was set.",
                "type": new TavilyUsageDataType()},
            "request_id": {"display_name": "Request ID",
                "desc": "Tavily's identifier for this request, to quote to their support.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request body for \c "POST /extract"
public class TavilyExtractRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "urls": {
                "display_name": "URLs",
                "short_desc": "The pages to read",
                "desc": "The pages to read.  Each is fetched and reduced to its main content, with "
                    "the navigation, advertising and boilerplate removed.",
                "type": AbstractDataProviderTypeMap."softlist",
                "required": True,
                "example_value": ("https://qore.org",),
            },
            "query": {
                "display_name": "Query",
                "short_desc": "Keep only the parts of each page relevant to this",
                "desc": "Keep only the parts of each page relevant to this query, rather than the "
                    "whole of it.  This is what keeps a long page from filling a model's context "
                    "window with the parts nobody asked about.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
            "extract_depth": {
                "display_name": "Extraction Depth",
                "short_desc": "How hard Tavily works to read the page",
                "desc": "How hard Tavily works to read the page.  `advanced` recovers content from "
                    "pages that render it with scripts, at higher cost and latency; `basic` is "
                    "enough for ordinary articles and documentation.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyExtractDepthValues,
            },
            "format": {
                "display_name": "Format",
                "short_desc": "The form the page text is returned in",
                "desc": "The form the page text is returned in.  `markdown` keeps headings, lists "
                    "and links; `text` is prose only; `html_tags` keeps the original tags.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyExtractFormatValues,
            },
            "chunks_per_source": {
                "display_name": "Chunks per Source",
                "short_desc": "How many relevant chunks to keep from each page",
                "desc": "How many separately-scored chunks to keep from each page, from 1 to 5.  "
                    "Has an effect only together with `query`.  Defaults to 3.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "timeout": {
                "display_name": "Timeout",
                "short_desc": "How long to wait for each page, in seconds",
                "desc": "How long to wait for each page, in seconds, from 1 to 60.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "include_images": {
                "display_name": "Images",
                "short_desc": "Return the images found on each page",
                "desc": "Return the images found on each page.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_favicon": {
                "display_name": "Favicons",
                "short_desc": "Return each page's favicon URL",
                "desc": "Return each page's favicon URL, for rendering a source list.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_usage": {
                "display_name": "Credit Usage",
                "short_desc": "Return what this request cost",
                "desc": "Return what this request cost in credits, as `usage`.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! One page read by \c "POST /extract"
public class TavilyExtractResultDataType inherits HashDataType {
    private {
        const Fields = {
            "url": {"display_name": "URL",
                "desc": "The page that was read.",
                "type": AbstractDataProviderTypeMap."*string"},
            "title": {"display_name": "Title",
                "desc": "The page title.",
                "type": AbstractDataProviderTypeMap."*string"},
            "raw_content": {"display_name": "Content",
                "desc": "The page's main content, in the requested format.",
                "type": AbstractDataProviderTypeMap."*string"},
            "images": {"display_name": "Images",
                "desc": "Images found on the page, when `include_images` was set.",
                "type": AbstractDataProviderTypeMap."*list"},
            "favicon": {"display_name": "Favicon",
                "desc": "The site's favicon URL, when `include_favicon` was set.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! A page \c "POST /extract" could not read
public class TavilyFailedResultDataType inherits HashDataType {
    private {
        const Fields = {
            "url": {"display_name": "URL",
                "desc": "The page that could not be read.",
                "type": AbstractDataProviderTypeMap."*string"},
            "error": {"display_name": "Error",
                "desc": "Why it could not be read.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "POST /extract"
public class TavilyExtractResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "results": {"display_name": "Results",
                "desc": "The pages that were read.",
                "type": new ListDataType("TavilyExtractResultList",
                    new TavilyExtractResultDataType(), True)},
            "failed_results": {"display_name": "Failed Results",
                "desc": "The pages that could not be read, each with the reason.  A partial "
                    "success is reported here rather than as an error, so this is worth reading "
                    "even when the request succeeded.",
                "type": new ListDataType("TavilyFailedResultList",
                    new TavilyFailedResultDataType(), True)},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the request took, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
            "usage": {"display_name": "Credit Usage",
                "desc": "What the request cost, when `include_usage` was set.",
                "type": new TavilyUsageDataType()},
            "request_id": {"display_name": "Request ID",
                "desc": "Tavily's identifier for this request, to quote to their support.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request body for \c "POST /crawl"
public class TavilyCrawlRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "url": {
                "display_name": "Start URL",
                "short_desc": "The page the crawl starts from",
                "desc": "The page the crawl starts from.  Everything reachable from it, within the "
                    "limits below, is fetched and read.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "https://docs.qore.org",
            },
            "instructions": {
                "display_name": "Instructions",
                "short_desc": "Describe in natural language which pages are wanted",
                "desc": "Describe in natural language which pages are wanted - \"the API reference "
                    "pages\", \"anything about pricing\" - and Tavily decides which links to follow.  "
                    "This is usually easier to get right than the path and domain patterns below, "
                    "and can be combined with them.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
            "max_depth": {
                "display_name": "Maximum Depth",
                "short_desc": "How many links away from the start URL to go",
                "desc": "How many links away from the start URL the crawl may go, from 1 to 5.  "
                    "Defaults to 1, which reads only the pages the start URL links to directly.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "max_breadth": {
                "display_name": "Maximum Breadth",
                "short_desc": "How many links to follow from each page",
                "desc": "How many links to follow from each page, from 1 to 500.  Defaults to 20.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "limit": {
                "display_name": "Page Limit",
                "short_desc": "How many pages to read in total",
                "desc": "How many pages to read in total.  Defaults to 50.  This is the ceiling on "
                    "what the crawl costs, so set it before setting the depth and breadth.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "select_paths": {
                "display_name": "Only These Paths",
                "short_desc": "Regular expressions the path must match",
                "desc": "Regular expressions the URL path must match for a page to be read, for "
                    "example `/docs/.*`.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "exclude_paths": {
                "display_name": "Exclude Paths",
                "short_desc": "Regular expressions that exclude a path",
                "desc": "Regular expressions that exclude a page from the crawl by its URL path.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "select_domains": {
                "display_name": "Only These Domains",
                "short_desc": "Regular expressions the domain must match",
                "desc": "Regular expressions the domain must match for a page to be read, for "
                    "example `^docs\\.qore\\.org$`.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "exclude_domains": {
                "display_name": "Exclude Domains",
                "short_desc": "Regular expressions that exclude a domain",
                "desc": "Regular expressions that exclude a domain from the crawl.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "allow_external": {
                "display_name": "Follow External Links",
                "short_desc": "Whether the crawl may leave the starting site",
                "desc": "Whether the crawl may follow links off the starting site.  Defaults to "
                    "`True`, so a crawl meant to stay on one site should set this to `False`.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "extract_depth": {
                "display_name": "Extraction Depth",
                "short_desc": "How hard Tavily works to read each page",
                "desc": "How hard Tavily works to read each page it fetches.  `advanced` recovers "
                    "content from pages that render it with scripts, at higher cost and latency.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyExtractDepthValues,
            },
            "format": {
                "display_name": "Format",
                "short_desc": "The form each page's text is returned in",
                "desc": "The form each page's text is returned in.  `markdown` keeps headings, "
                    "lists and links; `text` is prose only.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyCrawlFormatValues,
            },
            "chunks_per_source": {
                "display_name": "Chunks per Page",
                "short_desc": "How many relevant chunks to keep from each page",
                "desc": "How many separately-scored chunks to keep from each page, from 1 to 5.  "
                    "Defaults to 3.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "timeout": {
                "display_name": "Timeout",
                "short_desc": "How long the whole crawl may take, in seconds",
                "desc": "How long the whole crawl may take, in seconds, from 10 to 150.  Defaults "
                    "to 150.  A crawl that runs out of time returns what it has.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "include_images": {
                "display_name": "Images",
                "short_desc": "Return the images found on each page",
                "desc": "Return the images found on each page.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_favicon": {
                "display_name": "Favicons",
                "short_desc": "Return each page's favicon URL",
                "desc": "Return each page's favicon URL, for rendering a source list.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "include_usage": {
                "display_name": "Credit Usage",
                "short_desc": "Return what this request cost",
                "desc": "Return what this request cost in credits, as `usage`.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! One page read by \c "POST /crawl"
public class TavilyCrawlResultDataType inherits HashDataType {
    private {
        const Fields = {
            "url": {"display_name": "URL",
                "desc": "The page that was read.",
                "type": AbstractDataProviderTypeMap."*string"},
            "raw_content": {"display_name": "Content",
                "desc": "The page's main content, in the requested format.",
                "type": AbstractDataProviderTypeMap."*string"},
            "images": {"display_name": "Images",
                "desc": "Images found on the page, when `include_images` was set.",
                "type": AbstractDataProviderTypeMap."*list"},
            "favicon": {"display_name": "Favicon",
                "desc": "The site's favicon URL, when `include_favicon` was set.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "POST /crawl"
public class TavilyCrawlResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "base_url": {"display_name": "Start URL",
                "desc": "The page the crawl started from.",
                "type": AbstractDataProviderTypeMap."*string"},
            "results": {"display_name": "Pages",
                "desc": "The pages that were read, each with its content.",
                "type": new ListDataType("TavilyCrawlResultList",
                    new TavilyCrawlResultDataType(), True)},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the crawl took, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
            "usage": {"display_name": "Credit Usage",
                "desc": "What the crawl cost, when `include_usage` was set.",
                "type": new TavilyUsageDataType()},
            "request_id": {"display_name": "Request ID",
                "desc": "Tavily's identifier for this request, to quote to their support.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request body for \c "POST /map"
public class TavilyMapRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "url": {
                "display_name": "Start URL",
                "short_desc": "The page the map starts from",
                "desc": "The page the map starts from.  Only the links are followed; no page "
                    "content is fetched, which is what makes this far cheaper than a crawl.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "https://docs.qore.org",
            },
            "instructions": {
                "display_name": "Instructions",
                "short_desc": "Describe in natural language which pages are wanted",
                "desc": "Describe in natural language which pages are wanted, and Tavily decides "
                    "which links to follow.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
            "max_depth": {
                "display_name": "Maximum Depth",
                "short_desc": "How many links away from the start URL to go",
                "desc": "How many links away from the start URL the map may go, from 1 to 5.  "
                    "Defaults to 1.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "max_breadth": {
                "display_name": "Maximum Breadth",
                "short_desc": "How many links to follow from each page",
                "desc": "How many links to follow from each page, from 1 to 500.  Defaults to 20.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "limit": {
                "display_name": "URL Limit",
                "short_desc": "How many URLs to return in total",
                "desc": "How many URLs to return in total.  Defaults to 50.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "select_paths": {
                "display_name": "Only These Paths",
                "short_desc": "Regular expressions the path must match",
                "desc": "Regular expressions the URL path must match for a URL to be returned.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "exclude_paths": {
                "display_name": "Exclude Paths",
                "short_desc": "Regular expressions that exclude a path",
                "desc": "Regular expressions that exclude a URL by its path.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "select_domains": {
                "display_name": "Only These Domains",
                "short_desc": "Regular expressions the domain must match",
                "desc": "Regular expressions the domain must match for a URL to be returned.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "exclude_domains": {
                "display_name": "Exclude Domains",
                "short_desc": "Regular expressions that exclude a domain",
                "desc": "Regular expressions that exclude a domain.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "allow_external": {
                "display_name": "Follow External Links",
                "short_desc": "Whether the map may leave the starting site",
                "desc": "Whether the map may follow links off the starting site.  Defaults to "
                    "`True`, so a map meant to stay on one site should set this to `False`.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
            "timeout": {
                "display_name": "Timeout",
                "short_desc": "How long the whole map may take, in seconds",
                "desc": "How long the whole map may take, in seconds, from 10 to 150.  Defaults "
                    "to 150.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "include_usage": {
                "display_name": "Credit Usage",
                "short_desc": "Return what this request cost",
                "desc": "Return what this request cost in credits, as `usage`.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "POST /map"
public class TavilyMapResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "base_url": {"display_name": "Start URL",
                "desc": "The page the map started from.",
                "type": AbstractDataProviderTypeMap."*string"},
            "results": {"display_name": "URLs",
                "desc": "The URLs that were discovered.",
                "type": AbstractDataProviderTypeMap."*list"},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the map took, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
            "usage": {"display_name": "Credit Usage",
                "desc": "What the map cost, when `include_usage` was set.",
                "type": new TavilyUsageDataType()},
            "request_id": {"display_name": "Request ID",
                "desc": "Tavily's identifier for this request, to quote to their support.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! A file attached to a research task
public class TavilyResearchFileDataType inherits HashDataType {
    public {
        const Fields = {
            "name": {
                "display_name": "File Name",
                "short_desc": "The file name, including its extension",
                "desc": "The file name, including its extension.  Tavily accepts `.txt`, `.md` "
                    "and `.json`.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "q1-report.md",
            },
            "data": {
                "display_name": "Contents",
                "short_desc": "The file contents, base64-encoded",
                "desc": "The file contents, base64-encoded.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
            },
            "type": {
                "display_name": "Encoding",
                "short_desc": "How the contents are encoded",
                "desc": "How the contents are encoded.  Defaults to `base64`, which is the only "
                    "encoding Tavily documents.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request body for \c "POST /research"
public class TavilyResearchRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "input": {
                "display_name": "Task",
                "short_desc": "What to research, in natural language",
                "desc": "What to research, in natural language.  This is a task rather than a "
                    "query: the agent searches, reads and reasons until it can answer, so it can "
                    "be given something no single search would settle.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "Compare the licence terms of the three largest commercial "
                    "alternatives",
            },
            "model": {
                "display_name": "Model",
                "short_desc": "Which model runs the task, trading speed against depth",
                "desc": "Which model runs the task, trading speed and cost against depth.  `auto` "
                    "lets Tavily choose from the task itself.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyResearchModelValues,
            },
            "output_length": {
                "display_name": "Report Length",
                "short_desc": "How long the report should be",
                "desc": "How long the report should be.  Defaults to `standard`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyOutputLengthValues,
            },
            "output_schema": {
                "display_name": "Output Schema",
                "short_desc": "A JSON Schema describing the answer you want back",
                "desc": "A JSON Schema describing the answer you want back, which makes `content` "
                    "structured data rather than prose - the difference between a report someone "
                    "reads and a result a flow can act on.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "citation_format": {
                "display_name": "Citation Format",
                "short_desc": "How the report cites its sources",
                "desc": "How the report cites its sources within `content`.  The sources "
                    "themselves are always returned separately as `sources`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": TavilyCitationFormatValues,
            },
            "include_domains": {
                "display_name": "Preferred Domains",
                "short_desc": "Domains the agent should prefer, up to 20",
                "desc": "Domains the agent should prefer - a soft preference, not a restriction - "
                    "up to 20.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "exclude_domains": {
                "display_name": "Excluded Domains",
                "short_desc": "Domains the agent must not use, up to 20",
                "desc": "Domains the agent must not use - a hard blocklist - up to 20.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "files": {
                "display_name": "Files",
                "short_desc": "Documents of your own for the agent to read, up to 5",
                "desc": "Documents of your own for the agent to read alongside what it finds on "
                    "the web - up to 5 files and 80,000 words in total.  This is how a research "
                    "task is grounded in something the web does not have.",
                "type": new ListDataType("TavilyResearchFileList",
                    new TavilyResearchFileDataType(), True),
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "POST /research"
public class TavilyResearchTaskDataType inherits HashDataType {
    private {
        const Fields = {
            "request_id": {"display_name": "Task ID",
                "desc": "The task's identifier; read the finished report with **Get Research "
                    "Report** using this value.",
                "type": AbstractDataProviderTypeMap."*string"},
            "status": {"display_name": "Status",
                "desc": "Where the task has got to; `pending` when it has just been started.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": TavilyResearchStatusValues},
            "input": {"display_name": "Task",
                "desc": "The task that was started.",
                "type": AbstractDataProviderTypeMap."*string"},
            "model": {"display_name": "Model",
                "desc": "The model that is running the task.",
                "type": AbstractDataProviderTypeMap."*string"},
            "created_at": {"display_name": "Created",
                "desc": "When the task was created.",
                "type": AbstractDataProviderTypeMap."*string"},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the request to start the task took, in seconds - not how long "
                    "the research itself takes.",
                "type": AbstractDataProviderTypeMap."*float"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request for \c "GET /research/{request_id}"
public class TavilyGetResearchRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "request_id": {
                "display_name": "Task ID",
                "short_desc": "The research task to read",
                "desc": "The research task to read, as returned by **Start Research** in "
                    "`request_id`.\n\n"
                    "The dropdown lists the research tasks this account has started, taken from the "
                    "usage log.  The log is written a few minutes after a task starts, so a task "
                    "begun moments ago is not in it yet: use the `request_id` **Start Research** "
                    "returned.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "ref_data": TavilyDataProviderCommon::RefResearchTask,
                "example_value": "123e4567-e89b-12d3-a456-426614174111",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! A source a research report drew on
public class TavilyResearchSourceDataType inherits HashDataType {
    private {
        const Fields = {
            "url": {"display_name": "URL",
                "desc": "The source page.",
                "type": AbstractDataProviderTypeMap."*string"},
            "title": {"display_name": "Title",
                "desc": "The source's title.",
                "type": AbstractDataProviderTypeMap."*string"},
            "favicon": {"display_name": "Favicon",
                "desc": "The site's favicon URL.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "GET /research/{request_id}"
public class TavilyResearchReportDataType inherits HashDataType {
    private {
        const Fields = {
            "request_id": {"display_name": "Task ID",
                "desc": "The task this report belongs to.",
                "type": AbstractDataProviderTypeMap."*string"},
            "status": {"display_name": "Status",
                "desc": "Where the task has got to.  `content` and `sources` are present only "
                    "once it is `completed`.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": TavilyResearchStatusValues},
            "content": {"display_name": "Report",
                "desc": "The report itself: prose, or the structured data described by "
                    "`output_schema` when one was given.",
                "type": AbstractDataProviderTypeMap."auto"},
            "sources": {"display_name": "Sources",
                "desc": "The pages the report drew on.",
                "type": new ListDataType("TavilyResearchSourceList",
                    new TavilyResearchSourceDataType(), True)},
            "created_at": {"display_name": "Created",
                "desc": "When the task was created.",
                "type": AbstractDataProviderTypeMap."*string"},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the task has taken so far, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request for \c "GET /usage"
public class TavilyUsageRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "project_id": {
                "display_name": "Project",
                "short_desc": "Report usage for this project only",
                "desc": "Report usage for this project only, rather than for the whole account.  "
                    "Sent as the `X-Project-ID` header.\n\n"
                    "Tavily publishes no endpoint that lists projects, so this cannot be offered as "
                    "a dropdown; take the value from the Tavily dashboard.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! What one API key has spent
public class TavilyKeyUsageDataType inherits HashDataType {
    private {
        const Fields = {
            "usage": {"display_name": "Credits Used",
                "desc": "Credits this key has used in the current billing period.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "limit": {"display_name": "Credit Limit",
                "desc": "The key's own credit limit; absent when the key is not capped.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "search_usage": {"display_name": "Search Credits",
                "desc": "Credits this key has spent on searches.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "extract_usage": {"display_name": "Extract Credits",
                "desc": "Credits this key has spent reading pages.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "crawl_usage": {"display_name": "Crawl Credits",
                "desc": "Credits this key has spent crawling.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "map_usage": {"display_name": "Map Credits",
                "desc": "Credits this key has spent mapping.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "research_usage": {"display_name": "Research Credits",
                "desc": "Credits this key has spent on research tasks.",
                "type": AbstractDataProviderTypeMap."*softint"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! What the account has spent, and what it may spend
public class TavilyAccountUsageDataType inherits HashDataType {
    private {
        const Fields = {
            "current_plan": {"display_name": "Plan",
                "desc": "The subscription plan the account is on.",
                "type": AbstractDataProviderTypeMap."*string"},
            "plan_usage": {"display_name": "Plan Credits Used",
                "desc": "Credits used against the plan's allowance.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "plan_limit": {"display_name": "Plan Credit Limit",
                "desc": "The plan's credit allowance.  Comparing this with `plan_usage` is what "
                    "tells a flow whether it is about to run out.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "paygo_usage": {"display_name": "Pay-As-You-Go Credits Used",
                "desc": "Credits used beyond the plan's allowance.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "paygo_limit": {"display_name": "Pay-As-You-Go Limit",
                "desc": "The ceiling on credits beyond the plan's allowance; absent when uncapped.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "search_usage": {"display_name": "Search Credits",
                "desc": "Credits the account has spent on searches.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "extract_usage": {"display_name": "Extract Credits",
                "desc": "Credits the account has spent reading pages.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "crawl_usage": {"display_name": "Crawl Credits",
                "desc": "Credits the account has spent crawling.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "map_usage": {"display_name": "Map Credits",
                "desc": "Credits the account has spent mapping.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "research_usage": {"display_name": "Research Credits",
                "desc": "Credits the account has spent on research tasks.",
                "type": AbstractDataProviderTypeMap."*softint"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "GET /usage"
public class TavilyUsageResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "key": {"display_name": "This Key",
                "desc": "What the API key making this request has spent.",
                "type": new TavilyKeyUsageDataType()},
            "account": {"display_name": "Account",
                "desc": "What the whole account has spent, and what it may spend.",
                "type": new TavilyAccountUsageDataType()},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The request body for \c "POST /logs"
public class TavilyLogsRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "limit": {
                "display_name": "Entry Count",
                "short_desc": "How many entries to return, newest first",
                "desc": "How many entries to return, newest first, from 1 to 10000.  Defaults "
                    "to 10.",
                "type": AbstractDataProviderTypeMap."*softint",
                "example_value": 50,
            },
            "start_date": {
                "display_name": "From",
                "short_desc": "Return only entries on or after this date",
                "desc": "Return only entries on or after this date, as `YYYY-MM-DD`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "example_value": "2026-01-01",
            },
            "end_date": {
                "display_name": "To",
                "short_desc": "Return only entries on or before this date",
                "desc": "Return only entries on or before this date, as `YYYY-MM-DD`.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
            "endpoints": {
                "display_name": "Endpoints",
                "short_desc": "Return only entries for these endpoints",
                "desc": "Return only entries for these endpoints.  All of them are returned when "
                    "this is left empty.",
                "type": AbstractDataProviderTypeMap."*softlist",
                "element_allowed_values": TavilyLogEndpointValues,
            },
            "project_id": {
                "display_name": "Project",
                "short_desc": "Return only entries for this project",
                "desc": "Return only entries for this project.\n\n"
                    "Tavily publishes no endpoint that lists projects, so this cannot be offered as "
                    "a dropdown; take the value from the Tavily dashboard.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
            "filter_by_api_key": {
                "display_name": "This Key Only",
                "short_desc": "Return only entries for the key making the request",
                "desc": "Return only entries for the API key making the request, rather than for "
                    "every key on the account.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! One entry of the usage log
public class TavilyLogEntryDataType inherits HashDataType {
    private {
        const Fields = {
            "timestamp": {"display_name": "Timestamp",
                "desc": "When the request was made.",
                "type": AbstractDataProviderTypeMap."*string"},
            "endpoint": {"display_name": "Endpoint",
                "desc": "Which endpoint was called.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": TavilyLogEndpointValues},
            "depth": {"display_name": "Depth",
                "desc": "The search or extraction depth, or the research model, that was used.",
                "type": AbstractDataProviderTypeMap."*string"},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the request took, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
            "credits": {"display_name": "Credits",
                "desc": "What the request cost.",
                "type": AbstractDataProviderTypeMap."*softfloat"},
            "api_key": {"display_name": "API Key",
                "desc": "The key that made the request, masked to its last four characters.",
                "type": AbstractDataProviderTypeMap."*string"},
            "request_id": {"display_name": "Request ID",
                "desc": "Tavily's identifier for the request.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}

#! The response body of \c "POST /logs"
public class TavilyLogsResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "logs": {"display_name": "Entries",
                "desc": "The matching entries, newest first.",
                "type": new ListDataType("TavilyLogEntryList",
                    new TavilyLogEntryDataType(), True)},
            "count": {"display_name": "Entry Count",
                "desc": "How many entries were returned.",
                "type": AbstractDataProviderTypeMap."*softint"},
            "response_time": {"display_name": "Response Time",
                "desc": "How long the request took, in seconds.",
                "type": AbstractDataProviderTypeMap."*float"},
            "request_id": {"display_name": "Request ID",
                "desc": "Tavily's identifier for this request, to quote to their support.",
                "type": AbstractDataProviderTypeMap."*string"},
        };
    }

    constructor() {
        addQoreFields(Fields);
    }
}
}
