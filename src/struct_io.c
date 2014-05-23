#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "struct_io.h"
#include "struct_io.func.h"

/*
 * Code generation with macro make the code less readable and less harder to
 * work on it.  However it is a neat trick and I'm sure it fit in this case
 * very well because structure IO is very redondant, and hard to get it fast.
 */


/*
 * For coherence, functions to convert endianess of byte.
 */
static inline const uint8_t ntohc(uint8_t c) { return c; }
static inline const uint8_t htonc(uint8_t c) { return c; }


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


#define dynflen(s, field)                                               \
	s._##field##_len

static int adapt_dynfield(void **field, int *len, size_t fsize, int wlen)
{
	void *tmp;

	if (wlen <= *len)
		return 1;

	tmp = realloc(*field, *len * fsize);
	if (!tmp)
		return 0;

	*field = tmp;
	*len = wlen;
	return 1;
}


#define popbufval(dest, buf, type, conv) do {                           \
	dest = conv(*(type*)buf);                                       \
	buf += sizeof(type);                                            \
} while (0)

#define popbufstruct(dest, stname, buf) do {                            \
	sio_frombuf_##stname(&dest, buf);                               \
	buf += sio_size_##stname(&dest);                                \
} while (0)

#define popbufdynval(s, field, flen, buf, type, conv) do { int i;       \
	if (!adapt_dynfield((void**)&s.field, &dynflen(s, field),       \
	                    sizeof(type), flen(&s)))                    \
		return 0;                                               \
	for (i = 0; i < dynflen(s, field); i++)                         \
		popbufval(s.field[i], buf, type, conv);                 \
} while (0)

#define popbufdynstruct(s, field, stname, fsize, buf) do { int i;       \
	if (!adapt_dynfield((void**)&s.field, &dynflen(s, field),       \
	                    sizeof(*s.field), fsize(&s)))               \
		return 0;                                               \
	for (i = 0; i < dynflen(s, field); i++)                         \
		popbufstruct(s.field[i], stname, buf);                  \
} while (0)


#define BEGIN(name)                                                     \
	int sio_frombuf_##name(struct name *s, char *buf)               \
	{

#define FIELD_8(name)                                                   \
		popbufval(s->name, buf, uint8_t, ntohc);
#define FIELD_16(name)                                                  \
		popbufval(s->name, buf, uint16_t, ntohs);
#define FIELD_STRUCT(name, st)                                          \
		popbufstruct(s->name, st, buf);
#define FIELD_DYN_8(name, func)                                         \
		popbufdynval((*s), name, func, buf, uint8_t, ntohc);
#define FIELD_DYN_STRUCT(name, st, func)                                \
		popbufdynstruct((*s), name, st, func, buf);

#define END()                                                           \
		return 1;                                               \
	}
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


#define putbufval(buf, val, type, conv) do {                            \
	*(type*)buf = conv(val);                                        \
	buf += sizeof(type);                                            \
} while (0)

#define putbufstruct(buf, sval, stname) do {                            \
	sio_tobuf_##stname(&sval, buf);                                 \
	buf += sio_size_##stname(&sval);                                \
} while (0)

#define putbufdynval(s, field, buf, type, conv) do { int i;             \
	for (i = 0; i < dynflen(s, field); i++)                         \
		putbufval(buf, s.field[i], type, conv);                 \
} while (0)

#define putbufdynstruct(s, field, stname, buf) do { int i;              \
	for (i = 0; i < dynflen(s, field); i++)                         \
		putbufstruct(buf, s.field[i], stname);                  \
} while (0)


#define BEGIN(name)                                                     \
	void sio_tobuf_##name(struct name *s, char *buf)                \
	{

#define FIELD_8(name)                                                   \
		putbufval(buf, s->name, uint8_t, htonc);
#define FIELD_16(name)                                                  \
		putbufval(buf, s->name, uint16_t, htons);
#define FIELD_STRUCT(name, st)                                          \
		putbufstruct(buf, s->name, st);
#define FIELD_DYN_8(name, func)                                         \
		putbufdynval((*s), name, buf, uint8_t, htonc);
#define FIELD_DYN_STRUCT(name, st, func)                                \
		putbufdynstruct((*s), name, st, buf);

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


#define BEGIN(name)                                                     \
	int sio_read_##name(struct name *s, int fd)                     \
	{                                                               \
		int i;                                                  \
		(void)i;
#define FIELD(name, func)                                               \
		if (!readfull(fd, &s->name, sizeof(s->name)))           \
			return 0;                                       \
		s->name = func(s->name);
#define FIELD_8(name)  FIELD(name, ntohc)
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
#define FIELD_8(name)  FIELD(name, uint8_t, htonc)
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
