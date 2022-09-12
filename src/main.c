#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "nerdle.h"
#include "utils.h"
#include "interface.h"
#include "first_equations.h"

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

enum {
  CASE_SIZE,
  CASE_LIMIT,
};

static struct option long_options[] = {
  { "size", required_argument, 0, 0 },
  { "limit", required_argument, 0, 0 },
};

struct options {
  uint32_t sz;
  uint32_t limit;
};

static void options_parse(int argc, char **argv, struct options *opts)
{
  opts->sz = DEFAULT_SIZE;
  opts->limit = 0;

  while (true) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (option_index) {
      case CASE_SIZE:
        opts->sz = atoi(optarg);
        break;
      case CASE_LIMIT:
        opts->limit = atoi(optarg);
        break;
    }
  }
}

int main(int argc, char **argv)
{
  struct options opts;

  options_parse(argc, argv, &opts);

  printf("[nerdle] sz:%u\n", opts.sz);
  struct nerdle *nerdle = nerdle_create(opts.sz, opts.limit);

  nerdle_generate_best_variance_equations(nerdle);
  return 0;

  interface_t *in = interface_create();
  struct equation eq;

  interface_start(in);

  nerdle_set_first_equation(nerdle, &eq);

  for (uint32_t round = 0; round < MAX_NR_ROUND; ++round) {
    printf("[nerdle] -------------------------- {round:%u}\n", round);
    dump_equation(&eq);
    interface_write(in, &eq);
    if (interface_get_status(nerdle, in, round, &eq) == true) {
      printf("[nerdle] WIN !\n");
      break;
    }
    nerdle_dump_status(nerdle);
    nerdle_find_best_equation(nerdle, &eq);
  }

  free(in);
  nerdle_destroy(nerdle);
  return 0;
}
