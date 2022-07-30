# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore FtpDataProvider class definition

/** FtpDataProvider.qc Copyright 2022 Qore Technologies, s.r.o.

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
#! The FTP data provider class, provides API classes as children
public class FtpDataProvider inherits AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "FtpDataProvider",
            "constructor_options": ConstructorOptions,
            "supports_children": True,
        };

        #! Constructor arguments
        const ConstructorOptions = {
            "timeout": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(IntType),
                "desc": "I/O timeout to use in milliseconds",
            },
            "url": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "A URL for an FTP or FTPS connection",
                "required": True,
            },
        };

        FtpClient ftp;
    }

    private {
        const ChildMap = {
            "create-file": Class::forName("FtpDataProvider::FtpCreateFileDataProvider"),
            "delete": Class::forName("FtpDataProvider::FtpDeleteDataProvider"),
            "get-file": Class::forName("FtpDataProvider::FtpGetFileDataProvider"),
            "list": Class::forName("FtpDataProvider::FtpListDataProvider"),
            "mkdir": Class::forName("FtpDataProvider::FtpMkdirDataProvider"),
            "move": Class::forName("FtpDataProvider::FtpMoveDataProvider"),
            "stat": Class::forName("FtpDataProvider::FtpStatDataProvider"),
        };
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        ftp = getFtpConnection(options);
    }

    #! Creates the object from an FTP connection
    constructor(FtpClient ftp) {
        self.ftp = ftp;
    }

    #! Returns the data provider name
    string getName() {
        return "ftp";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "Data provider for FTP servers";
    }

    #! Return data provider summary info
    *list<hash<DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map $1.getStaticMember("StaticInfo").getValue(), ChildMap.iterator();
    }

    #! Returns an FTP connection from constructor options
    static FtpClient getFtpConnection(hash<auto> options) {
        FtpClient ftp(options.url);
        if (options."timeout") {
            ftp.setTimeout(options."timeout");
        }
        return ftp;
    }

    #! Returns the path based on the FtpClient's base path
    static string getPath(FtpClient ftp, string path1) {
        path1 =~ s/^\/+//;
        return ftp.path + "/" + path1;
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys ChildMap;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        *Class cls = ChildMap{name};
        if (!cls) {
            return;
        }
        return cls.newObject(ftp);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}