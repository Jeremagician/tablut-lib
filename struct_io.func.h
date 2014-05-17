#ifndef TEST

static int get_name_length(struct hello *s)
{
	return s->name_length;
}

static size_t get_piece_count(struct rules *s)
{
	return s->piece_count;
}

static size_t get_tile_count(struct rules *s)
{
	return s->board_size.x * s->board_size.y;
}

static size_t get_lobby_player_count(struct lobby *s)
{
	return s->player_count;
}

static size_t get_move_count(struct game_state *s)
{
	return s->move_count;
}

#else

static size_t get_dyn8_size(struct struct_io_test *s)
{
	(void)s;
	return 2;
}

static size_t get_dyns_size(struct struct_io_test *s)
{
	(void)s;
	return 2;
}

#endif
