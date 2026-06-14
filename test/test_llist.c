#include <stdbool.h>
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "llist.h"
#include "util.h"

// Global pointer used by the test suite fixture
static PLLinkedList *list = NULL;

// Helper variables for data insertion
static int val_10 = 10;
static int val_20 = 20;
static int val_30 = 30;

// =========================================================================
// TEST FIXTURE GENERATORS (SETUP / TEARDOWN)
// =========================================================================

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void setup_empty_list(void) {
    if (list) {
        pllinkedlist_free(&list);
    }
    list = pllinkedlist_init();
    CU_ASSERT_PTR_NOT_NULL_FATAL(list);
}

void teardown_list(void) {
    if (list) {
        pllinkedlist_free(&list);
    }
}

// =========================================================================
// INDIVIDUAL FUNCTION TEST CASES
// =========================================================================

void test_init_and_free(void) {
    PLLinkedList *test_list = pllinkedlist_init();
    CU_ASSERT_PTR_NOT_NULL(test_list);
    CU_ASSERT_PTR_NULL(test_list->head);
    CU_ASSERT_PTR_NULL(test_list->tail);
    CU_ASSERT_EQUAL(test_list->length(test_list), 0);
    CU_ASSERT_TRUE(test_list->is_empty(test_list));

    pllinkedlist_free(&test_list);
    CU_ASSERT_PTR_NULL(test_list);
}

void test_append_operations(void) {
    setup_empty_list();

    // Test append to empty list
    CU_ASSERT_TRUE(list->append(list, INT, &val_10));
    CU_ASSERT_EQUAL(list->length(list), 1);
    CU_ASSERT_FALSE(list->is_empty(list));
    CU_ASSERT_PTR_NOT_NULL(list->head);
    CU_ASSERT_PTR_EQUAL(list->head, list->tail);

    // Test second append
    CU_ASSERT_TRUE(list->append(list, INT, &val_20));
    CU_ASSERT_EQUAL(list->length(list), 2);
    CU_ASSERT_PTR_NOT_EQUAL(list->head, list->tail);

    teardown_list();
}

void test_get_operations(void) {
    setup_empty_list();

    list->append(list, INT, &val_10);
    list->append(list, INT, &val_20);

    // Test valid boundaries
    element_t el0 = list->get(list, 0);
    element_t el1 = list->get(list, 1);

    CU_ASSERT_EQUAL(el0.value.ival, val_10);
    CU_ASSERT_EQUAL(el1.value.ival, val_20);

    // Test out of bounds indexing
    element_t invalid_el = list->get(list, 5);
    CU_ASSERT_EQUAL(invalid_el.value.ival, 0);

    element_t negative_el = list->get(list, -1);
    CU_ASSERT_EQUAL(negative_el.value.ival, 0);

    teardown_list();
}

void test_insert_operations(void) {
    setup_empty_list();

    // Insert at index 0 on empty list (should fall back to append)
    CU_ASSERT_TRUE(list->insert(list, 0, INT, &val_10));
    CU_ASSERT_EQUAL(list->length(list), 1);

    // Insert at head (index 0) when elements exist
    CU_ASSERT_TRUE(list->insert(list, 0, INT, &val_20));
    CU_ASSERT_EQUAL(list->length(list), 2);
    CU_ASSERT_EQUAL(list->get(list, 0).value.ival, val_20);

    // Insert at arbitrary middle/end index
    CU_ASSERT_TRUE(list->insert(list, 1, INT, &val_30));
    CU_ASSERT_EQUAL(list->length(list), 3);
    CU_ASSERT_EQUAL(list->get(list, 1).value.ival, val_30);

    // Invalid insertion index bounds
    CU_ASSERT_FALSE(list->insert(list, 10, INT, &val_10));

    teardown_list();
}

