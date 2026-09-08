# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GroqApiDataProvider class definition

/** GroqApiDataProvider.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace GroqDataProvider {
#! Typed provider for one of the documented Groq REST operations
public class GroqApiDataProvider inherits GroqDataProviderCommon {
    private {
        string action;
        # Configuration is immutable after construction; requests keep their own local state.
        hash<GroqActionInfo> op;
    }

    #! Creates an action provider using an authenticated transport
    /** @param rest transport to copy without changing its path
        @param action action identifier in GroqActions::Operations
        @throw GROQ-ACTION-ERROR the action is unknown
    */
    constructor(RestClientIo::RestClientIo rest, string action) : GroqDataProviderCommon(rest) {
        self.action = action;
        if (!GroqActions::Operations{action}) {
            throw "GROQ-ACTION-ERROR", sprintf("unknown action %y", action);
        }
        op = GroqActions::Operations{action};
    }

    #! Returns the action identifier
    /** @return the action identifier */
    string getName() {
        return action;
    }

    private *AbstractDataProviderType getRequestTypeImpl() {
        return op.request_type;
    }

    private *AbstractDataProviderType getResponseTypeImpl() {
        return op.response_type;
    }

    private hash<DataProviderInfo> getStaticInfoImpl() {
        return <DataProviderInfo>{
            "name": action, "type": "GroqApiDataProvider", "supports_request": True,
        };
    }

    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        hash<auto> body = req ?? {};
        string path = op.path;
        foreach string key in ("model", "file_id", "batch_id", "id") {
            string placeholder = "{" + key + "}";
            if (index(path, placeholder) >= 0) {
                string id = remove body{key};
                if (!id || id == "." || id == "..") {
                    throw "GROQ-REQUEST-ERROR", sprintf("%s must be a non-empty resource identifier", key);
                }
                # Groq's model route uses a wildcard: it requires literal namespace slashes.
                # Encode each component and reject traversal before preserving these separators.
                if (key == "model") {
                    list<string> components = id.split("/");
                    foreach string component in (components) {
                        if (!component || component == "." || component == "..") {
                            throw "GROQ-REQUEST-ERROR", "model contains an invalid path component";
                        }
                    }
                    id = (map encode_url($1, True), components).join("/");
                } else {
                    id = encode_url(id, True);
                }
                path = replace(path, placeholder, id);
            }
        }
        auto response;
        switch (action) {
            case "download-file":
                return downloadFile(path);
            case "create-response":
                validateGeneration(body);
                if (exists body.truncation) {
                    throw "GROQ-REQUEST-ERROR", "Groq does not support Responses truncation";
                }
                foreach hash<auto> tool in (body.tools) {
                    if (tool.type == "function" && !trim(tool.name)) {
                        throw "GROQ-REQUEST-ERROR", "function tools require a name";
                    }
                    if (tool.type == "mcp" && (!trim(tool.server_label) || !tool.server_url.startsWith("https://"))) {
                        throw "GROQ-REQUEST-ERROR", "MCP tools require a server label and an HTTPS server URL";
                    }
                }
                if (!trim(body.model) || !body.input
                    || (body.input.typeCode() == NT_STRING && !trim(body.input))) {
                    throw "GROQ-REQUEST-ERROR", "response requires a model and non-empty input";
                }
                break;
            case "create-batch":
                if (body.completion_window !~ /^([0-9]+h|[0-9]+d)$/) {
                    throw "GROQ-REQUEST-ERROR", "completion_window must be an hour or day duration";
                }
                int hours = body.completion_window.toInt();
                if (body.completion_window.endsWith("d")) {
                    hours *= 24;
                }
                if (hours < 24 || hours > 168) {
                    throw "GROQ-REQUEST-ERROR", "completion_window must be between 24h and 7d";
                }
                break;
            case "transcribe-audio": case "translate-audio":
                if ((!body.file) == (!body.url)) {
                    throw "GROQ-REQUEST-ERROR", "supply exactly one of file or url";
                }
                if (exists body.temperature && (body.temperature < 0 || body.temperature > 1)) {
                    throw "GROQ-REQUEST-ERROR", "audio temperature must be between 0 and 1";
                }
                if (body.timestamp_granularities && body.response_format != "verbose_json") {
                    throw "GROQ-REQUEST-ERROR", "timestamps require response_format verbose_json";
                }
                return doMultipart(path, body).body;
            case "upload-file":
                return doMultipart(path, body).body;
            case "generate-speech":
                if (!trim(body.input) || (exists body.speed && (body.speed < 0.5 || body.speed > 5))) {
                    throw "GROQ-REQUEST-ERROR", "speech requires non-empty text and speed between 0.5 and 5";
                }
                if (body.model.startsWith("canopylabs/orpheus-")) {
                    if ((body.response_format ?? "wav") != "wav") {
                        throw "GROQ-REQUEST-ERROR", "Orpheus supports only wav output";
                    }
                    *list<hash<AllowedValueInfo>> voices = getReferenceData("voices", body);
                    if (voices && !(select voices, $1.value == body.voice)) {
                        throw "GROQ-REQUEST-ERROR", "voice is not supported by the selected Orpheus model";
                    }
                }
                response = doRestCommand("POST", path, body).body;
                return response.typeCode() == NT_STRING ? binary(response) : response;
        }
        response = doRestCommand(op.method, path, body.size() ? body : NOTHING).body;
        if (action == "get-model" && response.created.typeCode() == NT_INT) {
            response.created = 1970-01-01Z + seconds(response.created);
        }
        return response;
    }

    #! Checks generation options shared by chat and Responses
    /** @param req generation options
        @throw GROQ-REQUEST-ERROR options are incompatible with a completed response
    */
    static validateGeneration(hash<auto> req) {
        if (req.tools.size() > 128) {
            throw "GROQ-REQUEST-ERROR", "at most 128 tool definitions are supported";
        }
        if (req.stream) {
            throw "GROQ-REQUEST-ERROR", "streaming requires GroqLlmProvider";
        }
        if (exists req.temperature && (req.temperature < 0 || req.temperature > 2)) {
            throw "GROQ-REQUEST-ERROR", "temperature must be between 0 and 2";
        }
        if (exists req.top_p && (req.top_p < 0 || req.top_p > 1)) {
            throw "GROQ-REQUEST-ERROR", "top_p must be between 0 and 1";
        }
        foreach string key in ("max_tokens", "max_completion_tokens", "max_output_tokens") {
            if (exists req{key} && req{key} < 1) {
                throw "GROQ-REQUEST-ERROR", sprintf("%s must be positive", key);
            }
        }
    }
}
}
