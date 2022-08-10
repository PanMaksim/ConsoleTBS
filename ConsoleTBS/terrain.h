#pragma once

#include <string>
#include <utility>
#include <vector>

#include "creature_stats.h"

enum class TerrainType {
	kPlain,
	kHill,
	kMountain,
	kTerrainTypeMax
	//DEEPENING,
	//PIT
};

struct TerrainMovementCost {
	float entry_value;
	float leaving_value;
};

struct Terrain {
	char symbol_;
	std::string type_name_;
	TerrainMovementCost movement_cost_; // first for going into BattleTile, second for leaving BattleTile
	std::vector<CreatureStatMultiplier> effects_;
};

const Terrain* terrain_database_get_full_info(TerrainType type);
const TerrainMovementCost* terrain_database_get_movement_cost(TerrainType terrain);
const std::vector<CreatureStatMultiplier>* terrain_database_get_effects(TerrainType terrain);
