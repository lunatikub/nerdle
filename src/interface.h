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
 * @param in interface previously allocated with @c interface_create.
 */
void interface_start(interface_t *in);

#endif /* !__INTERFACE__ */
