# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore EmpathicBuildingDataProvider module definition

/*  EmpathicBuildingRestConnection.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the EmpathicBuildingDataProvider module
public namespace EmpathicBuildingDataProvider {
#! EmpathicBuilding REST connection
public class EmpathicBuildingRestConnection inherits RestClient::RestConnection {
    #! creates the EmpathicBuildingRestConnection object
    /** @param name the name of the connection
        @param description connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes. See below
        @param options connection options

        See @ref AbstractConnection::constructor() for \c attributes and \c options reference.

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string description, string url, hash<auto> attributes = {}, hash<auto> options = {})
            : RestConnection(name, description, url, attributes, options) {
        real_opts = {"url": real_url} + urlh{"username", "password"} + self.opts;
    }

    #! returns \c "esrest"
    string getType() {
        return "ebrest";
    }

    #! returns a data provider object for this connection
    /** @param constructor_options any additional constructor options for the data provider

        @return a data provider object for this connection; the data provider is @ref EmpathicBuildingDataProvider
    */
    DataProvider::AbstractDataProvider getDataProvider(*hash<auto> constructor_options) {
        return new EmpathicBuildingDataProvider({"restclient": get()});
    }

    #! returns a @ref RestClient::RestClient "RestClient" object
    /** @param connect if @ref True "True", then the connection is returned already connected
        @param rtopts this connection type does not accept any runtime options, so this parameter is ignored

        @return a @ref RestClient::RestClient "RestClient" object
    */
    private RestClient getImpl(bool connect = True, *hash<auto> rtopts) {
        return new RestClient(getConnectionOptions(), !connect);
    }
}
}