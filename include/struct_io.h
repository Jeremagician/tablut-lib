#ifndef SIO_INCLUDE
#error "Undefined SIO_INCLUDE"
#endif

/*
 * Generate some functions to help reading/writing data to a file or a buffer.
 * It use network byte order and try to be efficient as possible.
 *
 * Reading or writing directly to a file suffer from some limitations due to
 * dynamic fields.  On error, the file is in invalid state and it can be fatal
 * for files like socket.  This is why reading or writing to a buffer is highly
 * recommended, even if it's needs a completly different way of programming.
 * The buffer approache is often the rigth way of doing IO.
 *
 * Also, reading or writing directly to a file will be very slow compared to
 * reading or writing to a buffer.  So this functions are only there because
 * some applications may not have high requirement for reliability and
 * performances.
 */

#include <stdlib.h>
#include <stdint.h>


/*
 * Structure generation.
 *
 * An extra field is added for dynamic entries, user application must not
 * access it or modify it unless they know what they are doing.
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
#include SIO_INCLUDE
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


#define BEGIN(name)                                                     \
/*
 * Initializer to zero.  Portable application should only use this to
 * initialize the structure to zero.
 */                                                                     \
static const struct name name##_ZERO;                                   \
                                                                        \
/*
 * Read the structure from the given file descriptor.
 *
 * The only accepted structure is a structure initialized to zero or a
 * structure already passed to this function or to sio_frombuf().  On the later
 * case, dynamic fields can be invalidated, wich would make every pointer to
 * them invalid.
 *
 * The function block and terminate when the whole structure is read or when an
 * error occurs.
 *
 * On error, the structure contains undefined data.  But you can re-use it
 * on this function or sio_frombuf().  Even on error, some buffers can remains
 * allocated and must be freed using sio_free().  Most of the time, the file
 * will be in a undefined state.
 *
 * Return 1 on success or 0 on error, with errno set.
 */                                                                     \
int sio_read_##name(struct name *s, int fd);                            \
                                                                        \
/*
 * Write the structure to the given file descriptor.
 *
 * The function block and terminate when the whole structure is write or when an
 * error occurs.
 *
 * On error or on success, the structure remains untouched and completely valid.
 * However, on error, the file may be in an undefined state.
 *
 * Return 1 on success or 0 on error, with errno set.
 */                                                                     \
int sio_write_##name(struct name *s, int fd);                           \
                                                                        \
/*
 * Construct the structure from the given buffer.
 *
 * The only accepted structure is a structure initialized to zero or a
 * structure aready passed to this functions or to sio_read().  On the later
 * case, dynamic field can be invalidated, wich would make every pointer to
 * them invalid.
 *
 * The buffer is never modified during the whole process.
 *
 * Return 1 on success or 0 on error, with errno set.
 */                                                                     \
int sio_frombuf_##name(struct name *s, char *buf);                      \
                                                                        \
/*
 * Put the structure in the given buffer.
 *
 * Assume that the buffer can contains the whole structure.  You can retrieve
 * the size of the structure with sio_size().  During the whole process the
 * structure remains valid.
 */                                                                     \
void sio_tobuf_##name(struct name *s, char *buf);                       \
                                                                        \
/*
 * Retrieve the size of the structure, which is here the size it will take when
 * writed to a file or a buffer.
 */                                                                     \
size_t sio_size_##name(struct name *s);                                 \
                                                                        \
/*
 * Free the given structure.
 *
 * Assume that every dynamic fields of the structure is valid and can be freed
 * using free().  Dynmic fields set to NULL are ignored.
 */                                                                     \
void sio_free_##name(struct name *s);

#define FIELD_8(name)
#define FIELD_16(name)
#define FIELD_STRUCT(name, s)
#define FIELD_DYN_8(name, func)
#define FIELD_DYN_STRUCT(name, s, func)
#define END()
#include SIO_INCLUDE
#undef  BEGIN
#undef  FIELD_8
#undef  FIELD_16
#undef  FIELD_STRUCT
#undef  FIELD_DYN_8
#undef  FIELD_DYN_STRUCT
#undef  END


/* Undef for sanity, applications should undef it too for coherence */
#undef SIO_INCLUDE
