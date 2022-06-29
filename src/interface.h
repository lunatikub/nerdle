#ifndef __INTERFACE__
#define __INTERFACE__

#include <stdbool.h>

#include "nerdle.h"

/**
 * Opaque structure used as an interface with the game on the site.
 */
typedef struct interface interface_t;

/**
 * Create and allocate an interface with the game.
 * @warning interface has to freed.
 *
 * @return handle of the inferface if OK, otherwise return @c NULL.
 */
interface_t* interface_create(void);

/**
 * Start the interface.
 * Set the properties of the grid.
 *
 * @param in interface handle.
 */
void interface_start(interface_t *in);

/**
 * Write (keyboard emulation) an equation.
 *
 * @param in interface handle.
 * @param eq equation to write.
 */
void interface_write(interface_t *in, struct equation *eq);

/**
 * Wait end of a round.
 *
 * @param in interface handle.
 * @param round number of the round.
 * @param sz size of the equation.
 */
void interface_wait_round_end(interface_t *in, uint32_t round, uint32_t sz);

/**
 * At the end of a round, get the status of all locations.
 *
 * @param nerdle nerdle handle.
 * @param in interface handle.
 * @param round number of the round.
 * @param eq equation to evaluate.
 * @return true if win, otherwise return false.
 */
bool interface_get_status(struct nerdle *nerdle,
                          struct interface *in,
                          uint32_t round,
                          const struct equation *eq);

#endif /* !__INTERFACE__ */
