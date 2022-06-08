#pragma once

#include <string>
#include <utility>
#include <vector>

#include "creature_stats.h"

enum class TerrainType {
	kTerrainTypeMin,
	kPlain = kTerrainTypeMin,
	kHill,
	kMountain,
	kTerrainTypeMax
	//DEEPENING,
	//PIT
};

struct Terrain {
	char symbol_;
	std::string type_name_;
	std::pair<float, float> movement_cost_; // first for going into BattleTile, second for leaving BattleTile
	std::vector<std::pair<CreatureStatId, float>> effects_; // pairs of effect and effect size
};

const Terrain* terrain_database_get_full_info(TerrainType type);
const std::pair<float, float>* terrain_database_get_movement_cost(TerrainType terrain);
const std::vector<std::pair<CreatureStatId, float>>* terrain_database_get_effects(TerrainType terrain);
