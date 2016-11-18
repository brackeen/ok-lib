#include "ok_lib.h"
#include <stdio.h>

// MARK: Basic map example

static void basic_map_example() {
    typedef struct ok_map_of(const char *, char *) str_map_t;

    str_map_t map;
    ok_map_init(&map);

    // Put method
    ok_map_put(&map, "dave", "bananas");
    ok_map_put(&map, "mary", "grapes");

    // Put and get pointer to value
    *ok_map_put_and_get_ptr(&map, "sue") = "beans";
    *ok_map_put_and_get_ptr(&map, "john") = "fries";

    // Overwrite existing value
    ok_map_put(&map, "john", "salad");

    // Get
    printf("john: %s.\n", ok_map_get(&map, "john"));
    printf("mary: %s.\n", ok_map_get(&map, "mary"));

    // Overwrite via ok_map_get_ptr
    *ok_map_get_ptr(&map, "john") = "fries";
    printf("john: no, wait, %s.\n", ok_map_get(&map, "john"));

    // Examples when key doesn't exist
    printf("Map size: %zu\n", ok_map_count(&map));
    ok_map_get_ptr(&map, "cyrus"); // Returns NULL
    printf("Map size before ok_map_put_and_get_ptr(&map, \"cyrus\"): %zu\n", ok_map_count(&map));
    ok_map_put_and_get_ptr(&map, "cyrus"); // Mapping created - but value undefined!
    printf("Map size after ok_map_put_and_get_ptr(&map, \"cyrus\"): %zu\n", ok_map_count(&map));
    *ok_map_put_and_get_ptr(&map, "cyrus") = "(who knows)"; // Define the value so we don't crash

    // Iterator
    ok_map_foreach(&map, const char *key, char *value) {
        printf("> %s wants %s.\n", key, value);
    }

    // Clean up
    ok_map_deinit(&map);

    // Map with integer keys
    typedef struct ok_map_of(int, char *) int_map_t;

    int_map_t int_map;
    // On C11, you can use `ok_map_init(&map)` instead
    ok_map_init_custom(&int_map, ok_int32_hash, ok_32bit_equals);

    ok_map_put(&int_map, 10, "ten");
    ok_map_put(&int_map, 20, "twenty");
    ok_map_put(&int_map, 30, "thirty");

    printf("20 is `%s`.\n", ok_map_get(&int_map, 20));

    ok_map_deinit(&int_map);
}

// MARK: Example with custom key

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

static void custom_map_example() {
    typedef struct ok_map_of(point_t, char *) point_map_t;

    point_map_t map;
    ok_map_init_custom(&map, point_hash, point_equals);

    point_t key = {100.0, 200.0};
    ok_map_put(&map, key, "Buried treasure");

    printf("Value at (%f, %f): %s\n", (double)key.x, (double)key.y, ok_map_get(&map, key));

    ok_map_deinit(&map);
}

// MARK: Multimap example

typedef struct ok_vec_of(char *) str_vec_t;
typedef struct ok_map_of(const char *, str_vec_t) multimap_t;

static void multimap_add(multimap_t *map, const char *key, char *value) {
    str_vec_t *vec = ok_map_get_ptr(map, key);
    if (vec == NULL) {
        vec = ok_map_put_and_get_ptr(map, key);
        ok_vec_init(vec);
    }
    ok_vec_push(vec, value);
}

static void multimap_example() {
    multimap_t map;
    ok_map_init(&map);

    multimap_add(&map, "dave", "bananas");
    multimap_add(&map, "dave", "apples");
    multimap_add(&map, "dave", "oranges");
    multimap_add(&map, "mary", "grapes");
    multimap_add(&map, "mary", "strawberries");

    // Print
    ok_map_foreach(&map, const char *key, str_vec_t vec) {
        printf("%s likes: [ ", key);
        ok_vec_foreach(&vec, char *value) {
            printf("%s ", value);
        }
        printf("]\n");
    }

    // Cleanup
    ok_map_foreach(&map, const char *key, str_vec_t vec) {
        (void)key;
        ok_vec_deinit(&vec);
    }
    ok_map_deinit(&map);
}
