.load unit.dylib

with pairs(expected, actual) as (
    select 1, 1

    union all

    select "a", "a"

    union all

    select cast(1 as float), cast(1 as float)
)
select
    assert_equal(expected, actual) from pairs;