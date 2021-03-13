# sqlite-unit

A tiny [TAP](https://testanything.org/)-compatible framework for testing SQLite queries.

## Usage

The library currently supports:

* `assert_equal`
* `assert_null`
* `assert_not_null`

## Example

```sql
select
    assert_equal(1, 2),
    assert_equal(1, 1),
    assert_null(null),
    assert_not_null(null),
    assert_equal("a","a");
```

Output
```
not ok
# FAIL: 1 != 2
ok
ok
not ok
# FAIL: Expected value was null
ok
1..5
```
