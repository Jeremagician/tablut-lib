#ifndef _PACKET_H_
#define _PACKET_H_

#include "struct_io.h"


#define PACKET_STRUCT(name, id, s)					\
	PACKET_##name = id,
enum {
#include "packet.def.h"
	PACKET_NONE = 0
};
#undef  PACKET_STRUCT


#define PACKET_STRUCT(name, id, s)					\
	struct s s;
struct packet {
	struct packet_header header;
	union {
#include "packet.def.h"
	} data;
};
#undef  PACKET_STRUCT


#define INIT_PACKET { 0 }


void packet_frombuf(struct packet *packet, char *buf);
void packet_tobuf(struct packet *packet, char *buf);
size_t packet_length(int type, void *data);
void packet_free(struct packet *packet);


#endif /* _PACKET_H_ */
