#pragma once

#include "turn_based_game.h"

void initialize_global_ptr_to_game_object(TurnBasedGame* game_ptr_tmp);

void add_string_to_ui_log(const std::string* str);
void add_string_to_ui_log(const std::string str);