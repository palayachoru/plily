#pragma once

// =======
// MACROS
// =======

/// do..while block is used not to provide looping but, it's
/// a trick used to consider all the statments in one block
#define SWAP(a, b) \
  do { \
    __typeof__(a) _tmp = (a); \
    (a) = (b);  \
    (b) = _tmp; \
  } while (0)



// ===================
// TYPE DECLERATIONS
// ===================

/// Enum to identify type of value in the Element's union
typedef enum {INT, DOUBLE, STR} etype_t;


/// Representation of a single element struct
typedef struct {
  union {               ///< holds data of any one type (12 bytes)
    int    ival;
    double dval;
    char  *sval;
  } value;

  etype_t etype;       ///< identify the type of data in the union (4 bytes)
} element_t;


/// Representation of a single node struct
typedef struct node {
  struct node *next;  ///< Pointer to next node in chain
  element_t data;     ///< Data memeber of the node
} node_t;



// ========================
// Function Declarations
// ========================

/**
 * @brief Allocate memory for new element and initialize with values
 * @param etype - Enum type to specify the value type
 * @param *value - Void pointer to value
 * @return element_t* struct pointer or NULL
 */
element_t* pl_new_element(etype_t etype, void *value);

/**
 * @brief Allocate memory for new node and initialize with values
 * @param etype - Enum type to specify the value type
 * @param *value - Void pointer to value
 * @return node_t* struct pointer or NULL
 */
node_t* pl_new_node(etype_t etype, void *value);

void pl_free_element(element_t *element);

void pl_free_node(node_t *node);
