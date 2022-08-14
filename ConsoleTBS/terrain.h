#pragma once

#include <string>
#include <utility>
#include <vector>

#include "creature_stats.h"

namespace terrain {

	enum class Type {
		kPlain,
		kHill,
		kMountain,
		kTerrainTypeMax
		//DEEPENING,
		//PIT
	};

	struct MovementCost {
		float entry_value;
		float leaving_value;
	};

	struct Terrain {
		char symbol_;
		std::string type_name_;
		MovementCost movement_cost_; // first for going into BattleTile, second for leaving BattleTile
		std::vector<creature::StatMultiplier> effects_;
	};

	const Terrain* get_full_terrain_info_from_database(Type type);
	const MovementCost* get_movement_cost_from_database(Type terrain);
	const std::vector<creature::StatMultiplier>* get_terrain_effects_from_database(Type terrain);
}
