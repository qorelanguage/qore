/*
    ModuleManager.cpp

    Qore module manager

    Qore Programming Language

    Copyright (C) 2003 - 2021 Qore Technologies, s.r.o.

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

    Note that the Qore library is released under a choice of three open-source
    licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
    information.
*/

#include "qore/Qore.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/ModuleInfo.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/QoreException.h"
#include "qore/intern/QoreDir.h"
#include "qore/intern/QoreHashNodeIntern.h"

// dlopen() flags
#define QORE_DLOPEN_FLAGS RTLD_LAZY|RTLD_GLOBAL

#ifdef HAVE_GLOB_H
#include <glob.h>
#else
#include "qore/intern/glob.h"
#endif

#include <cctype>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

static const qore_mod_api_compat_s qore_mod_api_list_l[] = {
    {0, 25},
};
#define QORE_MOD_API_LEN (sizeof(qore_mod_api_list_l)/sizeof(struct qore_mod_api_compat_s))

// public symbols
const qore_mod_api_compat_s* qore_mod_api_list = qore_mod_api_list_l;
const unsigned qore_mod_api_list_len = QORE_MOD_API_LEN;

QoreModuleManager QMM;
ModuleManager MM;

static bool show_errors = false;

// for detecting recursive user module dependencies
typedef std::set<const char*, ltstr> mod_set_t;
static mod_set_t modset;

QoreModuleDefContext::strset_t QoreModuleDefContext::vset;

static int module_load_check(const char* path) {
    mod_set_t::iterator i = modset.lower_bound(path);
    if (i != modset.end() && !strcmp(*i, path))
        return -1;
    modset.insert(i, path);
    return 0;
}

static void module_load_clear(const char* path) {
    mod_set_t::iterator i = modset.find(path);
    assert(i != modset.end());
    modset.erase(i);
}

ModuleReExportHelper::ModuleReExportHelper(QoreAbstractModule* mi, bool reexp) : m(set_reexport(mi, reexp, reexport)) {
    //printd(5, "ModuleReExportHelper::ModuleReExportHelper() %p '%s' (reexp: %d) to %p '%s' (reexp: %d)\n", mi, mi ? mi->getName() : "n/a", reexp, m, m ? m->getName() : "n/a", reexport);
    if (m && mi && reexp) {
        //printd(5, "ModuleReExportHelper::ModuleReExportHelper() adding '%s' to '%s'\n", mi->getName(), m->getName());
        m->addModuleReExport(mi->getName());
    }
}

ModuleReExportHelper::~ModuleReExportHelper() {
    set_reexport(m, reexport);
}

QoreHashNode* QoreAbstractModule::getHashIntern(bool with_filename) const {
    QoreHashNode* h = new QoreHashNode(autoTypeInfo);

    qore_hash_private* ph = qore_hash_private::get(*h);

    if (with_filename)
        ph->setKeyValueIntern("filename", new QoreStringNode(filename));
    ph->setKeyValueIntern("name", new QoreStringNode(name));
    ph->setKeyValueIntern("desc", new QoreStringNode(desc));
    ph->setKeyValueIntern("version", new QoreStringNode(*version_list));
    ph->setKeyValueIntern("author", new QoreStringNode(author));
    if (!url.empty())
        ph->setKeyValueIntern("url", new QoreStringNode(url));
    if (!license.empty())
        ph->setKeyValueIntern("license", new QoreStringNode(license));
    if (!rmod.empty()) {
        QoreListNode* l = new QoreListNode(stringTypeInfo);
        for (name_vec_t::const_iterator i = rmod.begin(), e = rmod.end(); i != e; ++i)
            l->push(new QoreStringNode(*i), nullptr);
        ph->setKeyValueIntern("reexported-modules", l);
    }
    ph->setKeyValueIntern("injected", injected);
    ph->setKeyValueIntern("reinjected", reinjected);

    return h;
}

void QoreAbstractModule::reexport(ExceptionSink& xsink, QoreProgram* pgm) const {
    // import also any modules that should be reexported from the loaded module
    for (name_vec_t::const_iterator i = rmod.begin(), e = rmod.end(); i != e; ++i) {
        //printd(5, "QoreAbstractModule::reexport() '%s' pgm: %p '%s'\n", getName(), pgm, i->c_str());

        if (!qore_program_private::get(*pgm)->hasFeature(i->c_str())) {
            QMM.loadModuleIntern(xsink, xsink, i->c_str(), pgm);
        }
    }
}

QoreModuleContext::QoreModuleContext(const char* n, qore_root_ns_private* n_rns, ExceptionSink& xs)
        : name(n), rns(n_rns), parent(get_module_context()), xsink(xs) {
}

void QoreModuleContext::error(const char* fmt, ...) {
    va_list args;
    QoreStringNode* err = new QoreStringNodeMaker("module '%s': ", name);

    while (true) {
        va_start(args, fmt);
        int rc = err->vsprintf(fmt, args);
        va_end(args);
        if (!rc)
            break;
    }

    xsink.raiseExceptionArg("MODULE-LOAD-ERROR", new QoreStringNode(name), err);
}

void QoreModuleContext::commit() {
    rns->commitModule(*this);

    mcfl.mcfl_t::clear();
    mcnl.mcnl_t::clear();
}

void QoreModuleDefContext::set(const QoreProgramLocation* loc, const char* key, QoreValue val) {
    // special handling for "init" and "del"
    if (!strcmp(key, "init")) {
        if (init_c)
            parse_error(*loc, "module key 'init' was given multiple times");
        else {
            // check type when code is committed
            init_c = val.refSelf();
            init_loc = loc;
        }
    } else if (!strcmp(key, "del")) {
        if (del_c)
            parse_error(*loc, "module key 'del' was given multiple times");
        else {
            // check type when code is committed
            del_c = val.refSelf();
            del_loc = loc;
        }
    } else if (vset.find(key) == vset.end())
        parse_error(*loc, "module key '%s' is invalid", key);
    else if (vmap.find(key) != vmap.end())
        parse_error(*loc, "module key '%s' was given multiple times", key);
    else if (val.getType() != NT_STRING)
        parse_error(*loc, "module key '%s' assigned type '%s' (expecting 'string')", key, val.getTypeName());
    else
        vmap[key] = val.get<const QoreStringNode>()->c_str();
}

// called only during parsing
void QoreModuleDefContext::parseInit() {
    if (init_c)
        initClosure(init_loc, init_c, "init");
    if (del_c)
        initClosure(del_loc, del_c, "del");
}

void QoreModuleDefContext::initClosure(const QoreProgramLocation* loc, QoreValue& c, const char* n) {
    // initialize closure
    int lvids = 0;
    const QoreTypeInfo* typeInfo = nullptr;
    // check for local variables at the top level - this can only happen if the expresion is not a closure
    parse_init_value(c, nullptr, 0, lvids, typeInfo);
    if (lvids) {
        parseException(*loc, "ILLEGAL-LOCAL-VAR", "local variables may not be declared in module '%s' code", n);
        // discard variables immediately
        for (int i = 0; i < lvids; ++i) {
            pop_local_var();
        }
    }

    qore_type_t t = c.getType();
    if (t != NT_CLOSURE && t != NT_FUNCREF)
        parse_error(*loc, "the module '%s' key must be assigned to a closure or call reference (got type '%s')", n, c.getTypeName());
}

int QoreModuleDefContext::init(QoreProgram& pgm, ExceptionSink& xsink) {
    if (!init_c)
        return 0;

    {
        ProgramThreadCountContextHelper tch(&xsink, &pgm, true);
        if (xsink)
            return -1;

        ValueHolder cn(init_c.eval(&xsink), &xsink);
        assert(!xsink);
        assert(cn->getType() == NT_RUNTIME_CLOSURE || cn->getType() == NT_FUNCREF);
        cn->get<ResolvedCallReferenceNode>()->execValue(0, &xsink).discard(&xsink);
    }

    return xsink ? -1 : 0;
}

AbstractQoreNode* QoreModuleDefContext::takeDel() {
    if (!del_c) {
        return nullptr;
    }

    AbstractQoreNode* rv = del_c.get<AbstractQoreNode>();
    del_c.clear();
    return rv;
}

QoreModuleContextHelper::QoreModuleContextHelper(const char* name, QoreProgram* pgm, ExceptionSink& xsink)
    : QoreModuleContext(name, qore_root_ns_private::get(pgm ? *(pgm->getRootNS()) : *staticSystemNamespace), xsink) {
    set_module_context(this);
}

QoreModuleContextHelper::~QoreModuleContextHelper() {
    set_module_context(parent);
}

