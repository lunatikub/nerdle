#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "nerdle.h"

struct nerdle* nerdle_create(uint32_t sz)
{
  assert(sz >= LIMIT_MIN_EQ_SZ && sz <= LIMIT_MAX_EQ_SZ);

  struct nerdle *nerdle = calloc(1, sizeof(*nerdle));
  nerdle->sz = sz;
  return nerdle;
}

void nerdle_destroy(struct nerdle *nerdle)
{
  struct candidate *candidate = nerdle->candidates;

  while (candidate != NULL) {
    struct candidate *next = candidate->next;
    free(candidate);
    candidate = next;
  }
  free(nerdle);
}

static struct candidate* nerdle_candidate_new(struct equation *eq)
{
  struct candidate *candidate = calloc(1, sizeof(*candidate));
  candidate->eq = *eq;
  return candidate;
}

static void nerdle_candidate_insert_head(struct nerdle *nerdle, struct candidate *candidate)
{
  candidate->next = nerdle->candidates;
  if (nerdle->candidates != NULL) {
    nerdle->candidates->prev = candidate;
  }
  nerdle->candidates = candidate;
}

static bool nerdle_candidate_add(struct nerdle *nerdle, struct equation *eq)
{
  struct candidate *candidate = nerdle_candidate_new(eq);
  nerdle_candidate_insert_head(nerdle, candidate);
  ++nerdle->nr_candidate;
  if (nerdle->limit != 0 && nerdle->nr_candidate == nerdle->limit) {
    return false;
  }
  return true;
}


/**
 * Check if the symbol at the position is:
 *  + Not discarded.
 *  + At the right position if needed.
 *  + In the equation but not at the good position.
 */
static bool nerdle_check_symbol(struct nerdle *nerdle, enum symbol symbol, uint32_t position)
{
  if (nerdle->discarded[symbol] == true) {
    return false;
  }
  if (nerdle->right[position] != 0 && nerdle->right[position] != symbol) {
    return false;
  }
  if (nerdle->wrong[symbol][position] == true) {
    return false;
  }
  return true;
}

static bool nerdle_generate_equations_rec(struct nerdle *nerdle,
                                          struct equation *eq,
                                          uint32_t position)
{
  if (position == nerdle->sz) {
    if (equation_check_semantic(eq) == true &&  equation_check_equality(eq) == true) {
      if (nerdle_candidate_add(nerdle, eq) == false) {
        return false;
      }
    }
    return true;
  }

  for (uint32_t i = SYMBOL_0; i < SYMBOL_END; ++i) {
    if (nerdle_check_symbol(nerdle, i, position) == false) {
      continue;
    }
    if (equation_add_symbol(eq, i, position) == true) {
      if (nerdle_generate_equations_rec(nerdle, eq, position + 1) == false) {
        return false;
      }
    }
  }

  return true;
}

void nerdle_generate_equations(struct nerdle *nerdle)
{
  struct equation eq = { .sz = nerdle->sz };

  /* Optimization: only try the branchs starting [1-9]
     Reducing the number of initial branches of the tree */
  for (uint32_t i = SYMBOL_1; i <= SYMBOL_9; ++i) {
    eq.symbols[0] = i;
    if (nerdle_check_symbol(nerdle, i, 0) == false) {
      continue;
    }
    if (nerdle_generate_equations_rec(nerdle, &eq, 1) == false) {
      break;
    }
  }
}

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

void nerdle_set_first_equation(struct nerdle *nerdle, struct equation *eq)
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

static void remove_candidate(struct nerdle *nerdle, struct candidate *candidate)
{
  if (candidate->prev == NULL) { /* head */
    nerdle->candidates = candidate->next;
  } else {
    candidate->prev->next = candidate->next;
  }
  if (candidate->next != NULL) {
    candidate->next->prev = candidate->prev;
  }
  free(candidate);
  --nerdle->nr_candidate;
}

static struct candidate*
check_candidate(struct nerdle *nerdle, struct candidate *candidate)
{
  for (unsigned i = 0; i < nerdle->sz; ++i) {
    if (nerdle_check_symbol(nerdle, candidate->eq.symbols[i], i) == false) {
      struct candidate *next = candidate->next;
      remove_candidate(nerdle, candidate);
      return next;
    }
  }
  return candidate->next;
}

void nerdle_check_candidates(struct nerdle *nerdle)
{
  struct candidate *candidate = nerdle->candidates;
  while (candidate != NULL) {
    candidate = check_candidate(nerdle, candidate);
  }
}
