#define _POSIX_C_SOURCE 200809L   // Enables declarations for POSIX functions and symbols

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "llist.h"


// ================================================
// Forward declarations (what this module provides)
// ================================================
static element_t get(PLLinkedList *self, int index);

static bool append(PLLinkedList *self, etype_t etype, void *value);

static bool insert(PLLinkedList *self, int index, etype_t etype, void *value);

static element_t pop(PLLinkedList *self);

static bool ll_remove(PLLinkedList *self, etype_t etype, void *value);

static bool remove_at(PLLinkedList *self, int index);

static bool replace(PLLinkedList *self, int index, etype_t etype, void *value);

static int ll_index(PLLinkedList *self, etype_t etype, void *value);

static void reverse(PLLinkedList *self);

static size_t length(PLLinkedList *self);

static bool is_empty(PLLinkedList *self);

// UTILITY FUNCTION
static bool is_match(node_t *data, etype_t etype, void *value);



// ============================================
// Public API
// ============================================
PLLinkedList* pllinkedlist_init(void) {
  PLLinkedList *ll = calloc(1, sizeof(PLLinkedList));
  if (!ll) return NULL;

  // Initialize the members
  ll->head = NULL;
  ll->tail = NULL;
  ll->size = 0;

  // Initialize function pointers
  ll->get = get;
  ll->append = append;
  ll->insert = insert;
  ll->pop = pop;
  ll->remove = ll_remove;
  ll->remove_at = remove_at;
  ll->replace = replace;
  ll->index = ll_index;
  ll->reverse = reverse;
  ll->length = length;
  ll->is_empty = is_empty;

  return ll;
}


void pllinkedlist_free(PLLinkedList **self) {
  if (!self || !(*self)) return;

  node_t *head = (*self)->head;
  node_t *to_remove = NULL;

  while (head) {
    to_remove = head;
    head = head->next;

    pl_free_node(to_remove);
  }

  free( *self );  // free the PLLinkedList struct
  *self = NULL;   // set the pointer to null
}



// ============================================
// Core Functions Implementation
// ============================================
static element_t get(PLLinkedList *self, int index) {
  if (!self || index < 0 || (size_t)index >= self->size) return (element_t){0};

  node_t *curr = self->head;

  for (size_t i = 0; i < (size_t)index; i++) {
    curr = curr->next;

    if (!curr) return (element_t){0};    // protection againist inconsistent size
  }

  return curr->data;
}


static bool append(PLLinkedList *self, etype_t etype, void *value) {
  if (!self || !value) return false;

  // create a new node
  node_t *n = pl_new_node(etype, value);
  if (!n) return false;

  // in case the linked list is empty
  if (self->is_empty(self)) {
    self->head = n;
    self->tail = n;
  }
  else {
    self->tail->next = n;
    self->tail = n;
  }

  self->size++;
  return true;
}


static bool insert(PLLinkedList *self, int index, etype_t etype, void *value) {
  if (!self || !value || index < 0 || (size_t)index > self->size) return false;

  // in case - linked list empty or index is same as size
  if (self->is_empty(self) || (size_t)index == self->size)
    return self->append(self, etype, value);

  // create a new node
  node_t *n = pl_new_node(etype, value);
  if (!n) return false;

  // if index is zero
  if (index == 0) {
    n->next = self->head;
    self->head = n;
  }
  else {
    // get to the previous node of insert position
    node_t *prev = self->head;
    for (size_t i = 0; i < (size_t)index - 1; i++) {
      // will occure in case linked list is corrupted (size mismatch)
      if (!prev || !prev->next) return false;

      prev = prev->next;
    }

    n->next = prev->next;
    prev->next = n;
  }

  self->size++;
  return true;
}


static element_t pop(PLLinkedList *self) {
  if (!self || self->is_empty(self)) return (element_t){0};

  node_t *pop_node = NULL;

  // case 1: only one node is present
  if (self->head == self->tail) {
    pop_node = self->head;

    self->head = NULL;
    self->tail = NULL;
  }
  else {
    // case 2: multiple nodes are present
    node_t *curr = self->head;

    // walk to the node before tail
    while (curr->next != self->tail) curr = curr->next;

    pop_node = curr->next;
    curr->next = NULL;
    self->tail = curr;
  }

  element_t popped = pop_node->data;
  free(pop_node);

  self->size--;
  // if element_t contains str data, then caller is expected to free it
  // use pl_free_element() function to free the element after use
  return popped;
}


