
#include "turn_based_game.h"

#include <memory>
#include <string>
#include <utility>
#include <chrono>

#include "turn_based_game_global.h"
#include "user_input.h"
#include "creature.h"
#include "creature_actions.h"
#include "terrain.h"

void TurnBasedGame::battle_process() {
    // for testing
    std::chrono::time_point time_start{ std::chrono::high_resolution_clock::now() };
    std::chrono::time_point time_end{ std::chrono::high_resolution_clock::now() };
    std::chrono::milliseconds duration;
    // duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    // std::cout << "Execution time: " << duration.count() << "ms.\n";

    bool new_frame{ true };
    char user_input{};
    do {
        if (new_frame) {
            print_frame();
            clear_ui_log();
            time_end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
            std::cout << "Execution time: " << duration.count() << "ms.\n";
        }
        else { new_frame = true; }

        for (int iter{ 0 }; iter != (kWindowWidth_ / 2) - 6; ++iter) {
            std::cout << ' ';
        }
        std::cout << "User Input: ";
        std::cin >> user_input;
        time_start = std::chrono::high_resolution_clock::now();
        player_coordinate_selection_old_ = player_coordinate_selection_;
        std::cout << "\t\t\t\t\t\t\t\t\t\t\t\t";
        switch (user_input) {
        case UserInput::kShowInputHelp:
            ui_input_help_switch();
            new_frame = true;
            break;
        case UserInput::kMoveSelectionByCoordinate:
            new_frame = player_coordinate_selection_move_by_coordinate_input();
            if (new_frame) {
                battle_map_clear_old_player_selection();
                battle_map_update_player_selection();

                add_string_to_ui_log(
                    "Player selection moved to coordinate (" + std::to_string(player_coordinate_selection_.x + 1) + ", " +
                    std::to_string(player_coordinate_selection_.y + 1) + ") from (" +
                    std::to_string(player_coordinate_selection_old_.x) + ',' +
                    std::to_string(player_coordinate_selection_old_.y) + ')');
            }
            break;
        case UserInput::kMoveSelectionByDirection:
            if (player_coordinate_selection_move_by_direction_input() != nullptr) {
                new_frame = true;
            }
            if (new_frame) {
                battle_map_clear_old_player_selection();
                battle_map_update_player_selection();

                add_string_to_ui_log(
                    "Player seletion moved to coordinate (" + std::to_string(player_coordinate_selection_.x + 1) + ", " +
                    std::to_string(player_coordinate_selection_.y + 1) + ") from (" +
                    std::to_string(player_coordinate_selection_old_.x) + ',' +
                    std::to_string(player_coordinate_selection_old_.y) + ')');
            }
            break;
        case UserInput::kInteract:
            new_frame = interact_with_creature();
            break;
        case UserInput::kTileNumerationSwitch:
            new_frame = battle_map_tile_numeration_switch();
            break;
        case UserInput::kExit:
            return;
        default:
            std::cerr << "Unrecognised input. Do nothing.";
            new_frame = false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        //direction_input.release();
        std::cout << std::endl;
    } while (true);
}

void TurnBasedGame::start_new_battle() {
    std::cout << "Start new Battle\n";
    if (ui_status[UI_Status::kBattleMap]) {
        battle_map_clear();
        create_new_pv_window();

    }

    generate_new_battle_map();
    calculate_battle_map_visual();
    show_battle_map();
    battle_map_create_basic_ui();

    battle_map_show_landscape();

    player_army_.generate_random_army(); //test
    ai_army_.generate_random_army(); //test

    battle_map_add_army(&player_army_, BattleStartStatus::kAttaking);
    battle_map_add_army(&ai_army_, BattleStartStatus::kDefending);

    battle_map_update_player_selection();
}

void TurnBasedGame::battle_map_add_creature(Creature* creature, BattleMapCoordinate coordinate, BattleStartStatus status) {
    (*battle_map_info_)[coordinate.y][coordinate.x].creature_ = creature;
    const std::vector<std::pair<CreatureStatId, float>>* new_terrain_effects{
        terrain_database_get_effects((*battle_map_info_)[coordinate.y][coordinate.x].terrain_type_) };

    for (std::pair<CreatureStatId, float> effect : *new_terrain_effects) {
        creature->apply_stat_multiplier(effect.first, effect.second);
    }

    char creature_head_symbol = ((creature->get_army_id() == 0) ? kCreaturePlayerHeadSymbol_ : kCreatureEnemyHeadSymbol_);
    FrameCoordinate tile_center_coordinate{ battle_map_find_tile_center_frame_coordinate(coordinate) };
    std::array<char, kWindowWidth_>::pointer frame_coordinate_ptr{ frame_[tile_center_coordinate.y].data() + tile_center_coordinate.x };

    // switch for possible another starting ways in future
    switch (status) { // attacking from left
    case BattleStartStatus::kAttaking: // attacking from left
        *(frame_coordinate_ptr - 1) = kCreatureBackSymbol_;
        *frame_coordinate_ptr = kCreatureMiddleSymbol_;
        *(frame_coordinate_ptr + 1) = creature_head_symbol;
        break;
    case BattleStartStatus::kDefending:// defending at right
        *(frame_coordinate_ptr - 1) = creature_head_symbol;
        *frame_coordinate_ptr = kCreatureMiddleSymbol_;
        *(frame_coordinate_ptr + 1) = kCreatureBackSymbol_;
        break;
    }
}

void TurnBasedGame::battle_map_add_army(Army* army, BattleStartStatus status) {
    std::vector<Creature>* army_ptr{ army->get_army_ptr() };
    std::vector<Creature>::pointer army_ptr_current{ army_ptr->data() },
        army_ptr_end{ army_ptr->data() + army_ptr->size() };

    switch (status) {
    case BattleStartStatus::kAttaking: // ATK from left
        for (int battle_map_coordinate_x{}; army_ptr_current != army_ptr_end; ++battle_map_coordinate_x) {

            for (int battle_map_coordinate_y{}, shown_tiles_height{};
                army_ptr_current != army_ptr_end && shown_tiles_height != kBattleMapSizeHeight_;
                ++army_ptr_current, ++battle_map_coordinate_y, ++shown_tiles_height) {

                battle_map_add_creature(army_ptr_current,
                    BattleMapCoordinate{battle_map_coordinate_x, battle_map_coordinate_y}, status);
            }
        }
        break;

    case BattleStartStatus::kDefending: // DEF at right
        for (int battle_map_coordinate_x{ kBattleMapSizeWidth_ - 1 }; army_ptr_current != army_ptr_end; --battle_map_coordinate_x) {

            for (int battle_map_coordinate_y{}, shown_tiles_height{};
                army_ptr_current != army_ptr_end && shown_tiles_height != kBattleMapSizeHeight_;
                ++army_ptr_current, ++battle_map_coordinate_y, ++shown_tiles_height) {

                battle_map_add_creature(army_ptr_current,
                    BattleMapCoordinate{battle_map_coordinate_x, battle_map_coordinate_y}, status);
            }
        }
        break;
    }
}

bool TurnBasedGame::player_coordinate_selection_move_by_coordinate_input() {
    if (!ui_status[UI_Status::kBattleMap]) { return false; }

    std::cout << "Move by coordinate";
    std::cout << '\n' << "Enter x: ";
    int coordinate_x = get_user_input(0, kBattleMapSizeWidth_); // should update it to char for more intuitive exit to '~' like in ofer inputs
    if (coordinate_x == 0) {
        std::cout << "Close coordinate selection";
        return true;
    }
    std::cout << '\n';

    std::cout << "Enter y: ";
    int coordinate_y = get_user_input(0, kBattleMapSizeHeight_);
    if (coordinate_y == 0) {
        std::cout << "Close coordinate selection";
        return true;
    }
    std::cout << '\n';
    
    // try to build fastest way to given tile // Pathfinding AI AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 

    player_coordinate_selection_ = { coordinate_x - 1, coordinate_y - 1 }; // -1 cause of battle_map_info_ array numeration starts from 0
    return true;
}

// cin >> string and then move coordinate_selection until string ends
std::unique_ptr<std::vector<UserInput>> TurnBasedGame::player_coordinate_selection_move_by_direction_input() {
    if (!ui_status[UI_Status::kBattleMap]) { return nullptr; }

    std::cout << "Move by direction";
    std::cout << '\n';
    BattleMapCoordinate player_coordinate_selection_tmp{};
    bool failed{};
    std::unique_ptr<std::vector<UserInput>> direction_log;

    do {
        failed = false;
        player_coordinate_selection_tmp = player_coordinate_selection_;
        direction_log = std::make_unique<std::vector<UserInput>>();

        std::cout << "Enter direction to move: ";
        std::string direction_global;
        std::cin >> direction_global;
        //std::cout << '\n';

        for (char direction : direction_global) {
            switch (direction) {
            case UserInput::kExit:
                std::cout << "Stop movement input";
                return nullptr;
            case UserInput::kMoveUp:
                if (player_coordinate_selection_tmp.y == 0) {
                    std::cerr << "Error, moved too high\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInput::kMoveUp);
                    --player_coordinate_selection_tmp.y;
                }
                break;
            case UserInput::kMoveDown:
                if (player_coordinate_selection_tmp.y == kBattleMapSizeHeight_ - 1) {
                    std::cerr << "Error, moved too low\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInput::kMoveDown);
                    ++player_coordinate_selection_tmp.y;
                }
                break;
            case UserInput::kMoveLeft:
                if (player_coordinate_selection_tmp.x == 0) {
                    std::cerr << "Error, moved too left\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInput::kMoveLeft);
                    --player_coordinate_selection_tmp.x;
                }
                break;
            case UserInput::kMoveRight:
                if (player_coordinate_selection_tmp.x == kBattleMapSizeWidth_ - 1) {
                    std::cerr << "Error, moved too right\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInput::kMoveRight);
                    ++player_coordinate_selection_tmp.x;
                }
                break;
            default:
                std::cerr << "Error, unrecognised input, enter again.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                failed = true;
                break;
            }

            if (failed == true) {
                break;
            }
        }
    } while (failed != false);

    player_coordinate_selection_ = player_coordinate_selection_tmp;
    return direction_log;
}

void TurnBasedGame::battle_map_kill_creature(BattleMapCoordinate killed_creature_coordinate) {
    battle_map_clear_tile_from_creature_image(killed_creature_coordinate);
    (*battle_map_info_)[killed_creature_coordinate.y][killed_creature_coordinate.x].creature_ = nullptr;
}

void TurnBasedGame::check_possible_kill(BattleMapCoordinate creature_coordinate) {
    std::vector<BattleTile>::pointer creature_coordinate_ptr{ (*battle_map_info_)[creature_coordinate.y].data() + creature_coordinate.x };
    if (creature_coordinate_ptr->creature_->get_certain_stat_current_value(CreatureStatId::kHP) <= 0) {
        add_string_to_ui_log(*creature_coordinate_ptr->creature_->get_name() +
            ' ' + '(' + std::to_string(creature_coordinate.y + 1) + ',' + ' ' + std::to_string(creature_coordinate.x + 1) + ") died.");
        battle_map_kill_creature(creature_coordinate);
    }
}

bool TurnBasedGame::move_creature_by_coordinate(BattleMapCoordinate old_coordinate, BattleMapCoordinate new_coordinate) {
    std::vector<BattleTile>::pointer old_coordinate_ptr{ (*battle_map_info_)[old_coordinate.y].data() + old_coordinate.x },
                                        new_coordinate_ptr{ (*battle_map_info_)[new_coordinate.y].data() + new_coordinate.x };

    if (new_coordinate_ptr->creature_ != nullptr) { // sometimes can be unneeded because of check before function call
        std::cerr << "Error, tried to move on already occupied coordinate\n";
        return false;
    }

    // try to build fastest way to given tile // Pathfinding AI AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 

    add_string_to_ui_log(*old_coordinate_ptr->creature_->get_name() + '(' +
        std::to_string(old_coordinate.x) + ',' + ' ' +
        std::to_string(old_coordinate.y) + ") moved to coordinate (" + std::to_string(new_coordinate.x + 1) +
        ',' + ' ' + std::to_string(new_coordinate.y + 1) + ')');

    new_coordinate_ptr->creature_ = old_coordinate_ptr->creature_;
    old_coordinate_ptr->creature_ = nullptr;

    // update applied terrain effects
    if (old_coordinate_ptr->terrain_type_ != new_coordinate_ptr->terrain_type_) {
        Creature* creature{ new_coordinate_ptr->creature_ };
        const std::vector<std::pair<CreatureStatId, float>>* old_terrain_effects{ terrain_database_get_effects(old_coordinate_ptr->terrain_type_) };
        for (std::pair<CreatureStatId, float> effect : *old_terrain_effects) {
            creature->delete_stat_multiplier(effect.first, effect.second);
        }

        const std::vector<std::pair<CreatureStatId, float>>* new_terrain_effects{ terrain_database_get_effects(new_coordinate_ptr->terrain_type_) };
        for (std::pair<CreatureStatId, float> effect : *new_terrain_effects) {
            creature->apply_stat_multiplier(effect.first, effect.second);
        }
    }

    battle_map_clear_tile_from_creature_image(old_coordinate);

    // create creature "image" at new coordinate
    FrameCoordinate tile_center_of_new_coordinate{ battle_map_find_tile_center_frame_coordinate(new_coordinate) };
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr = &(frame_[tile_center_of_new_coordinate.y][tile_center_of_new_coordinate.x]);
    if (new_coordinate_ptr->creature_->get_army_id() == 0) {
        *(frame_coordinate_x_ptr - 1) = kCreatureBackSymbol_;
        *frame_coordinate_x_ptr = kCreatureMiddleSymbol_;
        *(frame_coordinate_x_ptr + 1) = kCreaturePlayerHeadSymbol_;
    }
    else {
        *(frame_coordinate_x_ptr - 1) = kCreatureEnemyHeadSymbol_;
        *frame_coordinate_x_ptr = kCreatureMiddleSymbol_;
        *(frame_coordinate_x_ptr + 1) = kCreatureBackSymbol_;
    }

    return true;
}

bool TurnBasedGame::creature_move_by_input(UserInput input_method) {
    std::vector<BattleTile>::pointer player_coordinate_selection_old_ptr{ 
        (*battle_map_info_)[player_coordinate_selection_.y].data() + player_coordinate_selection_.x };

    bool input_result;
    std::unique_ptr<std::vector<UserInput>> direction_log;
    switch (input_method) {
    case UserInput::kMoveSelectionByDirection:
        direction_log = player_coordinate_selection_move_by_direction_input();
        break;
    case UserInput::kMoveSelectionByCoordinate:
        input_result = player_coordinate_selection_move_by_coordinate_input(); // change to direction log
        break;
    default:
        return false;
    }

    if (direction_log == nullptr) {
        return false;
    }

    // should be separated to new function and modified to check not only moved distance but for example firing range
    // check distance
    {
        int moved_distance_y{},
            moved_distance_x{};

        //int moved_distance{}; // for possible multiplier
        float AP_cost_for_movement{};

        std::vector<std::vector<BattleTile>>::pointer battle_tile_y_ptr{ battle_map_info_->data() + player_coordinate_selection_old_.y };
        const std::pair<float, float>* movement_cost{ terrain_database_get_movement_cost(
                                                    (battle_tile_y_ptr->data() + player_coordinate_selection_old_.x)->terrain_type_) };

        for (std::vector<UserInput>::pointer direction_begin{direction_log->data()}, direction_end{direction_log->data() + direction_log->size()};
            direction_begin < direction_end; ++direction_begin) {

            AP_cost_for_movement += movement_cost->second; // AP for leaving tile

            switch (*direction_begin) {
            case UserInput::kMoveUp:
                --moved_distance_y;
                break;
            case UserInput::kMoveDown:
                ++moved_distance_y;
                break;
            case UserInput::kMoveRight:
                ++moved_distance_x;
                break;
            case UserInput::kMoveLeft:
                --moved_distance_x;
                break;
            default:
                return false;
            }
            //++moved_distance;

            movement_cost = terrain_database_get_movement_cost(
                ((*battle_map_info_)[player_coordinate_selection_old_.y + moved_distance_y])[player_coordinate_selection_old_.x + moved_distance_x].terrain_type_);

            AP_cost_for_movement += movement_cost->first; // AP for going into tile
        }

        if (AP_cost_for_movement > player_coordinate_selection_old_ptr->creature_->get_certain_stat_current_value(CreatureStatId::kMovementSpeed)) {
            std::cerr << "Error, tried to move too far. Used AP: " << AP_cost_for_movement <<
                ". When creature SPD = " << player_coordinate_selection_old_ptr->creature_->get_certain_stat_current_value(CreatureStatId::kMovementSpeed) << '\n';
            player_coordinate_selection_ = player_coordinate_selection_old_;
            return false;
        }
        add_string_to_ui_log("AP used for movement: " + std::to_string(AP_cost_for_movement));
    }


    //if (direction_log != nullptr) {
        //std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr;

        std::vector<BattleTile>::pointer player_coordinate_selection_ptr{ (*battle_map_info_)[player_coordinate_selection_.y].data() + player_coordinate_selection_.x };

        // move on empty battle_tile
        if (player_coordinate_selection_ptr->creature_ == nullptr) {
            move_creature_by_coordinate(player_coordinate_selection_old_, player_coordinate_selection_);
        }
        // macro - attack or main buff on target
        else if (player_coordinate_selection_ptr->creature_ != nullptr) {
            if (player_coordinate_selection_old_ptr->creature_->get_army_id()
                != player_coordinate_selection_ptr->creature_->get_army_id()) {

                add_string_to_ui_log(
                    *player_coordinate_selection_old_ptr->creature_->get_name() +
                     " (" + std::to_string(player_coordinate_selection_old_.x + 1) +
                        ", " + std::to_string(player_coordinate_selection_old_.y + 1) + 
                    ") attacked " + 
                    *player_coordinate_selection_old_ptr->creature_->get_name() +
                     " (" + std::to_string(player_coordinate_selection_.x + 1) +
                        ", " + std::to_string(player_coordinate_selection_.y + 1) + ')');

                CreatureActionResult attack_result{
                    creature_ability_default_attack(player_coordinate_selection_old_ptr->creature_, player_coordinate_selection_ptr->creature_) };

                log_creature_action_result(&attack_result, player_coordinate_selection_old_ptr->creature_, player_coordinate_selection_ptr->creature_);
                check_possible_kill(player_coordinate_selection_);
                check_possible_kill(player_coordinate_selection_old_);
                
            }
            // if moved on another creature owned by player
            else if (player_coordinate_selection_old_ptr->creature_->get_army_id()  // in future should add check for non same army allies
                    == player_coordinate_selection_ptr->creature_->get_army_id()) {

                add_string_to_ui_log(*player_coordinate_selection_old_ptr->creature_->get_name() + 
                    " (" + std::to_string(player_coordinate_selection_old_.x + 1) + ", " 
                    + std::to_string(player_coordinate_selection_old_.y + 1) +
                    ") moved on ally.");

                // !!!!! TO DO
            }
        }
    // }
    // reverse changes
    // else {
    //    player_coordinate_selection_ = player_coordinate_selection_old_;
    //    return false;
    //}
    battle_map_clear_old_player_selection();
    battle_map_update_player_selection();
    return true;
}

bool TurnBasedGame::interact_with_creature() {
    std::cout << "Interact";
    if ((*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x].creature_ == nullptr) {
        std::cerr << "\nNo target to interact with.\n";
        return false;
    }

    // switch to decide what to do with creature
    char user_input{};
    char user_input_end{};
    do {
        for (int iter{ 0 }; iter != (kWindowWidth_ / 2) - 6; ++iter) {
            std::cout << ' ';
        }
        std::cout << "\nInteract User Input: ";
        std::cin >> user_input;

        switch (user_input) {
        case UserInput::kMoveSelectionByCoordinate:
            return creature_move_by_input(UserInput::kMoveSelectionByCoordinate);
        case UserInput::kMoveSelectionByDirection:
            return creature_move_by_input(UserInput::kMoveSelectionByDirection);
        case UserInput::kExit:
            return false;
        default:
            std::cerr << "Unrecognised input. Do nothing.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    } while (true);

    return false;
}
