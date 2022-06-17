
#include "user_input.h"

#include <iostream>
#include <vector>
#include <string_view>

int get_user_input(int min, int max) {
	int value{};
	while (!(std::cin >> value) || (value < min || value > max)) {
		std::cerr << "Error, out of range or not number. Type again:\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return value;
}

const int user_input_max{ 11 };

const std::vector<UserInputDescription> user_input_database_description{
	{UserInputButton::kExit, "return"},
	{UserInputButton::kShowInputHelp, "show input help"}, // not key binding, because it shows only available input
	{UserInputButton::kMoveUp, "move up"},
	{UserInputButton::kMoveDown, "move down"},
	{UserInputButton::kMoveLeft, "move left"},
	{UserInputButton::kMoveRight, "move right"},
	{UserInputButton::kInteract, "interact"},
	{UserInputButton::kMoveSelectionByCoordinate, "move selection by coordinate"},
	{UserInputButton::kMoveSelectionByDirection, "move selection by direction"},
	{UserInputButton::kStartBattle, "start battle"},
	{UserInputButton::kTileNumerationSwitch, "tile numeration switch"},
};

// not best variant, but enum char is better for input reading, description will be called only by player question
const UserInputDescription* user_input_database_get_main_description(UserInputButton user_input_symbol) {
	switch (user_input_symbol) {
	case UserInputButton::kExit:
		return &user_input_database_description[0];
	case UserInputButton::kShowInputHelp:
		return &user_input_database_description[1];
	case UserInputButton::kMoveUp:
		return &user_input_database_description[2];
	case UserInputButton::kMoveDown:
		return &user_input_database_description[3];
	case UserInputButton::kMoveLeft:
		return &user_input_database_description[4];
	case UserInputButton::kMoveRight:
		return &user_input_database_description[5];
	case UserInputButton::kInteract:
		return &user_input_database_description[6];
	case UserInputButton::kMoveSelectionByCoordinate:
		return &user_input_database_description[7];
	case UserInputButton::kMoveSelectionByDirection:
		return &user_input_database_description[8];
	case UserInputButton::kStartBattle:
		return &user_input_database_description[9];
	case UserInputButton::kTileNumerationSwitch:
		return &user_input_database_description[10];
	default:
		return &user_input_database_description[0];
	}
}

const std::vector<UserInputDescription>* user_input_database_get_all_description() {
	return &user_input_database_description;
}