
#include "creature_stats.h"

#include <string>

const std::string creature_database_races_naming[static_cast<int>(CreatureRace::kCreatureRaceMax)]{
	"Human",
	"Orc"
};

const std::string* creature_database_get_race_naming(CreatureRace race) {
	return &creature_database_races_naming[static_cast<int>(race)];
}

const std::string creature_database_stats_naming[static_cast<int>(CreatureStatId::kCreatureStatMax)]{
	"HP",
	"SPD",
	"ATK ML",
	"DEF ML",
	"Armor PHS",
	"DMG ML PHS"
};

const std::string* creature_database_get_stat_naming(CreatureStatId stat) {
	return &creature_database_stats_naming[static_cast<int>(stat)];
}