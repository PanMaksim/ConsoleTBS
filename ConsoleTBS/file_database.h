#pragma once

#include <array>

enum class FileDatabaseId {
	FileDatabaseIdMin = 0,
	kCreatureTemplateDatabase = FileDatabaseIdMin,
	kCreatureNameDatabase,
	kUserInputDescription,
	FileDatabaseIdMax
};

extern std::array<bool, static_cast<int>(FileDatabaseId::FileDatabaseIdMax)> file_databases_status;

