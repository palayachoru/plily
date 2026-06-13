#pragma once

/**
 * @file llist.h
 * @brief Linked List (Single Linked List)
 *
 * node contains => data + next_ptr
 * size is used to track no of nodes
 * tail node is tracked to have O(1) time for append operation
 */


#include <stdbool.h>
#include <stddef.h>

#include "util.h"


/// Representation of a Single Linked List struct
typedef struct PLLinkedList {
  /// Get the value of a given index
  element_t* (* get)(struct PLLinkedList *self, int index);

  /// Append the value at the end of the linked list
  bool (* append)(struct PLLinkedList *self, etype_t etype, void *value);

  /// Insert the value at the given index
  bool (* insert)(struct PLLinkedList *self, int index, etype_t etype, void *value);

  /// Pop the last element from the linked list
  element_t (* pop)(struct PLLinkedList *self);

  /// Remove the first occurrence of the value
  bool (* remove)(struct PLLinkedList *self, etype_t etype, void *value);

  /// Remove the value at the given index
  bool (* remove_at)(struct PLLinkedList *self, int index);

  /// Replace the value at the given index
  bool (* replace)(struct PLLinkedList *self, int index, etype_t etype, void *value);

  /// Get index of first occurrence of the value
  int (* index)(struct PLLinkedList *self, etype_t etype, void *value);

  /// Reverse the linked List
  void (* reverse)(struct PLLinkedList *self);

  /// Get the size of the linked List
  size_t (* length)(struct PLLinkedList *self);

  /// Check if the list is empty
  bool (* is_empty)(struct PLLinkedList *self);

  node_t *head;      ///< Reference to head node of linked list
  node_t *tail;      ///< Reference to tail node of linked list
  size_t size;       ///< No of nodes in linked list
} PLLinkedList;



// ========================
// Function Declarations
// ========================
/**
 * @brief Allocate memory for PLLinkedList struct and initialize with values
 * @return PLLinkedList - reference to newly created PLLinkedList struct
 */
PLLinkedList* pllinkedlist_init(void);

/**
 * @breif Free the List along will all the element in it
 * @param **self - reference of reference to PLList
 */
void pllinkedlist_free(PLLinkedList **self);