static bool ll_remove(PLLinkedList *self, etype_t etype, void *value) {
  if (!self || !value || self->is_empty(self)) return false;

  node_t *curr = self->head;
  node_t *prev = NULL;

  while (curr) {
    if (is_match(curr, etype, value)) break;

    prev = curr;
    curr = curr->next;
  }

  // no match found and we have reached the end of linked list
  if (!curr) return false;

  // prev will be null ,if the match found at head node
  if (prev == NULL) {
    self->head = curr->next;

    // in case only one element is present
    if (self->head == NULL) self->tail = NULL;
  }
  else {
    prev->next = curr->next;

    // if the curr is last node then  update the tail
    if (self->tail == curr) self->tail = prev;
  }

  pl_free_node(curr);
  self->size--;
  return true;
}


static bool remove_at(PLLinkedList *self, int index) {
  if (!self || self->is_empty(self) || index < 0 || (size_t)index >= self->size)
    return false;

  node_t *curr = self->head;
  node_t *prev = NULL;

  // get to the node at the index
  for (size_t i = 0; i < (size_t)index; i++) {
    prev = curr;
    curr = curr->next;
  }

  // in case of first node
  if (prev == NULL) {
    self->head = curr->next;

    // in case only one element is present
    if (self->head == NULL) self->tail = NULL;
  }
  else {
    prev->next = curr->next;

    // if the curr is last node then  update the tail
    if (self->tail == curr) self->tail = prev;
  }

  pl_free_node(curr);
  self->size--;
  return true;
}


static bool replace(PLLinkedList *self, int index, etype_t etype, void *value) {
  if (!self || !value || self->is_empty(self) || index < 0 || (size_t)index >= self->size)
    return false;

  // check for invalid etype
  if (etype != INT && etype != DOUBLE && etype != STR) return false;

  node_t *curr = self->head;
  char *new_str = NULL;

  // go to node to be replaced with
  for (int i = 0; i < index; i++) curr = curr->next;
  if (!curr) return false;

  // if new value is a string, then, let's make a copy of the data
  if (etype == STR) {
    new_str = strdup((char *)value);
    if (!new_str) return false;
  }

  // if the existing node contains str data, then free the memory
  if (curr->data.etype == STR) free(curr->data.value.sval);

  // based on the type update the data
  switch (etype) {
    case INT: curr->data.value.ival = *(int *)value; break;
    case DOUBLE: curr->data.value.dval = *(double *)value; break;
    case STR: curr->data.value.sval = new_str; break;
  }

  curr->data.etype = etype;
  return true;
}


static int ll_index(PLLinkedList *self, etype_t etype, void *value) {
  if (!self || !value || self->is_empty(self)) return -1;

  node_t *curr = self->head;

  for (size_t i = 0; i < self->size; i++) {
    if (is_match(curr, etype, value)) return (int)i;

    curr = curr->next;
  }

  // if we reach here, no match found
  return -1;
}


static void reverse(PLLinkedList *self) {
  if (!self || self->is_empty(self) || self->size == 1) return;

  node_t *old_head = self->head;   // this will become tail node

  node_t *curr = self->head;
  node_t *prev = NULL;
  node_t *next = NULL;

  while (curr) {
    next = curr->next;    // preserve the next node's reference

    curr->next = prev;
    prev = curr;

    curr = next;          // update curr with preserved next node
  }

  // prev will point to head node, and curr will be null
  self->head = prev;
  self->tail = old_head;
}


static size_t length(PLLinkedList *self) {
  return self ? self->size : 0;
}


static bool is_empty(PLLinkedList *self) {
  return length(self) == 0;
}

// ============================================
// Util Functions Implementation
// ============================================
static bool is_match(node_t *n, etype_t etype, void *value) {
  if (!n || !value) return false;

  // if element type don't match we can immediately return
  if (n->data.etype != etype) return false;

  if (etype == INT) return (n->data.value.ival == *(int *)value);
  if (etype == DOUBLE) return (n->data.value.dval == *(double *)value);
  if (etype == STR) return strcmp(n->data.value.sval, (char *)value) == 0;
  return false;
}
