#include "utils.h"

void utils_eq_to_str(const struct equation *eq, char *str, uint32_t sz)
{
  for (uint32_t i = 0; i < sz; ++i) {
    enum symbol s = eq->symbols[i];
    if (s >= SYMBOL_0 && s <= SYMBOL_9) {
      str[i] = '0' + s;
    } else {
      switch (s) {
#define CASE_SYMBOL(S, C) case S: str[i] = C; break
        CASE_SYMBOL(SYMBOL_PLUS, '+');
        CASE_SYMBOL(SYMBOL_MINUS, '-');
        CASE_SYMBOL(SYMBOL_MULT, '*');
        CASE_SYMBOL(SYMBOL_DIV, '/');
        CASE_SYMBOL(SYMBOL_EQ, '=');
#undef CASE_SYMBOL
        default:;
      };
    }
  }
}

void utils_str_to_eq(const char *str, struct equation *eq, uint32_t sz)
{
  for (uint32_t i = 0; i < sz; ++i) {
    char c = str[i];
    if (c >= '0' && c <= '9') {
      eq->symbols[i] = c - '0';
    } else {
      switch(c) {
#define CASE_CHAR(C, S) case C: eq->symbols[i] = S; break
        CASE_CHAR('+', SYMBOL_PLUS);
        CASE_CHAR('-', SYMBOL_MINUS);
        CASE_CHAR('*', SYMBOL_MULT);
        CASE_CHAR('/', SYMBOL_DIV);
        CASE_CHAR('=', SYMBOL_EQ);
#undef CASE_CHAR
        default:;
      };
    }
  }
}
