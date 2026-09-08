# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GroqDataProviderCommon class definition

/** GroqDataProviderCommon.qc Copyright 2026 Qore Technologies, s.r.o.

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

#! contains all public definitions in the GroqDataProvider module
public namespace GroqDataProvider {
#! The Groq data provider common base class
/** Uses RestClientIo for async-capable multiplexed HTTP connections.
*/
public class GroqDataProviderCommon inherits DataProvider::AbstractDataProvider {
    public {
        #! The default Groq REST API URL
        const DefaultGroqUrl = GroqRestClientBase::DefaultUrl;

        #! The default Groq API version
        const DefaultGroqApiVersion = "v1";

        #! Constructor options
        const ConstructorOptions = {
            "apikey": <DataProviderOptionInfo>{
                "display_name": "Groq API Key",
                "short_desc": "Groq API key for Bearer authentication",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The Groq API key used for Bearer authentication.  Get your API key from "
                    "the [Groq Console](https://console.groq.com/keys).",
                "sensitive": True,
            },
            "url": <DataProviderOptionInfo>{
                "display_name": "Server URL",
                "short_desc": "The URL of the Groq API",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The URL of the Groq REST API.",
                "default_value": DefaultGroqUrl,
            },
            "restclient": <DataProviderOptionInfo>{
                "display_name": "REST Client",
                "short_desc": "The REST client object to use",
                "type": AbstractDataProviderType::get(
                    new Type("RestClientIo::RestClientIo"), NOTHING, {
                        DTT_ClientOnly: True,
                    }),
                "desc": "The REST client object to use",
            },
            "restclient_options": <DataProviderOptionInfo>{
                "display_name": "REST Client Options",
                "short_desc": "The options to use when creating a REST client",
                "type": AbstractDataProviderType::get(AutoHashType, NOTHING, {
                    DTT_ClientOnly: True,
                }),
                "desc": "Options to the RestClientIo constructor; only used if a RestClientIo "
                    "object is created for a call",
            },
            "api": <DataProviderOptionInfo>{
                "display_name": "API Version",
                "short_desc": "Groq API version segment",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The supported Groq API version.",
                "allowed_values": (<AllowedValueInfo>{"value": "v1", "display_name": "V1"},),
                "default_value": DefaultGroqApiVersion,
            },
        };

        #! Legacy retry error names retained for source compatibility
        const RetrySet = {
            "SOCKET-SSL-ERROR": True,
            "SOCKET-CLOSED": True,
        };

        #! Legacy retry limit retained for source compatibility
        const MaxIoRetries = 5;
    }

    private {
        #! The REST client object for API calls (async-capable)
        *RestClientIo::RestClientIo rest;
    }

    #! Creates the object from a RestClientIo client
    /** @param rest authenticated transport to copy without changing the caller's connection path */
    constructor(RestClientIo::RestClientIo rest) {
        self.rest = cast<RestClientIo::RestClientIo>(rest.copyWithUrl(rest.getUrl().toString()));
        self.rest.clearConnectionPath();
    }

