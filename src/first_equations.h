#ifndef __FIRST_EQUATIONS__
#define __FIRST_EQUATIONS__

#include <string.h>

#define D SYMBOL_DIV
#define X SYMBOL_MULT
#define P SYMBOL_PLUS
#define M SYMBOL_MINUS
#define E SYMBOL_EQ

/* 1 + 2 = 3 */
static const enum symbol first_eq_5[] = {
  1, P, 2, E, 3,
};

/* 10 - 2 = 8 */
static const enum symbol first_eq_6[] = {
  1, 0, M, 2, E, 8,
};

/* 23 - 19 = 4 */
static const enum symbol first_eq_7[] = {
  2, 3, M, 1, 9, E, 4,
};

/* 9 + 8 - 3 = 14 */
static const enum symbol first_eq_8[] = {
  9, P, 8, M, 3, E, 1, 4,
};

/* 20 + 3 - 8 = 15 */
static const enum symbol first_eq_9[] = {
  2, 0, P, 3, M, 8, E, 1, 5,
};

/* 10 + 20 / 5 = 14 */
static const enum symbol first_eq_10[] = {
  1, 0, P, 2, 0, D, 5, E, 1, 4,
};

/* 10 + 250 / 5 = 60 */
static const enum symbol first_eq_11[] = {
  1, 0, P, 2, 5, 0, D, 5, E, 6, 0,
};

/* 10 + 350 / 50 = 17 */
static const enum symbol first_eq_12[] = {
  1, 0, P, 3, 5, 0, D, 5, 0, E, 1, 7,
};

#undef D
#undef X
#undef P
#undef M
#undef E

static inline void nerdle_set_first_equation(struct nerdle *nerdle, struct equation *eq)
{
  switch (nerdle->sz) {
#define CASE_SET_EQ(SZ, SYMBOLS)                                \
    case SZ:                                                    \
      memcpy(eq->symbols, SYMBOLS, SZ * sizeof(enum symbol));   \
      eq->sz = SZ;                                              \
      break

    CASE_SET_EQ(5, first_eq_5);
    CASE_SET_EQ(6, first_eq_6);
    CASE_SET_EQ(7, first_eq_7);
    CASE_SET_EQ(8, first_eq_8);
    CASE_SET_EQ(9, first_eq_9);
    CASE_SET_EQ(10, first_eq_10);
    CASE_SET_EQ(11, first_eq_11);
    CASE_SET_EQ(12, first_eq_12);

#undef CASE_SET_EQ
  };
}

#endif /* !__FIRST_EQUATIONS__ */
