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
void interface_write(struct interface *in, struct equation *eq);

#endif /* !__INTERFACE__ */
