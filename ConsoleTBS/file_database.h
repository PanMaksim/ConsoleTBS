#pragma once

#include <array>

namespace f_db {

	enum class FileDatabaseId {
		kCreatureTemplateDatabase,
		kCreatureNameDatabase,
		kUserInputDescription,
		FileDatabaseIdMax
	};

	extern std::array<bool, static_cast<int>(FileDatabaseId::FileDatabaseIdMax)> file_databases_status;

}