QoreUserModuleDefContextHelper::QoreUserModuleDefContextHelper(const char* name, QoreProgram* p, ExceptionSink& xs) : old_name(set_user_module_context_name(name)), pgm(qore_program_private::get(*p)), po(0), xsink(xs), dup(false) {
}

void QoreUserModuleDefContextHelper::setNameInit(const char* name) {
    assert(vmap.find("name") == vmap.end());
    vmap["name"] = name;

    assert(!po);

    po = pgm->pwo.parse_options;
    pgm->pwo.parse_options |= MOD_HEADER_PO;
}

void QoreUserModuleDefContextHelper::close() {
    pgm->pwo.parse_options = po;
}

void UniqueDirectoryList::addDirList(const char* str) {
    if (!str)
        return;

    // duplicate string for invasive searches
    QoreString plist(str);
    str = (char*)plist.getBuffer();

    // add each directory
    while (char* p = (char*)strchr(str, ':')) {
#ifdef _Q_WINDOWS
        // don't match ':' as the second character in a path as a path separator
        if (p == str + 1) {
            p = (char*)strchr(p + 1, ':');
            if (!p)
                break;
        }
#endif
        if (p != str) {
            *p = '\0';
            // add string to list
            push_back(str);
        }
        str = p + 1;
    }

    // add last directory
    if (*str)
        push_back(str);
}

static QoreStringNode* loadModuleError(const char* name, ExceptionSink& xsink) {
    QoreStringNode* rv = new QoreStringNodeMaker("failed to load module '%s':\n", name);
    qore_es_private::appendList(xsink, *rv);
    xsink.clear();
    return rv;
}

void QoreBuiltinModule::addToProgramImpl(QoreProgram* pgm, ExceptionSink& xsink) const {
    QoreModuleContextHelper qmc(name.c_str(), pgm, xsink);
    // issue #3592: must add feature first
    pgm->addFeature(name.c_str());

    // make sure getProgram() returns this Program when module_ns_init() is called
    ProgramCallContextHelper pch(pgm);

    RootQoreNamespace* rns = pgm->getRootNS();
    QoreNamespace* qns = pgm->getQoreNS();

    module_ns_init(rns, qns);

    if (qmc.hasError()) {
        // rollback all module changes
        qmc.rollback();
        qore_program_private::get(*pgm)->removeFeature(name.c_str());
        return;
    }

    // commit all module changes
    qmc.commit();
}

QoreHashNode* QoreBuiltinModule::getHash(bool with_filename) const {
    QoreHashNode* h = getHashIntern(with_filename);

    qore_hash_private* ph = qore_hash_private::get(*h);

    ph->setKeyValueIntern("user", false);
    ph->setKeyValueIntern("api_major", api_major);
    ph->setKeyValueIntern("api_minor", api_minor);

    if (info) {
        ph->setKeyValueIntern("info", info->hashRefSelf());
    }

    return h;
}

QoreUserModule::~QoreUserModule() {
    assert(pgm);
    ExceptionSink xsink;
    if (del) {
        ProgramThreadCountContextHelper tch(&xsink, pgm, true);
        if (!xsink) {
            ValueHolder cn(del->eval(&xsink), &xsink);
            assert(!xsink);
            assert(cn->getType() == NT_RUNTIME_CLOSURE || cn->getType() == NT_FUNCREF);
            cn->get<ResolvedCallReferenceNode>()->execValue(0, &xsink).discard(&xsink);
            del->deref(&xsink);
        }
    }
    pgm->waitForTerminationAndDeref(&xsink);
}

void QoreUserModule::addToProgramImpl(QoreProgram* tpgm, ExceptionSink& xsink) const {
    //printd(5, "QoreUserModule::addToProgramImpl() mod '%s': tpgm %p po: %llx pgm dom: %llx\n", name.c_str(), tpgm, tpgm->getParseOptions64(), qore_program_private::getDomain(*pgm));
    // first check the module's functional domain
    int64 dom = qore_program_private::getDomain(*pgm);
    if (tpgm->getParseOptions64() & dom) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s' implements " \
            "functionality restricted in the Program object trying to import the module (%lx)",
            name.c_str(), tpgm->getParseOptions64() & dom);
        return;
    }
    // issue #3592: must add feature first
    qore_program_private::get(*tpgm)->addUserFeature(name.c_str());

    QoreModuleContextHelper qmc(name.c_str(), tpgm, xsink);
    ProgramThreadCountContextHelper ptcch(&xsink, tpgm, false);
    if (xsink) {
        // rollback all module changes
        qmc.rollback();
        qore_program_private::get(*tpgm)->removeUserFeature(name.c_str());
        return;
    }

    RootQoreNamespace* rns = tpgm->getRootNS();
    qore_root_ns_private::scanMergeCommittedNamespace(*rns, *(pgm->getRootNS()), qmc);

    if (qmc.hasError()) {
        // rollback all module changes
        qmc.rollback();
        qore_program_private::get(*tpgm)->removeUserFeature(name.c_str());
        return;
    }

    // commit all module changes
    qore_root_ns_private::copyMergeCommittedNamespace(*rns, *(pgm->getRootNS()));

    // add domain to current Program's domain
    qore_program_private::runtimeAddDomain(*tpgm, dom);

    QMM.trySetUserModuleDependency(this);
}

void QoreBuiltinModule::issueModuleCmd(const QoreProgramLocation* loc, const QoreString& cmd, ExceptionSink* xsink) {
    if (!module_parse_cmd) {
        if (xsink) {
            xsink->raiseException(*loc, "PARSE-COMMAND-ERROR", "module '%s' loaded from '%s' has not registered a parse command handler", name.getBuffer(), filename.getBuffer());
        }
        return;
    }

    // if parse exceptions have been disabled, then skip issuing the command
    if (!xsink) {
        return;
    }

    try {
        module_parse_cmd(cmd, xsink);
    } catch (AbstractException& e) {
        e.convert(xsink);
    }
}

ModuleManager::ModuleManager() {
}

// to add a directory to the module directory search list, can only be called before init()
void ModuleManager::addModuleDir(const char* dir) {
    QMM.addModuleDir(dir);
}

// no longer supported; throws an exception
void ModuleManager::addAutoModuleDir(const char* dir) {
    printd(0, "ModuleManager::addAutoModuleDir() support for auto module directories was removed in 0.8.4\n");
    assert(false);
}

// to add a list of directories to the module directory search list, can only be called before init()
void ModuleManager::addModuleDirList(const char* strlist) {
    QMM.addModuleDirList(strlist);
}

// no longer supported; throws an exception
void ModuleManager::addAutoModuleDirList(const char* strlist) {
    printd(0, "ModuleManager::addAutoModuleDir() support for auto module directories was removed in 0.8.4\n");
    assert(false);
}

void QoreModuleManager::init(bool se) {
    static const char* qt_blacklist_string = "because it was implemented with faulty namespace handling that does not work with newer versions of Qore; use the 'qt4' module based in libsmoke instead; it is much more complete";

    // setup possible user module keys
    QoreModuleDefContext::vset.insert("desc");
    QoreModuleDefContext::vset.insert("version");
    QoreModuleDefContext::vset.insert("author");
    QoreModuleDefContext::vset.insert("url");
    QoreModuleDefContext::vset.insert("license");

    mutex = new QoreThreadLock(&ma_recursive);

    // initialize blacklist
    // add old QT modules to blacklist
    mod_blacklist.insert(std::make_pair((const char*)"qt-core", qt_blacklist_string));
    mod_blacklist.insert(std::make_pair((const char*)"qt-gui", qt_blacklist_string));
    mod_blacklist.insert(std::make_pair((const char*)"qt-svn", qt_blacklist_string));
    mod_blacklist.insert(std::make_pair((const char*)"qt-opengl", qt_blacklist_string));

    show_errors = se;

    // setup module directory list from QORE_MODULE_DIR (if it hasn't already been manually set up)
    if (moduleDirList.empty())
        QoreModuleManager::addStandardModulePaths();
}

/* this internal helper function solves an issue with modules
   on MS Windows (or maybe macOS later).

   The problem with Windows is that we are distributing zip
   files which can be extracted anywhere. But these binaries
   had hardcoded paths for modules (/z/something/something or
   C:/msys64/home/pvanek/src/qore/RELEASE/share/qore-modules
   for example). These hardcoded paths are fine on Linux because
   the language should be distributed in a form of system packages
   (rpm, deb). But on Windows the user decides where to put
   (extract) the zip archive content. It can be anywhere.

   Of course there is QORE_MODULE_DIR env variable. But it
   requires logout/login on Windows to be in action. Also
   user has to set 4 paths there. And unfortunately 2 of
   them must contain qore version in the path. So it
   required to change env variable (and re-login) after any
   qore upgrade. It was annoying.

   So for now - if there is MODULES_RELATIVE_PATH defined
   from cmake (WIN builds only for now) the location of
   module(s) is constructed from the location of qore binary.
 */
