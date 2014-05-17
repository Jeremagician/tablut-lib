/*
 * A packet is just defined as an integer associated with a struct, or with
 * nothing.
 *
 * The first argument is the name of the packet, followed by the associated
 * integer.  It will defined a public constant of the form: PACKET_name.
 *
 * The associated structure (if any) must be defined in struct_io.def.h .
 */

#ifndef TEST

PACKET_STRUCT(ERROR, 0, error)
PACKET_STRUCT(HELLO, 1, hello)
PACKET_STRUCT(LOBBY, 2, lobby)
PACKET_STRUCT(GAME_STATE, 3, game_state)

#else

PACKET_STRUCT(TEST, 1, struct_io_test)
PACKET_EMPTY(TESTE, 2)

#endif
