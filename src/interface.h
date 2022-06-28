#ifndef __INTERFACE__
#define __INTERFACE__

#include <stdbool.h>

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
void interface_wait_round_end(struct interface *in, uint32_t round, uint32_t sz);

#endif /* !__INTERFACE__ */
