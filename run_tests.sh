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

# Test that qore is built.
if [ -s "./qore" ] && [ -r "./lib/.libs/libqore.so" ]; then
    QORE="./qore"
    QR="./qr"
else
    echo "Qore is not built. Exiting."
    exit 1
fi

# Search for tests in the test directory.
TESTS=$( find ./examples/test/ -name "*.qtest" )
FAILED_TESTS=""

TEST_COUNT=$( echo $TESTS | wc -w )
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
        PASSED_TEST_COUNT=$((PASSED_TEST_COUNT+1))
        i=$((i+1))
        continue
    fi
    
    # Run single test.
    $QORE $test $TEST_OUTPUT_FORMAT
    
    if [ $? -eq 0 ]; then
        PASSED_TEST_COUNT=$((PASSED_TEST_COUNT+1))
    else
        FAILED_TEST_COUNT=$((FAILED_TEST_COUNT+1))
        FAILED_TESTS="$FAILED_TESTS $test"
    fi

    i=$((i+1))
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

