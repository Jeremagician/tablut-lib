#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "struct_io.h"
#include "struct_io.func.h"


#define BEGIN(name)							\
	size_t struct_io_size_##name(struct name *s)			\
	{								\
		size_t size = 0; int i;					\
		(void)i;
#define FIELD_8(name)							\
		size += sizeof(s->name);
#define FIELD_16(name)							\
		size += sizeof(s->name);
#define FIELD_STRUCT(name, st)						\
		size += struct_io_size_##st(&s->name);
#define FIELD_DYN_8(name, func)						\
		size += func(s);
#define FIELD_DYN_STRUCT(name, st, func)				\
		for (i = 0; i < func(s); i++)				\
			size += struct_io_size_##st(&s->name[i]);
#define END()								\
		return size;						\
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


static int readfull(int fildes, void *buf, size_t nbyte)
{
	ssize_t ret = 0;
	size_t n = 0;
	while (ret >= 0 && n < nbyte) {
		ret = read(fildes, (char*)buf + n, nbyte - n);
		n += ret;
	}
	if (ret == -1)
		fprintf(stderr, "Reading %i bytes: %s\n", (int)nbyte, strerror(errno));
	return ret >= 0;
}


static int alloc_and_readfull(int fildes, void **buf, size_t size)
{
	void *tmp = malloc(size);
	if (!tmp) {
		fprintf(stderr, "Can't allocate dynamic field: %s",
		        strerror(errno));
		return 0;
	}
	if (!readfull(fildes, tmp, size)) {
		free(tmp);
		return 0;
	}
	*buf = tmp;
	return 1;
}


static int writefull(int fildes, const void *buf, size_t nbyte)
{
	ssize_t ret = 0;
	size_t n = 0;
	while (ret >= 0 && n < nbyte) {
		ret = write(fildes, (char*)buf + n, nbyte - n);
		n += ret;
	}
	if (ret == -1)
		fprintf(stderr, "Writing %i bytes: %s\n", (int)nbyte, strerror(errno));
	return ret >= 0;
}


/* pedantic disallow empty macros argument, here it is :P */
#define EMPTY

#define BEGIN(name)							\
	int struct_io_read_##name(struct name *s, int fd)		\
	{								\
		int i;							\
		(void)i;
#define FIELD(name, func)						\
		if (!readfull(fd, &s->name, sizeof(s->name)))		\
			return 0;					\
		s->name = func(s->name);
#define FIELD_8(name)  FIELD(name, EMPTY)
#define FIELD_16(name) FIELD(name, ntohs)
#define FIELD_STRUCT(name, st)						\
		if (!struct_io_read_##st(&s->name, fd))			\
			return 0;
#define FIELD_DYN_8(name, func)						\
		if (!alloc_and_readfull(fd, (void**)&s->name, func(s)))	\
			return 0;
#define FIELD_DYN_STRUCT(name, st, func)				\
		if (!(s->name = malloc(func(s) * sizeof(struct st))))	\
			return 0;					\
		for (i = 0; i < func(s); i++)				\
			if (!struct_io_read_##st(&s->name[i], fd))	\
				return 0;
#define END()								\
		return 1;						\
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


#define BEGIN(name)							\
	int struct_io_write_##name(struct name *s, int fd)		\
	{								\
		uint32_t _tmp__;					\
		int i;							\
		(void)_tmp__; (void)i;
#define FIELD(name, type, func)						\
		_tmp__ = func(s->name);					\
		if (!writefull(fd, &_tmp__, sizeof(s->name)))		\
			return 0;
#define FIELD_8(name)  FIELD(name, uint8_t, EMPTY)
#define FIELD_16(name) FIELD(name, uint16_t, htons)
#define FIELD_STRUCT(name, st)						\
		if (!struct_io_write_##st(&s->name, fd))		\
			return 0;
#define FIELD_DYN_8(name, func)						\
		if (!writefull(fd, s->name, func(s)))			\
			return 0;
#define FIELD_DYN_STRUCT(name, st, func)				\
		for (i = 0; i < func(s); i++)				\
			if (!struct_io_write_##st(&s->name[i], fd))	\
				return 0;
#define END()								\
		return 1;						\
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


/*
 * Freeing won't work if a size of a dynamic field depends of another dynamic
 * field.
 */
#define BEGIN(name)							\
	void struct_io_free_##name(struct name *s)			\
	{								\
		int i;							\
		(void)s; (void)i;
#define FIELD_8(name)
#define FIELD_16(name)
#define FIELD_STRUCT(name, st)						\
		struct_io_free_##st(&s->name);
#define FIELD_DYN_8(name, func)						\
		if (s->name)						\
			free(s->name);
#define FIELD_DYN_STRUCT(name, st, func)				\
		for (i = 0; i < func(s); i++)				\
			struct_io_free_##st(&s->name[i]);
#define END()								\
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END
