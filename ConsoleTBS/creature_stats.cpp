
#include "creature_stats.h"

#include <string_view>
#include <fstream>

const std::string_view creature_database_races_naming[static_cast<int>(CreatureRace::kCreatureRaceMax)]{
	"NoRace",
	"Human",
	"Orc"
};

const std::string_view* creature_database_get_race_naming(CreatureRace race) {
	return &creature_database_races_naming[static_cast<int>(race)];
}

const std::string_view creature_database_stats_naming[static_cast<int>(CreatureStatId::kCreatureStatMax)]{
	"HP",
	"SPD",
	"ATK ML",
	"DEF ML",
	"Armor PHS",
	"DMG ML PHS"
};

const std::string_view* creature_database_get_stat_naming(CreatureStatId stat) {
	return &creature_database_stats_naming[static_cast<int>(stat)];
}

CreatureStat::CreatureStat(int stat_max) : current{ stat_max }, max{ stat_max } {}

std::ifstream& operator>>(std::ifstream& in, CreatureStat& creature_stat) {
	in >> creature_stat.max;
	creature_stat.current = creature_stat.max;
	return in;
}