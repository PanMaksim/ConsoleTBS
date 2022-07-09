#include "file_database.h"

#include <array>

std::array<bool, static_cast<int>(FileDatabaseId::FileDatabaseIdMax)> file_databases_status{ static_cast<int>(FileDatabaseId::FileDatabaseIdMax), false };