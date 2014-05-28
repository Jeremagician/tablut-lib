#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTOCOL_VERSION 1

enum error_id
{
	ERROR_UNKNOWN        = 0,
	ERROR_UNSUPPORTED    = 1, /* Operation not implemented */
	ERROR_NOTIMPLEMENTED = 2, /* Unsupported protocol */
	ERROR_BAD_PACKET     = 3,
	ERROR_PLAYER_FULL    = 4,
	ERROR_SPECTATOR_FULL = 5,
	ERROR_BAD_MOVE       = 6
};

enum error_bad_move {
	ERROR_BAD_MOVE_TYPE,                   /* Only MOVE type is allowed for client */
	ERROR_BAD_MOVE_SAME_PLACE,             /* Cant' move to the same place */
	ERROR_BAD_MOVE_BAD_DIRECTION,          /* Trying to move in a wrong direction */
	ERROR_BAD_MOVE_CANT_BYPASS_PAWN,       /* Can't pass over another pawn */
	ERROR_BAD_MOVE_CANT_BYPASS_FORTRESS,   /* Can't pass through a fortress */
	ERROR_BAD_MOVE_BAD_COORDINATE_FROM,    /* Invalid 'from' coordinate */
	ERROR_BAD_MOVE_CELL_NOT_FREE,          /* The destination tile is not free */
	ERROR_BAD_MOVE_BAD_COORDINATE_TO,      /* Invalid 'to' coordinate */
	ERROR_BAD_MOVE_BAD_OWNER               /* The pawn is not owned by the player */
};

/* Lobby */
/* 00 = None, 01 = Muscovite, 10 = Swedish */
/* 0 = Player, 1 = Bot */
/* In case of bot the last 5 bit is his difficulty */

#define PLAYER_NAME_LENGTH 32

/* Bitfield */
enum player_info {
	PLAYER_INFO_SPECTATOR = (1 << 0),
	PLAYER_INFO_PROBE     = (1 << 7) /* One-shot connection */
};

enum message_type {
	MESSAGE_TYPE_EVERYONE = 0,
	MESSAGE_TYPE_TEAM = 1,
	MESSAGE_TYPE_PRIVATE = 2
};

enum move_type {
	MOVE_TYPE_POP = 0,
	MOVE_TYPE_DEPOP = 1,
	MOVE_TYPE_MOVE = 2,
	MOVE_TYPE_UNDO = 3
};

enum quit_reason {
	QUIT_REASON_UNKNOWN = 0,
	QUIT_REASON_QUIT = 1,
	QUIT_REASON_RAGEQUIT = 2,
	QUIT_REASON_INTERNAL = 3
};

enum pawn_type {
	PAWN_TYPE_MUSCOVITE = 1,
	PAWN_TYPE_SWEDISH = 2,
	PAWN_TYPE_KING = 3
};


#define SIO_PROTOCOL "protocol.sio.h"
#define SIO_INCLUDE SIO_PROTOCOL
#include "struct_io.h"
#undef  SIO_INCLUDE


unsigned get_piece_count(struct rules *s);
unsigned get_tile_count(struct rules *s);
unsigned get_lobby_player_count(struct lobby *s);
unsigned get_move_count(struct game_state *s);


#endif /* PROTOCOL_H_ */
