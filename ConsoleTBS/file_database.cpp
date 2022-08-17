#include "file_database.h"

#include <array>

std::array<bool, static_cast<int>(f_db::FileDatabaseId::FileDatabaseIdMax)> f_db::file_databases_status{ 0, 0, 0 };