void test_pop_operations(void) {
    setup_empty_list();

    // Pop empty list
    element_t empty_pop = list->pop(list);
    CU_ASSERT_EQUAL(empty_pop.value.ival, 0);

    // Populate list
    list->append(list, INT, &val_10);
    list->append(list, INT, &val_20);

    // Pop last element
    element_t popped_1 = list->pop(list);
    CU_ASSERT_EQUAL(popped_1.value.ival, val_20);
    CU_ASSERT_EQUAL(list->length(list), 1);

    // Pop final remaining element (head == tail case)
    element_t popped_2 = list->pop(list);
    CU_ASSERT_EQUAL(popped_2.value.ival, val_10);
    CU_ASSERT_EQUAL(list->length(list), 0);
    CU_ASSERT_PTR_NULL(list->head);
    CU_ASSERT_PTR_NULL(list->tail);

    teardown_list();
}

void test_remove_operations(void) {
    setup_empty_list();

    list->append(list, INT, &val_10);
    list->append(list, INT, &val_20);
    list->append(list, INT, &val_30);

    // Remove non-existent target
    int missing_val = 99;
    CU_ASSERT_FALSE(list->remove(list, INT, &missing_val));

    // Remove from the middle target
    CU_ASSERT_TRUE(list->remove(list, INT, &val_20));
    CU_ASSERT_EQUAL(list->length(list), 2);

    // Remove from head
    CU_ASSERT_TRUE(list->remove(list, INT, &val_10));
    CU_ASSERT_EQUAL(list->head->data.value.ival, val_30);

    teardown_list();
}

void test_remove_at_operations(void) {
    setup_empty_list();

    // Edge case: Remove from empty list
    CU_ASSERT_FALSE(list->remove_at(list, 0));

    // Populate list
    list->append(list, INT, &val_10);
    list->append(list, INT, &val_20);
    list->append(list, INT, &val_30);

    // Edge case: Out of bounds indices
    CU_ASSERT_FALSE(list->remove_at(list, -1));
    CU_ASSERT_FALSE(list->remove_at(list, 3));

    // Test case: Remove middle node (index 1 -> val_20)
    CU_ASSERT_TRUE(list->remove_at(list, 1));
    CU_ASSERT_EQUAL(list->length(list), 2);
    CU_ASSERT_EQUAL(list->get(list, 1).value.ival, val_30);

    // Test case: Remove head node (index 0 -> val_10)
    CU_ASSERT_TRUE(list->remove_at(list, 0));
    CU_ASSERT_PTR_EQUAL(list->head, list->tail); // Only 1 element left

    // Test case: Remove last remaining node (index 0 -> val_30)
    CU_ASSERT_TRUE(list->remove_at(list, 0));
    CU_ASSERT_TRUE(list->is_empty(list));
    CU_ASSERT_PTR_NULL(list->head);
    CU_ASSERT_PTR_NULL(list->tail);

    teardown_list();
}

void test_replace_operations(void) {
    setup_empty_list();

    // Edge case: Replace on empty list
    CU_ASSERT_FALSE(list->replace(list, 0, INT, &val_10));

    // Populate list
    list->append(list, INT, &val_10);
    list->append(list, INT, &val_20);

    // Edge case: Index out of bounds
    CU_ASSERT_FALSE(list->replace(list, 5, INT, &val_30));
    CU_ASSERT_FALSE(list->replace(list, -1, INT, &val_30));

    // Test case: Valid type replacements
    int replacement_int = 99;
    CU_ASSERT_TRUE(list->replace(list, 0, INT, &replacement_int));

    element_t el = list->get(list, 0);
    CU_ASSERT_EQUAL(el.value.ival, 99);

    // Edge case: Replace with an invalid/unsupported etype enum boundary
    int fallback_type = 999;
    CU_ASSERT_FALSE(list->replace(list, 1, (etype_t)fallback_type, &val_30));

    teardown_list();
}

void test_index_operations(void) {
    setup_empty_list();

    // Edge case: Look up on empty list
    CU_ASSERT_EQUAL(list->index(list, INT, &val_10), -1);

    // Populate list
    list->append(list, INT, &val_10);
    list->append(list, INT, &val_20);
    list->append(list, INT, &val_30);

    // Test case: Locate Head, Middle, and Tail instances
    CU_ASSERT_EQUAL(list->index(list, INT, &val_10), 0);
    CU_ASSERT_EQUAL(list->index(list, INT, &val_20), 1);
    CU_ASSERT_EQUAL(list->index(list, INT, &val_30), 2);

    // Edge case: Locate item not present in the list
    int missing_target = 404;
    CU_ASSERT_EQUAL(list->index(list, INT, &missing_target), -1);

    // Edge case: NULL value lookup protection
    CU_ASSERT_EQUAL(list->index(list, INT, NULL), -1);

    teardown_list();
}

