#include <assert.h>
#include <stdio.h>
#include "packet.h"
#include "struct_io.h"


size_t length_from_type(int type)
{
#define PACKET_STRUCT(name, id, s)					\
	if (type == PACKET_##name)					\
		return sizeof(struct s);
#define PACKET_EMPTY(name, id)						\
	if (type == PACKET_##name)					\
		return 0;
#include "packet.def.h"
#undef  PACKET_STRUCT
#undef  PACKET_EMPTY
	return 0;
}


size_t packed_length_from_type(int type, void *data)
{
	(void)data;
#define PACKET_STRUCT(name, id, s)					\
	if (type == PACKET_##name)					\
		return struct_io_size_##s(data);
#define PACKET_EMPTY(name, id)						\
	if (type == PACKET_##name)					\
		return 0;
#include "packet.def.h"
#undef  PACKET_STRUCT
#undef  PACKET_EMPTY
	return 0;
}


int send_packet(int fd, int type, void *data)
{
	struct packet p = INIT_PACKET;

	if (fd == -1)
		return 1;

	p.header.type = type;
	p.header.length = packed_length_from_type(type, data);

	if (!struct_io_write_packet_header(&p.header, fd))
		return 0;

#define PACKET_STRUCT(name, id, s)					\
	if (type == PACKET_##name)					\
		return struct_io_write_##s(data, fd);
#define PACKET_EMPTY(name, id)						\
	if (type == PACKET_##name)					\
		return 1;
#include "packet.def.h"
#undef  PACKET_STRUCT
#undef  PACKET_EMPTY
	return 1;
}


int grow(struct packet *p)
{
	void *tmp;
	size_t length;

	length = length_from_type(p->header.type);
	if (length < p->real)
		return 1;
	tmp = realloc(p->data, length);
	if (!tmp) {
		fprintf(stderr, "Can't grow packet from %u to %u\n",
		        (unsigned)p->real, (unsigned)length);
		return 0;
	}
	p->data = tmp;
	p->real = length;
	return 1;
}


int recv_packet(int fd, struct packet *p)
{
	if (!struct_io_read_packet_header(&p->header, fd))
		return 0;
	if (!grow(p))
		return 0;
#define PACKET_STRUCT(name, id, s)					\
	if (p->header.type == PACKET_##name)				\
		return struct_io_read_##s((struct s*)p->data, fd);
#define PACKET_EMPTY(name, id)						\
	if (p->header.type == PACKET_##name)				\
		return 1;
#include "packet.def.h"
#undef  PACKET_STRUCT
#undef  PACKET_EMPTY
	return 1;
}


void free_packet(struct packet *p)
{
	assert(p != NULL);
	if (p->data)
		free(p->data);
}
