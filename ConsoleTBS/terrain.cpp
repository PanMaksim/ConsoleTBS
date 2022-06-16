
#include "terrain.h"

#include <utility>
#include <vector>

#include "creature_stats.h"

const Terrain terrain_database_types[static_cast<int>(TerrainType::kTerrainTypeMax)]{
	{'P', "Plain", {1, 1}, {}},
	{'H', "Hill", {2, 0.5f}, {{CreatureStatId::kDEF_ML, 1.3f}}},
	{'M', "Mountain", {4, 3}, {{CreatureStatId::kDEF_ML, 2}}}
};

// can be replaced with opening file and reading from it
const Terrain* terrain_database_get_full_info(TerrainType terrain) {
	return &terrain_database_types[static_cast<int>(terrain)];
}

const std::pair<float, float>* terrain_database_get_movement_cost(TerrainType terrain) {
	return &terrain_database_types[static_cast<int>(terrain)].movement_cost_;
}

const std::vector<CreatureStatMultiplier>* terrain_database_get_effects(TerrainType terrain) {
	return &terrain_database_types[static_cast<int>(terrain)].effects_;
}