#ifndef STRUCT_IO_FUNC_H_
#define STRUCT_IO_FUNC_H_

#include "struct_io.h"

unsigned get_name_length(struct hello *s);
unsigned get_piece_count(struct rules *s);
unsigned get_tile_count(struct rules *s);
unsigned get_lobby_player_count(struct lobby *s);
unsigned get_move_count(struct game_state *s);

#ifdef TEST

unsigned get_dyn8_size(struct sio_test *s);
unsigned get_dyns_size(struct sio_test *s);

#endif

#endif /* STRUCT_IO_FUNC_H_ */
