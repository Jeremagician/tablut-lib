#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "struct_io.h"
#include "struct_io.func.h"


static inline void random_init(struct sio_test *s)
{
	static uint8_t dyn8[2];
	static struct sio_subtest dyns[2];

	dyn8[0] = random();
	dyn8[1] = random();
	dyns[0].nothing = random();
	dyns[1].nothing = random();

	s->f8   = random();
	s->f16  = random();
	s->fs.nothing = random();
	s->fdyn8 = &dyn8[0];
	s->fdyns = &dyns[0];
}


static void dump_buf(void *buf, size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
		fprintf(stderr, "%02X",
		        (unsigned int)*(((uint8_t*)buf) + i));
}


static int compare_field(const char *name,
                         void *original, void *target, size_t size)
{
	if (memcmp(original, target, size)) {
		fprintf(stderr, "Fields %s different:\n", name);
		fprintf(stderr, "\toriginal: "); dump_buf(original, size); fputc('\n', stderr);
		fprintf(stderr, "\ttarget: "); dump_buf(target, size); fputc('\n', stderr);
		return 0;
	}
	return 1;
}


#define return_on_different(field, original, target)			\
	if (!compare_field(#field, &original->field, &target->field,	\
	                   sizeof(target->field)))			\
		return 0;
#define return_on_different_dyn(field, original, target, size)		\
	if (!compare_field(#field, original->field, target->field,	\
	                   sizeof(*target->field) * (size_t)size))	\
		return 0;

static int compare(struct sio_test *original,
                   struct sio_test *test)
{
	return_on_different(f8, original, test);
	return_on_different(f16, original, test);
	return_on_different(fs.nothing, original, test);
	return_on_different_dyn(fdyn8, original, test, get_dyn8_size(test));
	return_on_different_dyn(fdyns, original, test, get_dyns_size(test));
	return 1;
}


#define TMP_TEMPLATE "test_struct_io_XXXXXX"
#define fail_on_false(cond)						\
	if (!(cond)) {							\
		fprintf(stderr, "%s failed\n", #cond);			\
		goto fail;						\
	}

int main(void)
{
	struct sio_test original, test = sio_test_ZERO;
	char template[] = TMP_TEMPLATE;
	int tmp;

	tmp = mkstemp(template);
	if (tmp == -1) {
		fprintf(stderr, "Temporary file %s: %s",
		        TMP_TEMPLATE, strerror(errno));
		return EXIT_FAILURE;
	}

	srandom(time(NULL));
	random_init(&original);
	fail_on_false(sio_write_sio_test(&original, tmp));
	lseek(tmp, 0, SEEK_SET);
	fail_on_false(sio_read_sio_test(&test, tmp));
	fail_on_false(compare(&original, &test));

	close(tmp);
	unlink(template);
	return EXIT_SUCCESS;
fail:
	close(tmp);
	unlink(template);
	return EXIT_FAILURE;
}
