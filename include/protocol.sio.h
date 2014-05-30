

#ifdef __cplusplus
extern "C" {
#endif

BEGIN(packet_header)
	FIELD_16(type)
	FIELD_16(length)
END()

BEGIN(vec2)
	FIELD_16(x)
	FIELD_16(y)
END()

BEGIN(error)
	FIELD_16(id)
	FIELD_16(desc)
END()

BEGIN(hello)
	FIELD_8(protocol_version)
	FIELD_8(player_id)
	FIELD_8(player_info)
	FIELD_ARR_8(player_name, PLAYER_NAME_LENGTH)
END()

BEGIN(piece)
	FIELD_8(id)
	FIELD_8(move)
	FIELD_8(capt)
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


#ifdef __cplusplus
}
#endif
