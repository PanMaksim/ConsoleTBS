#pragma once
#include <iostream>
#include <string_view>
#include <memory>

#include "terrain.h"
#include "creature.h"

namespace terrain {

	namespace battle_tile {

		enum class TileParameters {
			kLandscape,
			kCreature,
			kTileParametersMax
		};

		//enum class BattleTileModifier {
		//	kNoModifier,
		//	kForest,
		//	kRiver,
		//	kTileModifierMax
		//};

		//enum class Effect {
		//	kNoEffect,
		//	kEffectMax
		//};

		//void create_in_length(int print_times, char symbol = ' ') {
		//    for (int num{ 0 }; num != print_times; ++num) {
		//        std::cout << symbol;
		//    }
		//}

		//void create_tile_horizontal_edge() {
		//    std::cout << ' ';
		//    create_in_length(tile_size + 6, tile_horizontal_symbol);
		//    std::cout << '\n';
		//}

		//void create_tile_vertical_edge() {
		//    std::cout << tile_vertical_symbol;
		//    create_in_length(tile_size + 6);
		//    std::cout << tile_vertical_symbol;
		//
		//    //!!!!!!!!!!!!!!! for test
		//    std::cout << '\n';
		//}

		//void print_cell() {
		//    // box "hex"
		//    // print top edge
		//    create_tile_horizontal_edge();
		//    // print side edges
		//    for (int num{ 0 }; num != tile_size; ++num) {
		//        create_tile_vertical_edge();
		//    }
		//    // print bottom edge
		//    create_tile_horizontal_edge();
		//}

		struct BattleTile {

			// what it contains : unit, landscape, effects, etc
			terrain::Type terrain_type_{ terrain::Type::kPlain };
			std::shared_ptr<creature::Creature> creature_{ nullptr }; // owned by Army AND BattleTile

			//std::vector<BattleTileModifier> modifiers_;
			//BattleTileModifier modifier_;
		};

		const std::string_view* get_tile_parameter_name_from_database(TileParameters battle_tile_parameter);
		//void creature_update_battle_tile_effects(Creature* creature, const BattleTile* old_battle_tile, const BattleTile* new_battle_tile);

	}

}