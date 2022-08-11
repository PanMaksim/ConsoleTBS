#pragma once

#include <string_view>
#include <fstream>

enum class CreatureRace {
	kNoRace,
	kHuman,
	kOrc,
	kCreatureRaceMax
};

enum class CreatureStatId {
	kHP,
	kMovementSpeed,
	kATK_ML,
	kDEF_ML,
	kArmor_PHS,
	kDAMAGE_ML_PHS,
	kCreatureStatMax
};

struct CreatureStat {
	int current_;
	int max_;

	CreatureStat() = default;
	CreatureStat(int stat_max);
	~CreatureStat() = default;

	friend std::ifstream& operator>> (std::ifstream &in, CreatureStat& creature_stat);
};

struct CreatureStatMultiplier {
	CreatureStatId stat_id_;
	float multiplier_;
};

struct CreatureComplexID {
	//int faction_id;
	short army_id_;
	int creature_id_; 
};

const std::string_view* creature_database_get_race_naming(CreatureRace race);
const std::string_view* creature_database_get_stat_naming(CreatureStatId stat);
