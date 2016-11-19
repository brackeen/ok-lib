# ok-lib
Generic vector and hash maps for C.

## Goals
* Easy-to-use API.
* Vectors and hash maps that work with any type, including user-defined structs.
* C11 support where available (using `_Generic`).
* No function generation via macros.
* Single-file header - just include [`ok_lib.h`](ok_lib.h).

## Non-Goals
* Support C versions older than C99.
* Thread safety.
* Create the fastest or most memory efficient implementation. (However, `ok_map` compares favorably to other hashtable implementations in terms of speed and memory efficiency.)

## Vector Example
```C
typedef struct ok_vec_of(const char *) str_vec_t;

str_vec_t vec;
ok_vec_init(&vec);

ok_vec_push(&vec, "dave");
ok_vec_push(&vec, "mary");
ok_vec_push(&vec, "steve");

const char *value = ok_vec_get(&vec, 2);

printf("A person: %s\n", value);

ok_vec_foreach(&vec, const char *value) {
    printf("Name: %s\n", value);
}

ok_vec_deinit(&vec);

```
## Map Example
```C
typedef struct ok_map_of(const char *, char *) str_map_t;

str_map_t map;
ok_map_init(&map);

ok_map_put(&map, "dave", "bananas");
ok_map_put(&map, "mary", "grapes");
ok_map_put(&map, "steve", "pineapples");

char *value = ok_map_get(&map, "dave");

printf("dave likes %s.\n", value);

ok_map_foreach(&map, const char *key, char *value) {
    printf("> %s likes %s.\n", key, value);
}

ok_map_deinit(&map);
```

## Map Example With a Custom Key Type
```C
typedef struct {
    float x;
    float y;
} point_t;

static ok_hash_t point_hash(point_t p) {
    ok_hash_t hash_x = ok_float_hash(p.x);
    ok_hash_t hash_y = ok_float_hash(p.y);
    return ok_hash_combine(hash_x, hash_y);
}

static bool point_equals(const void *v1, const void *v2) {
    const point_t *p1 = (const point_t *)v1;
    const point_t *p2 = (const point_t *)v2;
    return (p1->x == p2->x && p1->y == p2->y);
}

...

typedef struct ok_map_of(point_t, char *) point_map_t;

point_map_t map;
ok_map_init_custom(&map, point_hash, point_equals);

point_t key = { 100.0, 200.0 };
ok_map_put(&map, key, "Buried treasure");

printf("Value at (%f, %f): %s\n", key.x, key.y, ok_map_get(&map, key));

ok_map_deinit(&map);

```

## Implementation Overview

The `ok_vec` is implemented as a structure containing an array that is reallocated as needed.

The `ok_map` is implemented using [open addressing](https://en.wikipedia.org/wiki/Open_addressing),
with linear probing and cleanup on deletion (no lazy deletion).

## Extras
* [More examples](extras/example)
* [C++ wrapper](extras/wrapper)
* [Tests](extras/test)

## License
[ZLIB](http://en.wikipedia.org/wiki/Zlib_License)
