
#include "battle_tile.h"

#include <string>

const std::string battle_tile_database_parameters_naming[static_cast<int>(BattleTileParameters::kBattleTileParametersMax)]{
	"Landscape",
	"Ownership"
};

const std::string* battle_tile_database_get_parameter_name(BattleTileParameters battle_tile_parameter) {
	return &battle_tile_database_parameters_naming[static_cast<int>(battle_tile_parameter)];
}

void creature_update_battle_tile_effects(Creature* creature, const BattleTile* old_battle_tile, const BattleTile* new_battle_tile) {

}