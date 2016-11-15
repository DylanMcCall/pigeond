#ifndef _PIGEON_TUNNEL_H
#define _PIGEON_TUNNEL_H

/**
 * pigeon_tunnel.h
 * An interface to control the Pigeon network tunnel device.
 * @author Dylan McCall <dmccall@sfu.ca>
 */

#include <stdbool.h>
#include <stdlib.h>

typedef struct _PigeonTunnel PigeonTunnel;

PigeonTunnel *pigeon_tunnel_open();
bool pigeon_tunnel_close(PigeonTunnel *pigeon_tunnel);
const char *pigeon_tunnel_get_dev_name(PigeonTunnel *pigeon_tunnel);
int pigeon_tunnel_read(PigeonTunnel *pigeon_tunnel, char *out_buffer, size_t buffer_size);
int pigeon_tunnel_write(PigeonTunnel *pigeon_tunnel, char *value, size_t size);

#endif