#ifdef MODULES_RELATIVE_PATH
#ifdef _Q_WINDOWS
#include <windows.h>
#endif
#endif
std::string module_dir_prefix(const char * path) {
#ifdef MODULES_RELATIVE_PATH
#ifdef _Q_WINDOWS
    // get windows qore.exe binary
    // Full path may be longer than MAX_PATH
    // Let's expand it until it fits
    std::vector<char> buff;
    DWORD copied = 0;
    do {
        buff.resize(buff.size() + MAX_PATH);
        copied = GetModuleFileName(0, &buff.at(0), buff.size());
    } while (copied >= buff.size());

    buff.resize(copied);
    std::string prefix_path(buff.data(), buff.size());
    size_t found_pos = prefix_path.rfind('\\');

    return prefix_path.substr(0, found_pos).append("/../").append(path);
#else // _Q_WINDOWS
#warning MODULES_RELATIVE_PATH has been set but the operating system is not supported yet
#endif
    // this can be used for macOS maybe (regarding Qt5)
    //QCFType<CFURLRef> bundleURL(CFBundleCopyExecutableURL(CFBundleGetMainBundle()));
    //if(bundleURL) {
    //    QCFString cfPath(CFURLCopyFileSystemPath(bundleURL, kCFURLPOSIXPathStyle));
#else // MODULES_RELATIVE_PATH
    return path;
#endif // MODULES_RELATIVE_PATH
}

void QoreModuleManager::addStandardModulePaths() {
   moduleDirList.addDirList(getenv("QORE_MODULE_DIR"));

   // append version-specifc user module directory
   moduleDirList.push_back(module_dir_prefix(USER_MODULE_VER_DIR));

   // append version-specifc module directory
   if (strcmp(MODULE_VER_DIR, USER_MODULE_VER_DIR))
      moduleDirList.push_back(module_dir_prefix(MODULE_VER_DIR));

   // append user-module directory
   moduleDirList.push_back(module_dir_prefix(USER_MODULE_DIR));

   // append qore module directory
   if (strcmp(MODULE_DIR, USER_MODULE_DIR))
      moduleDirList.push_back(module_dir_prefix(MODULE_DIR));
}

void ModuleManager::addStandardModulePaths() {
   QMM.addStandardModulePaths();
}

int ModuleManager::runTimeLoadModule(const char* name, ExceptionSink* xsink) {
    assert(name);
    assert(xsink);
    return QMM.runTimeLoadModule(*xsink, *xsink, name, getProgram());
}

int ModuleManager::runTimeLoadModule(const char* name, QoreProgram* pgm, ExceptionSink* xsink) {
    assert(name);
    assert(xsink);
    return QMM.runTimeLoadModule(*xsink, *xsink, name, pgm);
}

int ModuleManager::runTimeLoadModule(ExceptionSink* xsink, const char* name, QoreProgram* pgm,
    qore_binary_module_desc_t mod_desc_func) {
    assert(name);
    assert(xsink);
    return QMM.runTimeLoadModule(*xsink, *xsink, name, pgm, nullptr, QMLO_NONE, QP_WARN_MODULES, false, mod_desc_func);
}

int QoreModuleManager::runTimeLoadModule(ExceptionSink& xsink, ExceptionSink& wsink, const char* name,
        QoreProgram* pgm, QoreProgram* mpgm, unsigned load_opt, int warning_mask, bool reexport,
        qore_binary_module_desc_t mod_desc_func) {
    // grab the parse lock
    ProgramRuntimeParseContextHelper pah(&xsink, pgm);
    if (xsink)
        return -1;

    AutoLocker al2(mutex); // grab global module lock
    loadModuleIntern(xsink, wsink, name, pgm, reexport, MOD_OP_NONE, 0, 0, mpgm, load_opt, warning_mask,
        mod_desc_func);
    return xsink ? -1 : 0;
}

static const char* get_op_string(mod_op_e op) {
    if (op == MOD_OP_LT) return "<";
    if (op == MOD_OP_LE) return "<=";
    if (op == MOD_OP_EQ) return "=";
    if (op == MOD_OP_GE) return ">=";
    assert(op == MOD_OP_GT);
    return ">";
}

#define MVC_FAIL     0
#define MVC_OK       1
#define MVC_FINAL_OK 2
int check_component(mod_op_e op, int mod_ver, int req_ver, bool last) {
    // "promote" operator if not comparing last element
    if (!last) {
        if (op == MOD_OP_LT) op = MOD_OP_LE;
        else if (op == MOD_OP_GT) op = MOD_OP_GE;
    }
    //printd(5, "check_component(%d %s %d)\n", mod_ver, get_op_string(op), req_ver);
    if (op == MOD_OP_LT)
        return mod_ver < req_ver ? MVC_FINAL_OK : MVC_FAIL;
    if (op == MOD_OP_LE)
        return mod_ver < req_ver ? MVC_FINAL_OK : (mod_ver == req_ver ? MVC_OK : MVC_FAIL);
    if (op == MOD_OP_EQ)
        return mod_ver == req_ver ? MVC_OK : MVC_FAIL;
    if (op == MOD_OP_GE)
        return mod_ver > req_ver ? MVC_FINAL_OK : (mod_ver == req_ver ? MVC_OK : MVC_FAIL);
    assert(op == MOD_OP_GT);
    return mod_ver > req_ver ? MVC_FINAL_OK : MVC_FAIL;
}

// issue #2834: add context to exception description if possible
static void try_add_module_context(QoreStringNode* desc) {
    const char* mod = get_user_module_context_name();
    if (mod) {
        QoreStringMaker str("while loading module '%s': ", mod);
        desc->prepend(str.c_str(), str.size());
    }
}

static int check_qore_version(const char* name, mod_op_e op, version_list_t& version, ExceptionSink& xsink) {
    unsigned max = version.size() > 4 ? version.size() : 4;
    for (unsigned i = 0; i < max; ++i) {
        int mv = (!i ? QORE_VERSION_MAJOR :
                    (i == 1 ? QORE_VERSION_MINOR :
                    (i == 2 ? QORE_VERSION_SUB :
                    (i == 3 ? QORE_VERSION_PATCH : 0))));
        int rv = (i >= version.size() ? 0 : version[i]);
        int res = check_component(op, mv, rv, i == (max - 1));
        if (res == MVC_FAIL) {
            QoreStringNode* desc = new QoreStringNodeMaker("feature '%s' is built in, but the following version requirement is not satisfied: Qore library %s %s %s", name, QORE_VERSION, get_op_string(op), *version);
            try_add_module_context(desc);
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), desc);
            return -1;
        }
        if (res == MVC_FINAL_OK)
            break;
    }
    return 0;
}

static void check_module_version(QoreAbstractModule* mi, mod_op_e op, version_list_t& version, ExceptionSink& xsink) {
    unsigned max = version.size() > mi->version_list.size() ? version.size() : mi->version_list.size();
    //printd(5, "check_module_version(%s %s %s) max=%d vs=%d ms=%d\n", mi->getVersion(), get_op_string(op), version->getString(), max, version->size(), mi->version_list.size());
    for (unsigned i = 0; i < max; ++i) {
        int mv = (i >= mi->version_list.size() ? 0 : mi->version_list[i]);
        int rv = (i >= version.size() ? 0 : version[i]);
        int res = check_component(op, mv, rv, i == (max - 1));
        if (res == MVC_FAIL) {
            QoreStringNode* desc = new QoreStringNodeMaker("loaded module '%s' does not satisfy the following requirement: %s %s %s", mi->getName(), mi->getVersion(), get_op_string(op), *version);
            try_add_module_context(desc);
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(mi->getName()), desc);
            return;
        }
        if (res == MVC_FINAL_OK)
            break;
    }
}

static int qore_check_load_module_intern(QoreAbstractModule* mi, mod_op_e op, version_list_t* version, QoreProgram* pgm, ExceptionSink& xsink) {
    if (xsink) {
        return -1;
    }

    assert(mi);
    // check version if necessary
    if (version) {
        check_module_version(mi, op, *version, xsink);
        if (xsink)
            return -1;
    }

    if (pgm)
        mi->addToProgram(pgm, xsink);
    return 0;
}

void QoreModuleManager::getUniqueName(QoreString& nname, const char* name, const char* prefix) {
    int ver = 1;
    while (true) {
        nname.sprintf("!!%s-%s-%d", prefix, name, ver++);
        if (!findModuleUnlocked(nname.getBuffer()))
            break;
        nname.clear();
    }
}

