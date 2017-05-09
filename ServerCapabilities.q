# -*- mode: qore; indent-tabs-mode: nil -*-

/*  ServerCapabilities.q Copyright 2017 Qore Technologies, s.r.o.

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

#! LSP TextDocumentSyncKind definition
const TextDocumentSyncKind = {
    /**
     * Documents should not be synced at all.
     */
    "None": 0,

    /**
     * Documents are synced by always sending the full content
     * of the document.
     */
    "Full": 1,

    /**
     * Documents are synced by sending the full content on open.
     * After that only incremental updates to the document are
     * send.
     */
    "Incremental": 2
};

#! QLS LSP ServerCapabilities
const ServerCapabilities = {
    /**
     * Defines how text documents are synced. Is either a detailed structure defining each notification or
     * for backwards compatibility the TextDocumentSyncKind number.
     */
    "textDocumentSync": TextDocumentSyncKind.Full,

    /**
     * The server provides hover support.
     */
    "hoverProvider": "true",

    /**
     * The server provides completion support.
     */
    /*"completionProvider": {
        "resolveProvider": "false",
    },*/

    /**
     * The server provides signature help support.
     */
    #"signatureHelpProvider": SignatureHelpOptions,

    /**
     * The server provides goto definition support.
     */
    #"definitionProvider": "false",

    /**
     * The server provides find references support.
     */
    "referencesProvider": "true",

    /**
     * The server provides document highlight support.
     */
    #"documentHighlightProvider": "false",

    /**
     * The server provides document symbol support.
     */
    "documentSymbolProvider": "true",

    /**
     * The server provides workspace symbol support.
     */
    "workspaceSymbolProvider": "true",

    /**
     * The server provides code actions.
     */
    #"codeActionProvider": "false",

    /**
     * The server provides code lens.
     */
    /*"codeLensProvider": {
        "resolveProvider": "false",
    },*/

    /**
     * The server provides document formatting.
     */
    #"documentFormattingProvider": "false",

    /**
     * The server provides document range formatting.
     */
    #"documentRangeFormattingProvider": "false",

    /**
     * The server provides document formatting on typing.
     */
    #"documentOnTypeFormattingProvider": DocumentOnTypeFormattingOptions,

    /**
     * The server provides rename support.
     */
    #"renameProvider": "false",

    /**
     * The server provides document link support.
     */
    /*"documentLinkProvider": {
        "resolveProvider": "false",
    },*/

    /**
     * The server provides execute command support.
     */
    #"executeCommandProvider": ExecuteCommandOptions,
};
