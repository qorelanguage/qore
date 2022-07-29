# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore FtpDataProvider module definition

/** FtpMoveDataProvider.qc Copyright 2022 Qore Technologies, s.r.o.

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
#! The FTP move data provider class
public class FtpMoveDataProvider inherits AbstractDataProvider {
    public {
        #! FTP connection
        MyFtpClient ftp;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "FtpMoveDataProvider",
            "constructor_options": FtpDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Request type
        const RequestType = new FtpMoveRequestDataType();

        #! Response type
        const ResponseType = new FtpPathDataType();
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", FtpDataProvider::ConstructorOptions, options);
        ftp = FtpDataProvider::getFtpConnection(options);
    }

    #! Creates the object from an FTP connection
    constructor(MyFtpClient ftp) {
        self.ftp = ftp;
    }

    #! Returns the data provider name
    string getName() {
        return "move";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "FTP move data provider; moves a file on a remote FTP server given the source and target paths as "
            "arguments";
    }

    #! Makes a request and returns the response
    /** @param req the request info
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string source = ftp.getPath(req.source);
        string target = ftp.getPath(req.target);
        ftp.rename(source, target);
        return {
            "path": target,
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
        return RequestType;
    }

    #! Returns data provider static info
    hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