void QoreModuleManager::reinjectModule(QoreAbstractModule* mi) {
    // handle reinjections here
    QoreString nname;
    getUniqueName(nname, mi->getName(), "orig-");
    module_map_t::iterator i = map.find(mi->getName());
    assert(i != map.end());
    map.erase(i);
    mi->rename(nname);
    addModule(mi);
}

static const char* get_feature_from_path(QoreString& tmp) {
    char* buf = q_basename(tmp.c_str());
    size_t len = strlen(buf);
    tmp.set(buf, len, len + 1, QCS_DEFAULT);
    qore_offset_t i = tmp.find('-');
    if (i < 0) {
        i = tmp.find('.');
    }
    if (i > 0) {
        tmp.terminate(i);
    }
    return tmp.c_str();
}

int QoreModuleManager::loadModuleIntern(ExceptionSink& xsink, ExceptionSink& wsink, const char* name,
        QoreProgram* pgm, bool reexport, mod_op_e op, version_list_t* version, const char* src, QoreProgram* mpgm,
        unsigned load_opt, int warning_mask, qore_binary_module_desc_t mod_desc_func) {
    assert(!version || (version && op != MOD_OP_NONE));

    ReferenceHolder<QoreProgram> pholder(mpgm, &xsink);

    // temporary string buffer
    QoreString tmp_buf;
    const char* raw_path;
    bool is_bin;
    // see if "name" is a path
    if (strchrs(name, "./\\")) {
        tmp_buf = name;
        raw_path = name;

        if (tmp_buf.size() > 5 && !strcasecmp(".qmod", tmp_buf.c_str() + tmp_buf.size() - 5)) {
            is_bin = true;
        } else {
            is_bin = false;
        }
        name = get_feature_from_path(tmp_buf);
    } else {
        raw_path = nullptr;
        is_bin = false;
    }

    printd(5, "QoreModuleManager::loadModuleIntern() name: '%s' path: '%s' reexport: %d pgm: %p\n", name,
        raw_path ? raw_path : "n/a", reexport, pgm);

    // check for special "qore" feature
    if (!raw_path && !strcmp(name, "qore")) {
        if (version)
            return check_qore_version(name, op, *version, xsink);
        return 0;
    }

    module_map_t::iterator mmi = map.find(name);
    assert(mmi == map.end() || !strcmp(mmi->second->getName(), name));

    QoreAbstractModule* mi = (mmi == map.end() ? nullptr : mmi->second);

    // handle module reloads
    if (load_opt & QMLO_RELOAD) {
        assert(!version);
        assert(!src);
        // only loaded & injected modules can be reloaded
        if (!mi || !mi->isInjected())
            return xsink ? -1 : 0;

        // rename module and make private
        map.erase(mmi);

        QoreString orig_name(mi->getName());
        // rename to unique name
        QoreString nname;
        getUniqueName(nname, mi->getName(), "private");
        mi->rename(nname);
        mi->setOrigName(orig_name.getBuffer());
        mi->setPrivate();
        assert(mi->isUser());
        addModule(mi);

        QoreString modulePath;
        if (raw_path) {
            QoreProgram* p = pgm ? pgm : (load_opt & (QMLO_REINJECT | QMLO_PRIVATE) && mpgm ? mpgm : nullptr);
            if (!p) {
                p = getProgram();
            }

            if (p) {
                modulePath = raw_path;
                q_normalize_path(modulePath, p->parseGetScriptDir());
                raw_path = modulePath.c_str();
            }
        }

        QoreAbstractModule* nmi = loadUserModuleFromPath(xsink, wsink, raw_path ? raw_path : mi->getFileName(),
            mi->getOrigName(), pgm, reexport, pholder.release(), load_opt & QMLO_REINJECT ? mpgm : nullptr, load_opt,
            warning_mask);
        if (xsink) {
            mmi = map.find(mi->getName());
            assert(mmi != map.end());
            map.erase(mmi);
            mi->resetName();
            mi->setPrivate(false);
            addModule(mi);
            return -1;
        }

        assert(umset.find(mi->getName()) == umset.end());
        nmi->setLink(mi);
        trySetUserModuleDependency(mi);
        return 0;
    }

    // if the feature already exists in this program, then return
    if (pgm && qore_program_private::get(*pgm)->hasFeature(name)) {
        //printd(5, "QoreModuleManager::loadModuleIntern() '%s' pgm has feature\n", name);

        if (load_opt & QMLO_INJECT)
            xsink.raiseException("LOAD-MODULE-ERROR", "cannot load module '%s' for injection because the module " \
                "has already been loaded", name);

        // check version if necessary
        if (version) {
            // if no module is found, then this is a builtin feature
            if (!mi)
                check_qore_version(name, op, *version, xsink);
            else
                check_module_version(mi, op, *version, xsink);
        }

        if (mi)
            trySetUserModuleDependency(mi);
        return xsink ? -1 : 0;
    }

    // check if parse options allow loading any modules at all
    if (pgm && (pgm->getParseOptions64() & PO_NO_MODULES)) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "cannot load modules ('%s' " \
            "requested) into the current Program object because PO_NO_MODULES is set", name);
        return -1;
    }

    // if the feature already exists, then load the namespace changes into this program and register the feature

    if (mi && !(load_opt & QMLO_REINJECT)) {
        if (load_opt & QMLO_INJECT) {
            xsink.raiseException("LOAD-MODULE-ERROR", "cannot load module '%s' for injection because the " \
                "module has already been loaded; to reinject a module, call Program::loadApplyToUserModule() " \
                "with the reinject flag set to True", name);
            return -1;
        }

        //printd(5, "QoreModuleManager::loadModuleIntern() name: %s inject: %d, reinject: %d found: %p (%s, %s) injected: %d reinjected: %d\n", name, load_opt & QMLO_INJECT, load_opt & QMLO_REINJECT, mi, mi->getName(), mi->getFileName(), mi->isInjected(), mi->isReInjected());

        int rc = qore_check_load_module_intern(mi, op, version, pgm, xsink);
        // make sure to add reexport info if the module should be reexported
        if (reexport && !xsink) {
            ModuleReExportHelper mrh(mi, true);
        }
        return rc;
    }

    //printd(5, "QoreModuleManager::loadModuleIntern() this: %p name: %s not found\n", this, name);

    // see if we are loading a user module from explicit source
    if (src) {
        QoreString modulePath;
        if (raw_path) {
            QoreProgram* p = pgm ? pgm : (load_opt & (QMLO_REINJECT | QMLO_PRIVATE) && mpgm ? mpgm : nullptr);
            if (!p) {
                p = getProgram();
            }

            if (p) {
                modulePath = raw_path;
                q_normalize_path(modulePath, p->parseGetScriptDir());
                raw_path = modulePath.c_str();
            }
        }

        mi = loadUserModuleFromSource(xsink, wsink, raw_path ? raw_path : name, name, pgm, src, reexport,
            pholder.release(), warning_mask);
        return qore_check_load_module_intern(mi, op, version, pgm, xsink);
    }

    // see if this is actually a path
    if (raw_path) {
        QoreString modulePath;
        QoreProgram* p = pgm ? pgm : (load_opt & (QMLO_REINJECT | QMLO_PRIVATE) && mpgm ? mpgm : nullptr);
        if (!p) {
            p = getProgram();
        }

        if (p) {
            modulePath = raw_path;
            q_normalize_path(modulePath, p->parseGetScriptDir());
            raw_path = modulePath.c_str();
        }

        if (is_bin) {
            if (mpgm) {
                xsink.raiseException("LOAD-MODULE-ERROR", "cannot load a binary module with a Program container");
                return -1;
            }
            if (load_opt & QMLO_REINJECT) {
                xsink.raiseException("LOAD-MODULE-ERROR", "cannot reinject module '%s' because reinjection is not " \
                    "currently supported for binary modules", name);
                return -1;
            }

            mi = loadBinaryModuleFromPath(xsink, raw_path, name, pgm, reexport, mod_desc_func);
        } else if (QoreDir::folder_exists(modulePath, xsink)) {
            mi = loadSeparatedModule(xsink, wsink, raw_path, name, pgm, reexport, pholder.release(),
                load_opt & QMLO_REINJECT ? mpgm : nullptr, load_opt, warning_mask);
        } else {
            mi = loadUserModuleFromPath(xsink, wsink, raw_path, name, pgm, reexport, pholder.release(),
                load_opt & QMLO_REINJECT ? mpgm : nullptr, load_opt, warning_mask);
        }

        return qore_check_load_module_intern(mi, op, version, pgm, xsink);
    }

    // otherwise, try to find module in the module path
    QoreString str;
    struct stat sb;

    strdeque_t::const_iterator w = moduleDirList.begin();
    while (w != moduleDirList.end()) {
        // try to find module with supported api tags
        for (unsigned ai = 0; ai <= qore_mod_api_list_len; ++ai) {
            // build path to binary module
            str.clear();
            str.sprintf("%s" QORE_DIR_SEP_STR "%s", (*w).c_str(), name);

            // make new extension string
            if (ai < qore_mod_api_list_len)
                str.sprintf("-api-%d.%d.qmod", qore_mod_api_list[ai].major, qore_mod_api_list[ai].minor);
            else
                str.concat(".qmod");

            //printd(5, "ModuleManager::loadModule(%s) trying binary module: %s\n", name, str.getBuffer());
            if (!stat(str.getBuffer(), &sb)) {
                printd(5, "ModuleManager::loadModule(%s) found binary module: %s\n", name, str.getBuffer());
                if (mpgm) {
                    xsink.raiseException("LOAD-MODULE-ERROR", "cannot load a binary module with a Program container");
                    return -1;
                }

                mi = loadBinaryModuleFromPath(xsink, str.c_str(), name, pgm, reexport, mod_desc_func);
                return qore_check_load_module_intern(mi, op, version, pgm, xsink);
            }

            // build path to user module
            str.clear();
            str.sprintf("%s" QORE_DIR_SEP_STR "%s.qm", (*w).c_str(), name);

            //printd(5, "ModuleManager::loadModule(%s) trying user module: %s\n", name, str.getBuffer());
            if (!stat(str.getBuffer(), &sb)) {
                // see if this is a relative path; if so normalize it; we cannot send a relative path to loadUserModuleFromPath()
                // since it will try to normalize the path using the current program's directory as the cwd
                if (!q_absolute_path(str.getBuffer())) {
                    q_normalize_path(str);
                }
                printd(5, "ModuleManager::loadModule(%s) found user module: %s\n", name, str.getBuffer());
                mi = loadUserModuleFromPath(xsink, wsink, str.c_str(), name, pgm, reexport, pholder.release(),
                    load_opt & QMLO_REINJECT ? mpgm : nullptr, load_opt, warning_mask);
                return qore_check_load_module_intern(mi, op, version, pgm, xsink);
            }
        }

        // check whether it is a module folder
        QoreString modulePath(*w);
        modulePath += QORE_DIR_SEP_STR;
        modulePath += name;

        if (QoreDir::folder_exists(modulePath, xsink)) {
            mi = loadSeparatedModule(xsink, wsink, modulePath.c_str(), name, pgm, reexport, pholder.release(),
                load_opt & QMLO_REINJECT ? mpgm : nullptr, load_opt, warning_mask);
            return qore_check_load_module_intern(mi, op, version, pgm, xsink);
        }

        ++w;
    }

    QoreStringNode* desc = new QoreStringNodeMaker("feature '%s' is not builtin and no module with this name could " \
        "be found in the module path: ", name);
    moduleDirList.appendPath(*desc);
    xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), desc);
    return -1;
}

