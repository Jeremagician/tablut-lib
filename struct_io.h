#ifndef STRUCT_IO_H_
#define STRUCT_IO_H_


#include <stdlib.h>
#include <stdint.h>


/*
 * Structure
 */
#define BEGIN(name)							\
	struct name {
#define FIELD_8(name)							\
		uint8_t name;
#define FIELD_16(name)							\
		uint16_t name;
#define FIELD_STRUCT(name, st)						\
		struct st name;
#define FIELD_DYN_8(name, func)						\
		uint8_t *name;
#define FIELD_DYN_STRUCT(name, st, func)				\
		struct st *name;
#define END()								\
	};
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


/*
 * Read a structure from a file, write a structure to a file.
 * Return 0 on failure (with errno set), 1 on success.
 *
 * Read or write the struct from or to a buffer.  Assume the buffer has the
 * correct size.
 *
 * Compute the (packed) size of the given structure.
 */
#define BEGIN(name)							\
	int struct_io_read_##name(struct name *s, int fd);		\
	int struct_io_write_##name(struct name *s, int fd);		\
	void struct_io_frombuf_##name(struct name *s, char *buf);	\
	void struct_io_tobuf_##name(struct name *s, char *buf);		\
	size_t struct_io_size_##name(struct name *s);			\
	void struct_io_free_##name(struct name *s);
#define FIELD_8(name)
#define FIELD_16(name)
#define FIELD_STRUCT(name, s)
#define FIELD_DYN_8(name, func)
#define FIELD_DYN_STRUCT(name, s, func)
#define END()
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


#endif /* STRUCT_IO_H_ */
