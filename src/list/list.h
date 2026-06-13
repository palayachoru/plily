#pragma once

/**
 * @file list.h
 * @brief List (Dynamic Array)
 *
 * This implements a dynamic array that can hold heterogeneous data types. When
 * the array reaches capacity, its size automatically doubles to accommodate
 * additional elements.
 *
 * Storage model:
 *    elements are stored in contiguous memory; capacity and length are tracked separately.
 * Heterogeneous support:
 *    elements are stored as generic values (e.g., pointers, variants, or a union),
 *    allowing multiple data types in the same array.
 * Resizing strategy:
 *    if length == capacity, allocate a new buffer with capacity = max(1, capacity * 2),
 *    copy existing elements, free the old buffer, and continue; doubling gives
 *    amortized O(1) append.
 */

#include <stdbool.h>
#include <stddef.h>

#include "util.h"

#define INITIAL_CAPACITY  5    // Initial capacity of the List


/// Representation of a List (Dynamic Array) struct
typedef struct PLList {
  /// Get the value a the given index
  element_t* (* get)(struct PLList *self, int index);

  /// Append the value at the end of the list
  bool (* append)(struct PLList *self, etype_t etype, void *value);

  /// Insert the value at the given index
  bool (* insert)(struct PLList *self, int index, etype_t etype, void *value);

  /// Pop the last element from the list
  element_t* (* pop)(struct PLList *self);

  /// Remove the first occurrence of the value
  bool (* remove)(struct PLList *self, etype_t etype, void *value);

  /// Remove the value at the given index
  bool (* remove_at)(struct PLList *self, int index);

  /// Replace the value at the given index
  bool (* replace)(struct PLList *self, int index, etype_t etype, void *value);

  /// Get index of first occurrence of the value
  int (* index)(struct PLList *self, etype_t etype, void *value);

  /// Reverse the List
  void (* reverse)(struct PLList *self);

  /// Get the size of the List
  size_t (* length)(struct PLList *self);

  /// Check if the list is empty
  bool (* is_empty)(struct PLList *self);

  element_t **arr;   ///< Array to hold pointer to element_t struct
  size_t capacity;   ///< No of elements the list can hold (updates while resizes)
  size_t size;       ///< No of elements in list
} PLList;



// ========================
// Function Declarations
// ========================
/**
 * @brief Allocate memory for PLList struct and initialize with values
 * @return PLList - reference to newly created PLList struct
 */
PLList* pllist_init(void);

/**
 * @breif Free the List along will all the element in it
 * @param **self - reference of reference to PLList
 */
void pllist_free(PLList **self);
