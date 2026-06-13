#define _POSIX_C_SOURCE 200809L   // Enables declarations for POSIX functions and symbols

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


element_t* pl_new_element(etype_t etype, void *value) {
  if (!value) return NULL;

  element_t *element = calloc(1, sizeof(element_t));
  if (!element) return NULL;

  switch (etype) {
    case INT: element->value.ival = *(int *)value; break;
    case DOUBLE: element->value.dval = *(double *)value; break;
    case STR: {
      element->value.sval = strdup((char *)value);
      if (!element->value.sval) {
        free(element);
        return NULL;
      }
      break;
    }
    default: return NULL;
  }

  element->etype = etype;
  return element;
}


node_t* pl_new_node(etype_t etype, void *value) {
  if (!value) return NULL;

  node_t *n = calloc(1, sizeof(node_t));
  if (!n) return NULL;

  // Initialize node elements
  switch (etype) {
    case INT: n->data.value.ival = *(int *)value; break;
    case DOUBLE: n->data.value.dval = *(double *)value; break;
    case STR: {
      n->data.value.sval = strdup((char *)value);
      if (!n->data.value.sval) {
        free(n);
        return NULL;
      }
      break;
    }
    default: return NULL;
  }

  n->data.etype = etype;
  n->next = NULL;

  return n;
}


void pl_free_element(element_t *element) {
  if (!element) return;

  // free the memory allocated for string
  if (element->etype == STR) free(element->value.sval);

  free(element);
}


void pl_free_node(node_t *node) {
  if (!node) return;

  // free the memory allocated for string
  if (node->data.etype == STR) free(node->data.value.sval);

  free(node);
}
