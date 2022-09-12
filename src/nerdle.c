#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "nerdle.h"
#include "utils.h"

struct nerdle* nerdle_create(uint32_t sz, uint32_t limit)
{
  uint32_t s;
  uint32_t p;

  assert(sz >= LIMIT_MIN_EQ_SZ && sz <= LIMIT_MAX_EQ_SZ);

  struct nerdle *nerdle = calloc(1, sizeof(*nerdle));
  nerdle->sz = sz;
  nerdle->limit = limit;

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

static void nerdle_generate_best_variance_equations_rec(struct nerdle *nerdle,
                                                        struct equation *eq,
                                                        uint32_t position)
{
  if (position == nerdle->sz) {
    if (equation_check_semantic(eq) == true &&  equation_check_equality(eq) == true) {

      if (equation_get_variance(eq) == nerdle->sz) {
        char str[LIMIT_MAX_EQ_SZ];
        utils_eq_to_str(eq, str, nerdle->sz);
        printf("%.*s\n", nerdle->sz, str);
      }
    }
    return;
  }

  for (uint32_t i = SYMBOL_0; i < SYMBOL_END; ++i) {
    if (nerdle_check_symbol(nerdle, i, position) == false) {
      continue;
    }
    if (equation_add_symbol(eq, i, position) == true) {
      nerdle_generate_best_variance_equations_rec(nerdle, eq, position + 1);
    }
  }
}

void nerdle_generate_best_variance_equations(struct nerdle *nerdle)
{
  struct equation eq = { .sz = nerdle->sz };

  for (uint32_t i = SYMBOL_1; i <= SYMBOL_9; ++i) {
    eq.symbols[0] = i;
    if (nerdle_check_symbol(nerdle, i, 0) == false) {
      continue;
    }
    nerdle_generate_best_variance_equations_rec(nerdle, &eq, 1);
  }
}

/* warning: singleton include */
#include "first_equations.h"

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

void nerdle_find_best_equation(struct nerdle *nerdle, struct equation *eq)
{
  nerdle_check_candidates(nerdle);
  if (nerdle->nr_candidate == 0) {
    nerdle_generate_equations(nerdle);
  }
  assert(nerdle->nr_candidate > 0);

  struct candidate *candidate = nerdle->candidates;
  struct candidate *best_candidate = candidate;
  unsigned best_variance = 0;

  while (candidate != NULL) {
    unsigned variance = equation_get_variance(&candidate->eq);
    if (variance > best_variance) {
      best_variance = variance;
      best_candidate = candidate;
    }
    candidate = candidate->next;
  }

  memcpy(eq, &best_candidate->eq, sizeof(struct equation));
  remove_candidate(nerdle, best_candidate);
}
