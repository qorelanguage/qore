# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DeepSeekApiDataProvider class definition

/** DeepSeekApiDataProvider.qc Copyright 2026 Qore Technologies, s.r.o.

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

#! Public DeepSeek request contracts
public namespace DeepSeekDataProvider {
#! Provider for the documented DeepSeek REST actions
/** Operation configuration is immutable after construction; requests keep local state. */
public class DeepSeekApiDataProvider inherits DeepSeekDataProviderCommon {
    private {
        string action;
        hash<DeepSeekActionInfo> op;
    }

    #! Creates an action using an authenticated transport
    /** @param rest transport to copy
        @param action action identifier
        @throw DEEPSEEK-ACTION-ERROR unknown action
    */
    constructor(RestClientIo::RestClientIo rest, string action) : DeepSeekDataProviderCommon(rest) {
        if (!DeepSeekActions::Operations{action}) {
            throw "DEEPSEEK-ACTION-ERROR", sprintf("unknown action %y", action);
        }
        self.action = action;
        op = DeepSeekActions::Operations{action};
    }

    #! Returns this action's identifier
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
        return <DataProviderInfo>{"name": action, "type": "DeepSeekApiDataProvider", "supports_request": True};
    }

    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        hash<auto> body = req ?? {};
        string path = op.path;
        if (index(path, "{file_id}") >= 0) {
            string id = remove body.file_id;
            if (!trim(id) || id == "." || id == "..") {
                throw "DEEPSEEK-REQUEST-ERROR", "file_id must be a non-empty resource identifier";
            }
            path = replace(path, "{file_id}", encode_url(id, True));
        }
        switch (action) {
            case "fim-completion":
                validateGeneration(body);
                checkRange(body, "logprobs", 0, 20);
                break;
            case "create-response":
                validateGeneration(body);
                bool has_input = body.input.typeCode() == NT_STRING ? !!trim(body.input) : !!body.input;
                if (!has_input && !trim(body.instructions ?? "")) {
                    throw "DEEPSEEK-REQUEST-ERROR", "supply input or instructions";
                }
                if (body.text.format.type == "json_schema"
                    && (!trim(body.text.format.name ?? "") || !body.text.format.schema)) {
                    throw "DEEPSEEK-REQUEST-ERROR", "JSON Schema output requires a name and schema";
                }
                validateTools(body, True);
                break;
            case "list-files":
                checkRange(body, "limit", 1, 1000);
                if (body.size()) {
                    path += "?" + (map encode_url($1.key, True) + "=" + encode_url($1.value.toString(), True),
                        body.pairIterator()).join("&");
                }
                body = {};
                break;
            case "upload-file":
                return convertDates(uploadFile(body));
        }
        hash<auto> response = doRestCommand(op.method, path, body.size() ? body : NOTHING).body;
        if (action == "list-files") {
            response.data = map convertDates($1), response.data;
        }
        return convertDates(response);
    }

    #! Converts timestamp fields to the declared date types
    /** @param body response object
        @return response with dates; absent dates remain absent */
    static hash<auto> convertDates(hash<auto> body) {
        foreach string key in ("created", "created_at", "expires_at") {
            if (body{key}.typeCode() == NT_INT) {
                body{key} = 1970-01-01Z + seconds(body{key});
            }
        }
        return body;
    }

    #! Validates an optional numeric bound
    /** @param req request object
        @param key field name
        @param min minimum inclusive value
        @param max maximum inclusive value
        @throw DEEPSEEK-REQUEST-ERROR value out of range */
    static checkRange(hash<auto> req, string key, float min, float max) {
        if (exists req{key} && (req{key} < min || req{key} > max)) {
            throw "DEEPSEEK-REQUEST-ERROR", sprintf("%s must be between %g and %g", key, min, max);
        }
    }

    #! Validates generation options without sending an API request
    /** @param req request object
        @throw DEEPSEEK-REQUEST-ERROR invalid generation options */
    static validateGeneration(hash<auto> req) {
        if (!trim(req.model)) {
            throw "DEEPSEEK-REQUEST-ERROR", "model must not be empty";
        }
        if (req.stream) {
            throw "DEEPSEEK-REQUEST-ERROR", "streaming chat requires DeepSeekLlmProvider";
        }
        checkRange(req, "temperature", 0, 2);
        checkRange(req, "top_p", 0, 1);
        checkRange(req, "top_logprobs", 0, 20);
        foreach string key in ("max_tokens", "max_output_tokens") {
            if (exists req{key} && req{key} < 1) {
                throw "DEEPSEEK-REQUEST-ERROR", sprintf("%s must be positive", key);
            }
        }
        *list<string> stops = req.stop.typeCode() == NT_STRING ? (req.stop,) : req.stop;
        if (stops.size() > 16) {
            throw "DEEPSEEK-REQUEST-ERROR", "at most 16 stop sequences are supported";
        }
        foreach string stop in (stops) {
            if (!stop) {
                throw "DEEPSEEK-REQUEST-ERROR", "stop sequences must not be empty";
            }
        }
        foreach string key in ("user", "user_id") {
            if (exists req{key} && (req{key}.size() > 512 || req{key} !~ /^[a-zA-Z0-9_-]+$/)) {
                throw "DEEPSEEK-REQUEST-ERROR", "user identifier contains invalid characters or exceeds 512 characters";
            }
        }
    }

    #! Validates tool definitions and explicit tool selection
    /** @param req request object
        @param responses whether the request uses the Responses tool shape
        @throw DEEPSEEK-REQUEST-ERROR invalid tool definitions or selection */
    static validateTools(hash<auto> req, bool responses = False) {
        if (req.tools.size() > 128) {
            throw "DEEPSEEK-REQUEST-ERROR", "at most 128 tools are supported";
        }
        hash<string, bool> names;
        hash<string, bool> types;
        foreach hash<auto> tool in (req.tools) {
            types{tool.type} = True;
            if (tool.type == "custom" && tool.name != "apply_patch") {
                throw "DEEPSEEK-REQUEST-ERROR", "custom tools support only apply_patch";
            }
            if (tool.type != "function") {
                continue;
            }
            *string name = responses ? tool.name : tool.function.name;
            if (!name || name.size() > (responses ? 128 : 64) || name !~ /^[a-zA-Z0-9_-]+$/ || names{name}) {
                throw "DEEPSEEK-REQUEST-ERROR", "function names must be valid, non-empty, and unique";
            }
            names{name} = True;
        }
        if (req.tool_choice.typeCode() == NT_HASH) {
            hash<auto> choice = req.tool_choice;
            if (!types{choice.type}) {
                throw "DEEPSEEK-REQUEST-ERROR", "selected tool type is not present in tools";
            }
            if (choice.type == "function" && !names{responses ? choice.name : choice.function.name}) {
                throw "DEEPSEEK-REQUEST-ERROR", "selected function is not present in tools";
            }
        } else if (exists req.tool_choice) {
            if (req.tool_choice != "none" && req.tool_choice != "auto" && req.tool_choice != "required") {
                throw "DEEPSEEK-REQUEST-ERROR", "tool_choice must be none, auto, required, or a named tool object";
            }
            if (req.tool_choice == "required" && !req.tools) {
                throw "DEEPSEEK-REQUEST-ERROR", "required tool choice needs tools";
            }
        }
    }
}
}
