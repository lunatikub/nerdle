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
  /* Size of the equation */
  uint32_t sz;
  /* Status */
  enum status status[SYMBOL_END];
  enum symbol right[LIMIT_MAX_EQ_SZ];
  bool wrong[LIMIT_MAX_EQ_SZ][SYMBOL_END];
  /* Limit the number of branches in the generation of candidates */
  uint32_t limit;
  /* List of candidates */
  struct candidate *candidates;
  uint64_t nr_candidate;
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

/**
 * Update the status of a location of the equation.
 *
 * @param nerdle nerdle handle.
 * @param status status to set.
 * @param eq equation to update.
 * @param i index of the location.
 */
void nerdle_update_status(struct nerdle *nerdle, enum status status,
                          const struct equation *eq, uint32_t i);

/**
 * Dump the status of a round.
 *
 * @param nerdle nerdle handle.
 */
void nerdle_dump_status(const struct nerdle *nerdle);

#endif /* !__NERDLE__ */
