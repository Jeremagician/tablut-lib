#ifndef COMMON_STRUCT_IO_H_
#define COMMON_STRUCT_IO_H_


#include <stdlib.h>
#include <stdint.h>


/*
 * Structure and size of the packed structure.
 */
#define BEGIN(name)							\
	struct name {
#define FIELD_8(name)							\
		uint8_t name;
#define FIELD_16(name)							\
		uint16_t name;
#define FIELD_DYN(name, func)						\
		void* name;
#define END()								\
	};
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_DYN
#undef  END


/*
 * Read a structure from a file, write a structure to a file.
 * Return 0 on failure (with errno set), 1 on success.
 *
 * Compute the (packed) size of the given structure.
 */
#define BEGIN(name)							\
	int struct_io_read_##name(struct name *s, int fd);		\
	int struct_io_write_##name(struct name *s, int fd);		\
	size_t struct_io_size_##name(struct name *s);			\
	void struct_io_free_##name(struct name *s);
#define FIELD_8(name)
#define FIELD_16(name)
#define FIELD_DYN(name, func)
#define END()
#include "struct_io.def.h"
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_DYN
#undef  END


#endif /* COMMON_STRUCT_IO_H_ */
