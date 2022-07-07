#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "nerdle.h"

struct nerdle* nerdle_create(uint32_t sz)
{
  uint32_t s;
  uint32_t p;

  assert(sz >= LIMIT_MIN_EQ_SZ && sz <= LIMIT_MAX_EQ_SZ);

  struct nerdle *nerdle = calloc(1, sizeof(*nerdle));
  nerdle->sz = sz;

  for (s = 0; s < SYMBOL_END; ++s) {
    nerdle->status[s] = UNKNOWN;
  }
  for (p = 0; p < LIMIT_MAX_EQ_SZ; ++p) {
    nerdle->right[p] = SYMBOL_END;
    for (s = 0; s < SYMBOL_END; ++s) {
      nerdle->wrong[p][s] = false;
    }
  }

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
 */
static bool nerdle_check_symbol(struct nerdle *nerdle, enum symbol symbol, uint32_t pos)
{
  if (nerdle->status[symbol] == DISCARDED) {
    return false;
  }
  if (nerdle->right[pos] != SYMBOL_END &&
      nerdle->right[pos] != symbol) {
    return false;
  }
  if (nerdle->wrong[pos][symbol] == true) {
    return false;
  }
  return true;
}

void nerdle_update_status(struct nerdle *nerdle, enum status status,
                          const struct equation *eq, uint32_t pos)
{
  enum symbol symbol = eq->symbols[pos];

  switch (status) {
    case DISCARDED:
      if (nerdle->status[symbol] == UNKNOWN) {
        nerdle->status[symbol] = DISCARDED;
      }
      break;
    case WRONG:
      nerdle->status[symbol] = WRONG;
      nerdle->wrong[pos][symbol] = true;
      break;
    case RIGHT:
      nerdle->status[symbol] = RIGHT;
      nerdle->right[pos] = symbol;
      break;
    default:
      ;
  };
}

static char symbol_to_char(enum symbol symbol)
{
  if (symbol >= 0 && symbol <= 9) {
    return '0' + symbol;
  }
  switch (symbol) {
    case SYMBOL_PLUS: return '+';
    case SYMBOL_MINUS: return '-';
    case SYMBOL_MULT: return '*';
    case SYMBOL_DIV: return '/';
    case SYMBOL_EQ: return '=';
    default:;
  }
  return '_';
}

static char status_to_char(enum status status)
{
  switch (status) {
    case UNKNOWN: return 'U';
    case RIGHT: return 'R';
    case WRONG: return 'W';
    case DISCARDED: return 'D';
  };
  return 'X';
}

static void dump_status_status(const struct nerdle *nerdle)
{
  printf("[nerdle] status [");
  for (enum symbol symbol = 0; symbol < SYMBOL_END; ++symbol) {
    printf("{%c:%c},", symbol_to_char(symbol),
           status_to_char(nerdle->status[symbol]));
  }
  printf("]\n");
}

static void dump_status_right(const struct nerdle *nerdle)
{
  printf("[nerdle] right [");
  for (uint32_t pos = 0; pos < nerdle->sz; ++pos) {
    printf("%c", nerdle->right[pos] != SYMBOL_END ?
           symbol_to_char(nerdle->right[pos]) : ' ');
  }
  printf("]\n");
}

static void dump_status_wrong(const struct nerdle *nerdle)
{
  printf("[nerdle] wrong [");
  for (uint32_t pos = 0; pos < nerdle->sz; ++pos) {
    printf("{%u: [", pos);
    for (enum symbol symbol = 0; symbol < SYMBOL_END; ++symbol) {
      if (nerdle->wrong[pos][symbol] == true) {
        printf("%c, ", symbol_to_char(symbol));
      }
    }
    printf("]}, ");
  }
  printf("]\n");
}

static void dump_status_discarded(const struct nerdle *nerdle)
{
  printf("[nerdle] discarded [");
  for (enum symbol symbol = 0; symbol < SYMBOL_END; ++symbol) {
    if (nerdle->status[symbol] == DISCARDED) {
      printf("'%c', ", symbol_to_char(symbol));
    }
  }
  printf("]\n");
}

void nerdle_dump_status(const struct nerdle *nerdle)
{
  dump_status_status(nerdle);
  dump_status_right(nerdle);
  dump_status_wrong(nerdle);
  dump_status_discarded(nerdle);
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
  printf("[nerdle] generate %lu equations (limit:%u)\n",
         nerdle->nr_candidate, nerdle->limit);
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

/**
 * Remove a candidate from the list of candidate.
 */
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

/**
 * Check if a candidate doesn't respect the status, in this case remove it
 * from the lsit of candidate.
 */
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
  uint64_t nr_candidate_before = nerdle->nr_candidate;
  struct candidate *candidate = nerdle->candidates;
  while (candidate != NULL) {
    candidate = check_candidate(nerdle, candidate);
  }
  printf("[nerdle] remove %lu candidates, %lu candidates remaining\n",
         nr_candidate_before - nerdle->nr_candidate, nerdle->nr_candidate);
}

/**
 * Variance means the number of different symbol in the equation.
 */
static uint32_t candidate_get_variance(struct candidate *candidate)
{
  unsigned variance = 0;
  bool once[SYMBOL_END] = { false };

  for (unsigned i = 0; i < candidate->eq.sz; ++i) {
    enum symbol symbol = candidate->eq.symbols[i];
    if (once[symbol] == false) {
      once[symbol] = true;
      ++variance;
    }
  }

  return variance;
}

void nerdle_find_best_equation(struct nerdle *nerdle, struct equation *eq)
{
  assert(nerdle->nr_candidate > 0);

  struct candidate *candidate = nerdle->candidates;
  struct candidate *best_candidate = candidate;
  unsigned best_variance = 0;

  while (candidate != NULL) {
    unsigned variance = candidate_get_variance(candidate);
    if (variance > best_variance) {
      best_variance = variance;
      best_candidate = candidate;
    }
    candidate = candidate->next;
  }

  memcpy(eq, &best_candidate->eq, sizeof(struct equation));
  remove_candidate(nerdle, best_candidate);
}
