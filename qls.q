#!/usr/bin/env qore
# -*- mode: qore; indent-tabs-mode: nil -*-

/*  qls.q Copyright 2017 Qore Technologies, s.r.o.

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

#
# QLS - Language Server Protocol server implementation for Qore.
#

%require-types
%enable-all-warnings
%new-style
%strict-args

%requires qore >= 0.8.12

%requires json
%requires Mime

%requires ./Document.qm
%requires ./ErrorResponse.qm
%requires ./Messenger.qm
%requires ./Notification.qm

%include ./Files.q
%include ./ServerCapabilities.q

sub debugLog(string fmt) {
    string str = sprintf("%s: ", format_date("YYYY-MM-DD HH:mm:SS", now()));
    string msg = vsprintf(str + fmt + "\n", argv);
    FileOutputStream fos("qls_log.txt", True);
    fos.write(binary(msg));
    #stderr.write(msg);
}

#! LSP FileChangeType definition
const FileChangeType = {
	"Created": 1,
	"Changed": 2,
	"Deleted": 3,
};

%exec-class QLS

class QLS {
    private {
        #! LSP header and content parts delimiter
        const LSP_PART_DELIMITER = "\r\n";

        #! Whether QLS has been initialized ("initialize" method called)
        bool initialized = False;

        #! Whether client is fully initialized ("initialized" method called)
        bool clientInitialized = False;

        #! Whether QLS has been shut down ("shutdown" method called)
        bool shutdown = False;

        # Whether the main loop should still run (or QLS should quit)
        bool running = True; 

        # Exit code to use when quitting
        int exitCode = 0;

        #! PID of parent process
        int parentProcessId;

        #! JSON-RPC version string (ex. "2.0")
        string jsonRpcVer;

        #! Workspace root URI (ex. "file:///home/user/projects/lorem")
        string rootUri;

        #! Workspace root path (ex. "/home/user/projects/lorem")
        string rootPath;

        #! Client capabilities
        hash clientCapabilities;

        #! Map of JSON-RPC methods
        hash methodMap;

        #! Open text documents. Hash keys are document URIs.
        hash documents;

        #! Qore Documents in the current workspace.
        hash workspaceDocs;
    }

    constructor() {
        initMethodMap();

        set_return_value(main());
    }

    error(string fmt) {
        stderr.vprintf("ERROR: " + fmt + "\n", argv);
        exit(1);
    }

    private:internal initMethodMap() {
        methodMap = {
            # General methods
            "initialize": \meth_initialize(),
            "initialized": \meth_initialized(),
            "shutdown": \meth_shutdown(),
            "exit": \meth_exit(),
            "$/cancelRequest": \meth_cancelRequest(),

            # Workspace methods
            "workspace/didChangeConfiguration": \meth_ws_didChangeConfiguration(),
            "workspace/didChangeWatchedFiles": \meth_ws_didChangeWatchedFiles(),
            "workspace/symbol": \meth_ws_symbol(),
            "workspace/executeCommand": \meth_ws_executeCommand(),

            # Document methods
            "textDocument/didOpen": \meth_td_didOpen(),
            "textDocument/didChange": \meth_td_didChange(),
            "textDocument/willSave": \meth_td_willSave(),
            "textDocument/willSaveWaitUntil": \meth_td_willSaveWaitUntil(),
            "textDocument/didSave": \meth_td_didSave(),
            "textDocument/didClose": \meth_td_didClose(),
            "textDocument/completion": \meth_td_completion(),
            "completionItem/resolve": \meth_completionItem_resolve(),
            "textDocument/hover": \meth_td_hover(),
            "textDocument/signatureHelp": \meth_td_signatureHelp(),
            "textDocument/references": \meth_td_references(),
            "textDocument/documentHighlight": \meth_td_documentHighlight(),
            "textDocument/documentSymbol": \meth_td_documentSymbol(),
            "textDocument/formatting": \meth_td_formatting(),
            "textDocument/rangeFormatting": \meth_td_rangeFormatting(),
            "textDocument/onTypeFormatting": \meth_td_onTypeFormatting(),
            "textDocument/definition": \meth_td_definition(),
            "textDocument/codeAction": \meth_td_codeAction(),
            "textDocument/codeLens": \meth_td_codeLens(),
            "codeLens/resolve": \meth_codeLens_resolve(),
            "textDocument/documentLink": \meth_td_documentLink(),
            "documentLink/resolve": \meth_documentLink_resolve(),
            "textDocument/rename": \meth_td_rename(),
        };
    }


    #=================
    # Main logic
    #=================

    int main() {
        while (running) {
            # read JSON-RPC request
            hash received = Messenger::receive();
            if (received.error)
                error(received.error);
            if (!received.msg)
                break;

            # handle the request
            *string response = handleRequest(received.msg);

            # send back response if any
            if (response)
                Messenger::send(response);
        }

        return exitCode;
    }

    *string handleRequest(string msg) {
        # parse the request
        any parsed = parse_json(msg);

        # handle the request
        if (parsed.typeCode() == NT_HASH) {
            hash request = parsed;
            debugLog("req: %N", request);
            *string response;

            # find and run method handler
            if (initialized) {
                *code method = methodMap{request.method};
                if (method) {
                    response = method(request);
                }
                else {
                    if (request.id) # if it's a request, send response
                        response = ErrorResponse::methodNotFound(request);
                    # else it's notification -> ignore
                }
            }
            else { # run "initialize" method handler or return an error
                if (request.method == "initialize")
                    response = meth_initialize(request);
                else
                    response = ErrorResponse::notInitialized(request);
            }
            return response;
        }
        else {
            error("invalid JSON-RPC request");
        }
        return NOTHING;
    }


    #=================
    # Parsing
    #=================

    private:internal parseFilesInWorkspace() {
        stderr.printf("parsing files in workspace\n");

        # find all Qore files in the workspace
        list qoreFiles = findQoreFilesInWorkspace(rootPath);

        # create a list of file URIs
        int rootPathSize = rootPath.size();
        qoreFiles = map rootUri + $1.substr(rootPathSize), qoreFiles;
        stderr.printf("qore files: %N\n", qoreFiles);

        # measure start time
        date start = now_us();

        # parse everything
        map workspaceDocs{$1} = new Document($1), qoreFiles;

        # measure end time
        date end = now_us();
        stderr.printf("parsing of %d files took: %y\n", qoreFiles.size(), end-start);
    }


    #=================
    # General methods
    #=================

    #! "initialize" method handler
    private:internal *string meth_initialize(hash request) {
        jsonRpcVer = request.jsonrpc;
        reference initParams = \request.params;

        parentProcessId = initParams.processId;
        clientCapabilities = initParams.capabilities;
        if (initParams{"rootUri"})
            rootUri = initParams.rootUri;
        if (initParams{"rootPath"})
            rootPath = initParams.rootPath;
        if (!rootPath && rootUri)
            rootPath = parse_url(rootUri).path;
        if (!rootUri && rootPath)
            rootUri = "file://" + rootPath;

        parseFilesInWorkspace();

        hash result = {
            "capabilities": QLSServerCapabilities
        };

        initialized = True;
        return make_jsonrpc_response(jsonRpcVer, request.id, result);
    }

    #! "initialized" notification method handler
    private:internal *string meth_initialized(hash request) {
        clientInitialized = True;
        return NOTHING;
    }

    #! "shutdown" method handler
    private:internal *string meth_shutdown(hash request) {
        shutdown = True;
        return make_jsonrpc_response(jsonRpcVer, request.id, {});
    }

    #! "exit" notification method handler
    private:internal *string meth_exit(hash request) {
        if (!shutdown)
            exitCode = 1;
        running = False;
        return NOTHING;
    }

    #! "$/cancelRequest" notification method handler
    private:internal *string meth_cancelRequest(hash request) {
        # ignore for now
        return NOTHING;
    }


    #===================
    # Workspace methods
    #===================

    #! "workspace/didChangeConfiguration" notification method handler
    private:internal *string meth_ws_didChangeConfiguration(hash request) {
        # ignore for now
        return NOTHING;
    }

    #! "workspace/didChangeWatchedFiles" notification method handler
    private:internal *string meth_ws_didChangeWatchedFiles(hash request) {
        list changes = request.params.changes;
        foreach hash change in (changes) {
            switch (change.type) {
                case FileChangeType.Created:
                case FileChangeType.Changed:
                    workspaceDocs{change.uri} = new Document(change.uri);
                case FileChangeType.Deleted:
                    remove workspaceDocs{change.uri};
                default:
                    break;
            }
        }
        return NOTHING;
    }

    #! "workspace/symbol"  method handler
    private:internal *string meth_ws_symbol(hash request) {
        #stderr.printf("workspace/symbol request received\n");
        list symbols = ();
        string query = request.params.query;
        map symbols += $1.findMatchingSymbols(query), documents.iterator();
        map symbols += $1.findMatchingSymbols(query), workspaceDocs.iterator();
        #stderr.printf("found %d matching symbols\n", symbols.size());
        return make_jsonrpc_response(jsonRpcVer, request.id, symbols);
    }

    #! "workspace/executeCommand"  method handler
    private:internal *string meth_ws_executeCommand(hash request) {
        return ErrorResponse::methodNotFound(request);
    }


    #==================
    # Document methods
    #==================

    #! "textDocument/didOpen" notification method handler
    private:internal *string meth_td_didOpen(hash request) {
        reference textDoc = \request.params.textDocument;
        *Document doc;
        if (workspaceDocs{textDoc.uri})
            doc = remove workspaceDocs{textDoc.uri};
        else
            doc = new Document(textDoc.uri, textDoc.text, textDoc.languageId, textDoc.version);
        documents{textDoc.uri} = doc;

        if (doc.getParseErrorCount() > 0) {
            *list diagnostics = doc.getDiagnostics();
            return Notification::diagnostics(jsonRpcVer, textDoc.uri, diagnostics);
        }
        return Notification::diagnostics(jsonRpcVer, textDoc.uri);
    }

    #! "textDocument/didChange" notification method handler
    private:internal *string meth_td_didChange(hash request) {
        reference textDoc = \request.params.textDocument;
        reference doc = \documents{textDoc.uri};
        foreach hash change in (request.params.contentChanges) {
            doc.didChange(change);
        }
        doc.changeVersion(textDoc.version);

        if (doc.getParseErrorCount() > 0) {
            *list diagnostics = doc.getDiagnostics();
            return Notification::diagnostics(jsonRpcVer, textDoc.uri, diagnostics);
        }
        return Notification::diagnostics(jsonRpcVer, textDoc.uri);
    }

    #! "textDocument/willSave" notification method handler
    private:internal *string meth_td_willSave(hash request) {
        # ignore for now
        return NOTHING;
    }

    #! "textDocument/willSaveWaitUntil" method handler
    private:internal *string meth_td_willSaveWaitUntil(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/didSave" notification method handler
    private:internal *string meth_td_didSave(hash request) {
        # ignore for now
        return NOTHING;
    }

    #! "textDocument/didClose" notification method handler
    private:internal *string meth_td_didClose(hash request) {
        string uri = request.params.textDocument.uri;
        workspaceDocs{uri} = remove documents{uri};
        return NOTHING;
    }

    #! "textDocument/completion" method handler
    private:internal *string meth_td_completion(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "completionItem/resolve" method handler
    private:internal *string meth_completionItem_resolve(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/hover" method handler
    private:internal *string meth_td_hover(hash request) {
        Document doc = documents{request.params.textDocument.uri};
        return make_jsonrpc_response(jsonRpcVer, request.id, doc.hover(request.params.position));
    }

    #! "textDocument/signatureHelp" method handler
    private:internal *string meth_td_signatureHelp(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/references" method handler
    private:internal *string meth_td_references(hash request) {
        Document doc = documents{request.params.textDocument.uri};
        *list references = doc.findReferences(request.params.position, request.params.context.includeDeclaration);
        return make_jsonrpc_response(jsonRpcVer, request.id, references ?? list());
    }

    #! "textDocument/documentHighlight" method handler
    private:internal *string meth_td_documentHighlight(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/documentSymbol" method handler
    private:internal *string meth_td_documentSymbol(hash request) {
        Document doc = documents{request.params.textDocument.uri};
        *list symbols = doc.findSymbols();
        return make_jsonrpc_response(jsonRpcVer, request.id, symbols ?? list());
    }

    #! "textDocument/formatting" method handler
    private:internal *string meth_td_formatting(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/rangeFormatting" method handler
    private:internal *string meth_td_rangeFormatting(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/onTypeFormatting" method handler
    private:internal *string meth_td_onTypeFormatting(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/definition" method handler
    private:internal *string meth_td_definition(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/codeAction" method handler
    private:internal *string meth_td_codeAction(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/codeLens" method handler
    private:internal *string meth_td_codeLens(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "codeLens/resolve" method handler
    private:internal *string meth_codeLens_resolve(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/documentLink" method handler
    private:internal *string meth_td_documentLink(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "documentLink/resolve" method handler
    private:internal *string meth_documentLink_resolve(hash request) {
        return ErrorResponse::invalidRequest(request);
    }

    #! "textDocument/rename" method handler
    private:internal *string meth_td_rename(hash request) {
        return ErrorResponse::invalidRequest(request);
    }
}
