#include <stdlib.h>
#include <stdio.h>

#include "nerdle.h"
#include "utils.h"
#include "interface.h"

/**
 * This bot plays on the following URL: https://wordleplay.com/fr/nerdle
 *
 * You have to guess the hidden math equation in 6 tries and the color of the tiles changes
 * to show how close you are. To start playing, enter any mathematical valid equation.
 */

static void dump_equation(const struct equation *eq)
{
  char str[LIMIT_MAX_EQ_SZ];
  utils_eq_to_str(eq, str, eq->sz);
  printf("[nerdle] equation: %.*s\n", eq->sz, str);
}

int main(void)
{
  uint32_t sz = 9;
  printf("[nerdle] sz:%u\n", 9);
  struct nerdle *nerdle = nerdle_create(sz);
  interface_t *in = interface_create();
  struct equation eq;

  interface_start(in);

  nerdle_set_first_equation(nerdle, &eq);

  for (uint32_t round = 0; round < MAX_NR_ROUND; ++round) {
    printf("[nerdle] -------------------------- {round:%u}\n", round);
    dump_equation(&eq);
    interface_write(in, &eq);
    interface_wait_round_end(in, round, nerdle->sz);
    if (interface_get_status(nerdle, in, round, &eq) == true) {
      break;
    }
    nerdle_dump_status(nerdle);
    if (round == 0) {
      nerdle_generate_equations(nerdle);
    } else {
      nerdle_check_candidates(nerdle);
    }
    nerdle_find_best_equation(nerdle, &eq);
  }

  free(in);
  nerdle_destroy(nerdle);
  return 0;
}
