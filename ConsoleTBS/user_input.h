#pragma once
#include <utility>
#include <string>
#include <vector>

enum UserInput {
    kExit = '~', // kUserInputMin
    kShowInputHelp = '`', // not key binding, because it shows only available input
    kMoveUp = 'w',
    kMoveDown = 's',
    kMoveLeft = 'a',
    kMoveRight = 'd',
    kInteract = 'e',
    kMoveSelectionByCoordinate = 'Q',
    kMoveSelectionByDirection = 'q',
    kStartBattle = 'b',
    kTileNumerationSwitch = 'z',
};

int get_user_input(int min, int max);
const std::pair<char, std::string_view>* user_input_database_get_main_description(UserInput user_input_symbol);
const std::vector<std::pair<char, std::string_view>>* user_input_database_get_all_description();
// const std::vector<UserInput>* user_input_database_get_input_availability(UI_Status status);