QoreAbstractModule* QoreModuleManager::loadSeparatedModule(ExceptionSink& xsink, ExceptionSink& wsink,
        const char* path, const char* feature, QoreProgram* pgm, bool reexport, QoreProgram* mpgm,
        QoreProgram* path_pgm, unsigned load_opt, int warning_mask) {
    assert(feature);
    printd(5, "QoreModuleManager::loadSeparatedModule() path: %s, feature: %s, pgm: %p, reexport: %d, mpgm: %p, " \
        "path_pgm: %p, load_opt: %d warning_mask: %d\n", path, feature, pgm, reexport, mpgm, path_pgm,
        load_opt, warning_mask);
    if (module_load_check(feature)) {
        xsink.raiseException("LOAD-MODULE-ERROR", "cannot load user module '%s'; recursive module dependency " \
            "detected", feature);
        return nullptr;
    }
    ON_BLOCK_EXIT(module_load_clear, feature);

    QoreParseCountContextHelper pcch;
    // parse options for the module
    int64 parseOptions = USER_MOD_PO;
    // add in parse options from the current program, if any, disabling style and types options already set with
    // USER_MOD_PO
    if (pgm) {
        parseOptions |= (pgm->getParseOptions64() & ~(PO_FREE_OPTIONS | PO_REQUIRE_TYPES | PO_NO_GLOBAL_VARS));
    }

    QoreString modulePath(path);
    modulePath += QORE_DIR_SEP_STR;
    modulePath += feature;
    modulePath += ".qm";

    QoreProgram* p = pgm ? pgm : path_pgm;
    if (!p) {
        p = mpgm;
        if (!p) {
            p = getProgram();
        }
    }
    const char* td = p ? p->parseGetScriptDir() : nullptr;

    if (mpgm) {
        qore_program_private::forceReplaceParseOptions(*mpgm, parseOptions);
    } else {
        mpgm = new QoreProgram(parseOptions);
        mpgm->setScriptPath(modulePath.c_str());
    }
    // issue #3592: must add feature first
    if (qore_program_private::get(*mpgm)->addUserFeature(feature)) {
        xsink.raiseException("LOAD-MODULE-ERROR", "cannot load user module '%s'; feature '%s' is already loaded in " \
            "this Program container", path, feature);
        return nullptr;
    }

    std::unique_ptr<QoreUserModule> userModule(new QoreUserModule(td, modulePath.c_str(), feature, mpgm,
        load_opt, warning_mask));

    ModuleReExportHelper reExportHelper(userModule.get(), reexport);
    QoreUserModuleDefContextHelper qmd(feature, mpgm, xsink);

    std::string moduleCode = QoreDir::get_file_content(modulePath.c_str());

    // issue #3212: warning sink
    userModule->getProgram()->parsePending(moduleCode.c_str(), path, &xsink, &xsink, warning_mask);
    if (xsink) {
        xsink.appendLastDescription(" (while loading user module \"%s\" from path \"%s\")", feature, path);
        return nullptr;
    }

    QoreString regexClassesFunc(".+\\.(qc|ql)$");
    QoreDir moduleDir(&xsink, QCS_DEFAULT, path);
    ReferenceHolder<QoreListNode> fileList(moduleDir.list(&xsink, S_IFREG, &regexClassesFunc), &xsink);
    if (xsink) {
        xsink.appendLastDescription(" (while loading user module \"%s\" from path \"%s\")", feature, path);
        return nullptr;
    }
    for (size_t i = 0; i < fileList->size(); ++i) {
        QoreString filePath(path);
        filePath += QORE_DIR_SEP_STR;
        filePath += fileList->retrieveEntry(i).get<const QoreStringNode>()->c_str();

        std::string fileCode = QoreDir::get_file_content(filePath);
        userModule->getProgram()->parsePending(fileCode.c_str(), filePath.c_str(), &xsink, &xsink, warning_mask);
        if (xsink) {
            xsink.appendLastDescription(" (while loading user module \"%s\" from path \"%s\")", feature, path);
            return nullptr;
        }
    }
    userModule->getProgram()->parseCommit(&xsink);
    if (xsink) {
        xsink.appendLastDescription(" (while loading user module \"%s\" from path \"%s\")", feature, path);

        return nullptr;
    }

    return setupUserModule(xsink, userModule, qmd, load_opt, warning_mask);
}

void ModuleManager::registerUserModuleFromSource(const char* name, const char* src, QoreProgram* pgm, ExceptionSink* xsink) {
    QMM.registerUserModuleFromSource(name, src, pgm, *xsink);
}

QoreStringNode* ModuleManager::parseLoadModule(const char* name, QoreProgram* pgm) {
    ExceptionSink xsink;

    QMM.parseLoadModule(xsink, xsink, name, pgm);

    if (!xsink)
        return nullptr;

    return loadModuleError(name, xsink);
}

int QoreModuleManager::parseLoadModule(ExceptionSink& xsink, ExceptionSink& wsink, const char* name,
        QoreProgram* pgm, bool reexport) {
    //printd(5, "ModuleManager::parseLoadModule(name: %s, pgm: %p, reexport: %d)\n", name, pgm, reexport);

    assert(!xsink);

    char* p = strchrs(name, "<>=");
    if (p) {
        QoreString str(name, p - name);
        str.trim();

        QoreString op;
        do {
            if (!qore_isblank(*p))
                op.concat(*p);
            ++p;
        } while (*p == '<' || *p == '>' || *p == '=' || qore_isblank(*p));

        // get version operator
        mod_op_e mo;

        if (!op.compare("<"))
            mo = MOD_OP_LT;
        else if (!op.compare("<="))
            mo = MOD_OP_LE;
        else if (!op.compare("=") || !op.compare("=="))
            mo = MOD_OP_EQ;
        else if (!op.compare(">="))
            mo = MOD_OP_GE;
        else if (!op.compare(">"))
            mo = MOD_OP_GT;
        else {
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': cannot parse " \
                "module operator '%s'; expecting one of: '<', '<=', '=', '>=', or '>'", name, op.c_str());
            return -1;
        }

        version_list_t iv;
        char ec = iv.set(p);
        if (ec) {
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': only numeric " \
                "digits and '.' characters are allowed in module/feature version specifications, got '%c'", name, ec);
            return -1;
        }

        if (!iv.size()) {
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': empty version " \
                "specification given in feature/module request", name);
            return -1;
        }

        AutoLocker al(mutex); // make sure checking and loading are atomic
        return loadModuleIntern(xsink, wsink, str.c_str(), pgm, reexport, mo, &iv);
    }

    AutoLocker al(mutex); // make sure checking and loading are atomic
    return loadModuleIntern(xsink, wsink, name, pgm, reexport);
}

