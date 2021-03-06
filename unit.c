#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  bool passed;
  char* message;
} TestRun;

int number_of_runs = 0;

static void tap_output(TestRun run) {
  if (run.passed) {
    printf("ok\n");
  } else {
    printf("not ok\n");
    printf("# %s\n", run.message);
  }
}

static void assert_not_null(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  TestRun t;
  t.passed = false;

  if (sqlite3_value_type(argv[0]) != SQLITE_NULL) {
      t.passed = true;
  } else {
      t.message = "FAIL: Expected value was null";
  }

  tap_output(t);

  number_of_runs++;
}

static TestRun compare_values(int type, sqlite3_value **argv) {

  TestRun t;
  t.passed = false;

  if (type == SQLITE_INTEGER) {
    int value1 = sqlite3_value_int64(argv[0]);
    int value2 = sqlite3_value_int64(argv[1]);

    if (value1 == value2) {
        t.passed = true;
        t.message = NULL;
    } else {
        t.message = sqlite3_mprintf("FAIL: %d != %d", value1, value2);
    }

    return t;
  }

  if (type == SQLITE_TEXT) {
    char* value1 = (char*) sqlite3_value_text(argv[0]);
    char* value2 = (char*) sqlite3_value_text(argv[1]);

    if (strcmp(value1, value2) == 0) {
        t.passed = true;
        t.message = NULL;
    } else {
        t.message = sqlite3_mprintf("FAIL: %s != %s", value1, value2);
    }

    return t;
  }

  if (type == SQLITE_FLOAT) {
    double value1 = sqlite3_value_double(argv[0]);
    double value2 = sqlite3_value_double(argv[1]);

    if (value1 == value2) {
        t.passed = true;
        t.message = NULL;
    } else {
        t.message = sqlite3_mprintf("FAIL: %f != %f", value1, value2);
    }

    return t;
  }

  t.message = "Not implemented yet";
  return t;
}

static void assert_null(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  TestRun t;
  t.passed = false;

  if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
      t.passed = true;
  } else {
      t.message = "FAIL: Expected value was not null";
  }

  tap_output(t);

  number_of_runs++;
}

static void assert_equal(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  if (sqlite3_value_type(argv[0]) == SQLITE_NULL) return;

  int type1 = sqlite3_value_type(argv[0]);
  int type2 = sqlite3_value_type(argv[1]);

  if (type1 != type2) {
    TestRun t;
    t.passed = false;
    t.message = "Mismatched types";
    tap_output(t);
  } else {
    tap_output(compare_values(type1, argv));
  }

  number_of_runs++;
}

void print_summary(sqlite3_context *context){
    sqlite3 *db = sqlite3_context_db_handle(context);
    sqlite3_stmt *stmt = sqlite3_next_stmt(db, NULL);

    if (sqlite3_stmt_status(stmt, SQLITE_STMTSTATUS_RUN, 1) == 1) {
        printf("1..%d\n", number_of_runs);
        number_of_runs = 0;
    }
}

int sqlite3_unit_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  (void) pzErrMsg;

  sqlite3_create_function(db, "assert_equal", 2, SQLITE_UTF8, 0, 0, assert_equal, print_summary);
  sqlite3_create_function(db, "assert_null",  1, SQLITE_UTF8, 0, 0, assert_null, print_summary);
  sqlite3_create_function(db, "assert_not_null",  1, SQLITE_UTF8, 0, 0, assert_not_null, print_summary);
  return SQLITE_OK;
}