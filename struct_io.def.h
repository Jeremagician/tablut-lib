/*
 * Generic structure input/output.
 *
 * Each block BEGIN(name) ... END() define a structure named 'name' which
 * contains the field you put into.
 *
 * Available field for now is FIELD_8, FIELD_16 and FIELD_DYN.  The list can
 * be easily extended to support 4 bytes width and array of byte(s).  Behind
 * the scenes, each FIELD_* macros define field of type uint*_t.
 *
 * FIELD_DYN is special because it is a field where it size is know only when
 * the fields above has been already retrieved.  The second argument is a
 * function wich take a pointer to the structure and must return the computed
 * size.
 */

BEGIN(packet_header)
	FIELD_16(type)
	FIELD_16(length)
END()

#ifndef TEST

BEGIN(vec2)
	FIELD_16(x)
	FIELD_16(y)
END()

BEGIN(error)
	FIELD_16(id)
END()

BEGIN(hello)
	FIELD_8(protocol_version)
	FIELD_8(player_id)
	FIELD_8(name_length)
	FIELD_DYN_8(name, get_name_length)
END()

BEGIN(piece)
	FIELD_8(id)
	FIELD_8(move)
	FIELD_8(catch)
END()

BEGIN(rules)
	FIELD_8(piece_count)
	FIELD_DYN_STRUCT(pieces, piece, get_piece_count)
	FIELD_STRUCT(board_size, vec2)
	FIELD_DYN_8(tiles, get_tile_count)
END()

BEGIN(lobby_player)
	FIELD_8(id)
	FIELD_8(team)
END()

BEGIN(lobby)
	FIELD_STRUCT(rules, rules)
	FIELD_8(player_count)
	FIELD_DYN_STRUCT(players, lobby_player, get_lobby_player_count)
END()

BEGIN(move)
	FIELD_8(type)
	FIELD_STRUCT(from, vec2)
	FIELD_STRUCT(to  , vec2)
END()

BEGIN(game_state)
	FIELD_8(state)
	FIELD_16(move_count)
	FIELD_DYN_STRUCT(moves, move, get_move_count)
END()

#else

BEGIN(substruct_io_test)
	FIELD_8(nothing)
END()

BEGIN(struct_io_test)
	FIELD_8(f8)
	FIELD_16(f16)
	FIELD_STRUCT(fs, substruct_io_test)
	FIELD_DYN_8(fdyn8, get_dyn8_size)
	FIELD_DYN_STRUCT(fdyns, substruct_io_test, get_dyns_size)
END()

#endif
