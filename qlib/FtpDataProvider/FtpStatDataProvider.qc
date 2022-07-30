# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore FtpDataProvider module definition

/** FtpStatDataProvider.qc Copyright 2022 Qore Technologies, s.r.o.

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
#! The FTP stat data provider class
public class FtpStatDataProvider inherits AbstractDataProvider {
    public {
        #! FTP connection
        MyFtpClient ftp;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "FtpStatDataProvider",
            "constructor_options": FtpDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Request type
        const RequestType = new FtpPathDataType();

        #! Response type
        const ResponseType = new FtpStatResponseDataType();
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
        return "stat";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "FTP stat data provider; returns information about the path given as an argument";
    }

    #! Performs a "stat"-like action on a remote file
    static hash<auto> doStat(FtpClient ftp, string path, *bool assume_known) {
        string type;
        date mtime;
        int size;

        hash<FtpResponseInfo> mh = ftp.sendControlMessage("SIZE", path);
        if (mh."code" == 550) {
            if (FtpStatDataProvider::checkDir(ftp, path)) {
                mtime = 1970-01-01Z;
                size = 0;
                type = "DIRECTORY";
            } else {
                if (assume_known) {
                    mtime = 1970-01-01Z;
                    size = 0;
                    type = "UNKNOWN";
                } else {
                    throw "FTP-STAT-ERROR", sprintf("%y: server returned no information for path", path);
                }
            }
        } else {
            if ((mh."code" / 100) != 2) {
                throw "FTP-STAT-ERROR", sprintf("%s: SIZE failure: %d %y ", path, mh."code", mh.msg);
            }
            size = mh.msg.toInt();
            mtime = FtpStatDataProvider::getModifiedTime(ftp, path);

            if (FtpStatDataProvider::checkDir(ftp, path)) {
                type = "DIRECTORY";
            } else {
                type = "REGULAR";
            }
        }

        return {
            "name": basename(path),
            "path": path,
            "type": type,
            "mtime": mtime,
            "size": size,
        };
    }

    #! Returns the modified time for a remote file, if possible
    static date getModifiedTime(FtpClient ftp, string path) {
        hash<FtpResponseInfo> mh = ftp.sendControlMessage("MDTM", path);
        # assume we have a directory if we get a 550 response to the MDTM command
        if ((mh."code" / 100) != 2) {
            throw "FTP-ERROR", sprintf("%s: MDTM failure: %d %y ", path, mh."code", mh.msg);
        }
        return date(mh.msg);
    }

    static bool checkDir(FtpClient ftp, string path) {
        # check if it's a directory
        try {
            ftp.cwd(path);
            return True;
        } catch (hash<ExceptionInfo> ex) {
        }
        return False;
    }

    #! Makes a request and returns the response
    /** @param req the request info
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request

        @throw STAT-ERROR if the path cannot be read
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string path = ftp.getPath(req.path);
        return doStat(ftp, path);
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
