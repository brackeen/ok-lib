#include "ok_lib.h"
#include <assert.h>
#include <stdio.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wunused-value"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#endif
#ifdef WIN32
#define strcat(dest, src) strcat_s(dest, sizeof(dest), src)
#define strdup(str) _strdup(str)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

// MARK: Test framework

static int ok_test_total_count = 0;
static int ok_test_failure_count = 0;

#ifdef WIN32
#define OK_FILE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define OK_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define ok_assert(condition, message) do {\
    ok_test_total_count++; \
    if (!(condition)) { \
        ok_test_failure_count++; \
        printf("FAIL: %s:%i - function: %s - test: %s\n", OK_FILE, __LINE__, __func__, message);\
    } \
} while (0)

static void ok_tests_start(void) {
    ok_test_total_count = 0;
    ok_test_failure_count = 0;
}

static void ok_tests_finish(void) {
    printf("%s: %i of %i tests passed\n", OK_FILE, (ok_test_total_count - ok_test_failure_count),
           ok_test_total_count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// MARK: Common

static int counter = 0;

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

static ok_hash_t bad_hash(int v) {
    (void)v;
    // Don't do this! For testing only
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// MARK: Test vec

static void vec_int_ptr_func(int *value) {
    *value = counter++;
}

static void vec_int_func(int value) {
    counter += value;
}

static int vec_int_cmp(const void *v1, const void *v2) {
    const int a = *(const int *)v1;
    const int b = *(const int *)v2;

    return (a < b) ? -1 : (a > b);
}

static int vec_str_cmp(const void *a, const void *b) {
    const char *str1 = *(const char * const *)a;
    const char *str2 = *(const char * const *)b;

    return strcmp(str1, str2);
}

static void test_vec(void) {
    typedef struct ok_vec_of(int) vec_int_t;

    vec_int_t vec;
    ok_vec_init(&vec);
    ok_vec_push(&vec, 10);
    ok_vec_push(&vec, 10);
    ok_vec_push(&vec, 10);
    ok_assert(ok_vec_count(&vec) == 3, "ok_vec_push");

    ok_assert(ok_vec_get(&vec, 0) == 10 && ok_vec_get(&vec, 1) == 10 && ok_vec_get(&vec, 2) == 10,
              "ok_vec_get");

    *ok_vec_get_ptr(&vec, 1) = 20;
    *ok_vec_get_ptr(&vec, 2) = 30;

    ok_assert(ok_vec_get(&vec, 0) == 10 && ok_vec_get(&vec, 1) == 20 && ok_vec_get(&vec, 2) == 30,
              "ok_vec_get_ptr");

    ok_vec_remove_at(&vec, 1);
    ok_vec_insert_at(&vec, 1, 20);
    ok_assert(ok_vec_get(&vec, 0) == 10 && ok_vec_get(&vec, 1) == 20 && ok_vec_get(&vec, 2) == 30,
              "ok_vec_remove_at / ok_vec_insert_at");

    ok_vec_remove(&vec, 10);
    ok_vec_insert_at(&vec, 0, 10);
    ok_assert(ok_vec_get(&vec, 0) == 10 && ok_vec_get(&vec, 1) == 20 && ok_vec_get(&vec, 2) == 30,
              "ok_vec_remove / ok_vec_insert_at");

    ok_vec_remove_at(&vec, ok_vec_count(&vec) - 1);
    ok_vec_insert_at(&vec, ok_vec_count(&vec), 30);
    ok_assert(ok_vec_get(&vec, 0) == 10 && ok_vec_get(&vec, 1) == 20 && ok_vec_get(&vec, 2) == 30,
              "ok_vec_remove_at / ok_vec_insert_at end");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // ok_vec_foreach
    int sum = 0;
    ok_vec_foreach(&vec, int value) {
        sum += value;
    }
    ok_assert(sum == 60, "ok_vec_foreach");

    // ok_vec_foreach: No curly braces
    sum = 0;
    ok_vec_foreach(&vec, int value)
        sum += value;
    ok_assert(sum == 60, "ok_vec_foreach without curly braces");

    // ok_vec_foreach: Break
    int i = 0;
    sum = 0;
    ok_vec_foreach(&vec, int value) {
        sum += value;
        if (i == 1) {
            break;
        }
        i++;
    }
    ok_assert(sum == 30, "ok_vec_foreach with break");

    // ok_vec_foreach: Inner loop
    sum = 0;
    ok_vec_foreach(&vec, int value) {
        sum += value;
        ok_vec_foreach(&vec, int value2) {
            sum += value2;
        }
        sum -= 60;
    }
    ok_assert(sum == 60, "ok_vec_foreach with inner loop");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // ok_vec_foreach_rev
    sum = 0;
    ok_vec_foreach_rev(&vec, int value) {
        sum += value;
    }
    ok_assert(sum == 60, "ok_vec_foreach_rev");

    // ok_vec_foreach_rev: No curly braces
    sum = 0;
    ok_vec_foreach_rev(&vec, int value)
        sum += value;
    ok_assert(sum == 60, "ok_vec_foreach_rev without curly braces");

    // ok_vec_foreach_rev: Break
    i = 0;
    sum = 0;
    ok_vec_foreach_rev(&vec, int value) {
        sum += value;
        if (i == 1) {
            break;
        }
        i++;
    }
    ok_assert(sum == 50, "ok_vec_foreach_rev with break");

    // ok_vec_foreach_rev: Inner loop
    sum = 0;
    ok_vec_foreach_rev(&vec, int value) {
        sum += value;
        ok_vec_foreach_rev(&vec, int value2) {
            sum += value2;
        }
        sum -= 60;
    }
    ok_assert(sum == 60, "ok_vec_foreach_rev with inner loop");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // ok_vec_foreach_ptr
    sum = 0;
    ok_vec_foreach_ptr(&vec, int *value) {
        sum += *value;
    }
    ok_assert(sum == 60, "ok_vec_foreach_ptr");

    // ok_vec_foreach_ptr: No curly braces
    sum = 0;
    ok_vec_foreach_ptr(&vec, int *value)
        sum += *value;
    ok_assert(sum == 60, "ok_vec_foreach_ptr without curly braces");

    // ok_vec_foreach_ptr: Break
    i = 0;
    sum = 0;
    ok_vec_foreach_ptr(&vec, int *value) {
        sum += *value;
        if (i == 1) {
            break;
        }
        i++;
    }
    ok_assert(sum == 30, "ok_vec_foreach_ptr with break");

    // ok_vec_foreach_ptr: Inner loop
    sum = 0;
    ok_vec_foreach_ptr(&vec, int *value) {
        sum += *value;
        ok_vec_foreach_ptr(&vec, int *value2) {
            sum += *value2;
        }
        sum -= 60;
    }
    ok_assert(sum == 60, "ok_vec_foreach_ptr with inner loop");

    // ok_vec_foreach_ptr: set values
    i = 0;
    ok_vec_foreach_ptr(&vec, int *value) {
        *value = i;
        i += 10;
    }
    ok_assert(ok_vec_get(&vec, 0) == 0 && ok_vec_get(&vec, 1) == 10 && ok_vec_get(&vec, 2) == 20,
              "ok_vec_foreach_ptr: set values");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    *ok_vec_get_ptr(&vec, 0) = 10;
    *ok_vec_get_ptr(&vec, 1) = 20;
    *ok_vec_get_ptr(&vec, 2) = 30;

    // ok_vec_foreach_ptr_rev
    sum = 0;
    ok_vec_foreach_ptr_rev(&vec, int *value) {
        sum += *value;
    }
    ok_assert(sum == 60, "ok_vec_foreach_ptr_rev");

    // ok_vec_foreach_ptr_rev: No curly braces
    sum = 0;
    ok_vec_foreach_ptr_rev(&vec, int *value)
        sum += *value;
    ok_assert(sum == 60, "ok_vec_foreach_ptr_rev without curly braces");

    // ok_vec_foreach_ptr_rev: Break
    i = 0;
    sum = 0;
    ok_vec_foreach_ptr_rev(&vec, int *value) {
        sum += *value;
        if (i == 1) {
            break;
        }
        i++;
    }
    ok_assert(sum == 50, "ok_vec_foreach_ptr_rev with break");

    // ok_vec_foreach_ptr_rev: Inner loop
    sum = 0;
    ok_vec_foreach_ptr_rev(&vec, int *value) {
        sum += *value;
        ok_vec_foreach_ptr_rev(&vec, int *value2) {
            sum += *value2;
        }
        sum -= 60;
    }
    ok_assert(sum == 60, "ok_vec_foreach_ptr_rev with inner loop");

    // ok_vec_foreach_ptr_rev: set values
    i = 0;
    ok_vec_foreach_ptr_rev(&vec, int *value) {
        *value = i;
        i += 10;
    }
    ok_assert(ok_vec_get(&vec, 0) == 20 && ok_vec_get(&vec, 1) == 10 && ok_vec_get(&vec, 2) == 0,
              "ok_vec_foreach_ptr_rev: set values");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Apply ptr
    counter = 0;
    ok_vec_apply_ptr(&vec, vec_int_ptr_func);
    ok_assert(ok_vec_get(&vec, 0) == 0 && ok_vec_get(&vec, 1) == 1 && ok_vec_get(&vec, 2) == 2,
              "ok_vec_apply_ptr");

    // Apply
    counter = 0;
    ok_vec_apply(&vec, vec_int_func);
    ok_assert(counter == 3, "ok_vec_apply");

    // Sort
    *ok_vec_get_ptr(&vec, 0) = 2;
    *ok_vec_get_ptr(&vec, 1) = 0;
    *ok_vec_get_ptr(&vec, 2) = 1;
    ok_vec_sort(&vec, vec_int_cmp);
    ok_assert(ok_vec_get(&vec, 0) == 0 && ok_vec_get(&vec, 1) == 1 && ok_vec_get(&vec, 2) == 2,
              "ok_vec_sort");

    // ok_vec_clear, ok_vec_push_new, and capacity (reallocs)
    ok_vec_clear(&vec);
    ok_assert(ok_vec_count(&vec) == 0, "ok_vec_clear");
    const int count = 1000;
    for (i = 0; i < count; i++) {
        int *v = ok_vec_push_new(&vec);
        if (v) {
            *v = i;
        }
    }
    ok_assert(ok_vec_count(&vec) == (size_t)count, "ok_vec_push_new");

    // ok_vec_begin, ok_vec_end
    i = 0;
    sum = 0;
    int sum2 = 0;
    for (int *v = ok_vec_begin(&vec); v != ok_vec_end(&vec); v++) {
        sum2 += *v;
        sum += i;
        i++;
    }
    ok_assert(sum == sum2, "ok_vec_push_new / ok_vec_begin / ok_vec_end");

    // ok_vec_last
    ok_assert(count - 1 == *ok_vec_last(&vec), "ok_vec_last");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // ok_vec_push_all
    ok_vec_clear(&vec);
    vec_int_t vec2;
    ok_vec_init(&vec2);
    ok_vec_push(&vec2, 10);
    ok_vec_push(&vec2, 20);
    ok_vec_push(&vec2, 30);

    ok_vec_push_all(&vec, &vec2);
    ok_vec_deinit(&vec2);
    ok_assert(ok_vec_get(&vec, 0) == 10 && ok_vec_get(&vec, 1) == 20 && ok_vec_get(&vec, 2) == 30,
              "ok_vec_push_all");

    // Done
    ok_vec_deinit(&vec);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    struct vec_of_strs ok_vec_of(const char *);
    struct vec_of_strs str_vec = OK_VEC_INIT;

    ok_vec_push(&str_vec, "banana");
    ok_vec_push(&str_vec, "carrot");
    ok_vec_push(&str_vec, "apple");
    ok_vec_sort(&str_vec, vec_str_cmp);
    ok_assert(strcmp(ok_vec_get(&str_vec, 0), "apple") == 0 &&
              strcmp(ok_vec_get(&str_vec, 1), "banana") == 0 &&
              strcmp(ok_vec_get(&str_vec, 2), "carrot") == 0,
              "ok_vec_sort for strings");

    ok_vec_deinit(&str_vec);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    typedef struct ok_vec_of(point_t) vec_point_t;

    vec_point_t point_vec = OK_VEC_INIT;

    point_t p0 = {0, 1};
    ok_vec_push(&point_vec, p0);
    point_t p1 = {2, 3};
    ok_vec_push(&point_vec, p1);
    point_t p2 = {4, 5};
    ok_vec_push(&point_vec, p2);

    ok_assert(ok_vec_get(&point_vec, 0).x == p0.x && ok_vec_get(&point_vec, 0).y == p0.y &&
              ok_vec_get(&point_vec, 1).x == p1.x && ok_vec_get(&point_vec, 1).y == p1.y &&
              ok_vec_get(&point_vec, 2).x == p2.x && ok_vec_get(&point_vec, 2).y == p2.y,
              "ok_vec_push for custom type");

    ok_vec_deinit(&point_vec);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// MARK: Test map

static void test_map(void) {
    // str-to-str map

    struct str_map_s ok_map_of(const char *, const char *);
    struct str_map_s str_map;

    bool success = ok_map_init(&str_map);
    ok_assert(success, "ok_map_init");

    // Put / get
    ok_map_put(&str_map, "dave", "bananas");
    ok_map_put(&str_map, "mary", "carrots");
    ok_map_put(&str_map, "lucy", "apples");
    ok_assert(ok_map_count(&str_map) == 3, "ok_map_put / ok_map_count");
    ok_assert(strcmp(ok_map_get(&str_map, "dave"), "bananas") == 0 &&
              strcmp(ok_map_get(&str_map, "mary"), "carrots") == 0 &&
              strcmp(ok_map_get(&str_map, "lucy"), "apples") == 0,
              "ok_map_put / ok_map_get");

    // Put ptr / get ptr
    *ok_map_put_and_get_ptr(&str_map, "sue") = "beans";
    *ok_map_put_and_get_ptr(&str_map, "john") = "fries";
    ok_assert(ok_map_count(&str_map) == 5, "ok_map_put_and_get_ptr / ok_map_count");
    ok_assert(strcmp(*ok_map_get_ptr(&str_map, "sue"), "beans") == 0 &&
              strcmp(*ok_map_get_ptr(&str_map, "john"), "fries") == 0,
              "ok_map_put_and_get_ptr / ok_map_get");

    // Overwrite
    ok_map_put(&str_map, "john", "salad");
    ok_assert(ok_map_count(&str_map) == 5, "ok_map overwrite value / ok_map_count");
    ok_assert(strcmp(ok_map_get(&str_map, "john"), "salad") == 0,
              "ok_map overwrite value / ok_map_get");

    // Contains
    ok_map_put(&str_map, "cyrus", "(who knows)");
    ok_assert(ok_map_contains(&str_map, "cyrus"), "ok_map_contains");

    // Remove
    ok_map_remove(&str_map, "cyrus");
    ok_assert(ok_map_count(&str_map) == 5, "ok_map_remove");

    // Contains (false)
    ok_assert(ok_map_contains(&str_map, "cyrus") == false, "Contains (false)");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // ok_map_foreach

    char keys[1024] = {0};
    char values[1024] = {0};
    ok_map_foreach(&str_map, const char *key, const char *value) {
        strcat(keys, key);
        strcat(values, value);
    }
    ok_assert(strstr(keys, "dave") != NULL &&
              strstr(keys, "mary") != NULL &&
              strstr(keys, "lucy") != NULL &&
              strstr(keys, "sue") != NULL &&
              strstr(keys, "john") != NULL,
              "ok_map_foreach keys");
    ok_assert(strstr(values, "bananas") != NULL &&
              strstr(values, "carrots") != NULL &&
              strstr(values, "apples") != NULL &&
              strstr(values, "beans") != NULL &&
              strstr(values, "salad") != NULL,
              "ok_map_foreach values");

    // ok_map_foreach: No curly braces
    keys[0] = 0;
    const char *key;
    const char *value;
    ok_map_foreach(&str_map, key, value)
        strcat(keys, key);
    ok_assert(strstr(keys, "dave") != NULL &&
              strstr(keys, "mary") != NULL &&
              strstr(keys, "lucy") != NULL &&
              strstr(keys, "sue") != NULL &&
              strstr(keys, "john") != NULL,
              "ok_map_foreach keys / no curly braces");

    // ok_map_foreach: Break
    ok_map_foreach(&str_map, key, value) {
        if (strcmp(key, "dave")) {
            break;
        }
    }
    ok_assert(strcmp(ok_map_get(&str_map, "dave"), "bananas") == 0,
              "ok_map_foreach / break");

    // ok_map_foreach: Inner loop
    size_t count = 0;
    ok_map_foreach(&str_map, key, value) {
        ok_map_foreach(&str_map, const char *key2, const char *value2) {
            (void)value2;
            if (strcmp(key, key2) == 0) {
                count++;
            }
        }
    }
    ok_assert(count == ok_map_count(&str_map), "ok_map_foreach inner loop");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Copy

    struct str_map_s str_map2;
    ok_map_init(&str_map2);
    ok_map_put_all(&str_map2, &str_map);
    count = 0;
    ok_map_foreach(&str_map2, const char *key, const char *value) {
        if (strcmp(ok_map_get(&str_map, key), value) == 0) {
            count++;
        }
    }
    ok_assert(count == ok_map_count(&str_map) && count == ok_map_count(&str_map2), "ok_map_put_all");

    ok_map_deinit(&str_map2);
    ok_map_deinit(&str_map);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // str-to-int map

    struct str_int_map_s ok_map_of(const char *, int);
    struct str_int_map_s str_int_map;
    ok_map_init(&str_int_map);
    ok_map_put(&str_int_map, "The answer", 42);
    ok_assert(ok_map_get(&str_int_map, "The answer") == 42, "int values");
    ok_map_deinit(&str_int_map);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    //  int-to-str map

    struct int_map_s ok_map_of(int, char *);
    struct int_map_s int_map;

    // On C11, you can use `ok_map_init(&int_map)` instead
    ok_map_init_custom(&int_map, ok_int32_hash, ok_32bit_equals);
    for (int i = 0; i <= 9999; i++) {
        char *value = (char *)malloc(5);
        snprintf(value, 5, "%d", i);
        ok_map_put(&int_map, i, value);
    }
    count = 0;
    ok_map_foreach(&int_map, int key, char *value) {
        if (strtol(value, NULL, 10) == key) {
            count++;
        }
    }
    ok_assert(count == ok_map_count(&int_map), "int keys / realloc");

    // Free
    ok_map_foreach(&int_map, int key, char *value) {
        (void)key;
        free(value);
    }
    ok_map_deinit(&int_map);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Structs as values

    struct str_point_map_s ok_map_of(const char *, point_t);
    struct str_point_map_s str_point_map;
    ok_map_init(&str_point_map);

    point_t p;
    p.x = 100.0f;
    p.y = 200.0f;
    ok_map_put(&str_point_map, "player1", p);
    point_t p2 = ok_map_get(&str_point_map, "player1");
    ok_assert(point_equals(&p, &p2), "struct values");
    ok_map_deinit(&str_point_map);

    // Structs as keys

    struct point_map_s ok_map_of(point_t, int);
    struct point_map_s point_map;
    ok_map_init_custom(&point_map, point_hash, point_equals);
    for (int x = 0; x <= 9; x++) {
        for (int y = 0; y <= 9; y++) {
            point_t p;
            p.x = (float)x;
            p.y = (float)y;
            ok_map_put(&point_map, p, x + 10 * y);
        }
    }
    count = 0;
    ok_map_foreach(&point_map, point_t key, int value) {
        if (key.x + 10 * key.y == value) {
            count++;
        }
    }
    ok_assert(count == ok_map_count(&point_map), "struct keys / realloc");
    ok_map_deinit(&point_map);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Pointers as keys

    const void *key1 = "hello";
    const void *key2 = NULL;

    struct void_map_s ok_map_of(const void *, int);
    struct void_map_s void_map;
    ok_map_init_custom(&void_map, ok_const_ptr_hash, ok_ptr_equals);
    ok_map_put(&void_map, key1, 1);
    ok_map_put(&void_map, key2, 2);
    ok_assert(ok_map_get(&void_map, key1) == 1, "pointer keys");
    ok_assert(ok_map_get(&void_map, key2) == 2, "pointer keys");

    ok_map_deinit(&void_map);

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // Bad hash function

    struct int_map_s bad_map;
    ok_map_init_custom(&bad_map, bad_hash, ok_32bit_equals);
    for (int i = 0; i <= 9999; i++) {
        char *value = (char *)malloc(5);
        snprintf(value, 5, "%d", i);
        ok_map_put(&bad_map, i, value);
    }
    count = 0;
    ok_map_foreach(&bad_map, int key, char *value) {
        if (strtol(value, NULL, 10) == key) {
            count++;
        }
    }
    ok_assert(count == ok_map_count(&bad_map), "bad hash function");

    // Free
    ok_map_foreach(&bad_map, int key, char *value) {
        (void)key;
        free(value);
    }
    ok_map_deinit(&bad_map);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// MARK: Test queue

#if !defined(__EMSCRIPTEN__)

static void str_deallocator(void *value_ptr) {
    char *str = *(char **)value_ptr;
    //printf("Deallocating: %s\n", str);
    free(str);
}

static void test_queue(void) {
    typedef struct ok_queue_of(int) queue_int_t;

    int count = 100;
    bool success = false;

    queue_int_t queue;
    ok_queue_init(&queue);
    int int_value = 0;
    for (int i = 0; i < count; i++) {
        ok_queue_push(&queue, i);
    }
    for (int i = 0; i < count; i++) {
        success = ok_queue_pop(&queue, &int_value);
        if (!success || int_value != i) {
            success = false;
            break;
        }
    }
    ok_assert(success, "int queue error");
    ok_assert(ok_queue_pop(&queue, &int_value) == false, "int queue not empty");

    success = false;
    for (int i = 0; i < count; i++) {
        ok_queue_push(&queue, i);
        success = ok_queue_pop(&queue, &int_value);
        if (!success || int_value != i) {
            success = false;
            break;
        }
    }
    ok_assert(success, "int queue error (size 1)");
    ok_assert(ok_queue_pop(&queue, &int_value) == false, "int queue (size 1) not empty");
    ok_queue_deinit(&queue);

    struct ok_queue_of(char *) str_queue = OK_QUEUE_INIT;
    char *name = strdup("dave");
    ok_queue_push(&str_queue, name);
    name = strdup("mary");
    ok_queue_push(&str_queue, name);
    name = strdup("lucy");
    ok_queue_push(&str_queue, name);
    ok_queue_deinit_with_deallocator(&str_queue, str_deallocator);
    ok_assert(true, "queue deallocate test");

    typedef struct ok_queue_of(point_t) queue_point_t;
    queue_point_t point_queue;

    point_t point_array[3] = {{1, 2}, {3, 4}, {5, 6}};
    size_t num_points = sizeof(point_array) / sizeof(*point_array);

    success = false;
    ok_queue_init_with_capacity(&point_queue, 4);
    for (size_t i = 0; i < num_points; i++) {
        ok_queue_push(&point_queue, point_array[i]);
    }
    for (size_t i = 0; i < num_points; i++) {
        point_t point = {0, 0};
        success = ok_queue_pop(&point_queue, &point);
        if (!success || !point_equals(&point, &point_array[i])) {
            success = false;
            break;
        }
    }
    ok_assert(success, "point queue error");
    ok_queue_deinit(&point_queue);
}

#else

static void test_queue(void) {
    // Emscripten: Do nothing
}

#endif // __EMSCRIPTEN__

int main(void) {
    //ok_static_assert(2 + 2 == 5, "2+2 is not 5");
    ok_static_assert(true, "Identifier `true` must be true");

    ok_tests_start();

    test_vec();
    test_map();
    test_queue();

    ok_tests_finish();

    return ok_test_failure_count;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
