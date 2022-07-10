#pragma once
#include <utility>
#include <string>
#include <vector>

#include "file_database.h"

void test();

enum UserInputButton {
    kExit = '~', // kUserInputMin
    kShowInputHelp = '`', // not key binding, because it shows only available input
    kMoveUp = 'w',
    kMoveDown = 's',
    kMoveLeft = 'a',
    kMoveRight = 'd',
    kInteract = 'e',
    kMoveSelectionByDirection = 'q',
    kMoveSelectionByCoordinate = 'Q',
    kStartBattle = 'b',
    kTileNumerationSwitch = 'z',
};

struct UserInputDescription {
    UserInputButton button_;
    std::string description_;

    UserInputDescription(char button, std::string&& description);
};

extern const UserInputDescription description_to_show_input_help_button; // always holded in memory to prevent opening the database each time ui is cleaned (cleaning can be changed to not touch help_button, but in future sometimes there can be potencial another window over ui)

int get_user_input(int min, int max);
const UserInputDescription* user_input_database_get_main_description(UserInputButton user_input_symbol);
const std::vector<UserInputDescription>* user_input_database_get_all_description();

void open_user_input_database(FileDatabaseId database_id);
void close_user_input_database(FileDatabaseId database_id);