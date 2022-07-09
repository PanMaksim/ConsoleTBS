#pragma once
#include <utility>
#include <string>
#include <vector>

enum UserInputButton {
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

struct UserInputDescription {
    UserInputButton button;
    std::string_view description;
};

int get_user_input(int min, int max);
const UserInputDescription* user_input_database_get_main_description(UserInputButton user_input_symbol);
const std::vector<UserInputDescription>* user_input_database_get_all_description();
// const std::vector<UserInputButton>* user_input_database_get_input_availability(UI_Status status);