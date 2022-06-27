#include "equation.h"

bool equation_add_symbol(struct equation *eq,
                         enum symbol symbol,
                         uint32_t position)
{
  /* Only one symbol equal is allowed by equation. */
  if (symbol == SYMBOL_EQ) {
    for (uint32_t i = 0; i < position; ++i) {
      if (eq->symbols[i] == SYMBOL_EQ) {
        return false;
      }
    }
  }

  /* A symbol operator cannot follow an other symbol operator. */
  enum symbol last = eq->symbols[position - 1];
  if (last > SYMBOL_9 && symbol > SYMBOL_9) {
    return false;
  }

  eq->symbols[position] = symbol;
  return true;
}
