
#include "creature_stats.h"

#include <string_view>
#include <fstream>

using namespace creature;

const std::string_view race_naming_database[static_cast<int>(Race::kRaceMax)]{
		"NoRace",
		"Human",
		"Orc"
};

const std::string_view* creature::get_race_naming_from_database(Race race) {
	return &race_naming_database[static_cast<int>(race)];
}

const std::string_view stat_naming_database[static_cast<int>(StatId::kStatMax)]{
	"HP",
	"SPD",
	"ATK ML",
	"DEF ML",
	"Armor PHS",
	"DMG ML PHS"
};

const std::string_view* creature::get_stat_naming_from_database(StatId stat) {
	return &stat_naming_database[static_cast<int>(stat)];
}

creature::Stat::Stat(int stat_max) : current_{ stat_max }, max_{ stat_max } {}


namespace creature {
	std::ifstream& operator>>(std::ifstream& in, Stat& creature_stat) {
		in >> creature_stat.max_;
		creature_stat.current_ = creature_stat.max_;
		return in;
	}
}