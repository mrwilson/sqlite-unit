#!/usr/bin/env bash

function run_test() {
    TEST_FILE=$1
    EXPECTED_OUTPUT=$2

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

run_test test_pass.sql "PASS"
run_test test_fail.sql "Error: near line 3: FAIL: 0 != 1"

run_test test_text_pass.sql "PASS"
run_test test_text_fail.sql "Error: near line 3: FAIL: foo != bar"

run_test test_fail_mismatched_types.sql "Error: near line 3: Mismatched types"