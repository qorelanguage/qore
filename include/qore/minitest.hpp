//-----------------------------------------------------------------------------
// Minimalistic library to help with testing. Version 1.0.
//
// Supported features:
// a) minimal work needed to set up tests:
//      TEST()
//      {
//           assert(...this..);
//      }
//      TEST()
//      {
//          assert(...that...);
//      }
// b) tests can be executed in random order (this may catch very subtle bugs)
// c) one can execute tests only from last few changed files
//    so the last item of code-compile-test cycle will be fast
//-----------------------------------------------------------------------------
// 
// Copyright 2004 Pavel Vozenilek
//
// Distributed under the Boost Software License, Version 1.0.
// (See copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Disclaimer: not a Boost library.
//
// For more details see:
//  http://www.codeproject.com/cpp/minitest.asp
//

#ifndef MINITEST_PV2004_HPP_
#define MINITEST_PV2004_HPP_

#ifdef DEBUG

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <cstring>
#include <vector>
#include <algorithm>
#include <assert.h>


#ifndef MINITEST_NO_SUPPORT_FOR_LAST_CHANGED_FILES_TEST
// If the <sys/...> files are no available on your platform 
// you cannot use feature test-only-n-last-changed-files.
// Define MINITEST_NO_SUPPORT_FOR_LAST_CHANGED_FILES_TEST then.
//
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <ctime>
#  include <map>
#  include <functional>
#endif

namespace minitest {
namespace detail {

//-----------------------------------------------------------------------------
struct single_test_data
{
    const char* file;
    int line;
    typedef void(*test_t)(void);
    test_t test;

    single_test_data(test_t t, const char* f, int l) 
    : file(f), line(l), test(t) {}
};

//-----------------------------------------------------------------------------
template<typename Unused>
struct tests_collection
{
    typedef std::vector<single_test_data> all_tests_t; 
    
    // This is (1) to avoid ODR violation
    // and (2) postpone dynamic allocation until needed.
    static all_tests_t*& get() {
        static all_tests_t* all_tests;
        return all_tests;
    }

    static void add_test(void (*function)(void), const char* file, int line) {
        if (!get()) {
            get() = new all_tests_t();
            get()->reserve(256);
        }
        get()->push_back(single_test_data(function, file, line));
    }

    static void remove_test(const char* file, int line) {
        all_tests_t* p = get();
        if (!p) return; // cannot happen

        // optimized: speed critical part of code
        single_test_data* val = &(*p)[0];
        for (unsigned i = 0, n = p->size(); i != n; ++i, ++val) {
            
            if (val->line != line || val->file != file) { // strcmp() not needed
                continue;
            }

            if (i != n - 1) {
                single_test_data* last = &(*p)[n - 1];
                val->line = last->line;
                val->file = last->file;
                val->test = last->test;
            }
            p->pop_back();
            
            if (p->empty()) {
                delete get(); // clean up as much as possible
                get() = 0;
            }
            break;
        }
    }
};

//-----------------------------------------------------------------------------
inline void register_test(void (*function)(void), const char* file, int line) {
    // pointer indirection used to avoid eager inlining
    typedef void (*add_test_t)(void (*f)(void), const char*, int);
    add_test_t add_test = &tests_collection<void>::add_test;
    (*add_test)(function, file, line);
}

//-----------------------------------------------------------------------------
inline void unregister_test(const char* file, int line) {
    // pointer indirection used to avoid eager inlining
    typedef void (*remove_test_t)(const char*, int);
    remove_test_t remove_test = &tests_collection<void>::remove_test;
    (*remove_test)(file, line);
}

//-----------------------------------------------------------------------------
struct ltstr
{
    bool operator()(const char* s1, const char* s2) const
    {
        using namespace std;
        return strcmp(s1, s2) < 0;
    }
};

} // namespace detail

//-----------------------------------------------------------------------------
// Result of testing (returned always if app is not stopped by assert() etc).
// Depending whether all tests were OK or not some fields contain data or zeros.
//
struct result
{
    bool all_tests_succeeded;  // false if throw was caught
    unsigned sucessful_tests_count; // always filled
    typedef void (*test_function_t)(void);
    // 0 if all_tests_succeeded == true else failing test: it can be re-executed
    test_function_t failed_test; 
    const char* failed_test_file; // 0 if all_tests_succeeded == true
    int failed_test_line; // 0 if all_tests_succeeded == true

    // when all was OK
    result(unsigned count)
    : all_tests_succeeded(true), sucessful_tests_count(count), 
      failed_test(0), failed_test_file(0), failed_test_line(0) {}

