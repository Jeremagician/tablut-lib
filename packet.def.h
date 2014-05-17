/*
 * A packet is just defined as an integer associated with a struct, or with
 * nothing.
 *
 * The fisrt argument is the name of the packet, followed by the associated
 * integer.  It will defined a public constant of the form: PACKET_name.
 *
 * The associated structure (if any) must be defined in struct_io.def.h .
 */

#ifndef TEST

PACKET_STRUCT(MOVE,      1, move)
PACKET_STRUCT(GAUFRE,    2, gaufre)
PACKET_EMPTY (SURRENDER, 3)
PACKET_STRUCT(PLAYER_ID, 4, player_id)

#else

PACKET_STRUCT(TEST, 1, struct_io_test)
PACKET_EMPTY(TESTE, 2)

#endif
