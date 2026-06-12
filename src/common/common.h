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
  union {           ///< holds data of any one type (12 bytes)
    int    ival;
    double dval;
    char  *sval;
  } value;

  etype_t etype;   ///< identify the type of data in the union (4 bytes)
} element_t;
