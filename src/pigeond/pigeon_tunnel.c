#include "pigeon_tunnel.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#define CLONE_DEV "/dev/net/tun"

struct _PigeonTunnel {
	int tun_fd;
	char dev_name[IFNAMSIZ];
};

PigeonTunnel *pigeon_tunnel_open(const char *dev_name_template) {
	bool error = false;
	PigeonTunnel *pigeon_tunnel = NULL;
	int tun_fd;
	struct ifreq ifr = {0};

	if (!error) {
		pigeon_tunnel = malloc(sizeof(PigeonTunnel));
		if (pigeon_tunnel != NULL) {
			memset(pigeon_tunnel, 0, sizeof(*pigeon_tunnel));
		} else {
			error = true;
		}
	}

	if (!error) {
		tun_fd = open(CLONE_DEV, O_RDWR);
		if (tun_fd < 0) {
			perror("Error opening " CLONE_DEV);
			error = true;
		}
	}

	if (!error) {
		strncpy(ifr.ifr_name, dev_name_template, IFNAMSIZ);
		ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
		if (ioctl(tun_fd, TUNSETIFF, (void *) &ifr) < 0) {
			error = true;
		}
	}

	if (!error) {
		pigeon_tunnel->tun_fd = tun_fd;
		strncpy(pigeon_tunnel->dev_name, ifr.ifr_name, IFNAMSIZ);
	} else if (pigeon_tunnel != NULL) {
		close(tun_fd);
		free(pigeon_tunnel);
		pigeon_tunnel = NULL;
	}

	return pigeon_tunnel;
}

bool pigeon_tunnel_close(PigeonTunnel *pigeon_tunnel) {
	bool error = false;

	if (!error) {
		if (pigeon_tunnel == NULL) {
			error = true;
		}
	}

	if (!error) {
		close(pigeon_tunnel->tun_fd);
	}

	if (!error) {
		free(pigeon_tunnel);
	}

	return !error;
}

const char *pigeon_tunnel_get_dev_name(PigeonTunnel *pigeon_tunnel) {
	return pigeon_tunnel->dev_name;
}

bool pigeon_tunnel_set_mtu(PigeonTunnel *pigeon_tunnel, int mtu) {
	bool error = false;
	int socket_fd;
	struct ifreq ifr = {0};

	if (!error) {
		socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket_fd < 0) {
			error = true;
		}
	}

	if (!error) {
		strncpy(ifr.ifr_name, pigeon_tunnel->dev_name, IFNAMSIZ);
		ifr.ifr_addr.sa_family = AF_INET;
		ifr.ifr_mtu = mtu;
		if (ioctl(socket_fd, SIOCSIFMTU, (void *) &ifr) < 0) {
			error = true;
		}
	}

	return !error;
}

int pigeon_tunnel_get_mtu(PigeonTunnel *pigeon_tunnel) {
	bool error = false;
	int socket_fd;
	struct ifreq ifr = {0};

	if (!error) {
		socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket_fd < 0) {
			error = true;
		}
	}

	if (!error) {
		strncpy(ifr.ifr_name, pigeon_tunnel->dev_name, IFNAMSIZ);
		ifr.ifr_addr.sa_family = AF_INET;
		if (ioctl(socket_fd, SIOCGIFMTU, (void *) &ifr) < 0) {
			error = true;
		}
	}

	if (!error) {
		return ifr.ifr_mtu;
	} else {
		return -1;
	}
}

PigeonFrame *pigeon_tunnel_read(PigeonTunnel *pigeon_tunnel) {
	PigeonFrame *pigeon_frame;
	// FIXME: It would be better to use the MTU for the buffer size here, but
	// pigeon_tunnel_get_mtu is very inefficient.
	char buffer[ETHER_MAX_LEN] = {0};
	size_t bytes_read = read(pigeon_tunnel->tun_fd, &buffer, sizeof(buffer));

	if (bytes_read < 0) {
		perror("Error reading from tunnel device");
		pigeon_frame = NULL;
	} else if (bytes_read > ETHER_MAX_LEN) {
		fprintf(stderr, "Dropping oversized frame");
		fprintf(stderr, "Frame is %lu bytes. Expected <= %d bytes.\n", (unsigned long)bytes_read, ETHER_MAX_LEN);
		pigeon_frame = NULL;
		// We don't worry about undersized packets. Those will be padded automatically.
	} else {
		pigeon_frame = pigeon_frame_new(buffer, bytes_read);
	}

	return pigeon_frame;
}

int pigeon_tunnel_write(PigeonTunnel *pigeon_tunnel, PigeonFrame *pigeon_frame) {
	const char *buffer;
	size_t buffer_size = pigeon_frame_get_buffer(pigeon_frame, &buffer);
	size_t bytes_written = write(pigeon_tunnel->tun_fd, buffer, buffer_size);

	if (bytes_written < 0) {
		perror("Error writing to tunnel device");
	}

	return bytes_written;
}
