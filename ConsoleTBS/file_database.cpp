#include "file_database.h"

#include <array>

//should be changed to bitset
std::array<bool, static_cast<int>(FileDatabaseId::FileDatabaseIdMax)> file_databases_status{ static_cast<int>(FileDatabaseId::FileDatabaseIdMax), false };