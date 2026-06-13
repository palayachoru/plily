// tests/list_tests.c
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"   // adjust path if needed
#include "util.h"   // for etype_t, element_t

static PLList *lst = NULL;

/* Helper to free an element returned by pop (mirrors free_element behavior) */
static void free_popped_element(element_t *e) {
    if (!e) return;
    if (e->etype == STR) free(e->value.sval);
    free(e);
}

/* Setup / Teardown */
static int init_suite(void) {
    lst = pllist_init();
    return (lst ? 0 : 1);
}
static int clean_suite(void) {
    pllist_free(&lst);
    return 0;
}


/* Tests for pllist_init and basic properties */
static void test_init_and_free(void) {
    CU_ASSERT_PTR_NOT_NULL(lst);
    CU_ASSERT_PTR_NOT_NULL(lst->arr);
    CU_ASSERT_EQUAL(lst->capacity, INITIAL_CAPACITY);
    CU_ASSERT_EQUAL(lst->size, 0);
    CU_ASSERT_TRUE(lst->is_empty(lst));
}

/* Append: normal, null value, and resizing */
static void test_append_and_resize(void) {
    int a = 10;
    double d = 3.14;
    char *s = "hello";

    CU_ASSERT_TRUE(lst->append(lst, INT, &a));
    CU_ASSERT_TRUE(lst->append(lst, DOUBLE, &d));
    CU_ASSERT_TRUE(lst->append(lst, STR, s));

    CU_ASSERT_EQUAL(lst->size, 3);
    element_t *e0 = lst->get(lst, 0);
    CU_ASSERT_PTR_NOT_NULL(e0);
    CU_ASSERT_EQUAL(e0->etype, INT);
    CU_ASSERT_EQUAL(e0->value.ival, a);

    element_t *e1 = lst->get(lst, 1);
    CU_ASSERT_EQUAL(e1->etype, DOUBLE);
    CU_ASSERT_DOUBLE_EQUAL(e1->value.dval, d, 1e-9);

    element_t *e2 = lst->get(lst, 2);
    CU_ASSERT_EQUAL(e2->etype, STR);
    CU_ASSERT_STRING_EQUAL(e2->value.sval, s);

    /* Append NULL value should fail */
    CU_ASSERT_FALSE(lst->append(lst, INT, NULL));

    /* Force many appends to trigger resize */
    int base = 100;
    size_t initial_capacity = lst->capacity;
    for (int i = 0; i < 50; ++i) {
        int *pv = malloc(sizeof(int));
        CU_ASSERT_PTR_NOT_NULL_FATAL(pv);
        *pv = base + i;
        CU_ASSERT_TRUE(lst->append(lst, INT, pv));
        free(pv); /* value copied into element, safe to free */
    }
    CU_ASSERT_TRUE(lst->capacity >= initial_capacity);
}

/* get: valid and invalid indexes */
static void test_get_edge_cases(void) {
    /* invalid indexes */
    CU_ASSERT_PTR_NULL(lst->get(lst, -1));
    CU_ASSERT_PTR_NULL(lst->get(lst, (int)lst->size)); /* out of bounds */
}

/* insert: middle, at end, beyond end, negative index, NULL value */
static void test_insert_various(void) {
    int x = 42;

    /* insert at middle (index 1) */
    CU_ASSERT_TRUE(lst->insert(lst, 1, INT, &x));
    CU_ASSERT_EQUAL(lst->get(lst, 1)->value.ival, x);

    /* insert at index == size should append */
    int y = 99;
    size_t s_before = lst->size;
    CU_ASSERT_TRUE(lst->insert(lst, (int)s_before, INT, &y));
    CU_ASSERT_EQUAL(lst->get(lst, (int)s_before)->value.ival, y);

    /* insert at index > size should append (per implementation) */
    int z = 123;
    size_t s_now = lst->size;
    CU_ASSERT_TRUE(lst->insert(lst, (int)(s_now + 10), INT, &z));
    CU_ASSERT_EQUAL(lst->get(lst, (int)(lst->size - 1))->value.ival, z);

    /* negative index should fail */
    CU_ASSERT_FALSE(lst->insert(lst, -5, INT, &x));

    /* NULL value should fail */
    CU_ASSERT_FALSE(lst->insert(lst, 0, INT, NULL));
}

