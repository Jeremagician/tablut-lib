#ifndef TEST

static size_t get_board_size(struct gaufre *g)
{
	return g->width * g->height;
}

#else

static size_t get_dyn_size(struct struct_io_test *s)
{
	(void)s;
	return 2;
}

#endif
