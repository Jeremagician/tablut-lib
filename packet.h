#ifndef PACKET_H_
#define PACKET_H_

/*
 * Provide a way to easily manage packets.
 *
 * During the following, we will call 'network data' the raw stream of data wich
 * comes from the network.  'Packet data' is the network data converted to a
 * structure, wich can be easily manipulated by the user application.
 *
 * This module only provides heper function to manipulate a packet.  It does
 * not aim to handle packet IO, because the user application may have his own
 * way to do it.
 */

#include "struct_io.h"

struct packet {
	struct packet_header header;

	void *ndata;            /* Network data */
	void *pdata;            /* Packet data */
	size_t pdata_len;       /* Length in bytes of packet data */
};

static const struct packet PACKET_INIT = { 0 };

/*
 * Packet type, used to fill the field 'type' of a packet header.
 */
enum packet_type {
#define PACKET_STRUCT(name, id, s)                                      \
	PACKET_##name = id,
#include "packet.def.h"
#undef  PACKET_STRUCT
	PACKET_NONE = 0
};

/*
 * Limit of network data that can be sended using a single packet.
 * Every network data equal or greater than this won't fit in a packet
 * resulting in undefined behavior when sending them, since the packet length
 * will overflow and contains a wrong value.
 *
 * User application must unsure that this limit will never be reached.  One way
 * of doing that is by simulating the maximum packet size according to the
 * wanted paramters, and then refused them if it get above this.
 */
#define PACKET_MAX_NDATA_LENGTH                                         \
	(1 << (sizeof ((struct packet_header*)0)->type * 8))

/*
 * Fill the packet header according to the packet data inside it.
 * Packet data must match the given type, the size of the packet data is
 * ignored.
 */
void fill_packet_header(struct packet *p, enum packet_type type);

/*
 * Convert network data inside the given packet into packet data.
 *
 * Network data must correspond to the packet header.
 *
 * Packet data can be NULL, or already contain a buffer of size pdata_len.
 * In both case, if the packet data don't fit in the given buffer (if any),
 * his size is readjusted.
 *
 * Return 1 on success, 0 on failure with errno set to the corresponding error.
 */
int get_packet_data(struct packet *p);

/*
 * Free the packet data, note that is don't free the buffer itself but only the
 * data inside.  The packet data buffer still remains valid.
 */
void free_packet_data(struct packet *p);

#endif /* PACKET_H_ */
