
#include "terrain.h"

#include <utility>
#include <vector>

#include "creature_stats.h"

using namespace terrain;

const terrain::Terrain terrain_database_types[static_cast<int>(Type::kTerrainTypeMax)]{
	{'P', "Plain", {1, 1}, {}},
	{'H', "Hill", {2, 0.5f}, {{creature::stat::StatId::kDEF_ML, 1.3f}}},
	{'M', "Mountain", {4, 3}, {{creature::stat::StatId::kDEF_ML, 2}}}
};

// can be replaced with opening file and reading from it
const terrain::Terrain* terrain::get_full_terrain_info_from_database(Type terrain) {
	return &terrain_database_types[static_cast<int>(terrain)];
}

const terrain::MovementCost* terrain::get_movement_cost_from_database(Type terrain) {
	return &terrain_database_types[static_cast<int>(terrain)].movement_cost_;
}

const std::vector<creature::stat::StatMultiplier>* terrain::get_terrain_effects_from_database(Type terrain) {
	return &terrain_database_types[static_cast<int>(terrain)].effects_;
}