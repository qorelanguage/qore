/*
  modules/Tuxedo/fml_api.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/Namespace.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/params.h>
#include <qore/QoreString.h>
#include <qore/LockedObject.h>

#include "fml_api.h"
#include <fml32.h> // must be before <fml.h>
#include <fml.h>
#include "ScopeGuard.h"
#include "QoreTuxedoTypedBuffer.h"


#include <ctype.h>
#include <string>
#include <vector>
#include <utility>
#include <memory>

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::auto_ptr;

//-----------------------------------------------------------------------------
// Returns previous value (empty if none)
static string set_env_variable(const char* name, const char* value)
{
  const char* old = getenv(name);
  string result(old ? old : "");
  if (value && value[0]) {
    setenv(name, value, 1);
  } else {
   unsetenv(name);
  }
  return result;
}

//-----------------------------------------------------------------------------
// Serialized by mutex
static vector<pair<string, string> > set_env_variables(const vector<pair<string, string> >& vars)
{
  static LockedObject mutex;
  mutex.lock();
  ON_BLOCK_EXIT_OBJ(mutex, &LockedObject::unlock);

  vector<pair<string, string> > result;
  for (unsigned i = 0; i < vars.size(); ++i) {
    string old = set_env_variable(vars[i].first.c_str(), vars[i].second.c_str());
    result.push_back(make_pair(vars[i].first, old));
  }
  return result;
}

//-----------------------------------------------------------------------------
// See http://edocs.bea.com/tuxedo/tux91/fml/fml04.htm#101389
static vector<pair<string, string> > set_fml_env_variables(const vector<string>& files, bool is_fml32)
{
  string joined_files;
  for (unsigned i = 0, n = files.size(); i != n; ++i) {
    joined_files += files[i];
    if (i + 1 != n) joined_files += ","; // that's the Tuxedo way
  }

  vector<pair<string, string> > new_env;
  new_env.push_back(make_pair(is_fml32 ? "FIELDTBLS32" : "FIELDTBLS", joined_files));
  new_env.push_back(make_pair(is_fml32 ? "FLDTBLDIR32" : "FLDTBLDIR", string())); // not used, all files need to be fullpath
  
  return set_env_variables(new_env);
}

//-----------------------------------------------------------------------------
// Extract all names from given table description file.
// See http://edocs.bea.com/tuxedo/tux91/fml/fml04.htm#1010346
static vector<string> read_names_from_fml_description_file(const char* filename, ExceptionSink* xsink)
{
   vector<string> result;
  FILE* f = fopen(filename, "rt");
  if (!f) {
    xsink->raiseException("FML[32]_process_description_tables", "read_names_from_fml_description_file(): the file [ %s ] cannot be opened.", filename);
    return result;
  }
  ON_BLOCK_EXIT(fclose, f);

  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    char* thumb = line;
    while (isspace(*thumb)) ++thumb;
    if (!*thumb) continue;
    if (strstr(thumb, "*base") == thumb) continue;
    if (*thumb == '#') continue;
   
     char* name_end = thumb;
     while (!isspace(*name_end) && *name_end) ++name_end;
     *name_end = 0;
    
     if (name_end == thumb) continue; // ???
     result.push_back(thumb);
  }
  return result;
}

//-----------------------------------------------------------------------------
// Extract all names from all given table description file.
static vector<string> read_names_from_all_fml_description_files(const vector<string>& files, ExceptionSink* xsink)
{
   vector<string> result;
   for (unsigned i = 0; i < files.size(); ++i) {
     vector<string> aux = read_names_from_fml_description_file(files[i].c_str(), xsink);
     if (xsink->isException()) {
       return vector<string>();
     }
     result.insert(result.end(), aux.begin(), aux.end());
   }
   return result;
}

//-----------------------------------------------------------------------------
// Return files passed into FML[32]_process_description_tables()
static vector<string> extract_file_names(QoreNode* params, ExceptionSink* xsink)
{
  typedef vector<string> result_t;
  char* name = "FML[32]_process_description_tables";
  char* params_err = "One parameter, filename string or list of filenames, expected.";

  if (!get_param(params, 0)) {
    xsink->raiseException(name, params_err);
    return result_t();
  }
  if (get_param(params, 1)) {
    xsink->raiseException(name, params_err);
    return result_t();
  }

  vector<string> files;

  QoreNode* n = test_param(params, NT_STRING, 0);
  if (n) {
    char* filename = n->val.String->getBuffer();
    if (!filename || !filename[0]) {
      xsink->raiseException(name, "Empty string cannot be passed as the filename.");
      return result_t();
    }
    files.push_back(filename);
  } else {
    n = test_param(params, NT_LIST, 0);
    if (!n) {
      xsink->raiseException(name, params_err);
      return result_t();
    }
    List* l = n->val.list;
    int cnt = l->size();
    for (int i = 0; i < cnt; ++i) {
      n = l->retrieve_entry(i);
      if (!n) {
        xsink->raiseException(name, "The list of files cannot contain empty item.");
        return result_t();
      }
      if (n->type != NT_STRING) {
        xsink->raiseException(name, "The list needs to contains file names as strings.");
        return result_t();
      }
      char* filename = n->val.String->getBuffer();
      if (!filename || !filename[0]) {
        xsink->raiseException(name, "Empty string cannot be passed as the filename.");
        return result_t();
      }
      files.push_back(filename);
    }
  }
  if (files.empty()) {
    xsink->raiseException(name, "At least one file needs to be specified.");
    return result_t();
  }
  return files;
}

//-----------------------------------------------------------------------------
static QoreNode* process_description_tables(QoreNode* params, ExceptionSink* xsink, bool is_fml32)
{
  vector<string> files = extract_file_names(params, xsink);
  if (xsink->isException()) {
    return 0;
  }

  vector<string> all_names = read_names_from_all_fml_description_files(files, xsink);
  if (xsink->isException()) {
    return 0;
  }

  vector<pair<string, string> > old_env = set_fml_env_variables(files, is_fml32);

  // before returning the old variables back free the tables from memory
  // (assumption: Fldid[32] is idempotent)
  ON_BLOCK_EXIT((is_fml32 ? &Fidnm_unload : &Fidnm_unload32));
  ON_BLOCK_EXIT(set_env_variables, old_env);

  auto_ptr<Hash> result(new Hash);

  char* func_name = "FML[32]_process_description_tables";

  for (unsigned i = 0, n = all_names.size(); i != n; ++i) {
    char* name = (char*)all_names[i].c_str();
    FLDID32 id;
    if (is_fml32) {
      id = Fldid32(name);
    } else {
      id = Fldid(name);
    }
    if (id == BADFLDID) {
      xsink->raiseException(func_name, "Fldid[32](\"%s\") failed. Ferror = %d.", name, Ferror);
      return 0;
    }
    int type;
    if (is_fml32) {
      type = Fldtype32(id);
    } else {
      type = Fldtype(id);
    }
    List* list = new List();
    list->insert(new QoreNode((int64)id));
    list->insert(new QoreNode((int64)type));
    result->setKeyValue(name, new QoreNode(list), xsink);
    if (xsink->isException()) {
      return 0;
    }
  }
  return new QoreNode(result.release());
}

//-----------------------------------------------------------------------------
static QoreNode* f_fml_process_description_tables(QoreNode* params, ExceptionSink* xsink)
{
  return process_description_tables(params, xsink, false);
}

//-----------------------------------------------------------------------------
static QoreNode* f_fml32_process_description_tables(QoreNode* params, ExceptionSink* xsink)
{
  return process_description_tables(params, xsink, true);
}

//-----------------------------------------------------------------------------
static QoreNode* write_into_buffer(QoreNode* params, ExceptionSink* xsink, bool is_fml32)
{
  return 0; // TBD
}

//-----------------------------------------------------------------------------
static QoreNode* get_from_buffer(QoreNode* params, ExceptionSink* xsink, bool is_fml32)
{
  return 0; // TBD
}


//-----------------------------------------------------------------------------
static QoreNode* f_fml_write_into_buffer(QoreNode* params, ExceptionSink* xsink)
{
  return write_into_buffer(params, xsink, false);
}

//-----------------------------------------------------------------------------
static QoreNode* f_fml32_write_into_buffer(QoreNode* params, ExceptionSink* xsink)
{
  return write_into_buffer(params, xsink, true);
}

//-----------------------------------------------------------------------------
static QoreNode* f_fml_get_from_buffer(QoreNode* params, ExceptionSink* xsink)
{
  return get_from_buffer(params, xsink, false);
}

//-----------------------------------------------------------------------------
static QoreNode* f_fml32_get_from_buffer(QoreNode* params, ExceptionSink* xsink)
{
  return get_from_buffer(params, xsink, true);
}

//-----------------------------------------------------------------------------
void tuxedo_fml_init()
{
 builtinFunctions.add("processFMLDescripionTables", f_fml_process_description_tables, QDOM_NETWORK);
 builtinFunctions.add("processFML32DescripionTables", f_fml32_process_description_tables, QDOM_NETWORK);

 builtinFunctions.add("putFMLInTypedBuffer", f_fml_write_into_buffer, QDOM_NETWORK);
 builtinFunctions.add("putFML32InTypedBuffer", f_fml_write_into_buffer, QDOM_NETWORK);
 builtinFunctions.add("getFMLFromTypedBuffer", f_fml_get_from_buffer, QDOM_NETWORK);
 builtinFunctions.add("getFML32FromTypedBuffer", f_fml32_get_from_buffer, QDOM_NETWORK);
}

//-----------------------------------------------------------------------------
void tuxedo_fml_ns_init(Namespace* ns)
{
 // FML/FML32 types
  ns->addConstant("FLD_SHORT", new QoreNode((int64)FLD_SHORT));
  ns->addConstant("FLD_LONG", new QoreNode((int64)FLD_LONG));
  ns->addConstant("FLD_CHAR", new QoreNode((int64)FLD_CHAR));
  ns->addConstant("FLD_FLOAT", new QoreNode((int64)FLD_FLOAT));
  ns->addConstant("FLD_DOUBLE", new QoreNode((int64)FLD_DOUBLE));
  ns->addConstant("FLD_STRING", new QoreNode((int64)FLD_STRING));
  ns->addConstant("FLD_CARRAY", new QoreNode((int64)FLD_CARRAY));
  ns->addConstant("FLD_PTR", new QoreNode((int64)FLD_PTR));
  ns->addConstant("FLD_FML32", new QoreNode((int64)FLD_FML32));
  ns->addConstant("FLD_VIEW32", new QoreNode((int64)FLD_VIEW32));
  ns->addConstant("FLD_MBSTRING", new QoreNode((int64)FLD_MBSTRING));
}

// EOF

