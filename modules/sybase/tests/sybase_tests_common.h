#ifndef SYBASE_TESTS_COMMON_H_
#define SYBASE_TESTS_COMMON_H_

#ifdef SYBASE
// Server running locally

// Connection parameters used for tests
// 1. user name
// 2. password
// 3. database name 
//  
#define SYBASE_TEST_SETTINGS "sa", 0, "pavel"

#else
// MS SQL Server running on host machine, details in freedts.conf

#define SYBASE_TEST_SETTINGS "sa", "pavel", "mssql"

#endif

#endif

// EOF


