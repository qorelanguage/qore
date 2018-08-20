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

export LD_LIBRARY_PATH=$QORE_LIB_PATH
export QORE_MODULE_DIR=./qlib:$QORE_MODULE_DIR

# Test time commands.
TIME_OK=0
TIME_BIN=""
TIME_CMD=""
TIME_FORMAT="-------------------------------------\nUserTime: %U\nSystemTime: %S\nWallClockTime: %e\nMinorPageFaults: %R\nMajorPageFaults: %F\nAverageSharedTextSize: %X\nAverageUnsharedDataSize: %D\nAverageStackSize: %p\nAverageTotalSize: %K\nMaximumResidentSetSize: %M\nAverageResidentSetSize: %t\nFilesystemInputs: %I\nFilesystemOutputs: %O\nSocketMessagesSent: %s\nSocketMessagesReceived: %r\nExitStatus: %x"

test_time() {
    TIME_CMD="$TIME_BIN -f \"$TIME_FORMAT\""
    eval $TIME_CMD ls / >/dev/null 2>&1
    TIME_OK=$?
}

TIME_BINS="time /usr/bin/time /bin/time `which time`"
for tm in $TIME_BINS; do
    TIME_BIN=$tm
    test_time
    if [ $TIME_OK -eq 0 ]; then
        break
    fi
    TIME_CMD=""
done

if [ "$TIME_CMD" = "" ]; then
    TIME_CMD="time -p"
fi

# Print info about used variables etc.
echo "Using qore: $QORE, libqore: $LIBQORE"
echo "QORE_MODULE_DIR=$QORE_MODULE_DIR"
echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
printf "TIME_CMD: %s\n" "$TIME_CMD";echo

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
        echo "cmdline: LD_PRELOAD=$LIBQORE $QORE $test $TEST_OUTPUT_FORMAT"
        echo "-------------------------------------"
    fi

    # Run single test.
    eval LD_PRELOAD=$LIBQORE $TIME_CMD $QORE $test $TEST_OUTPUT_FORMAT

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
