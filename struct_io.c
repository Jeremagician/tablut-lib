#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "struct_io.h"
#include "struct_io.func.h"


#define BEGIN(name)                                                     \
	size_t sio_size_##name(struct name *s)                          \
	{                                                               \
		size_t size = 0; int i;                                 \
		(void)i;
#define FIELD_8(name)                                                   \
		size += sizeof(s->name);
#define FIELD_16(name)                                                  \
		size += sizeof(s->name);
#define FIELD_STRUCT(name, st)                                          \
		size += sio_size_##st(&s->name);
#define FIELD_DYN_8(name, func)                                         \
		size += func(s);
#define FIELD_DYN_STRUCT(name, st, func)                                \
		for (i = 0; i < func(s); i++)                           \
			size += sio_size_##st(&s->name[i]);
#define END()                                                           \
		return size;                                            \
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


static int adapt_buffer(void **buf, size_t len)
{
	void *tmp;
	tmp = malloc(len);
	if (!tmp)
		return 0;
	*buf = tmp;
	return 1;
}


#define BEGIN(name)                                                     \
	int sio_frombuf_##name(struct name *sp, char *buf)              \
	{                                                               \
		size_t offset = 0;                                      \
		int i; (void)i;                                         \
		void (*const free_s)(struct name*) =                    \
		      sio_free_##name;                                  \
		struct name s = { 0 };
#define FIELD_8(name)                                                   \
		s.name = ((uint8_t*)buf)[offset];                       \
		offset += sizeof(s.name);
#define FIELD_16(name)                                                  \
		s.name = ((uint16_t*)buf)[offset];                      \
		offset += sizeof(s.name);
#define FIELD_STRUCT(name, st)                                          \
		s.name = sp->name;                                      \
		sio_frombuf_##st(&s.name, &buf[offset]);                \
		offset += sio_size_##st(&s.name);
#define FIELD_DYN_8(name, func)                                         \
		if (!adapt_buffer((void**)&s.name, func(&s)))           \
			goto fail;                                      \
		for (i = 0; i < func(&s); i++) {                        \
			s.name[i] = ((uint8_t*)buf)[offset];            \
			offset += sizeof(*s.name);                      \
		}
#define FIELD_DYN_STRUCT(name, st, func)                                \
		if (!adapt_buffer((void**)&s.name, func(&s) * sizeof(struct st))) \
			goto fail;                                      \
		for (i = 0; i < func(&s); i++) {                        \
			sio_frombuf_##st(&s.name[i], &buf[offset]);     \
			offset += sio_size_##st(&s.name[i]);            \
		}
#define END()                                                           \
		*sp = s;                                                \
		return 1;                                               \
	fail:                                                           \
		free_s(&s);                                             \
		return 0;                                               \
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


#define BEGIN(name)                                                     \
	void sio_tobuf_##name(struct name *s, char *buf)                \
	{                                                               \
		size_t offset = 0;                                      \
		int i; (void)i;
#define FIELD_8(name)                                                   \
		buf[offset] = s->name;                                  \
		offset += sizeof(s->name);
#define FIELD_16(name)                                                  \
		buf[offset] = s->name;                                  \
		offset += sizeof(s->name);
#define FIELD_STRUCT(name, st)                                          \
		sio_tobuf_##st(&s->name, &buf[offset]);                 \
		offset += sio_size_##st(&s->name);
#define FIELD_DYN_8(name, func)                                         \
		for (i = 0; i < func(s); i++) {                         \
			buf[offset] = s->name[i];                       \
			offset += sizeof(*s->name);                     \
		}
#define FIELD_DYN_STRUCT(name, st, func)                                \
		for (i = 0; i < func(s); i++) {                         \
			sio_tobuf_##st(&s->name[i], &buf[offset]); \
			offset += sio_size_##st(&s->name[i]);     \
		}
#define END()                                                           \
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
		fprintf(stderr, "Can't allocate dynamic field: %s\n",
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

#define BEGIN(name)                                                     \
	int sio_read_##name(struct name *s, int fd)                     \
	{                                                               \
		int i;                                                  \
		(void)i;
#define FIELD(name, func)                                               \
		if (!readfull(fd, &s->name, sizeof(s->name)))           \
			return 0;                                       \
		s->name = func(s->name);
#define FIELD_8(name)  FIELD(name, EMPTY)
#define FIELD_16(name) FIELD(name, ntohs)
#define FIELD_STRUCT(name, st)                                          \
		if (!sio_read_##st(&s->name, fd))                       \
			return 0;
#define FIELD_DYN_8(name, func)                                         \
		if (!alloc_and_readfull(fd, (void**)&s->name, func(s))) \
			return 0;
#define FIELD_DYN_STRUCT(name, st, func)                                \
		if (!(s->name = malloc(func(s) * sizeof(struct st))))   \
			return 0;                                       \
		for (i = 0; i < func(s); i++)                           \
			if (!sio_read_##st(&s->name[i], fd))            \
				return 0;
#define END()                                                           \
		return 1;                                               \
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


#define BEGIN(name)                                                     \
	int sio_write_##name(struct name *s, int fd)                    \
	{                                                               \
		uint32_t _tmp__;                                        \
		int i;                                                  \
		(void)_tmp__; (void)i;
#define FIELD(name, type, func)                                         \
		_tmp__ = func(s->name);                                 \
		if (!writefull(fd, &_tmp__, sizeof(s->name)))           \
			return 0;
#define FIELD_8(name)  FIELD(name, uint8_t, EMPTY)
#define FIELD_16(name) FIELD(name, uint16_t, htons)
#define FIELD_STRUCT(name, st)                                          \
		if (!sio_write_##st(&s->name, fd))                      \
			return 0;
#define FIELD_DYN_8(name, func)                                         \
		if (!writefull(fd, s->name, func(s)))                   \
			return 0;
#define FIELD_DYN_STRUCT(name, st, func)                                \
		for (i = 0; i < func(s); i++)                           \
			if (!sio_write_##st(&s->name[i], fd))           \
				return 0;
#define END()                                                           \
		return 1;                                               \
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
 * field.  We must first store the size somewhere and then retrieve them when
 * needed.  I don't see rigth now a nice way of doing it.
 */
#define BEGIN(name)                                                     \
	void sio_free_##name(struct name *s)                            \
	{                                                               \
		int i;                                                  \
		(void)s; (void)i;
#define FIELD_8(name)
#define FIELD_16(name)
#define FIELD_STRUCT(name, st)                                          \
		sio_free_##st(&s->name);
#define FIELD_DYN_8(name, func)                                         \
		if (s->name)                                            \
			free(s->name);
#define FIELD_DYN_STRUCT(name, st, func)                                \
		for (i = 0; i < func(s); i++)                           \
			sio_free_##st(&s->name[i]);
#define END()                                                           \
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END
