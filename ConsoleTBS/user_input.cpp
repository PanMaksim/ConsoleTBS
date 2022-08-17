
#include "user_input.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <execution>

#include "file_database.h"
#include "logger.h"

const UserInputDescription description_to_show_input_help_button{ '`', "show input help"};
std::unique_ptr<std::vector<UserInputDescription>> user_input_database_description;

void load_user_input_database(FileDatabaseId database_id) {
	if (file_databases_status[static_cast<int>(database_id)] == true) {
		runtime_logger::log_in_file("ERROR, tried to open already opened database.", true);
		return;
	}

	std::ifstream txt_database;
	switch (database_id) {
	case FileDatabaseId::kUserInputDescription:
		txt_database.open("TextDatabases/user_input_description_database.txt", std::ios::app);
		if (!txt_database) {
			runtime_logger::log_in_file("ERROR, database not found.", true);
		}

		user_input_database_description = std::make_unique<std::vector<UserInputDescription>>();

		int database_size;
		txt_database >> database_size;
		user_input_database_description->reserve(database_size);

		for (int readed_description_counter{}; readed_description_counter != database_size; ++readed_description_counter) {
			char button_tmp;
			txt_database >> button_tmp;
			std::string str_tmp;
			std::getline(txt_database, str_tmp);
			user_input_database_description->emplace_back(button_tmp, std::move(str_tmp));
		}
		break;
	default:
		runtime_logger::log_in_file("Error, tried to open unknown database.", true);
		break;
	}

	file_databases_status[static_cast<int>(database_id)] = true;
}
void unload_user_input_database(FileDatabaseId database_id) {
	if (file_databases_status[static_cast<int>(database_id)] == false) {
		runtime_logger::log_in_file("ERROR, tried to close unopened database.", true);
		return;
	}

	switch (database_id) {
	case FileDatabaseId::kUserInputDescription:
		user_input_database_description.reset();
		break;
	default:
		runtime_logger::log_in_file("Error, tried to close unknown database.", true);
	}

	file_databases_status[static_cast<int>(database_id)] = false;
}

int get_user_input(int min, int max) {
	int value{};
	while (!(std::cin >> value) || (value < min || value > max)) {
		std::cerr << "Error, out of range or not number. Type again:\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return value;
}

// not best variant, but enum char is better for input reading, description_ will be called only if player will ask for it
const UserInputDescription* user_input_database_get_main_description(UserInputButton user_input_symbol) {
	if (file_databases_status[static_cast<int>(FileDatabaseId::kUserInputDescription)] == false) {
		runtime_logger::log_in_file("ERROR, tried to get input description when database is not open.", true);
	}

	switch (user_input_symbol) {
	case UserInputButton::kShowInputHelp:
		return &description_to_show_input_help_button;
	case UserInputButton::kExit:
		return &(*user_input_database_description)[0];
	case UserInputButton::kMoveUp:
		return &(*user_input_database_description)[1];
	case UserInputButton::kMoveDown:
		return &(*user_input_database_description)[2];
	case UserInputButton::kMoveLeft:
		return &(*user_input_database_description)[3];
	case UserInputButton::kMoveRight:
		return &(*user_input_database_description)[4];
	case UserInputButton::kInteract:
		return &(*user_input_database_description)[5];
	case UserInputButton::kMoveSelectionByDirection:
		return &(*user_input_database_description)[6];
	case UserInputButton::kMoveSelectionByCoordinate:
		return &(*user_input_database_description)[7];
	case UserInputButton::kStartBattle:
		return &(*user_input_database_description)[8];
	case UserInputButton::kTileNumerationSwitch:
		return &(*user_input_database_description)[9];
	default:
		return &(*user_input_database_description)[0];
	}
}

const std::vector<UserInputDescription>* user_input_database_get_all_description() {
	if (file_databases_status[static_cast<int>(FileDatabaseId::kUserInputDescription) == false]) {
		runtime_logger::log_in_file("ERROR, tried to get input description when database is not open.", true);
	}

	return &(*user_input_database_description);
}

UserInputDescription::UserInputDescription(char button, std::string&& description) : button_{ static_cast<UserInputButton>(button) }, description_{ description }{}