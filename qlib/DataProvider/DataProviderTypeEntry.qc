# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProviderTypeEntry.qc data provider type entry

/** DataProviderTypeEntry.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

    #! The description of the entry
    string desc;

    #! The path to the entry including the name as the last element
    string path;

    #! @ref True if the entry has a type
    /** If @ref True then the type has the same name as the entry
    */
    bool has_type;

    #! @ref True if the entry is locked and therefore cannot be deleted or updated
    /** @note only type entries can be locked
    */
    bool locked = False;

    #! The names of any children
    *list<string> children;
}

#! the DataProviderTypeEntry class
public class DataProviderTypeEntry inherits Qore::Serializable {
    private {
        #! The path to this entry
        string path;

        #! The name of this entry
        string name;

        #! Children of this entry
        *hash<string, DataProviderTypeEntry> children;

        #! Names of attributes that can be referenced as children
        *hash<string, DataProviderTypeEntry> type_children;

        #! Names of attributes that can be referenced as children that have their own fields
        *hash<string, DataProviderTypeEntry> rec_children;

        #! The type at this location
        *AbstractDataProviderType type;

        #! @ref True if the entry is locked and therefore cannot be deleted or updated
        /** @note only type entries can be locked
        */
        bool locked = False;

        #! @ref True if this entry is a "parent" type; if the children were added atomically with this entry
        bool parent = False;
    }

    #! Creates the entry as a root node
    /**
    */
    constructor() {
        path = "/";
        name = "/";
    }

    #! Creates the entry as a child node
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

    #! Returns the description of the entry
    string getDesc() {
        return type ? type.getDesc() : "type entry group";
    }

    #! Returns the path of the entry
    string getPath() {
        return path;
    }

    #! Returns True if the entry has a type
    bool hasType() {
        return exists type;
    }

    #! Returns True if the entry is a "parent" entry, meaning that all children were added with this entry
    bool isParent() {
        return parent;
    }

    #! Locks the entry, so it can't be removed or changed
    /** @return the previous lock status

        @throw TYPE-LOCK-ERROR the entry cannot be locked, because it does not have a type

        @note Locking is a one-way operation
    */
    bool lock() {
        if (!type) {
            throw "TYPE-LOCK-ERROR", sprintf("type entry %y cannot be locked, because it does not have a type", path);
        }

        bool rv = locked;
        locked = True;
        return rv;
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
        @param locked if the data type should be locked, prohibiting updates and deletions

        @return True if a new type was added, False if not

        @throw TYPE-ERROR a type has already been set for this entry
    */
    bool setType(AbstractDataProviderType type, bool locked = False) {
        if (self.type) {
            if (self.type != type) {
                throw "TYPE-ERROR", sprintf("%y already contains a type named %y; cannot set type %y", path,
                    self.type.getName(), type.getName());
            }
            return False;
        }

        self.type = type;
        self.locked = locked;
        parent = True;
        setTypeChildren();
        return True;
    }

    #! Sets or replaces the type for the entry
    /** @param type the type to set for this entry

        @return True if a new type was added, False if not

        @throw TYPE-LOCK-ERROR type entry cannot be replaced, because it is locked
    */
    *AbstractDataProviderType setOrReplaceType(AbstractDataProviderType type) {
        if (locked && type != self.type) {
            throw "TYPE-LOCK-ERROR", sprintf("type entry %y cannot be replaced, because it is locked", path);
        }
        *AbstractDataProviderType rv = self.type;
        self.type = type;
        parent = True;
        setTypeChildren();
        return rv;
    }

    #! Returns information about this entry
    /** @param rec_children_only if @ref True, then only children with fields are returned

        @return information about this entry
    */
    hash<DataProviderTypeEntryInfo> getInfo(*bool rec_children_only) {
        return <DataProviderTypeEntryInfo>{
            "name": name,
            "desc": getDesc(),
            "path": path,
            "has_type": exists type,
            "locked": locked,
            "children": rec_children_only
                ? keys (rec_children + children)
                : keys (type_children + children),
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
        foreach DataProviderTypeEntry entry in ((type_children + children).iterator()) {
            *list<string> child_list = entry.listTypes();
            if (child_list) {
                l += child_list;
            }
        }

        return l;
    }

    #! Returns a list of registered data provider type paths where each type listed is a "parent" type
    /** "Parent" types are types where children are added with the parent

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *list<string> listParentTypes() {
        *list<string> l;
        if (type && parent) {
            l += path;
        }
        foreach DataProviderTypeEntry entry in ((type_children + children).iterator()) {
            *list<string> child_list = entry.listParentTypes();
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
        return keys (type_children + children);
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
        return children{child} ?? type_children{child};
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

    #! Removes the given child, if it exists
    /** @throw TYPE-LOCK-ERROR the given child cannot be removed because it is locked
    */
    removeChild(string child) {
        if (children{child}.locked) {
            throw "TYPE-LOCK-ERROR", sprintf("%s/%s is locked and cannot be removed", path, child);
        }
        if (!children{child} && type_children{child}) {
            throw "TYPE-LOCK-ERROR", sprintf("%s/%s is an attribute of the type and cannot be removed", path, child);
        }
        remove children{child};
    }

    #! Locks all types
    lockAll() {
        if (type && !locked) {
            locked = True;
        }
        map $1.lockAll(), children.iterator();
    }

    #! Clears the type hierarchy of all non-locked types
    bool clear() {
        if (!locked && type) {
            remove type;
        }
        foreach DataProviderTypeEntry entry in (children.iterator()) {
            if (entry.clear()) {
                remove children{entry.getName()};
            }
        }
        return !type && !children;
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

    #! Sets up attributes as children
    private:internal setTypeChildren() {
        *hash<string, AbstractDataField> fields = type.getFields();
        if (fields) {
            type_children = map {$1.key: new DataProviderTypeEntry(path + "/" + name, $1.key, $1.value.getType())},
                fields.pairIterator();
            rec_children = map {$1.key: $1.value}, type_children.pairIterator(), $1.value.getType().getFields();
            if (!rec_children) {
                remove rec_children;
            }
        } else {
            remove type_children;
            remove rec_children;
        }
    }

    #! Creates the entry as a child node
    /**
    */
    private:internal constructor(string path, string name, AbstractDataProviderType type) {
        if (path == "/") {
            self.path = "/" + name;
        } else {
            self.path = path + "/" + name;
        }
        self.name = name;
        self.type = type;
        setTypeChildren();
    }
}
}