/* pop: normal and empty-list pop */
static void test_pop(void) {
    /* ensure there is at least one element */
    int v = 7;
    CU_ASSERT_TRUE(lst->append(lst, INT, &v));
    element_t *popped = lst->pop(lst);
    CU_ASSERT_PTR_NOT_NULL(popped);
    CU_ASSERT_EQUAL(popped->etype, INT);
    CU_ASSERT_EQUAL(popped->value.ival, v);
    free_popped_element(popped);
    /* pop until empty */
    while (!lst->is_empty(lst)) {
        element_t *e = lst->pop(lst);
        CU_ASSERT_PTR_NOT_NULL(e);
        free_popped_element(e);
    }
    CU_ASSERT_TRUE(lst->is_empty(lst));
    CU_ASSERT_PTR_NULL(lst->pop(lst)); /* popping empty returns NULL */
}

/* index and remove (value-based) for INT, DOUBLE, STR */
static void test_index_and_remove(void) {
    /* prepare */
    int ai = 5;
    double ad = 2.5;
    char *as = "abc";

    CU_ASSERT_TRUE(lst->append(lst, INT, &ai));
    CU_ASSERT_TRUE(lst->append(lst, DOUBLE, &ad));
    CU_ASSERT_TRUE(lst->append(lst, STR, as));
    size_t start_size = lst->size;

    /* index finds first occurrence */
    int idx_int = lst->index(lst, INT, &ai);
    CU_ASSERT_TRUE(idx_int >= 0);
    CU_ASSERT_EQUAL(lst->get(lst, idx_int)->value.ival, ai);

    int idx_double = lst->index(lst, DOUBLE, &ad);
    CU_ASSERT_TRUE(idx_double >= 0);
    CU_ASSERT_DOUBLE_EQUAL(lst->get(lst, idx_double)->value.dval, ad, 1e-9);

    int idx_str = lst->index(lst, STR, as);
    CU_ASSERT_TRUE(idx_str >= 0);
    CU_ASSERT_STRING_EQUAL(lst->get(lst, idx_str)->value.sval, as);

    /* remove by value */
    CU_ASSERT_TRUE(lst->remove(lst, INT, &ai));
    CU_ASSERT_EQUAL(lst->size, start_size - 1);
    /* removing non-existing returns false */
    int not_found = 9999;
    CU_ASSERT_FALSE(lst->remove(lst, INT, &not_found));

    /* remove by string (if still present) */
    lst->remove(lst, STR, as);
}

/* remove_at: valid, invalid indexes, check shifting */
static void test_remove_at(void) {
    /* create known sequence */
    pllist_free(&lst);
    lst = pllist_init();
    int aa = 1, b = 2, c = 3;
    CU_ASSERT_TRUE(lst->append(lst, INT, &aa));
    CU_ASSERT_TRUE(lst->append(lst, INT, &b));
    CU_ASSERT_TRUE(lst->append(lst, INT, &c));
    CU_ASSERT_EQUAL(lst->size, 3);

    /* remove middle */
    CU_ASSERT_TRUE(lst->remove_at(lst, 1));
    CU_ASSERT_EQUAL(lst->size, 2);
    CU_ASSERT_EQUAL(lst->get(lst, 0)->value.ival, aa);
    CU_ASSERT_EQUAL(lst->get(lst, 1)->value.ival, c);

    /* invalid remove_at */
    CU_ASSERT_FALSE(lst->remove_at(lst, -1));
    CU_ASSERT_FALSE(lst->remove_at(lst, 100));
}

