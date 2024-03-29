# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProviderTypeCache.qc module for data access and introspection

/** DataProviderTypeCache.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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

%requires reflection
%requires Util

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider type cache class
public class DataProviderTypeCache inherits Qore::Serializable {
    private {
        #! Data provider data type cache
        DataProviderTypeEntry root();

        #! Data provider type cache lock
        transient RWLock rwlock();

        #! The size of the cache
        int size = 0;
    }

    #! Register a new data provider type in the cache
    /** @param path a \c "/" separated path for the type; this is the lookup index
        @param type the new data provider type
        @param locked if the data type should be locked, prohibiting updates and deletions
        @param set_rollback set rollback code; for use in module registration

        @throw PROVIDER-ERROR the given provider has already been registered

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    registerType(string path, AbstractDataProviderType type, bool locked = False, *bool set_rollback) {
        list<string> type_path = DataProvider::getPathList(path);

        DataProviderTypeEntry type_entry = root;

        bool needs_lock = !rwlock.writeLockOwner();
        if (needs_lock) {
            rwlock.writeLock();
        }
        on_exit if (needs_lock) {
            rwlock.writeUnlock();
        }

        map type_entry = type_entry.getCreateChild($1), type_path;

        if (type_entry.setType(type, locked)) {
            ++size;
        }

        if (set_rollback) {
            # deregister the app automatically if module initialization fails
            ImplicitModuleTransaction::add(sub () { deregisterType(path); });
        }
    }

    #! Register or replaces a data provider type in the cache
    /** @param path a \c "/" separated path for the type; this is the lookup index
        @param type the new data provider type

        @return True if the type was replaced or added

        @throw TYPE-LOCK-ERROR type entry cannot be replaced, because it is locked

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    bool registerOrReplaceType(string path, AbstractDataProviderType type) {
        list<string> type_path = DataProvider::getPathList(path);

        DataProviderTypeEntry type_entry = root;

        bool needs_lock = !rwlock.writeLockOwner();
        if (needs_lock) {
            rwlock.writeLock();
        }
        on_exit if (needs_lock) {
            rwlock.writeUnlock();
        }

        map type_entry = type_entry.getCreateChild($1), type_path;

        *AbstractDataProviderType old_type = type_entry.setOrReplaceType(type);
        if (!old_type) {
            ++size;
        }
        return old_type != type;
    }

    #! Removes a type from the type cache
    /** @param path a \c "/" separated path for the type; this is the lookup index

        @return the type removed or @ref NOTHING if the type was not present

        @throw TYPE-LOCK-ERROR the given type cannot be removed, because it is locked
    */
    *AbstractDataProviderType removeType(string path) {
        list<string> type_path = DataProvider::getPathList(path);

        DataProviderTypeEntry type_entry = root;
        DataProviderTypeEntry prev;

        bool needs_lock = !rwlock.writeLockOwner();
        if (needs_lock) {
            rwlock.writeLock();
        }
        on_exit if (needs_lock) {
            rwlock.writeUnlock();
        }

        string node;
        foreach node in (type_path) {
            *DataProviderTypeEntry next_entry = type_entry.getChild(node);
            if (!next_entry) {
                return;
            }
            prev = type_entry;
            type_entry = next_entry;
        }

        *AbstractDataProviderType type = type_entry.getType();
        if (!type || !prev) {
            return;
        }
        prev.removeChild(node);
        if (type) {
            --size;
        }
        return type;
    }

    #! Returns the number of types in the cache
    int size() {
        return self.size;
    }

    #! Returns True if the cache is empty, False if not
    bool empty() {
        return !size;
    }

    #! Returns the root type entry
    DataProviderTypeEntry getRoot() {
        return root;
    }

    #! Clears the type hierarchy of all non-locked types
    clear() {
        bool needs_lock = !rwlock.writeLockOwner();
        if (needs_lock) {
            rwlock.writeLock();
        }
        on_exit if (needs_lock) {
            rwlock.writeUnlock();
        }

        root.clear();
    }

    #! Returns a list of registered data provider type paths
    /**
        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *list<string> listTypes() {
        return root.listTypes();
    }

    #! Returns a list of registered data provider type paths where the types are "parent" types
    /** "Parent" types are types where children were added with the parent

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *list<string> listParentTypes() {
        return root.listParentTypes();
    }

    #! Locks all types
    lockAll() {
        root.lockAll();
    }

    #! Returns the given data provider type or @ref nothing if not present
    /** @param path a \c "/" separated path for the type; trailing path elements can be a path into fields

        @return the given data provider type; @ref NOTHING is returned if the type cannot be resolved

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *AbstractDataProviderType getType(string path) {
        return getTypeImpl(path, False);
    }

    #! Returns the given data provider type or throws an exception if not present
    /** @param path a \c "/" separated path for the type; trailing path elements can be a path into fields

        @return the given data provider type; an exception is thrown if the data provider is unknown

        @throw PROVIDER-ERROR the given data provider type path is unknown

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    AbstractDataProviderType getTypeEx(string path) {
        return getTypeImpl(path, True);
    }

    #! Returns the given data provider type or @ref nothing if not present
    /** @param path a \c "/" separated path for the type; trailing path elements can be a path into fields
        @param rec_children_only if @ref True, then only children with fields are returned

        @return the given data provider type; @ref NOTHING is returned if the type cannot be resolved

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *hash<DataProviderTypeEntryInfo> getTypeInfo(string path, *bool rec_children_only) {
        return getTypeInfoImpl(path, False, rec_children_only);
    }

    #! Returns the given data provider type or throws an exception if not present
    /** @param path a \c "/" separated path for the type; trailing path elements can be a path into fields
        @param rec_children_only if @ref True, then only children with fields are returned

        @return the given data provider type; an exception is thrown if the data provider is unknown

        @throw PROVIDER-ERROR the given data provider type path is unknown

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    hash<DataProviderTypeEntryInfo> getTypeInfoEx(string path, *bool rec_children_only) {
        return getTypeInfoImpl(path, True, rec_children_only);
    }

    #! Returns the given data provider type and either throws an exception if not present or returns @ref NOTHING
    /** @param path a \c "/" separated path for the type
        @param throw_exception if @ref True then an exception is thrown if the path cannot be resolved, otherwise
        @ref NOTHING is returned
        @param type_loader a closure taking a list of strings giving the full path to the type and returning
        <tt>*AbstractDataProviderType</tt>

        @return the given data provider type or @ref NOTHING is returned if the path cannot be resolved and the
        \a throw_exception argument is @ref False

        @throw PROVIDER-ERROR the given data provider type path is unknown; only thrown if the \a throw_exception
        argument is @ref True

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *AbstractDataProviderType getTypeImpl(string path, bool throw_exception, *code type_loader) {
        # we have to grab the write lock if there is a type_loader
        bool need_lock = !rwlock.lockOwner();
        if (need_lock) {
            if (type_loader) {
                rwlock.writeLock();
            } else {
                rwlock.readLock();
            }
        }
        on_exit if (need_lock) {
            if (type_loader) {
                rwlock.writeUnlock();
            } else {
                rwlock.readUnlock();
            }
        }

        list<string> type_path = DataProvider::getPathList(path);
        DataProviderTypeEntry type_entry = root;
        foreach string node in (type_path) {
            *DataProviderTypeEntry next_entry = type_entry.getChild(node);
            if (!next_entry) {
                if (!need_lock && type_loader && rwlock.readLockOwner()) {
                    throw "TYPE-CACHE-LOCK-ERROR", "The calling thread is holding the read lock, but the write lock "
                        "must be acquired to load types dynamically with the type loader";
                }

                if (type_loader && (next_entry = type_loader(type_path))) {
                    type_entry = next_entry;
                    break;
                }
                # check for fields
                *AbstractDataProviderType type = type_entry.getType();
                if (type) {
                    return getTypeFromFields(path, type, type_path[$#..], throw_exception);
                }
                if (throw_exception) {
                    throw "PROVIDER-ERROR", sprintf("%y: data provider type element %y is unknown in %y; known children: %y",
                        path, node, type_entry.getPath(), type_entry.getChildNames());
                }
                return;
            }
            type_entry = next_entry;
        }

        *AbstractDataProviderType type = type_entry.getType();
        if (!type && throw_exception) {
            throw "PROVIDER-ERROR", sprintf("data provider type path %y does not contain a type but rather is an "
                "intermediate node with children: %y", path, type_entry.getChildNames());
        }
        return type;
    }

    #! Returns information for the given data provider type and either throws an exception if not present or returns @ref NOTHING
    /** @param path a \c "/" separated path for the type
        @param throw_exception if @ref True then an exception is thrown if the path cannot be resolved, otherwise
        @ref NOTHING is returned
        @param rec_children_only if @ref True, then only children with fields are returned

        @return information for the given data provider type or @ref NOTHING is returned if the path cannot be
        resolved and the \a throw_exception argument is @ref False

        @throw PROVIDER-ERROR the given data provider type path is unknown; only thrown if the \a throw_exception
        argument is @ref True

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    *hash<DataProviderTypeEntryInfo> getTypeInfoImpl(string path, bool throw_exception, *bool rec_children_only) {
        # we have to grab the write lock if there is a type_loader
        bool need_lock = !rwlock.lockOwner();
        if (need_lock) {
            rwlock.readLock();
        }
        on_exit if (need_lock) {
            rwlock.readUnlock();
        }

        list<string> type_path = DataProvider::getPathList(path);
        DataProviderTypeEntry type_entry = root;
        foreach string node in (type_path) {
            *DataProviderTypeEntry next_entry = type_entry.getChild(node);
            if (!next_entry) {
                if (throw_exception) {
                    throw "PROVIDER-ERROR", sprintf("%y: data provider type element %y is unknown in %y; known "
                        "children: %y", path, node, type_entry.getPath(), type_entry.getChildNames());
                }
                return;
            }
            type_entry = next_entry;
        }

        return type_entry.getInfo(rec_children_only);
    }

    #! Returns the RWLock to allow for external serialization
    RWLock getLock() {
        return rwlock;
    }

    #! Returns the given data provider type and either throws an exception if not present or returns @ref NOTHING
    /** @param path a \c "/" separated path for the type
        @param type the base type for the search
        @param type_path the path in the type for fields leading to the final return type
        @param throw_exception if @ref True then an exception is thrown if the path cannot be resolved, otherwise
        @ref NOTHING is returned

        @return the given data provider type or @ref NOTHING is returned if the path cannot be resolved and the
        \a throw_exception argument is @ref False

        @throw PROVIDER-ERROR the given data provider type path is unknown; only thrown if the \a throw_exception
        argument is @ref True

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    private static *AbstractDataProviderType getTypeFromFields(string path, AbstractDataProviderType type,
            list<string> type_path, bool throw_exception) {
        AbstractDataProviderType curr = type;
        foreach string elem in (type_path) {
            *AbstractDataProviderType new_type = curr.getFieldType(elem);
            if (!new_type) {
                if (throw_exception) {
                    throw "PROVIDER-ERROR", sprintf("%y: field %y is unknown in type %y; known fields: %y", path,
                        elem, curr.getName(), keys curr.getFields());
                }
                return;
            }
            curr = new_type;
        }
        return curr;
    }

    #! Deregisters the type at the given path
    /** @param path a \c "/" separated path for the type; this is the lookup index
    */
    private deregisterType(string path) {
        list<string> type_path = DataProvider::getPathList(path);

        *DataProviderTypeEntry type_entry = root;

        bool needs_lock = !rwlock.writeLockOwner();
        if (needs_lock) {
            rwlock.writeLock();
        }
        on_exit if (needs_lock) {
            rwlock.writeUnlock();
        }

        for (int i = 0, int e = type_path.size() - 1; i < e; ++i) {
            if (!(type_entry = type_entry.getChild())) {
                return;
            }
        }

        DataProviderTypeEntryPrivate::forceRemoveChild(type_entry, type_path.last());
    }
}
}