    // when an exception was caught
    result(unsigned count, test_function_t func, const char* file, int line)
    : all_tests_succeeded(false), sucessful_tests_count(count),
    failed_test(func), failed_test_file(file), failed_test_line(line) {}
};

//-----------------------------------------------------------------------------
// Execute all tests, either in current order or randomly (the randomness is taken
// from rand()). Return information how the tests passed - see "struct result"
// above for more details.
//
inline minitest::result execute_all_tests(bool random_shuffle_tests = true)
{
    detail::tests_collection<void>::all_tests_t* tests = detail::tests_collection<void>::get();
    if (tests == 0) {
        return result(0); // success, sort of
    }

    if (random_shuffle_tests) {
        // std::rand() is used internally. You can set its seed with std::srand().
        // Setting up seed is outside of scope for this library.
        std::random_shuffle(tests->begin(), tests->end());
    }

    for (unsigned i = 0, n = tests->size(); i != n; ++i) {
        try {
            (*tests)[i].test();
        } catch(...) {
            // failure: provide details
            return result(i, (*tests)[i].test, (*tests)[i].file, (*tests)[i].line);
        }
    }
    // success
    return result(tests->size());
}

#ifndef MINITEST_NO_SUPPORT_FOR_LAST_CHANGED_FILES_TEST
//-----------------------------------------------------------------------------
// Execute tests from up to N last changed source files.
// Time of last modification of source is read and tests from
// last updated ones are executed. This should help with fast
// re-testing after every modification of codebase.
//
// If source files are not available their tests are ignored.
// Returns the same kind of data as execute_all_tests().
//
// Beware: doesn't take into account modification of header files 
// and dependencies. It should be used only for quick-and-dirty 
// re-test after source change.
//
// Won't work properly if filesystem behaves in non-standard way
// or doesn't keep modification times.
// 
inline minitest::result test_last_changed_files(unsigned test_max_N_changed_files)
{
    if (test_max_N_changed_files == 0) {
        return result(0); // success, sort of
    }
    typedef detail::tests_collection<void>::all_tests_t all_tests_t;
    all_tests_t* tests = detail::tests_collection<void>::get();    
    if (tests == 0) {
        return result(0); // success, sort of
    }

    // get modification times for all files (just once per file
    // to avoid problems on slow drives)
    typedef std::map<const char*, time_t, detail::ltstr> file2time_t;
    file2time_t file2time;

    for (all_tests_t::iterator it1 =  tests->begin(), end1 = tests->end(); it1 != end1; ++it1) 
    {
        if (file2time.find(it1->file) != file2time.end()) {
            continue; // already checked
        }
        struct stat st;
        st.st_mtime = -1;
        if (stat(it1->file, &st) == -1) {
            continue; // file doesn't exist, not accessible, moved?
        }
        if (st.st_mtime == -1) {
            continue; //???
        }
        file2time[it1->file] = st.st_mtime;
    }
    if (file2time.empty()) {
        return result(0); // no files were found
    }

    // sort files by last modification time, from latest
    typedef std::map<time_t, std::vector<const char*>, std::greater<time_t> > time2files_t;
    time2files_t time2files;

    for (file2time_t::const_iterator it2 = file2time.begin(), end2 = file2time.end(); it2 != end2; ++it2) 
    { 
        time2files[it2->second].push_back(it2->first);
    }
    file2time.clear(); // no longer needed
    
    // extract proper number of files to be tested
    std::vector<const char*> files;
    files.reserve(test_max_N_changed_files + 1);
    
    for (time2files_t::iterator it3 = time2files.begin(), end3 = time2files.end(); it3 != end3; ++it3) 
    {
        files.insert(files.end(), it3->second.begin(), it3->second.end());
        if (files.size() >= test_max_N_changed_files) {
            break;
        }
    }
    time2files.clear(); // no longer needed

    std::sort(files.begin(), files.end(), detail::ltstr());

    // Go through all tests and if they are from recently 
    // changed file execute them.

    unsigned count = 0;
    for (all_tests_t::const_iterator it4 = tests->begin(), end4 = tests->end(); it4 != end4; ++it4) 
    {
        if (!std::binary_search(files.begin(), files.end(), it4->file, detail::ltstr())) {
            continue;
        }

        // finally we have test from recently modified source file
        try {
            it4->test();
        } catch(...) {
            // failure: provide details
            return result(count, it4->test, it4->file, it4->line);
        }

        ++count;
    }    
    return result(count); // success
}
#endif //!MINITEST_NO_SUPPORT_FOR_LAST_CHANGED_FILES_TEST
 
} // namespace minitest

//-----------------------------------------------------------------------------
// Macros to create and automatically register individual tests.
// Usage:
/*

#ifdef DO_TESTING

TEST()
{
   assert(...);
}

TEST()
{
  ...
  assert(...);
}

...

#endif
*/

#ifdef TEST
#  error TEST macro already used
#endif
#ifdef MINITEST_IMPL1
#  error MINITEST_IMPL1 already used
#endif
#ifdef MINITEST_IMPL2
#  error MINITEST_IMPL2 already used
#endif

// This macro does:
// 1. Registers void(*)(void) function as test during
//    statics initialization time.
// 2. When statics are destroyed it unregisters this function
//   (useful e.g. when dynamic libraries are unloaded).
// 3. Provides test function signature invisibly to the user.
//
// The trick with dummy boolean must be used, constructor
// doesn't always work on Intel 7 as it should. Destructor does.
//
#define MINITEST_IMPL2(file, line)                                  \
    static void minitest_internal_function_##line ();               \
                                                                    \
    static bool minitest_internal_initialize_##line () {            \
        minitest::detail::register_test(                            \
        &minitest_internal_function_##line, file, line);            \
        return false;                                               \
    }                                                               \
                                                                    \
    static bool minitest_internal_dummy_##line =                    \
        minitest_internal_initialize_##line ();                     \
                                                                    \
    struct minitest_internal_unloader_##line {                      \
        ~minitest_internal_unloader_##line () {                     \
            minitest::detail::unregister_test(file, line);          \
        }                                                           \
    };                                                              \
                                                                    \
    static minitest_internal_unloader_##line                        \
        minitest_internal_unloader_instance_##line;                 \
                                                                    \
    static void minitest_internal_function_##line ()                \
    /**/    
        
#define MINITEST_IMPL1(f, l)    MINITEST_IMPL2(f, l)
#define TEST()                MINITEST_IMPL1(__FILE__, __LINE__)

//-----------------------------------------------------------------------------
#include <qore/QoreString.h>

// Support for execution of QoreString as a QoreProgram.
// The function needs to be named 'test'.
extern void run_Qore_test(QoreString& str, const char* file, int line, const char* details = 0);

#endif // DEBUG
#endif 

// EOF

