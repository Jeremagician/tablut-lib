#include <assert.h>
#include <stdio.h>
#include "packet.h"


static void assert_packet_type(enum packet_type type)
{
#ifndef NDEBUG
	assert(type >= 0 && type < 1 << (sizeof((struct packet_header*)0)->type * 8));

#define PACKET_STRUCT(name, id, s)                                      \
	if (type == id)                                                 \
		return;
#include "packet.def.h"
#undef  PACKET_STRUCT

	assert(!"Given packet type does not match a packet type");
#endif
}


static size_t get_network_data_length(enum packet_type type, void *pdata)
{
	size_t length = 0;

#define PACKET_STRUCT(name, id, s)                                      \
	if (type == id)                                                 \
		length = sio_size_##s(pdata);
#include "packet.def.h"
#undef  PACKET_STRUCT

	/*
	 * Make sure packet data isn't too big.  Raising an assertion here is
	 * discutable, because it can be tricky to know in advance the size of
	 * the packet data.  But it will force the programmer to found a proper
	 * way to limit the scope of what his application can do.
	 */
	assert(length < PACKET_MAX_NDATA_LENGTH);
	return length;
}


void fill_packet_header(struct packet *p, enum packet_type type)
{
	assert(p != NULL);
	assert(p->pdata != NULL);
	assert_packet_type(type);

	p->header.type = type;
	p->header.length = get_network_data_length(type, p->pdata);
}


static size_t get_packet_data_length(enum packet_type type)
{
#define PACKET_STRUCT(name, id, s)                                      \
	if (type == id)                                                 \
		return sizeof(struct s);
#include "packet.def.h"
#undef  PACKET_STRUCT
	return 0;
}


static int grow_buffer(void **buf, size_t size)
{
	void *tmp;

	tmp = realloc(*buf, size);
	if (!tmp)
		return 0;
	*buf = tmp;
	return 1;
}


#define assert_valid_pdata(p)                                           \
	assert(!((p)->pdata_len == 0 && (p)->pdata != NULL));

int get_packet_data(struct packet *p)
{
	size_t length;

	assert(p != NULL);
	assert(p->ndata != NULL);
	assert_packet_type(p->header.type);
	assert_valid_pdata(p);

	length = get_packet_data_length(p->header.type);

	if (!p->pdata || (p->pdata_len > 0 && p->pdata_len < length)) {
		if (!grow_buffer(&p->pdata, length))
			return 0;
		p->pdata_len = length;
	}

#define PACKET_STRUCT(name, id, s)                                      \
	if (p->header.type == id)                                       \
		return sio_frombuf_##s(p->pdata, p->ndata);
#include "packet.def.h"
#undef  PACKET_STRUCT

	/* Should never be reached */
	return 0;
}


#define assert_valid_ndata(p)                                           \
	assert(!((p)->ndata_len == 0 && (p)->ndata != NULL));

int get_network_data(struct packet *p)
{
	assert(p != NULL);
	assert(p->pdata != NULL);
	assert_packet_type(p->header.type);
	assert_valid_ndata(p);

	if (!p->ndata || (p->ndata_len > 0 && p->ndata_len < p->header.length)) {
		if (!grow_buffer(&p->ndata, p->header.length))
			return 0;
		p->ndata_len = p->header.length;
	}

#define PACKET_STRUCT(name, id, s)                                      \
	if (p->header.type == id)                                       \
		return sio_tobuf_##s(p->pdata, p->ndata), 1;
#include "packet.def.h"
#undef  PACKET_STRUCT

	/* Should never be reached */
	return 0;
}


void free_packet_data(struct packet *p)
{
	assert(p != NULL);
	assert(p->pdata != NULL);
	assert_packet_type(p->header.type);
	assert_valid_pdata(p);

#define PACKET_STRUCT(name, id, s)                                      \
	if (p->header.type == id)                                       \
		sio_free_##s(p->pdata);
#include "packet.def.h"
#undef  PACKET_STRUCT
}


void free_packet(struct packet *p)
{
	assert(p != NULL);
	assert(p->pdata != NULL);
	assert_packet_type(p->header.type);
	assert_valid_pdata(p);
	assert_valid_ndata(p);

	if (p->pdata && p->pdata_len > 0) {
		free_packet_data(p);
		free(p->pdata);
		p->pdata_len = 0;
	}

	if (p->ndata && p->ndata_len > 0) {
		free(p->ndata);
		p->ndata_len = 0;
	}
}
