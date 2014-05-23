#include "struct_io.func.h"

int get_name_length(struct hello *s)
{
	return s->name_length;
}

size_t get_piece_count(struct rules *s)
{
	return s->piece_count;
}

size_t get_tile_count(struct rules *s)
{
	return s->board_size.x * s->board_size.y;
}

size_t get_lobby_player_count(struct lobby *s)
{
	return s->player_count;
}

size_t get_move_count(struct game_state *s)
{
	return s->move_count;
}

#ifdef TEST

size_t get_dyn8_size(struct sio_test *s)
{
	(void)s;
	return 2;
}

size_t get_dyns_size(struct sio_test *s)
{
	(void)s;
	return 2;
}

#endif
