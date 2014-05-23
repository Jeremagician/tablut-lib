#ifndef STRUCT_IO_FUNC_H_
#define STRUCT_IO_FUNC_H_

#include "struct_io.h"

int get_name_length(struct hello *s);
size_t get_piece_count(struct rules *s);
size_t get_tile_count(struct rules *s);
size_t get_lobby_player_count(struct lobby *s);
size_t get_move_count(struct game_state *s);

#ifdef TEST

size_t get_dyn8_size(struct sio_test *s);
size_t get_dyns_size(struct sio_test *s);

#endif

#endif /* STRUCT_IO_FUNC_H_ */
