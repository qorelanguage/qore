# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore EmpathicBuildingDataProvider module definition

/** EmpathicBuildingWatchSensorDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! Empathiic Building sensor event constant
/** The event data has the same structure as sensor info
*/
public const EVENT_EB_SENSOR = "sensor-event";

#! The EmpathicBuilding watch sensor data provider class
public class EmpathicBuildingWatchSensorDataProvider inherits EmpathicBuildingDataProviderBase,
        DataProvider::DelayedObservable {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "watch",
            "desc": "Empathic Building watch sensor data provider",
            "type": "EmpathicBuildingWatchSensorDataProvider",
            "supports_observable": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = AbstractDataProviderTypeMap."nothing";

        #! Response type
        const ResponseType = new EmpathicBuildingSensorType();

        #! Default poll interval in milliseconds
        const PollInterval = 10000;
    }

    private {
        #! Org data
        hash<auto> org;

        #! Loc data
        hash<auto> loc;

        #! Sensor data
        hash<auto> sensor;

        #! callable value to start a new thread; must take a callable object as a parameter and return an integer TID
        *code start_thread;

        #! TID of the I/O thread
        int tid;

        #! I/O thread counter
        Counter cnt();

        #! I/O thread lock
        Mutex m();

        #! I/O thread condition
        Condition cond();

        #! Stop flag
        bool stop_flag;

        #! Poll interval in milliseconds
        timeout poll_interval_ms = PollInterval;
    }

    #! Create the object from the REST connection
    constructor(RestClient::RestClient rest, *Logger::LoggerInterface logger, hash<auto> org, hash<auto> loc,
            hash<auto> sensor) : EmpathicBuildingDataProviderBase(rest, logger) {
        self.org = org;
        self.loc = loc;
        self.sensor = sensor;

        printf("sensor: %y\n", sensor);
    }

    #! Destroys the object
    destructor() {
        stop();
    }

    #! Returns the data provider name
    string getName() {
        return "watch";
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("EmpathicBuilding data provider for org: `%s` (%s), location `%s` (%s), sensor `%s` (%s) and "
            "url: `%s`", org.id, org.name, loc.id, loc.name, sensor.id, sensor.name ?? sensor.uuid,
            rest.getSafeURL());
    }

    #! Called when all observers have been added to the object
    /** This method is meant to trigger event generation
    */
    observersReady() {
        start();
    }

    #! Connects and starts receiving messages
    private start() {
        hash<auto> info;
        try {
            cnt.inc();
            on_error cnt.dec();
%ifdef PO_NO_THREAD_CONTROL
            tid = start_thread(\eventLoop());
%else
            tid = start_thread ? start_thread(\ioThread()) : background ioThread();
%endif
        } catch (hash<ExceptionInfo> ex) {
            LoggerWrapper::error("Failed to start I/O thread: %s", get_exception_string(ex));
            rethrow;
        }
    }

    #! Stops listening for new events
    private stop() {
        if (tid) {
            stop_flag = True;
            m.lock();
            on_exit m.unlock();
            cond.signal();
        }

        cnt.waitForZero();
    }

    #! I/O thread
    private ioThread() {
        on_exit {
            debug("Exiting the I/O thread");
            cnt.dec();
        }

        hash<auto> last;

        while (!stop_flag) {
            hash<auto> info;
            try {
                hash<auto> event = doRestCommand("GET", "v1/organizations/" + org.id + "/locations/" + loc.id
                    + "/sensors/" + sensor.id, NOTHING, \info).body;
                event = EmpathicBuildingSensorDataProvider::getSensorData(event);
                if (event != last) {
                    notifyObservers(EVENT_EB_SENSOR, event);
                    last = event;
                } else {
                    debug("No new event data available");
                }
            } catch (hash<ExceptionInfo> ex) {
                LoggerWrapper::error("Failed to acquire sensor data for org %s location %s sensor %s: %s", org.id,
                    loc.id, sensor.id, get_exception_string(ex));
            }

            debug("Sleeping for %d ms until next poll", poll_interval_ms);

            m.lock();
            on_exit m.unlock();
            cond.wait(m, poll_interval_ms);
        }
    }

    #! Returns a hash of all supported event types
    /** @return a hash of all supported event types

        @note only called if the provider supports the observer pattern / event API
    */
    private hash<string, hash<DataProviderMessageInfo>> getEventTypesImpl() {
        return {
            EVENT_EB_SENSOR: <DataProviderMessageInfo>{
                "desc": "EmpathicBuilding sensor data event",
                "type": EmpathicBuildingSensorType,
            },
        };
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}