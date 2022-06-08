#pragma once

#include <string_view>

enum class CreatureRace {
	kCreatureRaceMin,
	kHuman = kCreatureRaceMin,
	kOrc,
	kCreatureRaceMax
};

enum class CreatureStatId {
	kCreatureStatMin,
	kHP = kCreatureStatMin,
	kMovementSpeed,
	kATK_ML,
	kDEF_ML,
	kArmor_PHS,
	kDAMAGE_ML_PHS,
	kCreatureStatMax
};

struct CreatureStat {
	int current;
	int max;
};

const std::string_view* creature_database_get_race_naming(CreatureRace race);
const std::string_view* creature_database_get_stat_naming(CreatureStatId stat);
