#include "struct_io.func.h"

unsigned get_name_length(struct hello *s)
{
	return s->name_length;
}

unsigned get_piece_count(struct rules *s)
{
	return s->piece_count;
}

unsigned get_tile_count(struct rules *s)
{
	return s->board_size.x * s->board_size.y;
}

unsigned get_lobby_player_count(struct lobby *s)
{
	return s->player_count;
}

unsigned get_move_count(struct game_state *s)
{
	return s->move_count;
}

#ifdef TEST

unsigned get_dyn8_size(struct sio_test *s)
{
	(void)s;
	return 2;
}

unsigned get_dyns_size(struct sio_test *s)
{
	(void)s;
	return 2;
}

#endif
