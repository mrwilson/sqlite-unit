#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void assert_equal(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){

  if (sqlite3_value_type(argv[0]) == SQLITE_NULL) return;

  if (sqlite3_value_type(argv[0]) == SQLITE_INTEGER) {
    if (sqlite3_value_int64(argv[0]) == sqlite3_value_int64(argv[1])) {
        printf("PASS\n");
    } else {
        sqlite3_result_error(context, sqlite3_mprintf("FAIL: %d != %d",sqlite3_value_int64(argv[0]),sqlite3_value_int64(argv[1]))  , -1);
    }
  } else {
     sqlite3_result_error(context, "Not implemented yet", -1);
  }
}

int sqlite3_unit_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  (void) pzErrMsg;
  return sqlite3_create_function(
    db, "assertEqual", 2, SQLITE_UTF8, 0, assert_equal, 0, 0);
}