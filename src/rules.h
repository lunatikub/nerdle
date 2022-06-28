#ifndef __RULES__
#define __RULES__

/*
 * Game Rules:
 *   + The guess is accepted only with the correct equation
 *   + Each guess must contain one “=”
 *   + You can only use 1 2 3 4 5 6 7 8 9 0 numbers and + - * / = signs
 *   + The equation must have an integer to the right of the '='
 *   + Each guess is not commutative (a+b=c and b+a=c are different guesses)
 */

/**
 * Limits of the equation size.
 */
#define LIMIT_MIN_EQ_SZ 5
#define LIMIT_MAX_EQ_SZ 12

/**
 * Maximum number of round.
 */
#define MAX_NR_ROUND 6

#endif /* !__RULES__ */
