# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore Observable class definition

/** Observable.qc Copyright 2022 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! This class represents an observable subject that will notify all of its registered observers
public class Observable {
    private {
        #! maps from object hash -> object
        hash<string, hash<ObserverEventInfo>> observer_map;
    }

    #! The method registers the given observer
    /** @param observer the observer to be registered
        @param ... event IDs for notifications; if no additional arguments are passed, then the observer is
        notified for all events

        @throw EVENT-ERROR type error; additional argument is not a string event ID

        @since Qorus 5.0 this method supports event IDs after the \a observer argument
    */
    registerObserver(Observer observer) {
        hash<ObserverEventInfo> info = <ObserverEventInfo>{
            "observer": observer,
        };
        foreach auto event_id in (argv) {
            if (!event_id.typeCode() == NT_STRING) {
                throw "EVENT-ERROR", sprintf("arguments following the observer must be event ID strings, got "
                    "type %y (%y) in argument %d instead", event_id.type(), event_id, $# + 2);
            }
            info.event_map{event_id} = True;
        }
        observer_map{observer.uniqueHash()} = info;
    }

    #! The method registers the given observer
    /** @param observer the observer to be registered
        @param ... event IDs for notifications; if no additional arguments are passed, then the observer is
        notified for all events

        @note this variant is meant for Java com.qoretechnologies.qorus.Observer objects

        @since Qorus 5.0 this method supports event IDs after the \a observer argument
    */
    registerObserver(object observer) {
        hash<ObserverEventInfo> info = <ObserverEventInfo>{
            "observer": new ObserverWrapper(observer),
        };
        foreach auto event_id in (argv) {
            if (!event_id.typeCode() == NT_STRING) {
                throw "EVENT-ERROR", sprintf("arguments following the observer must be event ID strings, got "
                    "type %y (%y) in argument %d instead", event_id.type(), event_id, $# + 2);
            }
            info.event_map{event_id} = True;
        }
        observer_map{observer.uniqueHash()} = info;
    }

    #! The method deregisters the given observer
    /** @param observer the observer to be deregistered

        @return True if found otherwise False
    */
    bool deregisterObserver(Observer observer) {
        return exists (remove observer_map{observer.uniqueHash()});
    }

    #! The method deregisters the given observer
    /** @param observer the observer to be deregistered

        @return True if found otherwise False

        @note this variant is meant for Java com.qoretechnologies.qorus.Observer objects
    */
    bool deregisterObserver(object observer) {
        return exists (remove observer_map{observer.uniqueHash()});
    }

    #! The method notifies all subscribed observers
    /** @param event_id unique ID identifying an event
        @param data_ hash representing new data to be passed to observers' update() methods
    */
    notifyObservers(string event_id, hash<auto> data_) {
        map $1.observer.update(event_id, data_), observer_map.iterator(), !$1.event_map || $1.event_map{event_id};
    }

    # for backwards-compatibility
    bool unregisterObserver(object observer) {
        deregisterObserver(observer);
    }

    #! Reimplement this method to implement logging
    /** @param level the Logger level
        @param fmt the format string
        @param ... any arguments for any format placeholders in \a fmt
    */
    log(int level, string fmt) {
        delete argv;
        # this method intentionally left blank
    }
}

#! This class represents an observable subject that waits for external confirmation before raising events
public class DelayedObservable inherits Observable {
    #! Called when all observers have been added to the object
    /** This method is meant to trigger event generation
    */
    abstract observersReady();
}
}

# private namespace; not exported
namespace Priv {
# Stores an observer with any subscribed events
hashdecl ObserverEventInfo {
    # Observer
    Observer observer;

    # Map of subscribed event IDs; if the map is empty, then all events are registered
    hash<string, bool> event_map;
}

# wraps a foreign observer object as an Observer
class ObserverWrapper inherits Observer {
    private {
        object obj;
    }

    # creates the wrapper object
    constructor(object obj) {
        # ensure that the class has a compatible "update()" method
        Class cls = Class::getClass(obj);
        on_error rethrow "OBSERVER-ERROR", sprintf("cannot wrap object of class %y; cannot find a compatible "
            "\"update(string, hash<auto>)\" method: %s: %s", cls.getName(), $1.err, $1.desc);
        cls.getNormalVariant("update", StringType, HashType);
        self.obj = obj;
    }

    # calls the wrapped object's update method
    update(string event_id, hash<auto> data_) {
        obj.update(event_id, data_);
    }
}
}