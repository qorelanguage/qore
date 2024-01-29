# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore EmpathicBuildingDataProvider module definition

/** EmpathicBuildingSensorsDataProvider.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! The parent class for Empathic Building sensors
public class EmpathicBuildingSensorsDataProvider inherits EmpathicBuildingDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "sensors",
            "desc": "Empathic Building sensors data provider; parent provider for APIs related to "
                "sensors",
            "type": "EmpathicBuildingSensorsDataProvider",
            "constructor_options": EmpathicBuildingDataProvider::ConstructorOptions,
            "supports_children": True,
            "children_can_support_apis": True,
            "children_can_support_observers": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });
    }

    private {
        #! Org info
        hash<auto> org;

        #! Loc data
        hash<auto> loc;

        #! Sensor map
        hash<string, hash<auto>> sensormap;
    }

    #! Creates the object from a REST connection
    constructor(RestClient::RestClient rest, *Logger::LoggerInterface logger, hash<auto> org, hash<auto> loc)
            : EmpathicBuildingDataProviderBase(rest, logger) {
        self.org = org;
        self.loc = loc;
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("EmpathicBuilding data provider for sensors for org: `%s` (%s), location `%s` (%s), and "
            "url: `%s`", org.id, org.name, loc.id, loc.name, rest.getSafeURL());
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Return data provider summary info
    *list<hash<DataProvider::DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map <DataProviderSummaryInfo>{
            "name": $1,
            "desc": sprintf("Empathic Building org %y loc %y sensor %y (%s)", org.id, loc.id, $1,
                EmpathicBuildingSensorDataProvider::getSensorDesc(sensormap{$1})),
            "type": "EmpathicBuildingSensorDataProvider",
            "supports_children": True,
            "children_can_support_apis": True,
        }, getChildProviderNames();
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys getAllSensors();
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        if (*hash<auto> sensor = getAllSensors(){name}) {
            return new EmpathicBuildingSensorDataProvider(rest, logger, org, loc, sensor);
        }
    }

    #! Returns access info for all tables
    private hash<string, hash<auto>> getAllSensors() {
        if (exists sensormap) {
            return sensormap;
        }

        hash<auto> info;
        try {
            sensormap = cast<*hash<string, hash<auto>>>(map {($1.vendor_id ? ($1.vendor_id + "-" + $1.id) : $1.id): $1}, doRestCommand("GET", "v1/organizations/"
                + org.id + "/locations/" + loc.id + "/sensors", NOTHING, \info).body) ?? {};
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            *hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            rethrow ex.err, ex.desc, ex.arg + ex_arg;
        }

        return sensormap;
    }
}
}
