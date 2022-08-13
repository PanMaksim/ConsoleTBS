#pragma once

#include <string_view>
#include <fstream>

namespace creature {

	enum class Race {
		kNoRace,
		kHuman,
		kOrc,
		kRaceMax
	};

	enum class StatId {
		kHP,
		kMovementSpeed,
		kATK_ML,
		kDEF_ML,
		kArmor_PHS,
		kDAMAGE_ML_PHS,
		kStatMax
	};

	struct Stat {
		int current_;
		int max_;

		Stat() = default;
		Stat(int stat_max);
		~Stat() = default;

		friend std::ifstream& operator>> (std::ifstream& in, Stat& creature_stat);
	};

	struct StatMultiplier {
		StatId stat_id_;
		float multiplier_;
	};

	struct ComplexID {
		//int faction_id;
		short army_id_;
		int creature_id_;
	};

	const std::string_view* get_race_naming_from_database(Race race);
	const std::string_view* get_stat_naming_from_database(StatId stat);

}