void QoreModuleManager::registerUserModuleFromSource(const char* name, const char* src, QoreProgram* pgm,
        ExceptionSink& xsink) {
    AutoLocker al(mutex); // make sure checking and loading are atomic
    loadModuleIntern(xsink, xsink, name, pgm, false, MOD_OP_NONE, 0, src);
}

// const char* path, const char* feature, ReferenceHolder<QoreProgram>& pgm
QoreAbstractModule* QoreModuleManager::setupUserModule(ExceptionSink& xsink, std::unique_ptr<QoreUserModule>& mi,
        QoreUserModuleDefContextHelper& qmd, unsigned load_opt, int warning_mask) {
    // see if a module with this name is already registered
    QoreAbstractModule* omi = findModuleUnlocked(mi->getName());
    if (omi)
        qmd.setDuplicate();

    printd(5, "QoreModuleManager::setupUserModule() '%s' omi: %p\n", mi->getName(), omi);

    if (xsink) {
        return nullptr;
    }

    const char* name = qmd.get("name");

    if (!name) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': no feature name " \
            "present in module", mi->getFileName());
        return nullptr;
    }

    //printd(5, "QoreModuleManager::setupUserModule() path: %s name: %s feature: %s\n", mi->getFileName(), name,
    //  mi->getName());

    if (strcmp(mi->getName(), name)) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': provides feature " \
            "'%s', expecting feature '%s', skipping, rename module to %s.qm to load", mi->getFileName(), name,
            mi->getName(), name);
        return nullptr;
    }

    // see if a module with this name is already registered
    if (omi) {
        if (!(load_opt & QMLO_REINJECT)) {
            // if the module is the same, then do not return an error unless trying to inject
            if (mi->equalTo(omi)) {
                if (load_opt & QMLO_INJECT) {
                    xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature " \
                        "'%s' already loaded therefore cannot be used for injection unless the reinject flag is set",
                        mi->getFileName(), name);
                    return nullptr;
                }
                return omi;
            }
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s' " \
                "already registered by '%s'", mi->getFileName(), name, omi->getFileName());
            return nullptr;
        }
    }

    // get qore module description
    const char* desc = qmd.get("desc");
    if (!desc) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "description", mi->getFileName(), name);
        return nullptr;
    }

    // get qore module version
    const char* version = qmd.get("version");
    if (!version) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "version", mi->getFileName(), name);
        return nullptr;
    }

    // get qore module author
    const char* author = qmd.get("author");
    if (!author) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "author", mi->getFileName(), name);
        return nullptr;
    }

    const char* url = qmd.get("url");

    const char* license = qmd.get("license");
    QoreString license_str(license ? license : "unknown");

    // init & run module initialization code if any
    if (qmd.init(*mi->getProgram(), xsink)) {
        return nullptr;
    }

    mi->set(desc, version, author, url, license_str, qmd.takeDel());

    // handle reinjections here
    if (omi) {
        assert(load_opt & QMLO_REINJECT);
        reinjectModule(omi);
        name = mi->getName();
        assert(umset.find(omi->getName()) == umset.end());
        mi->setLink(omi);
    } else if (mi->isPrivate()) {
        QoreString orig_name(mi->getName());
        // rename to unique name
        QoreString nname;
        getUniqueName(nname, mi->getName(), "private");
        mi->rename(nname);
        mi->setOrigName(orig_name.getBuffer());
        name = mi->getName();
    }

    //printd(5, "QoreModuleManager::setupUserModule() path: %s name: %s feature: %s injected: %d reinjected: %d " \
    //  "orig: %s\n", mi->getFileName(), name, mi->getName(), mi->isInjected(), mi->isReInjected(),
    //  mi->getOrigName() ? mi->getOrigName() : "n/a");

    qore_program_private::get(*mi->getProgram())->addUserFeature(mi->getName());

    omi = mi.release();
    addModule(omi);
    trySetUserModule(name);

    return omi;
}

QoreAbstractModule* QoreModuleManager::loadUserModuleFromPath(ExceptionSink& xsink, ExceptionSink& wsink,
        const char* path, const char* feature, QoreProgram* tpgm, bool reexport, QoreProgram* pgm,
        QoreProgram* path_pgm, unsigned load_opt, int warning_mask) {
    assert(feature);
    printd(5, "QoreModuleManager::loadUserModuleFromPath() path: '%s' feature: '%s' tpgm: %p ('%s') path_pgm: %p " \
        "('%s')\n", path, feature, tpgm, tpgm && tpgm->parseGetScriptDir() ? tpgm->parseGetScriptDir() : "n/a",
        path_pgm, path_pgm && path_pgm->parseGetScriptDir() ? path_pgm->parseGetScriptDir() : "n/a");

    QoreParseCountContextHelper pcch;

    // parse options for the module
    int64 po = USER_MOD_PO;
    // add in parse options from the current program, if any, disabling style and types options already set with
    // USER_MOD_PO
    if (tpgm) {
        po |= (tpgm->getParseOptions64() & ~(PO_FREE_OPTIONS|PO_REQUIRE_TYPES|PO_NO_GLOBAL_VARS));
    }

    QoreProgram* p = tpgm ? tpgm : path_pgm;
    if (!p) {
        p = pgm;
        if (!p)
            p = getProgram();
    }
    const char* td = p ? p->parseGetScriptDir() : nullptr;

    if (pgm) {
        qore_program_private::forceReplaceParseOptions(*pgm, po);
    } else {
        pgm = new QoreProgram(po);
    }
    // issue #3592: add feature to module container program immediately
    if (qore_program_private::get(*pgm)->addUserFeature(feature)) {
        xsink.raiseException("LOAD-MODULE-ERROR", "cannot load user module '%s'; feature '%s' is already loaded in " \
            "this Program container", path, feature);
        return nullptr;
    }

    //printd(5, "QoreModuleManager::loadUserModuleFromPath(path: '%s') cwd: '%s' tpgm: %p po: " QLLD
    //  " allow-injection: %s tpgm allow-injection: %s pgm allow-injection: %s\n", path, td ? td : "n/a", tpgm, po,
    //  po & PO_ALLOW_INJECTION ? "true" : "false",
    //  (tpgm ? tpgm->getParseOptions64() & PO_ALLOW_INJECTION : 0) ? "true" : "false",
    //  pgm->getParseOptions64() & PO_ALLOW_INJECTION ? "true" : "false");

    // note: the module will contain a normalized path which will be used for parsing
    std::unique_ptr<QoreUserModule> mi(new QoreUserModule(td, path, feature, pgm, load_opt, warning_mask));

    td = mi->getFileName();
    //printd(5, "QoreModuleManager::loadUserModuleFromPath() normalized path: '%s'\n", td);

    if (!QoreDir::file_exists(td)) {
        xsink.raiseException("LOAD-MODULE-ERROR", "cannot load user module '%s'; the module doesn't exist", td);
        return nullptr;
    }

    if (module_load_check(td)) {
        xsink.raiseException("LOAD-MODULE-ERROR", "cannot load user module '%s'; recursive module dependency " \
            "detected", td);
        return nullptr;
    }
    ON_BLOCK_EXIT(module_load_clear, td);

    ModuleReExportHelper mrh(mi.get(), reexport);
    QoreUserModuleDefContextHelper qmd(feature, pgm, xsink);
    // issue #3212: warning mask
    mi->getProgram()->parseFile(td, &xsink, &wsink, warning_mask);

    if (xsink) {
        xsink.appendLastDescription(" (while loading user module \"%s\" from path \"%s\")", feature, path);
    }

    return setupUserModule(xsink, mi, qmd, load_opt, warning_mask);
}

