#define _POSIX_C_SOURCE 200809L   // Enables declarations for POSIX functions and symbols

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "list.h"
#include "util.h"

// ================================================
// Forward declarations (what this module provides)
// ================================================
static element_t* get(PLList *self, int index);

static bool append(PLList *self, etype_t etype, void *value);

static bool insert(PLList *self, int index, etype_t etype, void *value);

static element_t* pop(PLList *self);

static bool pll_remove(PLList *self, etype_t etype, void *value);

static bool remove_at(PLList *self, int index);

static bool replace(PLList *self, int index, etype_t etype, void *value);

static int pll_index(PLList *self, etype_t etype, void *value);

static void reverse(PLList *self);

static size_t length(PLList *self);

static bool is_empty(PLList *self);

// UTILITY FUNCTION
static bool resize(PLList *self);



// ============================================
// Public API
// ============================================
PLList* pllist_init(void) {
  PLList *lst = calloc(1, sizeof(PLList));
  if (!lst) return NULL;

  // Initialize the array member
  lst->arr = calloc(INITIAL_CAPACITY, sizeof(element_t *));
  if (!lst->arr) { free(lst); return NULL; }

  // Initialize other members of PLList
  lst->capacity = INITIAL_CAPACITY;
  lst->size = 0;

  // Initialize function pointers
  lst->get = get;
  lst->append = append;
  lst->insert = insert;
  lst->pop = pop;
  lst->remove = pll_remove;
  lst->remove_at = remove_at;
  lst->replace = replace;
  lst->index = pll_index;
  lst->reverse = reverse;
  lst->length = length;
  lst->is_empty = is_empty;

  return lst;
}


void pllist_free(PLList **self) {
  if (!self || !(*self)) return;

  for (size_t i = 0; i < (*self)->size; i++) {
    pl_free_element( (*self)->arr[i] );
  }

  free( (*self)->arr );     // free the array of element_t pointers
  free( *self );            // free the PLList struct

  *self = NULL;             // set the reference to NULL
}


// ============================================
// Core Functions Implementation
// ============================================
static element_t* get(PLList *self, int index) {
  if (!self || index < 0 || (size_t)index >= self->size) return NULL;

  return self->arr[index];
}


static bool append(PLList *self, etype_t etype, void *value) {
  if (!self || !value) return false;

  // check if the array is full, then resize the arr member
  if ((self->size == self->capacity) && !resize(self)) return false;

  // allocate an element_t struct
  element_t *element = pl_new_element(etype, value);
  if (!element) return false;

  // as size is initialize to 0 at start, it always points to empty space
  self->arr[self->size++] = element;
  return true;
}


static bool insert(PLList *self, int index, etype_t etype, void *value) {
  if (!self || !value || index < 0) return false;

  // 1. If index is equal to or greater than size, then append it
  if ( (size_t)index > self->size ) return append(self, etype, value);

  // 2. Resize the array if it's full
  if ((self->capacity == self->size + 1) && !resize(self)) return false;

  // 3. Create a new element
  element_t *element = pl_new_element(etype, value);
  if (!element) return false;

  // 4. Move the elements to right starting from index position
  for (size_t i = self->size; i > (size_t)index; i--) {
    self->arr[i] = self->arr[i - 1];
  }

  // 5. Insert the value at index position
  self->arr[index] = element;

  self->size++;
  return true;
}


static element_t* pop(PLList *self) {
  if (!self || self->size == 0) return NULL;

  // caller is expected to free the memory of element_t
  return self->arr[--self->size];
}


static int pll_index(PLList *self, etype_t etype, void *value) {
  if (!self || !value || self->size == 0) return -1;

  element_t *element = NULL;

  for (size_t i = 0; i < self->size; i++) {
    element = self->arr[i];

    if (element->etype == etype) {
      switch (etype) {
        case INT:
          if (element->value.ival == *(int *)value) return (int)i;
          break;
        case DOUBLE:
          if (fabs(element->value.dval - *(double *)value) < 1E-9) return (int)i;
          break;
        case STR:
          if (strcmp(element->value.sval, (char *)value) == 0) return (int)i;
          break;
      }
    }
  }

  // finally, if we reach here - no match found
  return -1;
}


static bool remove_at(PLList *self, int index){
  if (!self || index < 0 || (size_t)index >= self->size) return false;

  element_t *to_remove = self->arr[index];

  size_t elements_to_move = ( self->size - (size_t)index ) - 1;
  if (elements_to_move > 0) {
    // move the elements one step towards left
    memmove(&self->arr[index], &self->arr[index + 1], elements_to_move * sizeof(element_t *));
  }

  pl_free_element(to_remove);        // free the removed element

  self->arr[self->size - 1] = NULL;  // clear unused array slot
  self->size--;                      // decrement the size

  return true;
}


static bool pll_remove(PLList *self, etype_t etype, void *value) {
  if (!self || !value) return false;

  // identify the first occurrance of the passed in value
  int index = pll_index(self, etype, value);
  if (index < 0) return false;

  return remove_at(self, index);
}


static bool replace(PLList *self, int index, etype_t etype, void *value) {
  if (!self || index < 0 || (size_t)index >= self->size || !value) return false;

  // element to update
  element_t *ele_update = self->arr[index];
  char *new_str = NULL;

  // allocate memory for a new string for replacement
  if (etype == STR) {
    new_str = strdup((char *)value);
    if (!new_str) return false;
  }

  // if the value currently present in array is string, then free it
  if (ele_update->etype == STR)
    free(ele_update->value.sval);

  // replace the value in the current position
  switch (etype) {
    case INT: ele_update->value.ival = *(int *)value; break;
    case DOUBLE: ele_update->value.dval = *(double *)value; break;
    case STR: ele_update->value.sval = new_str; break;
    default: return false;
  }

  ele_update->etype = etype;   // update the value time
  return true;
}


static void reverse(PLList *self) {
  if (!self || self->size <= 1) return;

  // use two pointer approch to swap the positions
  for (size_t l = 0, r = self->size - 1; l < r; l++, r--) {
    SWAP(self->arr[l], self->arr[r]);
  }
}


static size_t length(PLList *self) {
  return (!self ? 0 : self->size);
}


static bool is_empty(PLList *self) {
  return (!self ? true : self->size == 0);
}

// ============================================
// Util Functions Implementation
// ============================================
static bool resize(PLList *self) {
  if (!self || (self->size < self->capacity) ) return false;

  size_t new_capacity = self->capacity * 2;

  element_t **new_arr = realloc(self->arr, new_capacity * sizeof(element_t *));
  if (!new_arr) return false;

  self->arr = new_arr;
  self->capacity = new_capacity;
  return true;
}
