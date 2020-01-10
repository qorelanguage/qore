# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProviderTypeEntry.qc data provider type entry

/** DataProviderTypeEntry.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
# strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider type entry info
public hashdecl DataProviderTypeEntryInfo {
    #! The name of the entry
    string name;

    #! The path to the entry including the name as the last element
    string path;

    #! @ref True if the entry has a type
    /** If @ref True then the type has the same name as the entry
    */
    bool has_type;

    #! The names of any children
    *list<string> children;
}

#! the DataProviderTypeEntry class
public class DataProviderTypeEntry {
    private {
        #! The path to this entry
        string path;

        #! The name of this entry
        string name;

        #! Children of this entry
        *hash<string, DataProviderTypeEntry> children;

        #! The type at this location
        *AbstractDataProviderType type;
    }

    #! Creates the entry as a root node
    /**
    */
    constructor() {
        path = "/";
        name = "/";
    }

    #! Creates the entry as a root node
    /**
    */
    constructor(string path, string name) {
        if (path == "/") {
            self.path = "/" + name;
        } else {
            self.path = path + "/" + name;
        }
        self.name = name;
    }

    #! Returns the name of the entry
    string getName() {
        return name;
    }

    #! Returns the path of the entry
    string getPath() {
        return path;
    }

    #! Adds a child node to the entry
    /** @param child the name of the child node to add

        @throw CHILD-ERROR a child with the same name already exists
    */
    addChild(string child) {
        if (children{child}) {
            throw "CHILD-ERROR", sprintf("%y already contains a child named %y", path, child);
        }
        children{child} = new DataProviderTypeEntry(path, child);
    }

    #! Sets the type for the entry
    /** @param type the type to set for this entry

        @throw TYPE-ERROR a type has already been set for this entry
    */
    setType(AbstractDataProviderType type) {
        if (self.type) {
            if (self.type != type) {
                throw "TYPE-ERROR", sprintf("%y already contains a type named %y; cannot set type %y", path,
                    self.type.getName(), type.getName());
            }
        } else {
            self.type = type;
        }
    }

    #! Returns information about this entry
    /** @return information about this entry
    */
    hash<DataProviderTypeEntryInfo> getInfo() {
        return <DataProviderTypeEntryInfo>{
            "name": name,
            "path": path,
            "has_type": exists type,
            "children": keys children,
        };
    }

    #! Returns a list of registered data provider type paths
    /**
        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *list<string> listTypes() {
        *list<string> l;
        if (type) {
            l += path;
        }
        foreach DataProviderTypeEntry entry in (children.iterator()) {
            *list<string> child_list = entry.listTypes();
            if (child_list) {
                l += child_list;
            }
        }

        return l;
    }

    #! Returns a list of child entry names, if any
    /** @return a list of child entry names, if any
    */
    *list<string> getChildNames() {
        return keys children;
    }

    #! Returns the type at the entry level, if any
    /** @return the type at the entry level, if any
    */
    *AbstractDataProviderType getType() {
        return type;
    }

    #! Returns the type at the entry level, if any, otherwise throws an exception
    /** @return the type at the entry level, if any, otherwise throws an exception

        @throw NO-TYPE this entry has no type, only children
    */
    *AbstractDataProviderType getTypeEx() {
        *AbstractDataProviderType type = getType();
        if (!type) {
            throw "NO-TYPE", sprintf("%y has no type, only children", path);
        }
        return type;
    }

    #! Returns the given child, if any
    /** @return the given child, if any
    */
    *DataProviderTypeEntry getChild(string child) {
        return children{child};
    }

    #! Returns the given child, if any, otherwise throws an exception
    /** @return the given child, if any, otherwise throws an exception

        @throw INVALID-CHILD the given child is unknown
    */
    *DataProviderTypeEntry getChildEx(string child) {
        *DataProviderTypeEntry rv = getChild(child);
        if (!rv) {
            throw "INVALID-CHILD", sprintf("%y has no child %y; known children: %y", path, child, keys children);
        }
        return rv;
    }

    #! Returns the given child and creates it if necessary
    /** @return the given child and creates it if necessary
    */
    DataProviderTypeEntry getCreateChild(string child) {
        if (*DataProviderTypeEntry rv = children{child}) {
            return rv;
        }
        return children{child} = new DataProviderTypeEntry(path, child);
    }
}
}
