#ifndef STRUCT_IO_H_
#define STRUCT_IO_H_


#include <stdlib.h>
#include <stdint.h>


/*
 * Structure generation.
 *
 * An extra field is added for dynamic entries, user application must not
 * access it or modify it.
 */
#define BEGIN(name)                                                     \
	struct name {
#define FIELD_8(name)                                                   \
		uint8_t name;
#define FIELD_16(name)                                                  \
		uint16_t name;
#define FIELD_STRUCT(name, st)                                          \
		struct st name;
#define FIELD_DYN_8(name, func)                                         \
		uint8_t *name; int _##name##_len;
#define FIELD_DYN_STRUCT(name, st, func)                                \
		struct st *name; int _##name##_len;
#define END()                                                           \
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
 * Initializer to zero.  Portable application should only use this to initialize
 * the structure to zero.
 *
 * Read a structure from a file, write a structure to a file.
 * Return 0 on failure (with errno set), 1 on success.
 *
 * Read or write the struct from or to a buffer.  Assume the buffer has the
 * correct size.
 *
 * When reading the structure from a buffer or a file, you can put a structure
 * filled with 0, or with a structure already passed to this functions and not
 * already freed.  In the later, the allocated field (if any) can be reused.
 * You can not have any expectation on what will be reused or not and
 * application must considere old buffers as invalid, specially if reference
 * where keep somewhere else.
 * On error, it is guaranteed that data inside the structure remains valid, but
 * as stated just above, pointers may have change.
 *
 * Compute the (packed) size of the given structure.
 *
 * Free the given structure.
 */
#define BEGIN(name)                                                     \
	static const struct name name##_ZERO;                           \
                                                                        \
	int sio_read_##name(struct name *s, int fd);                    \
	int sio_write_##name(struct name *s, int fd);                   \
	int sio_frombuf_##name(struct name *s, char *buf);              \
	void sio_tobuf_##name(struct name *s, char *buf);               \
	size_t sio_size_##name(struct name *s);                         \
	void sio_free_##name(struct name *s);
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
