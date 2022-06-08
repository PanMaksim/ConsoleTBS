#pragma once

#include "turn_based_game.h"

extern TurnBasedGame* game_ptr; // for log strings

void initialize_global_ptr_to_game_object(TurnBasedGame* game_ptr_tmp);