#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void assertion_error(sqlite3_context* context, char* message) {
    sqlite3_result_error(context, message, -1);
}

static void assert_not_null(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  if (sqlite3_value_type(argv[0]) != SQLITE_NULL) {
      printf("PASS\n");
  } else {
      assertion_error(context, sqlite3_mprintf("FAIL: Expected value was null"));
  }
}

static char* compare_values(int type, sqlite3_value **argv) {

  if (type == SQLITE_INTEGER) {
    int value1 = sqlite3_value_int64(argv[0]);
    int value2 = sqlite3_value_int64(argv[1]);

    if (value1 == value2) {
        return NULL;
    } else {
        return sqlite3_mprintf("FAIL: %d != %d", value1, value2);
    }
  }

  if (type == SQLITE_TEXT) {
    char* value1 = (char*) sqlite3_value_text(argv[0]);
    char* value2 = (char*) sqlite3_value_text(argv[1]);

    if (strcmp(value1, value2) == 0) {
        return NULL;
    } else {
        return sqlite3_mprintf("FAIL: %s != %s", value1, value2);
    }
  }

  if (type == SQLITE_FLOAT) {
    double value1 = sqlite3_value_double(argv[0]);
    double value2 = sqlite3_value_double(argv[1]);

    if (value1 == value2) {
        return NULL;
    } else {
        return sqlite3_mprintf("FAIL: %f != %f", value1, value2);
    }
  }

  return "Not implemented yet";

}

static void assert_null(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
      printf("PASS\n");
  } else {
      assertion_error(context, sqlite3_mprintf("FAIL: Expected value was not null"));
  }
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
    sqlite3_result_error(context, "Mismatched types", -1);
    return;
  }

  char* message = compare_values(type1, argv);

  if (message == NULL) {
    printf("PASS\n");
  } else {
    assertion_error(context, message);
  }

}

int sqlite3_unit_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  (void) pzErrMsg;

  sqlite3_create_function(db, "assert_equal", 2, SQLITE_UTF8, 0, assert_equal, 0, 0);
  sqlite3_create_function(db, "assert_null",  1, SQLITE_UTF8, 0, assert_null,  0, 0);
  sqlite3_create_function(db, "assert_not_null",  1, SQLITE_UTF8, 0, assert_not_null,  0, 0);
  return SQLITE_OK;
}