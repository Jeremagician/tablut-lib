#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packet.h"
#include "struct_io.h"


static int test_fill(void)
{
	struct packet p = PACKET_INIT;
	struct substruct_io_test test = { 121 };
	int ret = 1;
	size_t length;

	p.pdata = &test;
	fill_packet_header(&p, PACKET_TEST);

	if (p.header.type != PACKET_TEST) {
		fprintf(stderr, "Packet type should be %i instead of %i.\n",
		        PACKET_TEST, p.header.type);
		ret = 0;
	}

	length = struct_io_size_substruct_io_test(&test);
	if (p.header.length != length) {
		fprintf(stderr, "Packet length should be %zi instead of %zi.\n",
		        length, p.header.length);
		ret = 0;
	}

	return ret;
}


static int test_packet_data(void)
{
	struct packet p = PACKET_INIT;
	struct substruct_io_test test = { 210 };
	char buf[sizeof(test)];

	struct_io_tobuf_substruct_io_test(&test, buf);

	p.header.type = PACKET_TEST;
	p.header.length = struct_io_size_substruct_io_test(&test);
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
		fprintf(stderr, "Wrong packet data %zi, expected %zi\n",
		        p.pdata_len, sizeof(test));
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
