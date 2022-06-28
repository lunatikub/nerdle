#include <stdlib.h>
#include <assert.h>

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
    if (nerdle_generate_equations_rec(nerdle, &eq, 1) == false) {
      break;
    }
  }
}
