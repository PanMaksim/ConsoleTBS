#include "file_database.h"

#include <array>

std::array<bool, static_cast<int>(file_database::ID::ID_Max)> file_database::status{ 0, 0, 0 };