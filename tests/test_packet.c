#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packet.h"


static int test_fill(void)
{
	struct packet p = PACKET_INIT;
	struct error test = { 121, 0 };
	int ret = 1;
	size_t length;

	p.pdata = &test;
	fill_packet_header(&p, PACKET_ERROR);

	if (p.header.type != PACKET_ERROR) {
		fprintf(stderr, "Packet type should be %i instead of %i.\n",
		        PACKET_ERROR, p.header.type);
		ret = 0;
	}

	length = sio_size_error(&test);
	if (p.header.length != length) {
		fprintf(stderr, "Packet length should be %lu instead of %lu.\n",
		        (unsigned long)length, (unsigned long)p.header.length);
		ret = 0;
	}

	return ret;
}


static int test_packet_data(void)
{
	struct packet p = PACKET_INIT;
	struct error test = { 210, 0 };

	/*
	 * No needs to allocate a buffer because there are no dynamic field in
	 * sio_subtest.  Avoid a possibility of failure :)
	 */
	char buf[sizeof(struct error)];

	sio_tobuf_error(&test, buf);

	p.header.type = PACKET_ERROR;
	p.header.length = sio_size_error(&test);
	p.ndata = buf;

	if (!get_packet_data(&p)) {
		fprintf(stderr, "get_packet_data: %s\n", strerror(errno));
		return 0;
	}

	if (!p.pdata) {
		fprintf(stderr, "No packet data found\n");
		return 0;
	}

	if (p.pdata_len != sizeof(test)) {
		fprintf(stderr, "Wrong packet data length %lu, expected %lu\n",
		        (unsigned long)p.pdata_len,
		        (unsigned long)sizeof(test));
		return 0;
	}

	return 1;
}


#define fail_on_false(cond)                                             \
	if (!(cond)) {                                                  \
		fprintf(stderr, "%s\n", #cond);                         \
		goto fail;                                              \
	}

int main(void)
{
	fail_on_false(test_fill());
	fail_on_false(test_packet_data());

	return EXIT_SUCCESS;
fail:
	return EXIT_FAILURE;
}
