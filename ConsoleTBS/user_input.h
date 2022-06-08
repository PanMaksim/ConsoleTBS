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

enum UI_Status {
    kWindowsAndInterfacesStatusMin,
    kPlayerViewWindow = kWindowsAndInterfacesStatusMin,
    kUI_Window,
    kBattleMap,
    kCreatureStats,
    kUI_InputHelp,
    kBattleMapTileNumeration,
    kCreatureSelected, // for situations when something will be shown over creature stats
    // kCreatureInteracted, // for future, example when interaction creates new "windows"
    kWindowsAndInterfacesStatusMax,
};

int get_user_input(int min, int max);
const std::pair<char, std::string>* user_input_database_get_main_description(UserInput user_input_symbol);
const std::vector<std::pair<char, std::string>>* user_input_database_get_all_description();
// const std::vector<UserInput>* user_input_database_get_input_availability(UI_Status status);