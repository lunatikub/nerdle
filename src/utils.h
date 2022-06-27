#ifndef __UTILS__
#define __UTILS__

#include "equation.h"

/**
 * Convert an equation to a string.
 *
 * @param eq equation to convert.
 * @param str output string.
 * @param sz size of the equation.
 */
void utils_eq_to_str(const struct equation *eq, char *str, uint32_t sz);

/**
 * Convert a string to an equation.
 *
 * @param str string to convert.
 * @param eq output equation.
 * @param sz size of the equation.
 */
void utils_str_to_eq(const char *str, struct equation *eq, uint32_t sz);

#endif /* !__UTILS__ */
