
#include "turn_based_game.h"

#include <memory>
#include <string>
#include <sstream>
#include <utility>
#include <chrono>
#include <execution>

#include "turn_based_game_global.h"
#include "logger.h"
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

    const std::vector<UserInputButton> allowed_user_input{
            UserInputButton::kExit,
            UserInputButton::kMoveSelectionByCoordinate,
            UserInputButton::kMoveSelectionByDirection,
            UserInputButton::kInteract,
            UserInputButton::kTileNumerationSwitch
    };
    bool new_frame{ true };
    char user_input{};
    do {
        if (new_frame) {
            print_frame();
            clear_ui_log();
            time_end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
            {
                std::stringstream sstr;
                sstr << "Execution time: " << duration.count() << "ms.";
                log_in_file(sstr);
            }
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
        case UserInputButton::kShowInputHelp:
            ui_input_help_switch(allowed_user_input);
            new_frame = true;
            break;
        case UserInputButton::kMoveSelectionByCoordinate:
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
        case UserInputButton::kMoveSelectionByDirection:
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
        case UserInputButton::kInteract:
            new_frame = interact_with_creature();
            break;
        case UserInputButton::kTileNumerationSwitch:
            new_frame = battle_map_tile_numeration_switch();
            break;
        case UserInputButton::kExit:
            return;
        default:
            std::cerr << "Unrecognised input. Do nothing.";
            new_frame = false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
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

void TurnBasedGame::battle_map_add_creature(Creature* creature_ptr, BattleMapCoordinate battle_map_coordinate, BattleStartStatus battle_status) {
    (*battle_map_info_)[battle_map_coordinate.y][battle_map_coordinate.x].creature_ = creature_ptr;
    const std::vector<CreatureStatMultiplier>* new_terrain_effects{
        terrain_database_get_effects((*battle_map_info_)[battle_map_coordinate.y][battle_map_coordinate.x].terrain_type_) };

    std::for_each(std::execution::par_unseq, new_terrain_effects->begin(), new_terrain_effects->end(), 
        [&creature_ptr](CreatureStatMultiplier effect) { creature_ptr->apply_stat_multiplier(effect); });

    char creature_head_symbol = ((creature_ptr->get_army_id() == 0) ? kCreaturePlayerHeadSymbol_ : kCreatureEnemyHeadSymbol_);
    FrameCoordinate tile_center_coordinate{ battle_map_find_tile_center_frame_coordinate(battle_map_coordinate) };
    std::array<char, kWindowWidth_>::pointer frame_coordinate_ptr{ frame_[tile_center_coordinate.y].data() + tile_center_coordinate.x };

    // switch for possible another starting ways in future
    switch (battle_status) {
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

void TurnBasedGame::battle_map_add_army(Army* army_ptr, BattleStartStatus battle_status) {
    std::vector<Creature>* army_vector_ptr{ army_ptr->get_army_ptr() };
    std::vector<Creature>::pointer army_vector_ptr_current{ army_vector_ptr->data() },
        army_vector_ptr_end{ army_vector_ptr->data() + army_vector_ptr->size() };

    switch (battle_status) {
    case BattleStartStatus::kAttaking: // ATK from left
        for (int battle_map_coordinate_x{}; army_vector_ptr_current != army_vector_ptr_end; ++battle_map_coordinate_x) {

            for (int battle_map_coordinate_y{}, shown_tiles_height{};
                army_vector_ptr_current != army_vector_ptr_end && shown_tiles_height != kBattleMapSizeHeight_;
                ++army_vector_ptr_current, ++battle_map_coordinate_y, ++shown_tiles_height) {

                battle_map_add_creature(army_vector_ptr_current,
                    BattleMapCoordinate{battle_map_coordinate_x, battle_map_coordinate_y}, battle_status);
            }
        }
        break;

    case BattleStartStatus::kDefending: // DEF at right
        for (int battle_map_coordinate_x{ kBattleMapSizeWidth_ - 1 }; army_vector_ptr_current != army_vector_ptr_end; --battle_map_coordinate_x) {

            for (int battle_map_coordinate_y{}, shown_tiles_height{};
                army_vector_ptr_current != army_vector_ptr_end && shown_tiles_height != kBattleMapSizeHeight_;
                ++army_vector_ptr_current, ++battle_map_coordinate_y, ++shown_tiles_height) {

                battle_map_add_creature(army_vector_ptr_current,
                    BattleMapCoordinate{battle_map_coordinate_x, battle_map_coordinate_y}, battle_status);
            }
        }
        break;
    }
}

bool TurnBasedGame::player_coordinate_selection_move_by_coordinate_input() {
    if (!ui_status[UI_Status::kBattleMap]) { return false; }

    std::cout << "Move by coordinate";
    std::cout << '\n' << "Enter x: ";
    int coordinate_x = get_user_input(0, kBattleMapSizeWidth_);
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
    
    player_coordinate_selection_ = { coordinate_x - 1, coordinate_y - 1 }; // -1 cause of battle_map_info_ array numeration starts from 0
    return true;
}

// cin >> string and then move coordinate_selection until string ends
std::unique_ptr<std::vector<UserInputButton>> TurnBasedGame::player_coordinate_selection_move_by_direction_input() {
    if (!ui_status[UI_Status::kBattleMap]) { return nullptr; }

    std::cout << "Move by direction\n";
    BattleMapCoordinate player_coordinate_selection_tmp{};
    bool failed{};
    std::unique_ptr<std::vector<UserInputButton>> direction_log;

    do {
        failed = false;
        player_coordinate_selection_tmp = player_coordinate_selection_;
        direction_log = std::make_unique<std::vector<UserInputButton>>();

        std::cout << "Enter direction to move: ";
        std::string direction_global;
        std::cin >> direction_global;

        for (char direction : direction_global) {
            switch (direction) {
            case UserInputButton::kExit:
                std::cout << "Stop movement input";
                return nullptr;
            case UserInputButton::kMoveUp:
                if (player_coordinate_selection_tmp.y == 0) {
                    std::cerr << "Error, moved too high\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInputButton::kMoveUp);
                    --player_coordinate_selection_tmp.y;
                }
                break;
            case UserInputButton::kMoveDown:
                if (player_coordinate_selection_tmp.y == kBattleMapSizeHeight_ - 1) {
                    std::cerr << "Error, moved too low\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInputButton::kMoveDown);
                    ++player_coordinate_selection_tmp.y;
                }
                break;
            case UserInputButton::kMoveLeft:
                if (player_coordinate_selection_tmp.x == 0) {
                    std::cerr << "Error, moved too left\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInputButton::kMoveLeft);
                    --player_coordinate_selection_tmp.x;
                }
                break;
            case UserInputButton::kMoveRight:
                if (player_coordinate_selection_tmp.x == kBattleMapSizeWidth_ - 1) {
                    std::cerr << "Error, moved too right\n";
                    failed = true;
                }
                else {
                    direction_log->push_back(UserInputButton::kMoveRight);
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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!TMP
Army* TurnBasedGame::find_army_by_owned_creature(Creature* creature_ptr) {
    int army_id = creature_ptr->get_army_id();
    if (army_id == player_army_.get_army_id()) {
        return &player_army_;
    }
    else if (army_id == ai_army_.get_army_id()) {
        return &ai_army_;
    }
    log_in_file("ERROR, not found an army", true);
    return nullptr;
}

void TurnBasedGame::battle_map_kill_creature(BattleMapCoordinate killed_creature_battle_map_coordinate) {
    battle_map_clear_tile_from_creature_image(killed_creature_battle_map_coordinate);
    (*battle_map_info_)[killed_creature_battle_map_coordinate.y][killed_creature_battle_map_coordinate.x].creature_ = nullptr;
}

void TurnBasedGame::check_possible_kill(Creature* creature_ptr, BattleMapCoordinate creature_battle_map_coordinate) {
    if (creature_ptr->get_certain_stat_current_value(CreatureStatId::kHP) <= 0) {
        add_string_to_ui_log(*creature_ptr->get_name() + 
            ' ' + '(' + std::to_string(creature_battle_map_coordinate.y + 1) + ',' + ' ' + std::to_string(creature_battle_map_coordinate.x + 1) + ") died.");

        battle_map_kill_creature(creature_battle_map_coordinate);
        find_army_by_owned_creature(creature_ptr)->kill_creature(creature_ptr->get_creature_id());
    }
}

bool TurnBasedGame::move_creature_by_coordinate(BattleMapCoordinate battle_map_coordinate_old, BattleMapCoordinate battle_map_coordinate_new) {
    std::vector<BattleTile>::pointer old_coordinate_ptr{ (*battle_map_info_)[battle_map_coordinate_old.y].data() + battle_map_coordinate_old.x },
                                        new_coordinate_ptr{ (*battle_map_info_)[battle_map_coordinate_new.y].data() + battle_map_coordinate_new.x };

    if (new_coordinate_ptr->creature_ != nullptr) { // sometimes can be unneeded because of check before function call
        log_in_file("Error, tried to move on already occupied coordinate.", true);
        return false;
    }

    add_string_to_ui_log(*old_coordinate_ptr->creature_->get_name() + '(' +
        std::to_string(battle_map_coordinate_old.x) + ',' + ' ' +
        std::to_string(battle_map_coordinate_old.y) + ") moved to coordinate (" + std::to_string(battle_map_coordinate_new.x + 1) +
        ',' + ' ' + std::to_string(battle_map_coordinate_new.y + 1) + ')');

    new_coordinate_ptr->creature_ = old_coordinate_ptr->creature_;
    old_coordinate_ptr->creature_ = nullptr;

    // update applied terrain effects
    if (old_coordinate_ptr->terrain_type_ != new_coordinate_ptr->terrain_type_) {
        Creature* creature{ new_coordinate_ptr->creature_ };
        const std::vector<CreatureStatMultiplier>* terrain_effects{ terrain_database_get_effects(old_coordinate_ptr->terrain_type_) };
        std::for_each(std::execution::par_unseq, terrain_effects->begin(), terrain_effects->end(),
            [&creature](CreatureStatMultiplier effect) { creature->delete_stat_multiplier(effect); });

        terrain_effects = terrain_database_get_effects(new_coordinate_ptr->terrain_type_);
        std::for_each(std::execution::par_unseq, terrain_effects->begin(), terrain_effects->end(), 
            [&creature](CreatureStatMultiplier effect) { creature->apply_stat_multiplier(effect); });
    }

    battle_map_clear_tile_from_creature_image(battle_map_coordinate_old);

    // create creature_ptr "image" at new battle_map_coordinate
    FrameCoordinate tile_center_of_new_coordinate{ battle_map_find_tile_center_frame_coordinate(battle_map_coordinate_new) };
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

bool TurnBasedGame::creature_move_by_input(UserInputButton input_method) {
    Creature* creature_on_old_coordinate_ptr{ (*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x].creature_ };

    bool coordinate_input_result{ false }; // used only intil user_input moving by battle_map_coordinate not fixed 
    std::unique_ptr<std::vector<UserInputButton>> direction_log;
    switch (input_method) {
    case UserInputButton::kMoveSelectionByDirection:
        direction_log = player_coordinate_selection_move_by_direction_input();
        break;
    case UserInputButton::kMoveSelectionByCoordinate:
        coordinate_input_result = player_coordinate_selection_move_by_coordinate_input(); // must be changed to direction log
        if (coordinate_input_result == false) {
            return false;
        }
        break;
    default:
        return false;
    }

    //if (direction_log == nullptr) {
    //    return false;
    //}

    if (coordinate_input_result == false) { // !!!!!!!!!!!!!!!!tmp until moving by battle_map_coordinate will be fixed, checking that input was got not from selection_by_coordinate
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
            const TerrainMovementCost* movement_cost{ terrain_database_get_movement_cost(
                                                        (battle_tile_y_ptr->data() + player_coordinate_selection_old_.x)->terrain_type_) };

            for (std::vector<UserInputButton>::pointer direction_begin{ direction_log->data() }, direction_end{ direction_log->data() + direction_log->size() };
                direction_begin < direction_end; ++direction_begin) {

                AP_cost_for_movement += movement_cost->leaving_value; // AP for leaving tile

                switch (*direction_begin) {
                case UserInputButton::kMoveUp:
                    --moved_distance_y;
                    break;
                case UserInputButton::kMoveDown:
                    ++moved_distance_y;
                    break;
                case UserInputButton::kMoveRight:
                    ++moved_distance_x;
                    break;
                case UserInputButton::kMoveLeft:
                    --moved_distance_x;
                    break;
                default:
                    return false;
                }
                //++moved_distance;

                movement_cost = terrain_database_get_movement_cost(
                    ((*battle_map_info_)[player_coordinate_selection_old_.y + moved_distance_y])[player_coordinate_selection_old_.x + moved_distance_x].terrain_type_);

                AP_cost_for_movement += movement_cost->entry_value; // AP for going into tile
            }

            if (AP_cost_for_movement > creature_on_old_coordinate_ptr->get_certain_stat_current_value(CreatureStatId::kMovementSpeed)) {
                std::cerr << "Error, tried to move too far. Used AP: " << AP_cost_for_movement <<
                    ". When creature SPD = " << creature_on_old_coordinate_ptr->get_certain_stat_current_value(CreatureStatId::kMovementSpeed) << '\n';
                player_coordinate_selection_ = player_coordinate_selection_old_;
                return false;
            }
            add_string_to_ui_log("AP used for movement: " + std::to_string(AP_cost_for_movement));
        }
    }
    //if (direction_log != nullptr) {
        //std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr;

    Creature* creature_on_new_coordinate_ptr{ (*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x].creature_ };

    // move on empty battle_tile
    if (creature_on_new_coordinate_ptr == nullptr) {
        move_creature_by_coordinate(player_coordinate_selection_old_, player_coordinate_selection_);
    }
    // macro - attack or main buff on target
    else if (creature_on_new_coordinate_ptr != nullptr) { // in future should add check for non same army_ptr allies
        if (creature_on_old_coordinate_ptr->get_army_id()
            != creature_on_new_coordinate_ptr->get_army_id()) {

            add_string_to_ui_log(
                *creature_on_old_coordinate_ptr->get_name() +
                " (" + std::to_string(player_coordinate_selection_old_.x + 1) +
                ", " + std::to_string(player_coordinate_selection_old_.y + 1) +
                ") attacked " +
                *creature_on_new_coordinate_ptr->get_name() +
                " (" + std::to_string(player_coordinate_selection_.x + 1) +
                ", " + std::to_string(player_coordinate_selection_.y + 1) + ')');

            CreatureActionResult attack_result{
                creature_ability_default_attack(creature_on_old_coordinate_ptr, creature_on_new_coordinate_ptr) };

            log_creature_action_result(&attack_result, creature_on_old_coordinate_ptr, creature_on_new_coordinate_ptr);
            check_possible_kill(creature_on_new_coordinate_ptr, player_coordinate_selection_);
            check_possible_kill(creature_on_old_coordinate_ptr, player_coordinate_selection_old_);

        }
        // if moved on another creature_ptr owned by player
        else if (creature_on_old_coordinate_ptr->get_army_id()  // in future should add check for non same army_ptr allies
            == creature_on_new_coordinate_ptr->get_army_id()) {

            add_string_to_ui_log(*creature_on_old_coordinate_ptr->get_name() +
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

    // switch to decide what to do with creature_ptr
    char user_input{};
    char user_input_end{};
    do {
        for (int iter{ 0 }; iter != (kWindowWidth_ / 2) - 6; ++iter) {
            std::cout << ' ';
        }
        std::cout << "\nInteract User Input: ";
        std::cin >> user_input;

        switch (user_input) {
        case UserInputButton::kMoveSelectionByCoordinate:
            return creature_move_by_input(UserInputButton::kMoveSelectionByCoordinate);
        case UserInputButton::kMoveSelectionByDirection:
            return creature_move_by_input(UserInputButton::kMoveSelectionByDirection);
        case UserInputButton::kExit:
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
