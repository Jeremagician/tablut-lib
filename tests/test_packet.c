#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packet.h"
#include "struct_io.h"


static int test_send(int tmp)
{
	static uint8_t dyn8[2];
	struct substruct_io_test dyns[2];
	struct struct_io_test test = {3, 1, {0}, &dyn8[0], &dyns[0]};

	if (!send_packet(tmp, PACKET_TEST, &test))
		return 0;
	if (!send_packet(tmp, PACKET_TESTE, NULL))
		return 0;
	return 1;
}


static int test_recv(int tmp)
{
	struct packet p = INIT_PACKET;

	lseek(tmp, 0, SEEK_SET);
	if (!recv_packet(tmp, &p))
		return 0;
	if (p.header.type != PACKET_TEST) {
		fprintf(stderr, "Unexpected paquet %d, expected %d\n",
		        p.header.type, PACKET_TEST);
		return 0;
	}
	if (p.header.length != struct_io_size_struct_io_test(
		    (struct struct_io_test*)p.data)) {
		fprintf(stderr, "Unexpected paquet length %u, expected %u\n",
		        (unsigned)p.header.length,
		        (unsigned)struct_io_size_struct_io_test(
			        (struct struct_io_test*)p.data));
		return 0;
	}

	struct_io_free_struct_io_test((struct struct_io_test*)p.data);

	if (!recv_packet(tmp, &p))
		return 0;
	if (p.header.type != PACKET_TESTE) {
		fprintf(stderr, "Unexpected paquet %d, expected %d\n",
		        p.header.type, PACKET_TESTE);
		return 0;
	}
	if (p.header.length != 0) {
		fprintf(stderr, "Unexpected paquet length %u, expected 0\n",
		        (unsigned)p.header.length);
		return 0;
	}

	free_packet(&p);

	return 1;
}


#define TMP_TEMPLATE "test_packet_XXXXXX"
#define fail_on_false(cond) if (!(cond)) { goto fail; }

int main(void)
{
	char template[] = TMP_TEMPLATE;
	int tmp;

	tmp = mkstemp(template);
	if (tmp == -1) {
		fprintf(stderr, "Temporary file %s: %s\n",
		        template, strerror(errno));
		goto fail;
	}

	fail_on_false(test_send(tmp));
	fail_on_false(test_recv(tmp));

	close(tmp);
	unlink(template);
	return EXIT_SUCCESS;
fail:
	if (tmp != -1) {
		close(tmp);
		unlink(template);
	}
	return EXIT_FAILURE;
}
