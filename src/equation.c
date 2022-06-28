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

bool equation_check_semantic(struct equation *eq)
{
  /* Last symbol cannot be an operator. */
  if (eq->symbols[eq->sz - 1] > SYMBOL_9) {
    return false;
  }

  /* At least we need almost two operators: one '=' and one other. */
  uint32_t nr_op = 0;
  bool flag_equal = false;
  for (uint32_t i = 0; i < eq->sz; ++i) {
    if (eq->symbols[i] > SYMBOL_9) {
      if (eq->symbols[i] == SYMBOL_EQ) {
        flag_equal = true;
      }
      ++nr_op;
    }
  }
  if (nr_op < 2 || flag_equal == false) {
    return false;
  }

  return true;
}
