#include "ok_lib.h"
#include <stdio.h>

static int ok_str_cmp(const void *a, const void *b) {
    const char *str1 = *(const char * const *)a;
    const char *str2 = *(const char * const *)b;

    return strcmp(str1, str2);
}

static void vec_example() {
    typedef struct ok_vec_of(char *) str_vec_t;

    str_vec_t vec;
    ok_vec_init(&vec);

    ok_vec_push(&vec, "steve");
    ok_vec_push(&vec, "dave");
    ok_vec_push(&vec, "mary");

    printf("A person: %s\n", ok_vec_get(&vec, 2));

    ok_vec_sort(&vec, ok_str_cmp);

    ok_vec_foreach(&vec, char *value) {
        printf("Sorted Name: %s\n", value);
    }

    ok_vec_deinit(&vec);
}