QoreAbstractModule* QoreModuleManager::loadUserModuleFromSource(ExceptionSink& xsink, ExceptionSink& wsink,
        const char* path, const char* feature, QoreProgram* tpgm, const char* src, bool reexport, QoreProgram* pgm,
        int warning_mask) {
    assert(feature);
    //printd(5, "QoreModuleManager::loadUserModuleFromSource() path: %s feature: %s tpgm: %p\n", path, feature, tpgm);

    QoreParseCountContextHelper pcch;

    // parse options for the module
    int64 po = USER_MOD_PO;
    // add in parse options from the current program, if any, disabling style and types options already set with USER_MOD_PO
    if (tpgm) {
        po |= (tpgm->getParseOptions64() & ~(PO_FREE_OPTIONS|PO_REQUIRE_TYPES));
    }

    if (pgm) {
        qore_program_private::forceReplaceParseOptions(*pgm, po);
    } else {
        pgm = new QoreProgram(po);
    }
    // issue #3592: add feature to module container program immediately
    if (qore_program_private::get(*pgm)->addUserFeature(feature)) {
        xsink.raiseException("LOAD-MODULE-ERROR", "cannot load user module '%s'; feature '%s' is already loaded in " \
            "this Program container", path, feature);
        return nullptr;
    }

    std::unique_ptr<QoreUserModule> mi(new QoreUserModule(nullptr, path, feature, pgm, QMLO_NONE));

    ModuleReExportHelper mrh(mi.get(), reexport);

    QoreUserModuleDefContextHelper qmd(feature, pgm, xsink);

    mi->getProgram()->parse(src, path, &xsink, &wsink, warning_mask);

    if (xsink) {
        xsink.appendLastDescription(" (while loading user module \"%s\" from source with given path \"%s\")", feature,
            path);
    }

    return setupUserModule(xsink, mi, qmd);
}

QoreAbstractModule* QoreModuleManager::loadBinaryModuleFromPath(ExceptionSink& xsink, const char* path,
        const char* feature, QoreProgram* pgm, bool reexport, qore_binary_module_desc_t mod_desc) {
    QoreModuleInfo mod_info;

    void* ptr = dlopen(path, QORE_DLOPEN_FLAGS);
    if (!ptr) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(path), "error loading qore module '%s': %s", path, dlerror());
        return nullptr;
    }

    DLHelper dlh(ptr);

    QoreString feature_str;

    if (!mod_desc) {
        if (!feature) {
            feature_str = feature;
            feature = get_feature_from_path(feature_str);
        }
        // check for new-style module declaration
        QoreStringMaker sym("%s_qore_module_desc", feature);
        mod_desc = (qore_binary_module_desc_t)dlsym(ptr, sym.c_str());
    }
    if (mod_desc) {
        mod_desc(mod_info);
        return loadBinaryModuleFromDesc(xsink, &dlh, mod_info, path, feature, pgm, reexport);
    }

    // get module name
    const char* name = (const char*)dlsym(ptr, "qore_module_name");
    if (name) {
        mod_info.name = name;
    }

    // get qore module API major number
    int* api_major = (int*)dlsym(ptr, "qore_module_api_major");
    if (api_major) {
        mod_info.api_major = *api_major;
    }

    // get qore module API minor number
    int* api_minor = (int*)dlsym(ptr, "qore_module_api_minor");
    if (api_minor) {
        mod_info.api_minor = *api_minor;
    }

    // get license type
    qore_license_t* module_license = (qore_license_t*)dlsym(ptr, "qore_module_license");
    if (module_license) {
        mod_info.license = *module_license;
    }

    // get optional license string
    const char* module_license_str = (const char*)dlsym(ptr, "qore_module_license_str");
    if (module_license_str) {
        mod_info.license_str = module_license_str;
    }

    // get initialization function
    qore_module_init_t* module_init = (qore_module_init_t*)dlsym(ptr, "qore_module_init");
    if (module_init) {
        mod_info.init = *module_init;
    }

    // get namespace initialization function
    qore_module_ns_init_t* module_ns_init = (qore_module_ns_init_t*)dlsym(ptr, "qore_module_ns_init");
    if (module_ns_init) {
        mod_info.ns_init = *module_ns_init;
    }

    // get deletion function
    qore_module_delete_t* module_delete = (qore_module_delete_t*)dlsym(ptr, "qore_module_delete");
    if (module_delete) {
        mod_info.del = *module_delete;
    }

    // get parse command function
    qore_module_parse_cmd_t* pcmd = (qore_module_parse_cmd_t*)dlsym(ptr, "qore_module_parse_cmd");
    if (pcmd) {
        mod_info.parse_cmd = *pcmd;
    }

    // get qore module description
    const char* desc = (const char*)dlsym(ptr, "qore_module_description");
    if (desc) {
        mod_info.desc = desc;
    }

    // get qore module version
    const char* version = (const char*)dlsym(ptr, "qore_module_version");
    if (version) {
        mod_info.version = version;
    }

    // get qore module author
    const char* author = (const char*)dlsym(ptr, "qore_module_author");
    if (author) {
        mod_info.author = author;
    }

    // get qore module URL (optional)
    const char* url = (const char*)dlsym(ptr, "qore_module_url");
    if (url) {
        mod_info.url = url;
    }

    const char** dep_list = (const char**)dlsym(ptr, "qore_module_dependencies");
    if (dep_list) {
        const char* dep = dep_list[0];
        //printd(5, "dep_list=%p (0=%s)\n", dep_list, dep);
        for (int j = 0; dep; dep = dep_list[++j]) {
            mod_info.dependencies.push_back(dep);
        }
    }

    return loadBinaryModuleFromDesc(xsink, &dlh, mod_info, path, feature, pgm, reexport);
}

QoreAbstractModule* QoreModuleManager::loadBinaryModuleFromDesc(ExceptionSink& xsink, DLHelper* dlh,
        QoreModuleInfo& mod_info, const char* path, const char* feature, QoreProgram* pgm, bool reexport) {
    // take info hash immediately, if any
    ReferenceHolder<QoreHashNode> info(mod_info.info, &xsink);

    // get module name
    if (mod_info.name.empty()) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(path), "module '%s': no feature name " \
            "present in module", path);
        return nullptr;
    }

    const char* name = mod_info.name.c_str();

    // ensure provided feature matches with expected feature
    if (feature && mod_info.name != feature) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': provides feature " \
            "'%s', expecting feature '%s', skipping, rename module to %s.qmod to load", path, name,
            feature, name);
        return nullptr;
    }

    // get qore module API major number
    if (mod_info.api_major < 0) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': no qore " \
            "module API major number", path, name);
        return nullptr;
    }

    // get qore module API minor number
    if (mod_info.api_minor < 0) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': no qore " \
            "module API minor number", path, name);
        return nullptr;
    }

    // get initialization function
    if (!mod_info.init) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "module init method", path, name);
        return nullptr;
    }

    // get namespace initialization function
    if (!mod_info.ns_init) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "namespace init method", path, name);
        return nullptr;
    }

    // get deletion function
    if (!mod_info.del) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "delete method", path, name);
        return nullptr;
    }

    // get qore module description
    if (mod_info.desc.empty()) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "description", path, name);
        return nullptr;
    }

    // get qore module version
    if (mod_info.version.empty()) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "version", path, name);
        return nullptr;
    }

    // get qore module author
    if (mod_info.author.empty()) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing " \
            "author", path, name);
        return nullptr;
    }

    // load dependencies
    for (std::string& dep : mod_info.dependencies) {
        //printd(5, "loading module dependency=%s\n", dep);
        loadModuleIntern(xsink, xsink, dep.c_str(), pgm);
        if (xsink) {
            return nullptr;
        }
    }

    // see if a module with this name is already registered
    QoreAbstractModule* mi = findModuleUnlocked(name);
    if (mi) {
        // if the module is the same, then do not return an error
        if (mi->isPath(path))
            return mi;
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s' already " \
            "registered by '%s'", path, name, mi->getFileName());
        return nullptr;
    }

    // check if it's been blacklisted
    bl_map_t::const_iterator i = mod_blacklist.find(name);
    if (i != mod_blacklist.end()) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': '%s' is blacklisted %s",
            path, name, i->second);
        return nullptr;
    }

    if (!is_module_api_supported(mod_info.api_major, mod_info.api_minor)) {
        QoreStringNode* str = new QoreStringNodeMaker("module '%s': feature '%s': API mismatch, module supports " \
            "API %d.%d, however only version", path, name, mod_info.api_major, mod_info.api_minor);

        if (qore_mod_api_list_len > 1)
            str->concat('s');
        // add all supported api pairs to the string
        for (unsigned j = 0; j < qore_mod_api_list_len; ++j) {
            str->sprintf(" %d.%d", qore_mod_api_list[j].major, qore_mod_api_list[j].minor);
            if (j != qore_mod_api_list_len - 1) {
                if (qore_mod_api_list_len > 2) {
                    if (j != qore_mod_api_list_len - 2) {
                        str->concat(",");
                    } else {
                        str->concat(", and");
                    }
                } else {
                    str->concat(" and");
                }
            }
            if (j == qore_mod_api_list_len - 1) {
                str->concat(' ');
                if (qore_mod_api_list_len > 1) {
                    str->concat("are");
                } else {
                    str->concat("is");
                }
                str->concat(" supported");
            }
        }

        printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->c_str());
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), str);
        return nullptr;
    }

    //printd(5, "module_license_str: '%s' license_str: '%s'\n", module_license_str ? module_license_str : "n/a", license_str.getBuffer());

    switch (mod_info.license) {
        case QL_GPL: if (mod_info.license_str.empty()) mod_info.license_str = "GPL"; break;
        case QL_LGPL: if (mod_info.license_str.empty()) mod_info.license_str = "LGPL"; break;
        case QL_MIT: if (mod_info.license_str.empty()) mod_info.license_str = "MIT"; break;
        default:
            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': " \
                "invalid qore_module_license symbol (%d)", path, name, mod_info.license);
            return nullptr;
    }

    if (qore_license != QL_GPL && mod_info.license == QL_GPL) {
        xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': qore " \
            "library initialized with non-GPL license, but module requires GPL", path, name);
        return nullptr;
    }

    printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) %s: calling module_init@%p\n", path,
        name, *mod_info.init);

    // this is needed for backwards-compatibility for modules that add builtin functions in the module initialization code
    QoreModuleContextHelper qmc(name, pgm, xsink);
    try {
        printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) %s: calling module_init@%p (tid: %d)\n", path,
            name, *mod_info.init, q_gettid());
        assert(q_gettid());

        QoreStringNode* str = (*mod_info.init)();
        if (str) {
            // rollback all module changes
            qmc.rollback();

            xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': " \
                "initialization error: %s", path, name, str->c_str());
            str->deref();
            return nullptr;
        }
    } catch (AbstractException& e) {
        // rollback all module changes
        qmc.rollback();
        e.convert(&xsink);
        return nullptr;
    }

    if (qmc.hasError()) {
        // rollback all module changes
        qmc.rollback();
        return nullptr;
    }

    // commit all module changes - to the current program or to the static namespace
    qmc.commit();

    mi = new QoreBuiltinModule(nullptr, path, name, mod_info.desc.c_str(), mod_info.version.c_str(),
        mod_info.author.c_str(), mod_info.url.c_str(), mod_info.license_str.c_str(), mod_info.api_major,
        mod_info.api_minor, *mod_info.init, *mod_info.ns_init, *mod_info.del, mod_info.parse_cmd,
        dlh ? dlh->release() : nullptr, info.release());
    QMM.addModule(mi);

    ModuleReExportHelper mrh(mi, reexport);

    printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) registered '%s'\n", path, name);
    return mi;
}

