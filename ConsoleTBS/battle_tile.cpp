
#include "battle_tile.h"

#include <string_view>

using namespace terrain::battle_tile;

const std::string_view battle_tile_database_parameters_naming[static_cast<int>(TileParameters::kTileParametersMax)]{
	"Landscape",
	"Ownership"
};

const std::string_view* terrain::battle_tile::get_tile_parameter_name_from_database(TileParameters battle_tile_parameter) {
	return &battle_tile_database_parameters_naming[static_cast<int>(battle_tile_parameter)];
}

//void creature_update_battle_tile_effects(Creature* creature, const BattleTile* old_battle_tile, const BattleTile* new_battle_tile) {
//
//}