#include <assert.h>
#include <stdio.h>
#include "packet.h"
#include "struct_io.h"


void packet_frombuf(struct packet *packet, char *buf)
{
#define PACKET_STRUCT(name, id, s)					\
	if (packet->header.type == id)					\
		struct_io_frombuf_##s(&packet->data.s, buf);
#include "packet.def.h"
#undef  PACKET_STRUCT
}


void packet_tobuf(struct packet *packet, char *buf)
{
#define PACKET_STRUCT(name, id, s)					\
	if (packet->header.type == id)					\
		struct_io_tobuf_##s(&packet->data.s, buf);
#include "packet.def.h"
#undef  PACKET_STRUCT
}


size_t packet_length(int type, void *data)
{
#define PACKET_STRUCT(name, id, s)					\
	if (type == id)							\
		return struct_io_size_##s(data);
#include "packet.def.h"
#undef  PACKET_STRUCT
	return 0;
}

void packet_free(struct packet *packet)
{
#define PACKET_STRUCT(name, id, s)					\
	if (packet->header.type == id)					\
		struct_io_free_##s(&packet->data.s);
#include "packet.def.h"
#undef  PACKET_STRUCT
}
