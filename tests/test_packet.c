#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "packet.h"
#include "struct_io.h"


static int test_tobuf(void *buf)
{
	struct packet p = INIT_PACKET;
	p.data.substruct_io_test.nothing = 31;

	packet_tobuf(&p, buf);
	return 1;
}


static int test_frombuf(void *buf)
{
	struct packet p = INIT_PACKET;

	packet_frombuf(&p, buf);
	return 1;
}


#define fail_on_false(cond) if (!(cond)) { goto fail; }

int main(void)
{
	int buf;

	fail_on_false(test_tobuf(&buf));
	fail_on_false(test_frombuf(&buf));

	return EXIT_SUCCESS;
fail:
	return EXIT_FAILURE;
}
