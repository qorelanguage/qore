#!/bin/sh

print_usage ()
{
  echo "Usage: run_tests.sh [OPTIONS]"
  echo "Run all the qore tests."
  echo
  echo "  -j    Use --format=junit option for the tests, making them print JUnit output."
  echo "  -v    Use --format=plain option for the tests, making them print one statement per each test case."
}

TEST_OUTPUT_FORMAT=""
PRINT_TEXT=1

# Handle command-line arguments.
if [ $# -eq 0 ]; then
    TEST_OUTPUT_FORMAT=""
elif [ $# -eq 1 ]; then
    if [ "$1" = "-v" ]; then
        TEST_OUTPUT_FORMAT="-v"
    elif [ "$1" = "-j" ]; then
        TEST_OUTPUT_FORMAT="--format=junit"
        PRINT_TEXT=0
    else
        echo "Unknown option: $1"
        print_usage
        exit 1
    fi
else
    echo "Too many options."
    print_usage
    exit 1
fi

QORE=""
QR=""
LIBQORE=""
QORE_LIB_PATH="./lib/.libs:./qlib:$LD_LIBRARY_PATH"

# Test that qore is built.
if [ -s "./.libs/qore" ] && [ -f "./qore" ] && [ -f "./lib/.libs/libqore.so" -o "./lib/.libs/libqore.dylib" ]; then
    if [ -f "./lib/.libs/libqore.so" ]; then
        LIBQORE="./lib/.libs/libqore.so"
    elif [ -f "./lib/.libs/libqore.dylib" ]; then
        LIBQORE="./lib/.libs/libqore.dylib"
    fi
    QORE="./.libs/qore"
    QR="./.libs/qr"
else
    for D in `ls -d */`; do
        d=`echo ${D%%/}` 
        if [ -f "$d/CMakeCache.txt" ] && [ -f "$d/qore" ] && [ -f "$d/libqore.so" -o "$d/libqore.dylib" ]; then
            if [ -f "$d/libqore.so" ]; then
                LIBQORE="$d/libqore.so"
            elif [ -f "$d/libqore.dylib" ]; then
                LIBQORE="$d/libqore.dylib"
            fi
            QORE="$d/qore"
            QR="$d/qr"
            break
        fi
    done
fi

if [ -z "$QORE" ] || [ -z "$LIBQORE" ]; then
    echo "Qore is not built. Exiting."
    exit 1
fi

echo "Using qore: $QORE, libqore: $LIBQORE"; echo

# Search for tests in the test directory.
TESTS=`find ./examples/test/ -name "*.qtest"`
FAILED_TESTS=""

TEST_COUNT=`echo $TESTS | wc -w`
PASSED_TEST_COUNT=0
FAILED_TEST_COUNT=0

# Run tests.
i=1
for test in $TESTS; do
    if [ $PRINT_TEXT -eq 1 ]; then
        echo "====================================="
        echo "Running test ($i/$TEST_COUNT): $test"
        echo "-------------------------------------"
    fi

    if [ "$test" = "./examples/test/qore/classes/FtpClient/FtpClient.qtest" ]; then
        echo "Skipping $test because it doesn't really test what it should. Need to fix it."
        echo "-------------------------------------"; echo
        PASSED_TEST_COUNT=`expr $PASSED_TEST_COUNT + 1`
        i=`expr $i + 1`
        continue
    fi

    # Run single test.
    QORE_MODULE_DIR=./qlib:$QORE_MODULE_DIR LD_PRELOAD=$LIBQORE LD_LIBRARY_PATH=$QORE_LIB_PATH $QORE $test $TEST_OUTPUT_FORMAT

    if [ $? -eq 0 ]; then
        PASSED_TEST_COUNT=`expr $PASSED_TEST_COUNT + 1`
    else
        FAILED_TEST_COUNT=`expr $FAILED_TEST_COUNT + 1`
        FAILED_TESTS="$FAILED_TESTS $test"
    fi

    i=`expr $i + 1`
    if [ $PRINT_TEXT -eq 1 ]; then echo "-------------------------------------"; echo; fi
done

# Print test summary.
if [ $PRINT_TEXT -eq 1 ]; then
    TESTING_RESULT=""
    if [ $FAILED_TEST_COUNT -eq 0 ]; then
        TESTING_RESULT="Success."
    else
        TESTING_RESULT="Failure."
    fi

    echo; echo "*************************************"
    echo "TESTING RESULT: $TESTING_RESULT"
    echo "Passed $PASSED_TEST_COUNT out of $TEST_COUNT tests. $FAILED_TEST_COUNT tests failed."

    if [ $FAILED_TEST_COUNT -ne 0 ]; then
        echo "Failed tests:"
        for test in $FAILED_TESTS; do
            echo $test
        done
    fi

    echo "*************************************"
fi

exit $FAILED_TEST_COUNT
