#ifndef __NERDLE__
#define __NERDLE__

#include <stdint.h>

#include "rules.h"
#include "equation.h"

struct candidate {
  struct equation eq;
  struct candidate *next;
  struct candidate *prev;
};

struct nerdle {
  uint32_t sz; /* size of the equation */
  struct candidate *candidates;
  uint64_t nr_candidate;
  uint32_t limit;
  enum symbol right[LIMIT_MAX_EQ_SZ];
  bool discarded[SYMBOL_END];
  bool wrong[SYMBOL_END][LIMIT_MAX_EQ_SZ];
};

/**
 * Create a nerdle IA.
 *
 * @param sz size of the equation.
 * @return nerdle handle allocated.
 */
struct nerdle* nerdle_create(uint32_t sz);

/**
 * Destroy a nerdle IA previously allocated from @c nerdle_create.
 *
 * @param nerdle nerdle handle.
 */
void nerdle_destroy(struct nerdle *nerdle);

/**
 * Generate all the equations.
 *
 * @param nerdle nerdle handle.
 */
void nerdle_generate_equations(struct nerdle *nerdle);

/**
 * Set the first equation.
 *
 * @param nerdle nerdle handle.
 * @param eq equation to set.
 */
void nerdle_set_first_equation(struct nerdle *nerdle, struct equation *eq);

/**
 * Remove all candidates not respecting the status [right/discarded/wrong_position].
 *
 * @param nerdle nerdle handle.
 */
void nerdle_check_candidates(struct nerdle *nerdle);

/**
 * Find the best equations in the list of candidates.
 * Best is based on the variance of the symbols.
 *
 * @param nerdle nerdle handle.
 * @param eq best equation output.
 */
void nerdle_find_best_equation(struct nerdle *nerdle, struct equation *eq);

#endif /* !__NERDLE__ */