    #! Creates the object from constructor options
    /** @param options an API key and URL, or a supplied REST client
        @throw CONSTRUCTOR-ERROR credentials are absent or an option is invalid
    */
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.restclient) {
            rest = cast<RestClientIo::RestClientIo>(
                copts.restclient.copyWithUrl(copts.restclient.getUrl().toString()));
        }
        if (!rest) {
            if (!copts.apikey.val()) {
                throw "CONSTRUCTOR-ERROR",
                    "missing required 'apikey' argument for Groq API communication";
            }
            hash<auto> opts = (copts.restclient_options ?? {}) + {
                "url": copts.url,
                "api": copts.api,
                "apikey": copts.apikey,
            };
            rest = new GroqRestClient::GroqRestClientIo(opts);
        }
        rest.clearConnectionPath();
    }

    #! Returns names of supported resource dropdowns
    /** @return reference data names; custom resource identifiers are also accepted */
    *hash<string, bool> getSupportedReferenceData() {
        return {
            "models": True, "chat-models": True, "response-models": True,
            "transcription-models": True, "translation-models": True, "speech-models": True,
            "voices": True, "files": True, "batch-files": True, "adapter-files": True,
            "batches": True, "fine-tunings": True,
        };
    }

    private *list<hash<AllowedValueInfo>> getReferenceDataImpl(string type, *hash<auto> action_opts) {
        if (type == "voices") {
            if (!action_opts.model) {
                return;
            }
            *list<string> voices;
            switch (action_opts.model) {
                case "canopylabs/orpheus-v1-english":
                    voices = ("autumn", "diana", "hannah", "austin", "daniel", "troy");
                    break;
                case "canopylabs/orpheus-arabic-saudi":
                    voices = ("abdullah", "fahad", "sultan", "lulwa", "noura", "aisha");
                    break;
                default:
                    return;
            }
            return map <AllowedValueInfo>{"value": $1, "display_name": $1.substr(0, 1).upr() + $1.substr(1)}, voices;
        }
        string path;
        switch (type) {
            case "models": case "chat-models": case "response-models":
            case "transcription-models": case "translation-models": case "speech-models":
                path = "models";
                break;
            case "files": case "batch-files": case "adapter-files":
                path = "files";
                break;
            case "batches": path = "batches"; break;
            case "fine-tunings": path = "/v1/fine_tunings"; break;
            default: return;
        }
        list<hash<AllowedValueInfo>> rv = ();
        foreach hash<auto> record in (doRestCommand("GET", path).body."data") {
            if (!record.id || (exists record.active && !record.active)) {
                continue;
            }
            string id = record.id;
            if (path == "models") {
                bool whisper = id.startsWith("whisper-");
                bool speech = id.startsWith("canopylabs/orpheus-") || id.startsWith("playai-tts");
                if (((type == "chat-models" || type == "response-models") && (whisper || speech))
                    || (type == "transcription-models" && !whisper)
                    || (type == "translation-models" && id != "whisper-large-v3")
                    || (type == "speech-models" && !speech)) {
                    continue;
                }
            }
            if ((type == "batch-files" && record.purpose != "batch")
                || (type == "adapter-files" && record.purpose != "fine_tuning")) {
                continue;
            }
            push rv, <AllowedValueInfo>{
                "value": id,
                "display_name": record.filename ?? record.name ?? id,
            };
        }
        return rv;
    }

    #! Sends one REST request without replaying potentially billable operations
    /** @param method HTTP method
        @param path relative OpenAI-compatible path or absolute native API path
        @param body optional JSON request body
        @param hdr optional HTTP headers
        @return the parsed REST response
        @throw REST-RESPONSE-ERROR the service rejected the request
    */
    private hash<auto> doRestCommand(string method, string path, auto body, *hash<auto> hdr) {
        return rest.restDoRequest(method, apiPath(path), body, hdr);
    }

    #! Resolves the two documented Groq API namespaces
    private static string apiPath(string path) {
        return path.startsWith("/") ? path : "/openai/v1/" + path;
    }

    #! Downloads file contents without interpreting their MIME type or JSON content
    private binary downloadFile(string path) {
        RestClientIo::RestClientIo client = cast<RestClientIo::RestClientIo>(
            rest.copyWithUrl(rest.getUrl().toString()));
        client.setValidator(new GroqFileDownloadValidator());
        return binary(client.restGet(apiPath(path)).body);
    }

    #! Sends named files and repeated form fields without JSON encoding the multipart body
    private hash<auto> doMultipart(string path, hash<auto> req) {
        Mime::MultiPartFormDataMessage msg();
        *hash<auto> file = remove req.file;
        foreach hash<auto> entry in (req.pairIterator()) {
            auto value = entry.value;
            if (!exists value) {
                continue;
            }
            string name = entry.key;
            if (value.typeCode() == NT_LIST) {
                foreach auto item in (value) {
                    msg.addRepeatedPart(item.toString(), name + "[]");
                }
            } else {
                msg.addPart(value.toString(), name);
            }
        }
        if (file) {
            if (!file.name || !exists file.content) {
                throw "GROQ-REQUEST-ERROR", "file requires a non-empty name and content";
            }
            data content = file.content.typeCode() == NT_BINARY ? file.content : parse_base64_string(file.content);
            if (!content.size()) {
                throw "GROQ-REQUEST-ERROR", "file content must not be empty";
            }
            if (file.name =~ /[\r\n]/) {
                throw "GROQ-REQUEST-ERROR", "file name must not contain line breaks";
            }
            *string mime = file.mime_type;
            if (!mime || mime == "auto") {
                mime = get_mime_type_from_ext(file.name) ?? "application/octet-stream";
            }
            if (mime =~ /[\r\n]/) {
                throw "GROQ-REQUEST-ERROR", "file MIME type must not contain line breaks";
            }
            msg.addPart(content, "file", file.name, mime);
        }
        hash<MessageInfo> msg_info = msg.getMsgAndHeaders();
        return rest.restDoRawRequest("POST", apiPath(path), msg_info.body, msg_info.hdr);
    }
}

#! Successful file downloads are opaque bytes; errors retain normal REST decoding
class GroqFileDownloadValidator inherits RestSchemaValidator::NullRestSchemaValidator {
    private hash<RestResponseClientInfo> parseResponseImpl(string method, string path, int code,
            *data body, hash<auto> hdr) {
        if (code >= 200 && code < 300) {
            return <RestResponseClientInfo>{"code": code, "hdr": hdr, "body": binary(body)};
        }
        return NullRestSchemaValidator::parseResponseImpl(method, path, code, body, hdr);
    }
}
}
