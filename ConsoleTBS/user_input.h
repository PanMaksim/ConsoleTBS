#pragma once
#include <utility>
#include <string>
#include <vector>

#include "file_database.h"

namespace u_input {

    enum UserInputButton {
        kExit = '~', // kUserInputMin
        kMoveUp = 'w',
        kMoveDown = 's',
        kMoveLeft = 'a',
        kMoveRight = 'd',
        kInteract = 'e',
        kMoveSelectionByDirection = 'q',
        kMoveSelectionByCoordinate = 'Q',
        kStartBattle = 'b',
        kTileNumerationSwitch = 'z',
        kCreatureOwnershipSwitch = 'x',
        kShowInputHelp = '`',
        kUserInputButtonMax = 12
    };

    struct UserInputDescription {
        UserInputButton button_;
        std::string description_;

        UserInputDescription(char button, std::string&& description);
        ~UserInputDescription() = default;
    };

    extern const UserInputDescription description_to_show_input_help_button; // always holded in memory to prevent opening the database each time ui is cleaned (cleaning can be changed to not touch help_button, but in future sometimes there can be potencial another window over ui)

    int get_user_input(int min, int max);
    const UserInputDescription* user_input_database_get_main_description(UserInputButton user_input_symbol);
    const UserInputDescription* user_input_database_get_main_description(int button_number);
    const std::vector<UserInputDescription>* user_input_database_get_all_description();

    void load_user_input_database(file_database::ID database_id);
    void unload_user_input_database(file_database::ID database_id);

}