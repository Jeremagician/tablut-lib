#ifndef _PACKET_H_
#define _PACKET_H_

#include "struct_io.h"


#define PACKET_STRUCT(name, id, s)					\
	PACKET_##name = id,
#define PACKET_EMPTY(name, id)						\
	PACKET_##name = id,
enum {
#include "packet.def.h"
	PACKET_NONE = 0
};
#undef  PACKET_STRUCT
#undef  PACKET_EMPTY


struct packet {
	struct packet_header header;
	char *data;

	size_t real;
};

#define INIT_PACKET {{0, 0}, 0}

int send_packet(int fd, int type, void *data);
int recv_packet(int fd, struct packet *p);
void free_packet(struct packet *p);


#endif /* _PACKET_H_ */
