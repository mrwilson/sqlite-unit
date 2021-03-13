#!/usr/bin/env bash

function run_test() {
    TEST_FILE=$1
    EXPECTED_OUTPUT=$(echo -e $2)

    ACTUAL_OUTPUT=$(sqlite3 < "test/$TEST_FILE" 2>&1 | tr -d '\r')

    if [[ "$ACTUAL_OUTPUT" = "$EXPECTED_OUTPUT" ]]; then
      echo "Test passed: ${TEST_FILE}"
    else
      echo "Test failed: ${TEST_FILE}. Expected \"${EXPECTED_OUTPUT}\" but was \"${ACTUAL_OUTPUT}\""
    fi
}

for lib in *.c; do
  echo "Compiling ${lib}"
  gcc \
    -dynamiclib \
    -lsqlite3 \
    -DSQLITE_VTAB_INNOCUOUS=0 \
    ${lib} \
    -o $(echo $lib | sed 's/\.c/.dylib/')
done

run_test test_int_pass.sql "ok\n1..1"
run_test test_int_fail.sql "not ok\n# FAIL: 0 != 1\n1..1"

run_test test_text_pass.sql "ok\n1..1"
run_test test_text_fail.sql "not ok\n# FAIL: foo != bar\n1..1"

run_test test_float_pass.sql "ok\n1..1"
run_test test_float_fail.sql "not ok\n# FAIL: 1.000000 != 2.000000\n1..1"

run_test test_null_pass.sql "ok\n1..1"
run_test test_null_fail.sql "not ok\n# FAIL: Expected value was not null\n1..1"

run_test test_not_null_pass.sql "ok\n1..1"
run_test test_not_null_fail.sql "not ok\n# FAIL: Expected value was null\n1..1"

run_test test_multi_pass.sql "ok\nok\nok\n1..3"

run_test test_fail_mismatched_types.sql "not ok\n# Mismatched types\n1..1"