void test_reverse_operations(void) {
    setup_empty_list();

    // Edge case: Reverse empty list
    list->reverse(list);
    CU_ASSERT_TRUE(list->is_empty(list));

    // Edge case: Reverse single item list
    list->append(list, INT, &val_10);
    list->reverse(list);
    CU_ASSERT_EQUAL(list->length(list), 1);
    CU_ASSERT_PTR_EQUAL(list->head, list->tail);

    // Test case: Reverse multi-node structural layout
    list->append(list, INT, &val_20);
    list->append(list, INT, &val_30);

    node_t *expected_new_tail = list->head;
    node_t *expected_new_head = list->tail;

    list->reverse(list);

    CU_ASSERT_PTR_EQUAL(list->head, expected_new_head);
    CU_ASSERT_PTR_EQUAL(list->tail, expected_new_tail);
    CU_ASSERT_EQUAL(list->get(list, 0).value.ival, val_30);
    CU_ASSERT_EQUAL(list->get(list, 2).value.ival, val_10);

    teardown_list();
}

void test_length_operations(void) {
    setup_empty_list();

    // Initial boundary
    CU_ASSERT_EQUAL(list->length(list), 0);

    // Increment validations
    list->append(list, INT, &val_10);
    CU_ASSERT_EQUAL(list->length(list), 1);

    list->append(list, INT, &val_20);
    CU_ASSERT_EQUAL(list->length(list), 2);

    // Decrement validations
    list->pop(list);
    CU_ASSERT_EQUAL(list->length(list), 1);

    // Edge case: Passing a NULL pointer layout context
    CU_ASSERT_EQUAL(list->length(NULL), 0);

    teardown_list();
}

void test_is_empty_operations(void) {
    setup_empty_list();

    // Empty state assertion
    CU_ASSERT_TRUE(list->is_empty(list));

    // Non-empty state assertion
    list->append(list, INT, &val_10);
    CU_ASSERT_FALSE(list->is_empty(list));

    // Back to empty state assertion
    list->pop(list);
    CU_ASSERT_TRUE(list->is_empty(list));

    teardown_list();
}

// =========================================================================
// SUITE REGISTRATION AND EXECUTIVE RUNNER
// =========================================================================

int main(void) {
    // 1. Initialize CUnit registry
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return (int)CU_get_error();
    }

    // 2. Add Test Suite context framework instance
    CU_pSuite pSuite = CU_add_suite("PLLinkedList_Comprehensive_Suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return (int)CU_get_error();
    }

    // 3. Register all test blocks sequentially
    if ((NULL == CU_add_test(pSuite, "test_init_and_free", test_init_and_free)) ||
        (NULL == CU_add_test(pSuite, "test_append_operations", test_append_operations)) ||
        (NULL == CU_add_test(pSuite, "test_get_operations", test_get_operations)) ||
        (NULL == CU_add_test(pSuite, "test_insert_operations", test_insert_operations)) ||
        (NULL == CU_add_test(pSuite, "test_pop_operations", test_pop_operations)) ||
        (NULL == CU_add_test(pSuite, "test_remove_operations", test_remove_operations)) ||
        (NULL == CU_add_test(pSuite, "test_remove_at_operations", test_remove_at_operations)) ||
        (NULL == CU_add_test(pSuite, "test_replace_operations", test_replace_operations)) ||
        (NULL == CU_add_test(pSuite, "test_index_operations", test_index_operations)) ||
        (NULL == CU_add_test(pSuite, "test_reverse_operations", test_reverse_operations)) ||
        (NULL == CU_add_test(pSuite, "test_length_operations", test_length_operations)) ||
        (NULL == CU_add_test(pSuite, "test_is_empty_operations", test_is_empty_operations)) ) {
          CU_cleanup_registry();
          return (int)CU_get_error();
      }

    // 4. Run tests with basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Get status reports on explicit asset assertion failures
    unsigned int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures > 0 ? 1 : 0;
}
