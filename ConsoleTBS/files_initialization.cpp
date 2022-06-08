
#include "files_initialization.h"

#include "turn_based_game.h"

TurnBasedGame* game_ptr; // for log strings

void initialize_global_ptr_to_game_object(TurnBasedGame* game_ptr_tmp) {
	game_ptr = game_ptr_tmp;
}