/* replace: change types and string memory handling */
static void test_replace(void) {
    pllist_free(&lst);
    lst = pllist_init();

    int ai = 10;
    CU_ASSERT_TRUE(lst->append(lst, INT, &ai));
    CU_ASSERT_TRUE(lst->replace(lst, 0, INT, &ai));
    CU_ASSERT_EQUAL(lst->get(lst, 0)->value.ival, ai);

    /* replace int with double */
    double d = 7.25;
    CU_ASSERT_TRUE(lst->replace(lst, 0, DOUBLE, &d));
    CU_ASSERT_EQUAL(lst->get(lst, 0)->etype, DOUBLE);
    CU_ASSERT_DOUBLE_EQUAL(lst->get(lst, 0)->value.dval, d, 1e-9);

    /* replace double with string */
    char *s = "replaced";
    CU_ASSERT_TRUE(lst->replace(lst, 0, STR, s));
    CU_ASSERT_EQUAL(lst->get(lst, 0)->etype, STR);
    CU_ASSERT_STRING_EQUAL(lst->get(lst, 0)->value.sval, s);

    /* replace out-of-range */
    CU_ASSERT_FALSE(lst->replace(lst, 10, INT, &ai));
    /* replace with NULL value */
    CU_ASSERT_FALSE(lst->replace(lst, 0, INT, NULL));
}

/* reverse: empty, single, multiple */
static void test_reverse(void) {
    pllist_free(&lst);
    lst = pllist_init();

    /* empty and single */
    lst->reverse(lst);
    int s1 = 11;
    CU_ASSERT_TRUE(lst->append(lst, INT, &s1));
    lst->reverse(lst);
    CU_ASSERT_EQUAL(lst->get(lst, 0)->value.ival, s1);

    /* multiple */
    int b = 2, c = 3, d = 4;
    CU_ASSERT_TRUE(lst->append(lst, INT, &b));
    CU_ASSERT_TRUE(lst->append(lst, INT, &c));
    CU_ASSERT_TRUE(lst->append(lst, INT, &d));
    lst->reverse(lst);
    /* after reverse, order should be d,c,b,1 */
    CU_ASSERT_EQUAL(lst->get(lst, 0)->value.ival, d);
    CU_ASSERT_EQUAL(lst->get(lst, 1)->value.ival, c);
    CU_ASSERT_EQUAL(lst->get(lst, 2)->value.ival, b);
    CU_ASSERT_EQUAL(lst->get(lst, 3)->value.ival, s1);
}

/* length and is_empty */
static void test_length_and_is_empty(void) {
    pllist_free(&lst);
    lst = pllist_init();
    CU_ASSERT_TRUE(lst->is_empty(lst));
    CU_ASSERT_EQUAL(lst->length(lst), 0);

    int v = 2;
    CU_ASSERT_TRUE(lst->append(lst, INT, &v));
    CU_ASSERT_FALSE(lst->is_empty(lst));
    CU_ASSERT_EQUAL(lst->length(lst), 1);
}

/* Suite registration */
int main(void) {
    CU_pSuite pSuite = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return (int)CU_get_error();

    pSuite = CU_add_suite("pllist_suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return (int)CU_get_error();
    }

    CU_add_test(pSuite, "init_and_free", test_init_and_free);
    CU_add_test(pSuite, "append_and_resize", test_append_and_resize);
    CU_add_test(pSuite, "get_edge_cases", test_get_edge_cases);
    CU_add_test(pSuite, "insert_various", test_insert_various);
    CU_add_test(pSuite, "pop", test_pop);
    CU_add_test(pSuite, "index_and_remove", test_index_and_remove);
    CU_add_test(pSuite, "remove_at", test_remove_at);
    CU_add_test(pSuite, "replace", test_replace);
    CU_add_test(pSuite, "reverse", test_reverse);
    CU_add_test(pSuite, "length_and_is_empty", test_length_and_is_empty);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return (int)CU_get_error();
}
