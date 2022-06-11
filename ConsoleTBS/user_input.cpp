
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

const std::vector<std::pair<char, std::string_view>> user_input_database_description{
	{UserInput::kExit, "return"},
	{UserInput::kShowInputHelp, "show input help"}, // not key binding, because it shows only available input
	{UserInput::kMoveUp, "move up"},
	{UserInput::kMoveDown, "move down"},
	{UserInput::kMoveLeft, "move left"},
	{UserInput::kMoveRight, "move right"},
	{UserInput::kInteract, "interact"},
	{UserInput::kMoveSelectionByCoordinate, "move selection by coordinate"},
	{UserInput::kMoveSelectionByDirection, "move selection by direction"},
	{UserInput::kStartBattle, "start battle"},
	{UserInput::kTileNumerationSwitch, "tile numeration switch"},
};

// not best variant, but enum char is better for input reading, description will be called only by player question
const std::pair<char, std::string_view>* user_input_database_get_main_description(UserInput user_input_symbol) {
	switch (user_input_symbol) {
	case UserInput::kExit:
		return &user_input_database_description[0];
	case UserInput::kShowInputHelp:
		return &user_input_database_description[1];
	case UserInput::kMoveUp:
		return &user_input_database_description[2];
	case UserInput::kMoveDown:
		return &user_input_database_description[3];
	case UserInput::kMoveLeft:
		return &user_input_database_description[4];
	case UserInput::kMoveRight:
		return &user_input_database_description[5];
	case UserInput::kInteract:
		return &user_input_database_description[6];
	case UserInput::kMoveSelectionByCoordinate:
		return &user_input_database_description[7];
	case UserInput::kMoveSelectionByDirection:
		return &user_input_database_description[8];
	case UserInput::kStartBattle:
		return &user_input_database_description[9];
	case UserInput::kTileNumerationSwitch:
		return &user_input_database_description[10];
	}

	return &user_input_database_description[0];
}

const std::vector<std::pair<char, std::string_view>>* user_input_database_get_all_description() {
	return &user_input_database_description;
}