#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "struct_io.h"
#include "struct_io.func.h"


static inline void random_init(struct struct_io_test *s)
{
	static uint8_t dyn[2];
	dyn[0] = random();
	dyn[1] = random();

	s->field_8   = random();
	s->field_16  = random();
	s->field_dyn = &dyn[0];
}


static int compare(struct struct_io_test *original,
                   struct struct_io_test *test)
{
	if (memcmp(&original->field_8, &test->field_8, sizeof(test->field_8))) {
		fprintf(stderr,
		        "Fields field_8 different: got %u, expected %u\n",
		        (unsigned)original->field_8, (unsigned)test->field_8);
		return 0;
	}
	if (memcmp(&original->field_16, &test->field_16, sizeof(test->field_16))) {
		fprintf(stderr,
		        "Fields field_16 different: got %u, expected %u\n",
		        (unsigned)original->field_16, (unsigned)test->field_16);
		return 0;
	}
	if (get_dyn_size(original) != get_dyn_size(test)) {
		fprintf(stderr, "Different size for dynamic field: %u and %u\n",
			(unsigned)get_dyn_size(original), (unsigned)get_dyn_size(test));
		return 0;
	}
	if (memcmp(original->field_dyn, test->field_dyn, get_dyn_size(test))) {
		fprintf(stderr,
		        "Fields field_dyn different: got %u, expected %u\n",
		        *(unsigned*)original->field_dyn, *(unsigned*)test->field_dyn);
		return 0;
	}
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
	struct struct_io_test original, test = {0};
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
	fail_on_false(struct_io_write_struct_io_test(&original, tmp));
	lseek(tmp, 0, SEEK_SET);
	fail_on_false(struct_io_read_struct_io_test(&test, tmp));
	fail_on_false(compare(&original, &test));

	close(tmp);
	unlink(template);
	return EXIT_SUCCESS;
fail:
	close(tmp);
	unlink(template);
	return EXIT_FAILURE;
}
