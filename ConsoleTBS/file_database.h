#pragma once

#include <array>

namespace file_database {

	enum class ID {
		kCreatureTemplateDatabase,
		kCreatureNameDatabase,
		kUserInputKeyDescription,
		ID_Max
	};

	extern std::array<bool, static_cast<int>(ID::ID_Max)> status;

}
