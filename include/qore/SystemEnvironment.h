/*
  SystemEnvironment.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_SYSTEMENVIRONMENT_H

#define _QORE_SYSTEMENVIRONMENT_H

//! class used to safely manipulate the system environment
/** On some platforms (HP-UX for example), the system environment cannot be accessed
    safely from multiple threads without a lock.  This class guarantees thread-safe
    access to the environment on all systems (as long as all accesses are made through
    this class).  There is only one of the objects; the constructor and destructor are
    not exported in the public interface of the library and therefore can only be 
    instantiated internally anyway.
    To make multiple updates atomically within the environment lock, use AtomicEnvironmentSetter
    @see AtomicEnvironmentSetter
 */
class SystemEnvironment {
      friend class AtomicEnvironmentSetter;

   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SystemEnvironment(const SystemEnvironment&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SystemEnvironment& operator=(const SystemEnvironment&);

   protected:
      //! sets the given environment variable to the value passed, respecting the overwrite flag
      /** unlocked
	 @param name the name of the environment variable to set
	 @param value the value of the environment variable
	 @param overwrite the overwrite flag
       */
      DLLLOCAL static int set_intern(const char *name, const char *value, bool overwrite = 1);

      //! returns the value of the environment variable as a new QoreString object, 0 if not present, caller owns the pointer returned
      /** unlocked
	 @param name the name of the environment variable
	 @return a QoreString pointer (or 0 if the variable does not exist), caller owns the pointer returned	 
       */
      DLLLOCAL static class QoreString *get_intern(const char *name);

      //! returns the value of the environment variable as a new QoreStringNode object, 0 if not present, caller owns the reference count of the pointer returned
      /** unlocked
	 @param name the name of the environment variable
	 @return a QoreStringNode pointer (or 0 if the variable does not exist), caller owns the reference count of the pointer returned	 
       */
      DLLLOCAL static class QoreStringNode *get_as_string_node_intern(const char *name);

      //! appends the value of the given environment variable to a QoreString, returns 0 for OK, -1 for not found
      /** unlocked
	 @param name the name of the environment variable
	 @param str a reference to a QoreString object where the value will be concatenated if the environment variable exists
	 @return 0 for OK (environment variable found and value concatenated to string), or -1 for not found
       */
      DLLLOCAL static int get_intern(const char *name, class QoreString &str);
      
      //! unsets the given environment variable
      /** unlocked
	 @param name the name of the environment variable to unset
	 @return 0 for OK, non-0 for error
       */
      DLLLOCAL static int unset_intern(const char *name);
      
   public:
      DLLLOCAL SystemEnvironment();
      DLLLOCAL ~SystemEnvironment();

      //! sets the given environment variable to the value passed, respecting the overwrite flag
      /**
	 @param name the name of the environment variable to set
	 @param value the value of the environment variable
	 @param overwrite the overwrite flag
       */
      DLLEXPORT static int set(const char *name, const char *value, bool overwrite = 1);

      //! returns the value of the environment variable as a new QoreString object, 0 if not present, caller owns the pointer returned
      /**
	 @param name the name of the environment variable
	 @return a QoreString pointer (or 0 if the variable does not exist), caller owns the pointer returned	 
       */
      DLLEXPORT static class QoreString *get(const char *name);

      //! returns the value of the environment variable as a new QoreStringNode object, 0 if not present, caller owns the reference count of the pointer returned
      /**
	 @param name the name of the environment variable
	 @return a QoreStringNode pointer (or 0 if the variable does not exist), caller owns the reference count of the pointer returned	 
       */
      DLLEXPORT static class QoreStringNode *getAsStringNode(const char *name);

      //! appends the value of the given environment variable to a QoreString, returns 0 for OK, -1 for not found
      /**
	 @param name the name of the environment variable
	 @param str a reference to a QoreString object where the value will be concatenated if the environment variable exists
	 @return 0 for OK (environment variable found and value concatenated to string), or -1 for not found
       */
      DLLEXPORT static int get(const char *name, class QoreString &str);
      
      //! unsets the given environment variable
      /**
	 @param name the name of the environment variable to unset
	 @return 0 for OK, non-0 for error
       */
      DLLEXPORT static int unset(const char *name);
      
      //! returns true if the environment variable exists and has a value, false if not
      /**
	 @param name the name of the environment variable to check
	 @return true if the environment variable exists and has a value, false if not
       */
      DLLEXPORT static bool valueExists(const char* name); // true if exists and is not empty
};

DLLEXPORT extern SystemEnvironment SysEnv;

//! class allowing for multiple updates to the system environment within a single lock in a thread-safe way, can be used on the stack
/** some systems (HP-UX, for examplex) require a lock to access the environment in a multi-threaded environment.
    This class allows for multiple updates to be done within the scope of the lock
    @see SystemEnvironment
 */
class AtomicEnvironmentSetter
{
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AtomicEnvironmentSetter(const AtomicEnvironmentSetter&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AtomicEnvironmentSetter& operator=(const AtomicEnvironmentSetter&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

   public:
      DLLEXPORT AtomicEnvironmentSetter();
      DLLEXPORT ~AtomicEnvironmentSetter();

      //! sets the given environment variable to the value passed, respecting the overwrite flag
      /**
	 @param name the name of the environment variable to set
	 @param value the value of the environment variable
	 @param overwrite the overwrite flag
       */
      DLLEXPORT int set(const char *name, const char *value, bool overwrite = 1);

      //! returns the value of the environment variable as a new QoreString object, 0 if not present, caller owns the pointer returned
      /**
	 @param name the name of the environment variable
	 @return a QoreString pointer (or 0 if the variable does not exist), caller owns the pointer returned	 
       */
      DLLEXPORT class QoreString *get(const char *name);

      //! returns the value of the environment variable as a new QoreStringNode object, 0 if not present, caller owns the reference count of the pointer returned
      /**
	 @param name the name of the environment variable
	 @return a QoreStringNode pointer (or 0 if the variable does not exist), caller owns the reference count of the pointer returned	 
       */
      DLLEXPORT class QoreStringNode *getAsStringNode(const char *name);

      //! appends the value of the given environment variable to a QoreString, returns 0 for OK, -1 for not found
      /**
	 @param name the name of the environment variable
	 @param str a reference to a QoreString object where the value will be concatenated if the environment variable exists
	 @return 0 for OK (environment variable found and value concatenated to string), or -1 for not found
       */
      DLLEXPORT int get(const char *name, class QoreString &str);
      
      //! unsets the given environment variable
      /**
	 @param name the name of the environment variable to unset
	 @return 0 for OK, non-0 for error
       */
      DLLEXPORT int unset(const char *name);
      
      //! returns true if the environment variable exists and has a value, false if not
      /**
	 @param name the name of the environment variable to check
	 @return true if the environment variable exists and has a value, false if not
       */
      DLLEXPORT bool valueExists(const char* name); // true if exists and is not empty
};

#endif
