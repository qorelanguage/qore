#ifndef SYBASE_TESTS_COMMON_H_
#define SYBASE_TESTS_COMMON_H_

// Common settings for all unit tests.
// Change the values as needed for machine where the tests are run.

#ifdef SYBASE
// Server running locally

// Connection parameters (comma separated) used for tests
// 1. user name
// 2. password
// 3. database name 
//  
#define SYBASE_TEST_SETTINGS "sa", 0, "pavel"

#else
// MS SQL Server running on VMWare host machine, details in freedts.conf
//#define SYBASE_TEST_SETTINGS "sa", "pavel", "mssql"

// local Sybase connected via FreeTDS, details in freedts.conf
#define SYBASE_TEST_SETTINGS "sa", 0, "pavel"

#endif

#endif

// EOF


