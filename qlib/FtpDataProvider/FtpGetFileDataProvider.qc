# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore FtpDataProvider module definition

/** FtpGetFileDataProvider.qc Copyright 2022 Qore Technologies, s.r.o.

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

#! contains all public definitions in the FtpDataProvider module
public namespace FtpDataProvider {
#! The FTP get file data provider class
public class FtpGetFileDataProvider inherits AbstractDataProvider {
    public {
        #! FTP connection
        FtpClient ftp;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "FtpGetFileDataProvider",
            "constructor_options": FtpDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Request type
        const RequestType = new FtpGetFileRequestDataType();

        #! Response type
        const ResponseType = new FtpGetFileResponseDataType();
    }

    #! Gets the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", FtpDataProvider::ConstructorOptions, options);
        ftp = FtpDataProvider::getFtpConnection(options);
    }

    #! Gets the object from an FTP connection
    constructor(FtpClient ftp) {
        self.ftp = ftp;
    }

    #! Returns the data provider name
    string getName() {
        return "get-file";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "FTP get file data provider; gets a file in the given location";
    }

    #! Makes a request and returns the response
    /** @param req the request info
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string path = FtpDataProvider::getPath(ftp, req.path);
        OutputStream out = req.text
            ? new StringOutputStream(req.encoding)
            : new BinaryOutputStream();
        ftp.get(path, out);
        return {
            "path": path,
            "data": req.text
                ? cast<StringOutputStream>(out).getData()
                : cast<BinaryOutputStream>(out).getData(),
        };
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider
    */
    private *AbstractDataProviderType getRequestTypeImpl() {
        return RequestType;
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *AbstractDataProviderType getResponseTypeImpl() {
        return ResponseType;
    }

    #! Returns data provider static info
    hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
