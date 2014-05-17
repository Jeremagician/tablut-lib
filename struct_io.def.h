/*
 * Generic structure input/output.
 *
 * Each block BEGIN(name) ... END() define a structure named 'name' which
 * contains the field you put into.
 *
 * Available field for now is FIELD_8, FIELD_16 and FIRLD_DYN.  The list can
 * be easily extended to support 4 bytes width and array of byte(s).  Behind
 * the scenes, each FIELD_* macros define field of type uint*_t.
 *
 * FIELD_DYN is special because it is a field where it size is know only when
 * the fields above has been already retrieved.  The second argument is a
 * function wich take a pointer to the structure and must return the computed
 * size.
 */

BEGIN(packet_header)
	FIELD_8(type)
	FIELD_16(length)
END()

#ifndef TEST

BEGIN(gaufre)
	FIELD_8(state)
	FIELD_16(id)
	FIELD_16(pid)
	FIELD_16(width)
	FIELD_16(height)
	FIELD_DYN(board, get_board_size)
END()

BEGIN(move)
	FIELD_16(x)
	FIELD_16(y)
END()

BEGIN(player_id)
	FIELD_8(id)
END()

#else

BEGIN(struct_io_test)
	FIELD_8(field_8)
	FIELD_16(field_16)
	FIELD_DYN(field_dyn, get_dyn_size)
END()

#endif
