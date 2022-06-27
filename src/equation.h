#ifndef __EQUATION__
#define __EQUATION__

#include <stdbool.h>
#include <stdint.h>

#include "rules.h"

/**
 * Alphabet of an nerdle equation.
 */
enum symbol {
  SYMBOL_0, /* '0' */
  SYMBOL_1, /* '1' */
  SYMBOL_2, /* '2' */
  SYMBOL_3, /* '3' */
  SYMBOL_4, /* '4' */
  SYMBOL_5, /* '5' */
  SYMBOL_6, /* '6' */
  SYMBOL_7, /* '7' */
  SYMBOL_8, /* '8' */
  SYMBOL_9, /* '9' */
  SYMBOL_PLUS,  /* '+' */
  SYMBOL_MINUS, /* '-' */
  SYMBOL_MULT,  /* '*' */
  SYMBOL_DIV,   /* '/' */
  SYMBOL_EQ,    /* '=' */
  SYMBOL_END,
};

struct equation {
  enum symbol symbols[LIMIT_MAX_EQ_SZ];
};

/**
 * Add a symbol to an equation.
 * Check validity of the add:
 *   1. only one symbol equal is allowed by equation.
 *   2. a symbol operator cannot follow an otheer symbol operator.
 *
 * @param eq equation.
 * @param symbol symbol to add.
 * @param position position in the equation.
 * @return true if the symbol can be added, otherwise return false.
 */
bool equation_add_symbol(struct equation *eq,
                         enum symbol symbol,
                         uint32_t position);

#endif /* !__EQUATION__ */
