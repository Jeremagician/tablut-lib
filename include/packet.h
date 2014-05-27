#ifndef PACKET_H_
#define PACKET_H_

/*
 * Provide a way to easily manage packets.
 *
 * During the following, we will call 'network data' the raw stream of data wich
 * comes from the network.  'Packet data' is the network data converted to a
 * structure, wich can be easily manipulated by the user application.
 *
 * This module only provides helper functions to manipulate a packet.  It does
 * not aim to handle packet IO, because the user application may have his own
 * way to do it.  The user application can of course reuse packet.def.h to
 * generate some functions for IO which fit his needs.
 */

#include "struct_io.h"

/*
 * User can control network data and packet data in the following way.
 *
 * If data is NULL, the corespinding length must be NULL.  If the length is
 * zero but data is valid, then it is assumed that the buffer don't belongs
 * to the packet and is user controled, the buffer will also not be affacted
 * by functions wich manipulate the packet.
 *
 * If data is valid and his size above zero, it is assumed that the data is
 * controlled by the function who manipulate it and can be modify by them.
 * Data will be free by calling free_packet().
 */
struct packet {
	struct packet_header header;

	void *ndata;            /* Network data */
	size_t ndata_len;       /* Length in bytes of network data */
	void *pdata;            /* Packet data */
	size_t pdata_len;       /* Length in bytes of packet data */
};

static const struct packet PACKET_INIT;

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
	(1 << (sizeof((struct packet_header*)0)->type * 8))

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
 * Convert packet data inside the given packet into network data.  Use packet
 * header to found the type of the packet data.
 *
 * Network data can be NULL, or already contains a buffer of size ndata_len.
 * In both case, if the packet data don't fit in the given buffer (if any),
 * his size is readjusted.
 *
 * Return 1 on success, 0 on failure with errno set to the corresponding error.
 */
int get_network_data(struct packet *p);

/*
 * Free the packet data, note that is don't free the buffer itself but only the
 * data inside.  The packet data buffer still remains valid and untouched.
 */
void free_packet_data(struct packet *p);

/*
 * Free data buffers in the packet, if they are not user controlled.  Reset the
 * size if needed.  If packet data contains something, it will be freed using
 * free_packet_data().
 */
void free_packet(struct packet *p);

#endif /* PACKET_H_ */