void QoreModuleManager::delOrig(QoreAbstractModule* mi) {
    while (mi) {
        const char *n = mi->getName();
        //printd(5, "QoreModuleManager::delOrig() mi: %p '%s'\n", mi, mi->getName());

        module_map_t::iterator i = map.find(n);
        assert(i != map.end());
        assert(i->second == mi);

        QoreAbstractModule* next = mi->getNext();
        map.erase(i);
        delete mi;
        mi = next;
    }
}

// deletes only user modules
void QoreModuleManager::delUser() {
    //md_map.show("md_map");
    //rmd_map.show("rmd_map");

    // first delete user modules in dependency order
    while (!umset.empty()) {
        strset_t::iterator ui = umset.begin();
        module_map_t::iterator i = map.find((*ui).c_str());
        assert(i != map.end());
        QoreAbstractModule* m = i->second;
        assert(m->isUser());

        delOrig(m->getNext());
        //printd(5, "QoreModuleManager::delUser() deleting '%s' (%s) %p\n", (*ui).c_str(), i->first, m);
        umset.erase(ui);

        removeUserModuleDependency(m->getName(), m->getOrigName());

        map.erase(i);
        delete m;
    }

#ifdef DEBUG
    for (module_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i) {
        if (i->second->isUser()) {
            printd(0, "QoreModuleManager::delUser() '%s' %p not yet removed\n", i->second->getName(), i->second);

            for (md_map_t::iterator i = md_map.begin(), e = md_map.end(); i != e; ++i) {
                QoreString str("[");
                for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si) {
                    str.sprintf("'%s',", (*si).c_str());
                }
                str.concat("]");

                printd(0, " + md_map '%s' -> %s\n", i->first.c_str(), str.getBuffer());
            }

            rmd_map.show("rmd_map");

            assert(false);
        }
    }
#endif

    assert(md_map.empty());
    assert(rmd_map.empty());
}

void QoreModuleManager::cleanup() {
    QORE_TRACE("ModuleManager::cleanup()");

    module_map_t::iterator i;
    while ((i = map.begin()) != map.end()) {
        QoreAbstractModule* m = i->second;
        map.erase(i);
        delete m;
    }

    assert(modset.empty());
}

void QoreModuleManager::issueParseCmd(const QoreProgramLocation* loc, const char* mname, const QoreString& cmd) {
    ExceptionSink xsink;

    QoreProgram* pgm = getProgram();

    AutoLocker al(mutex); // make sure checking and loading are atomic
    loadModuleIntern(xsink, xsink, mname, pgm);

    if (xsink) {
        parseException(*loc, "PARSE-COMMAND-ERROR", loadModuleError(mname, xsink));
        return;
    }

    QoreAbstractModule* mi = findModule(mname);
    assert(mi);

    mi->issueModuleCmd(loc, cmd, pgm->getParseExceptionSink());
}

#define QORE_MAX_MODULE_ERROR_DESC 200
void QoreModuleManager::issueRuntimeCmd(const char* mname, QoreProgram* pgm, const QoreString& cmd, ExceptionSink* xsink) {
    // ensure the program is in context
    QoreProgramContextHelper pch(pgm);

    AutoLocker al(mutex); // make sure checking and loading are atomic
    loadModuleIntern(*xsink, *xsink, mname, pgm);
    if (*xsink) {
        return;
    }

    QoreAbstractModule* mi = findModule(mname);
    assert(mi);

    mi->issueModuleCmd(&loc_builtin, cmd, xsink);

    // enrich exception description if present
    if (*xsink) {
        // truncate command at first eol or at max 200 chars
        qore_offset_t i = cmd.find('\n');
        if (i == -1) {
            i = cmd.find('\r');
        }
        if (((i == -1) && (cmd.size() > QORE_MAX_MODULE_ERROR_DESC)) || (i > QORE_MAX_MODULE_ERROR_DESC)) {
            i = QORE_MAX_MODULE_ERROR_DESC;
        }
        if (i > 0) {
            QoreString cmd_copy(&cmd, i);
            cmd_copy.trim();
            xsink->appendLastDescription(": module command error from command '%s...'", cmd_copy.c_str());
        } else {
            xsink->appendLastDescription(": module command error from command '%s'", cmd.c_str());
        }
    }
}

QoreHashNode* ModuleManager::getModuleHash() {
   return QMM.getModuleHash();
}

QoreHashNode* QoreModuleManager::getModuleHash() {
    bool with_filename = !(runtime_get_parse_options() & PO_NO_EXTERNAL_INFO);
    QoreHashNode* h = new QoreHashNode(hashTypeInfo);
    qore_hash_private* ph = qore_hash_private::get(*h);
    AutoLocker al(mutex);
    for (module_map_t::const_iterator i = map.begin(); i != map.end(); ++i) {
        if (!i->second->isPrivate())
            ph->setKeyValueIntern(i->second->getName(), i->second->getHash(with_filename));
    }
    return h;
}

QoreListNode* ModuleManager::getModuleList() {
   return QMM.getModuleList();
}

QoreListNode* QoreModuleManager::getModuleList() {
    bool with_filename = !(runtime_get_parse_options() & PO_NO_EXTERNAL_INFO);
    QoreListNode* l = new QoreListNode(hashTypeInfo);
    AutoLocker al(mutex);
    for (module_map_t::const_iterator i = map.begin(); i != map.end(); ++i) {
        if (!i->second->isPrivate())
            l->push(i->second->getHash(with_filename), nullptr);
    }
    return l;
}

char version_list_t::set(const char* v) {
    ver = v;

    // set version list
    ver.trim();

    char* a;
    char* p = a = (char*)ver.getBuffer();
    while (*p) {
        if (*p == '.') {
            char save = *p;
            *p = '\0';
            push_back(atoi(a));
            //printd(5, "this=%p a=%s\n", this, a);
            *p = save;
            a = p + 1;
        } else if (!isdigit(*p))
            return *p;
        ++p;
    }
    //printd(5, "this=%p a=%s FINAL\n", this, a);
    push_back(atoi(a));
    return '\0';
}
