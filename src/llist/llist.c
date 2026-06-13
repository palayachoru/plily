#include "util.h"
#define _POSIX_C_SOURCE 200809L   // Enables declarations for POSIX functions and symbols

#include <stdlib.h>

#include "llist.h"


// ================================================
// Forward declarations (what this module provides)
// ================================================
static element_t* get(PLLinkedList *self, int index);

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
static element_t* get(PLLinkedList *self, int index) {
  if (!self || index < 0 || (size_t)index >= self->size) return NULL;

  node_t *head = self->head;

  for (size_t i = 0; i < self->size; i++) {
    head = head->next;

    if (!head) return NULL;    // protection againist inconsistent size
  }

  return &head->data;
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
    for (size_t i = 0; i < (size_t)index - 1; i++) prev = prev->next;

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
