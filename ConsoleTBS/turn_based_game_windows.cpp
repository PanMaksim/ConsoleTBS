#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <utility>
#include <algorithm>
#include <execution>

#include "turn_based_game.h"
#include "turn_based_game_global.h"
#include "logger.h"
#include "random.h"
#include "army.h"
#include "terrain.h"
#include "battle_tile.h"

using namespace tbs;

void tbs::TurnBasedGame::create_new_main_game_window() {
    // initializing strings (at first launch) and cleaning in other situations
    std::for_each(std::execution::par_unseq, frame_.begin(), frame_.end(), 
        [=](std::string& str) {
            str = std::string(kWindowWidth_, ' ');
            str[kWindowWidth_ - 1] = '\n';
        });

    //create upper border
    std::array<std::string, kWindowHeight_>::iterator frame_coordinate_y_ptr{ frame_.begin() };

    std::fill(std::execution::par_unseq, frame_coordinate_y_ptr->begin() + 2, frame_coordinate_y_ptr->end() - 2, kGameWindowHorizontalSymbol_);

    //create left and right borders
    std::for_each(std::execution::par_unseq, frame_.begin() + 1, frame_.end() - 1, 
        [=](std::string& str) {
            str[1] = kGameWindowVerticalSymbol_;
            str[kWindowWidth_ - 2] = kGameWindowVerticalSymbol_;
        });

    //create lower border
    frame_coordinate_y_ptr = frame_.end() - 1;
    (*frame_coordinate_y_ptr)[1] = kGameWindowVerticalSymbol_;
    (*frame_coordinate_y_ptr)[kWindowWidth_ - 2] = kGameWindowVerticalSymbol_;

    std::fill(std::execution::par_unseq, frame_coordinate_y_ptr->begin() + 2, frame_coordinate_y_ptr->end() - 2, kGameWindowHorizontalSymbol_);
}

void tbs::TurnBasedGame::create_new_ui_window(bool called_on_free_space) {
    ui_status[UI_Status::kCreatureStats] = false;
    ui_status[UI_Status::kUI_InputHelp] = false;

    if (called_on_free_space) {
        std::for_each(std::execution::par_unseq, frame_.begin() + ui_window_height_start_, frame_.begin() + ui_window_height_end_,
            [=](std::string& str) {
                *(str.begin() + ui_window_width_start_) = kGameWindowVerticalSymbol_;
            });
    }
    else {
        std::for_each(std::execution::par_unseq, frame_.begin() + ui_window_height_start_, frame_.begin() + ui_window_height_end_,
            [=](std::string& str) {
                std::string::iterator frame_coordinate_x_ptr{ str.begin() + ui_window_width_start_ };
                *frame_coordinate_x_ptr++ = kGameWindowVerticalSymbol_;

                std::fill(std::execution::par_unseq, frame_coordinate_x_ptr, str.begin() + ui_window_width_end_, ' ');
            });
    }

    *((frame_.begin() + ui_window_height_end_)->begin() + ui_window_width_start_) = kGameWindowVerticalSymbol_;

    // Hint to button_ that calls list of allowed for user input buttons
    FrameCoordinate coordinate{ ui_window_width_start_ + 2, ui_window_input_help_coordinate_height };
    if (frame_[coordinate.y][coordinate.x] != u_input::UserInputButton::kShowInputHelp) {
        add_string_to_ui(coordinate, &u_input::description_to_show_input_help_button);
    }

    // border for log window
    std::array<std::string, kWindowHeight_>::iterator frame_coordinate_y_ptr = frame_.begin() + ui_window_height_end_ - kUserInterfaceLogWindowHeight_;
    std::fill(std::execution::par_unseq, frame_coordinate_y_ptr->data() + ui_window_width_start_ + 1, frame_coordinate_y_ptr->data() + ui_window_width_end_, kGameWindowHorizontalSymbol_);

    ui_status[UI_Status::kUI_Window] = true;
    ui_status[UI_Status::kUI_WindowLog] = true;
}

void tbs::TurnBasedGame::create_new_pv_window() {
    std::for_each(std::execution::par_unseq, frame_.data() + pv_window_height_start_, frame_.data() + pv_window_height_end_,
        [=](std::string& str) {
            std::fill(std::execution::par_unseq, str.data() + pv_window_width_start_, str.data() + pv_window_width_end_, ' ');
        });

    ui_status[UI_Status::kPlayerViewWindow] = true;
}

void tbs::TurnBasedGame::set_ui_status_flags_to_default(){
    std::fill(std::execution::par_unseq, ui_status.begin(), ui_status.end(), false);
}

void tbs::TurnBasedGame::calculate_window_borders() {
    pv_window_height_start_ = 1; // starting from 1 and ending at window_height - 3 cause of main game borders from both sides and numeration from 0
    pv_window_height_end_ = kWindowHeight_ - 1;

    ui_window_height_start_ = 1;
    ui_window_height_end_ = kWindowHeight_ - 1;

    int interface_main_actual_window_width = static_cast<int>((kWindowWidth_ * 0.20) - 2);
    //int player_view_actual_window_width = window_width - interface_main_actual_window_width - 2;

    ui_window_width_start_ = kWindowWidth_ - 3 - interface_main_actual_window_width;
    ui_window_width_end_ = kWindowWidth_ - 3;
    ui_window_string_width_ = ui_window_width_end_ - ui_window_width_start_;
    runtime_logger::log_in_file("UI size = " + std::to_string(ui_window_string_width_));

    pv_window_width_start_ = 2;
    pv_window_width_end_ = ui_window_width_start_ - 1;

    ui_window_input_help_coordinate_height = ui_window_height_end_ - kUserInterfaceLogWindowHeight_ - 1;

    ui_log_window_height_start_ = ui_window_height_end_ - kUserInterfaceLogWindowHeight_;
}

void tbs::TurnBasedGame::print_frame() {
    std::stringstream frame_stream;
    std::for_each(std::execution::seq, frame_.begin(), frame_.end(), [&](const std::string& str) { frame_stream << str; });
    std::cout << frame_stream.str();
}

void tbs::TurnBasedGame::clear_ui_log() {
    std::for_each(std::execution::par_unseq, frame_.data() + ui_log_window_height_start_ + 1, frame_.data() + ui_log_window_height_start_ + ui_log_window_height_current_ + 1,
        [=](std::string& str) {
            std::string::pointer frame_coordinate_x_ptr{ str.data() + ui_window_width_start_ + ui_visual_indent_width };
            if (*frame_coordinate_x_ptr != ' ' && *(frame_coordinate_x_ptr + 1) != ' ') {
                std::fill(std::execution::par_unseq, frame_coordinate_x_ptr, str.data() + ui_window_width_end_ - 1, ' '); // not checking if it is already clear, because in this small log string will be faster just clear it all
            }
        });
    
    ui_log_window_height_current_ = 1;
}

void tbs::TurnBasedGame::battle_map_clear() {
    if (battle_map_info_ != nullptr) {
        if (ui_status[UI_Status::kBattleMapTileNumeration]) { battle_map_tile_numeration_turn_off(); }
        player_coordinate_selection_ = { 0, 0 }; 

        pv_visual_indent_height_ = 0;
        pv_visual_indent_width_ = 0;

        battle_map_info_.reset();
        ui_status[UI_Status::kBattleMap] = false;
        ui_status[UI_Status::kUI_WindowLog] = false;
    }
}

void tbs::TurnBasedGame::generate_new_battle_map() {
    using namespace terrain;
    battle_map_clear();
    battle_map_info_ = std::make_unique<std::vector<std::vector<battle_tile::BattleTile>>>(kBattleMapSizeHeight_, std::vector<battle_tile::BattleTile>(kBattleMapSizeWidth_));

    //set random Terrain
    std::for_each(std::execution::par_unseq, battle_map_info_->data(), battle_map_info_->data() + kBattleMapSizeHeight_,
        [](std::vector<battle_tile::BattleTile>& battle_tile_line) {
            std::for_each(std::execution::par_unseq, battle_tile_line.data(), battle_tile_line.data() + kBattleMapSizeWidth_, // not generate because we need to change only terrain
                [](battle_tile::BattleTile& battle_tile) { battle_tile.terrain_type_ = static_cast<terrain::Type>
                    (random::get_random_number(static_cast<int>(terrain::Type::kPlain), static_cast<int>(terrain::Type::kTerrainTypeMax) - 1));
                });
        });
}

void tbs::TurnBasedGame::calculate_battle_map_visual() {
    // + 1 after battle_map_indent_height for visual at the bottom
    if ((kBattleMapSizeHeight_ * kTileVisualHeight_) + 1 + pv_visual_indent_height_ + 1 > pv_window_height_end_ - pv_window_height_start_) {
#ifdef debug_log
        runtime_logger::log_in_file("ERROR: battle_map_height is too large to be shown all at once.", true);
#endif
    }
    // + 2 after battle_map_indent_width for visual
    if ((kBattleMapSizeWidth_ * kTileVisualWidth_) + 1 + pv_visual_indent_width_ + 2 > pv_window_width_end_ - pv_window_width_start_) {
#ifdef debug_log
        runtime_logger::log_in_file("ERROR: battle_map_width is too large to be shown all at once.", true);
#endif
    }
    // it is just cout no actual STOP!!!, program will be stopped by system error in the proccess of adding battle map to frame_

    {
        int battle_map_visual_size_height{ kBattleMapSizeHeight_ * kTileVisualHeight_ + 1 },
            battle_map_visual_size_width{ kBattleMapSizeWidth_ * kTileVisualWidth_ };

        pv_visual_indent_height_ = ((kWindowHeight_ - battle_map_visual_size_height) / 2);
        pv_visual_indent_width_ = ((pv_window_width_end_ - pv_window_width_start_ - battle_map_visual_size_width) / 2);

        if (pv_window_height_end_ - pv_window_height_start_ - battle_map_visual_size_height - pv_visual_indent_height_ != pv_visual_indent_height_) {
            --pv_visual_indent_height_;
        }
        if (pv_window_width_end_ - pv_window_width_start_ - battle_map_visual_size_width - pv_visual_indent_width_ != pv_visual_indent_width_) {
            --pv_visual_indent_width_;
        }
    }
}

void tbs::TurnBasedGame::show_battle_map() {

    std::array<std::string, kWindowHeight_>::pointer frame_coordinate_y_ptr{
        frame_.data() + pv_window_height_start_ + pv_visual_indent_height_ };

    std::string::pointer frame_coordinate_x_ptr{
        frame_coordinate_y_ptr->data() + pv_window_width_start_ + pv_visual_indent_width_ + 1 };

    int shown_tiles_width, 
        tile_border_visual_size_current;

    // create upper edge (different visual from others)
    for (shown_tiles_width = 0; shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, frame_coordinate_x_ptr += kTileVisualWidth_) {
        std::fill_n(std::execution::par_unseq, frame_coordinate_x_ptr, kTileVisualWidth_ - 1, kTileHorizontalSymbol_);
    }

    ++frame_coordinate_y_ptr;
    frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + pv_window_width_start_ + pv_visual_indent_width_;

    for (int shown_tiles_height = 0; shown_tiles_height != kBattleMapSizeHeight_; ++shown_tiles_height) {

        // create vertical edges
        for (tile_border_visual_size_current = 0; tile_border_visual_size_current != kTileVisualHeight_ - 1;
            ++tile_border_visual_size_current, ++frame_coordinate_y_ptr) {

            frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + pv_window_width_start_ + pv_visual_indent_width_;
            for (shown_tiles_width = 0; shown_tiles_width != kBattleMapSizeWidth_ + 1; // to create right border for last tile
                ++shown_tiles_width, frame_coordinate_x_ptr += kTileVisualWidth_) {

                *frame_coordinate_x_ptr = kTileVerticalSymbol_;
            }
        }

        // create bottom edges
        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + pv_window_width_start_ + pv_visual_indent_width_;

        for (shown_tiles_width = 0; shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, frame_coordinate_x_ptr += kTileVisualWidth_ - 1) {
            *frame_coordinate_x_ptr++ = kTileVerticalSymbol_;
            std::fill_n(std::execution::par_unseq, frame_coordinate_x_ptr, kTileVisualWidth_ - 1, kTileHorizontalSymbol_);
        }
        *frame_coordinate_x_ptr = kTileVerticalSymbol_;

        ++frame_coordinate_y_ptr;
    }

    ui_status[UI_Status::kBattleMap] = true;
}

void tbs::TurnBasedGame::battle_map_show_landscape() {
    std::array<std::string, kWindowHeight_>::iterator frame_coordinate_y_ptr{
        frame_.begin() + pv_window_height_start_ - 1 + pv_visual_indent_height_ };

    std::string::iterator frame_coordinate_x_ptr;
    int coordinate_x_start{ pv_window_width_start_ + pv_visual_indent_width_ - 2 };

    std::for_each(std::execution::seq, battle_map_info_->begin(), battle_map_info_->end(),  // can be changed to unseq execution if there will be locks to prevent dataracing
        [=, &frame_coordinate_y_ptr, &frame_coordinate_x_ptr](std::vector<battle_tile::BattleTile>& battle_tile_line) {
            frame_coordinate_y_ptr += kTileVisualHeight_;
            frame_coordinate_x_ptr = frame_coordinate_y_ptr->begin() + coordinate_x_start;
            std::for_each(std::execution::seq, battle_tile_line.begin(), battle_tile_line.end(), [=, &frame_coordinate_x_ptr](battle_tile::BattleTile& battle_tile) {
                frame_coordinate_x_ptr += kTileVisualWidth_;
                *frame_coordinate_x_ptr = terrain::get_full_terrain_info_from_database(battle_tile.terrain_type_)->symbol_;
                });
        });
}

bool tbs::TurnBasedGame::battle_map_tile_numeration_turn_on() {
    tbs::global::add_string_to_ui_log("Turn on tile numeration");
    FrameCoordinate coordinate{ pv_window_width_start_ + pv_visual_indent_width_ + kTileVisualWidth_ / 2, pv_window_height_start_ + pv_visual_indent_height_ - 1 };

    // create numeration at top
    int shown_tiles_width{};
    for (char tile_number_width_first{ '0' }, tile_number_width_second{ '1' };
        shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, ++tile_number_width_second, coordinate.x += kTileVisualWidth_) {

        if (tile_number_width_second > '9') {
            ++tile_number_width_first;
            tile_number_width_second = '0';
        }
        if (tile_number_width_first != '0') {
            frame_[coordinate.y][coordinate.x - 1] = tile_number_width_first;
        }

        frame_[coordinate.y][coordinate.x] = tile_number_width_second;
    }

    // create numeration at left and right
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! - 1 only for laptop
    coordinate.y = pv_window_height_start_ + pv_visual_indent_height_ + kTileVisualHeight_ / 2 - 1;
    {
        int frame_coordinate_x_left{ pv_window_width_start_ + pv_visual_indent_width_ - 2 },
            frame_coordinate_x_right{ pv_window_width_end_ - pv_visual_indent_width_ + 2 };
        int shown_tiles_height{};

        for (char tile_number_height_first{ '0' }, tile_number_height_second{ '1' };
            shown_tiles_height != kBattleMapSizeHeight_; ++shown_tiles_height, ++tile_number_height_second, coordinate.y += kTileVisualHeight_) {

            if (tile_number_height_second > '9') {
                ++tile_number_height_first;
                tile_number_height_second = '0';
            }

            // left
            if (tile_number_height_first != '0') {
                frame_[coordinate.y][frame_coordinate_x_left] = tile_number_height_first;
            }

            frame_[coordinate.y + 1][frame_coordinate_x_left] = tile_number_height_second;

            //right
            if (tile_number_height_first != '0') {
                frame_[coordinate.y][frame_coordinate_x_right] = tile_number_height_first;
            }

            frame_[coordinate.y + 1][frame_coordinate_x_right] = tile_number_height_second;
        }
    }

    // create numeration at bottom
    coordinate.x = pv_window_width_start_ + pv_visual_indent_width_ + kTileVisualWidth_ / 2;
    coordinate.y = kBattleMapSizeHeight_ * kTileVisualHeight_ + 1 + pv_visual_indent_height_ + 2;
    shown_tiles_width = 0;

    for (char tile_number_width_first{ '0' }, tile_number_width_second{ '1' };
        shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, ++tile_number_width_second, coordinate.x += kTileVisualWidth_) {

        if (tile_number_width_second > '9') {
            ++tile_number_width_first;
            tile_number_width_second = '0';
        }
        if (tile_number_width_first != '0') {
            frame_[coordinate.y][coordinate.x - 1] = tile_number_width_first;
        }

        frame_[coordinate.y][coordinate.x] = tile_number_width_second;
    }

    ui_status[UI_Status::kBattleMapTileNumeration] = true;
    return true;
}

bool tbs::TurnBasedGame::battle_map_tile_numeration_turn_off() {
    tbs::global::add_string_to_ui_log("Turn off tile numeration");
    FrameCoordinate coordinate{ pv_window_width_start_ + pv_visual_indent_width_ + kTileVisualWidth_ / 2, pv_window_height_start_ + pv_visual_indent_height_ - 1 };

    // delete numeration at top
    for (int shown_tiles_width{}; shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, coordinate.x += kTileVisualWidth_) {

        frame_[coordinate.y][coordinate.x - 1] = ' ';
        frame_[coordinate.y][coordinate.x] = ' ';
    }

    // delete numeration at left and right
    coordinate.y = pv_window_height_start_ + pv_visual_indent_height_ + kTileVisualHeight_ / 2;
    {
        int frame_coordinate_x_left{ pv_window_width_start_ + pv_visual_indent_width_ - 2 },
            frame_coordinate_x_right{ pv_window_width_end_ - pv_visual_indent_width_ + 2 };

        for (int shown_tiles_height{};
            shown_tiles_height != kBattleMapSizeHeight_; ++shown_tiles_height, coordinate.y += kTileVisualHeight_) {

            // left
            frame_[coordinate.y][frame_coordinate_x_left] = ' ';
            frame_[coordinate.y + 1][frame_coordinate_x_left] = ' ';

            //right
            frame_[coordinate.y][frame_coordinate_x_right] = ' ';
            frame_[coordinate.y + 1][frame_coordinate_x_right] = ' ';
        }
    }

    // delete numeration at bottom
    coordinate.x = pv_window_width_start_ + pv_visual_indent_width_ + kTileVisualWidth_ / 2;
    coordinate.y = kBattleMapSizeHeight_ * kTileVisualHeight_ + 1 + pv_visual_indent_height_ + 2;

    for (int shown_tiles_width{};
        shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, coordinate.x += kTileVisualWidth_) {

        frame_[coordinate.y][coordinate.x - 1] = ' ';
        frame_[coordinate.y][coordinate.x] = ' ';
    }

    ui_status[UI_Status::kBattleMapTileNumeration] = false;
    return true;
}

bool tbs::TurnBasedGame::battle_map_tile_numeration_switch() {
    return (!ui_status[UI_Status::kBattleMapTileNumeration]) ? battle_map_tile_numeration_turn_on() : battle_map_tile_numeration_turn_off();
}

FrameCoordinate tbs::TurnBasedGame::battle_map_find_tile_center_frame_coordinate(BattleMapCoordinate coordinate) { // enter val numeration from 0
    int first_tile_coordinate_x{ pv_window_width_start_ + pv_visual_indent_width_ + (kTileVisualWidth_ / 2) },
        first_tile_coordinate_y{ pv_window_height_start_ + pv_visual_indent_height_ + 2 };

    if (coordinate.y > kBattleMapSizeHeight_ || coordinate.x > kBattleMapSizeWidth_) {
#ifdef debug_log
        runtime_logger::log_in_file("ERROR: battle_map_find_tile_center_coordinate out of range", true);
#endif
        return { first_tile_coordinate_x, first_tile_coordinate_y }; // return [0, 0] tile
    }
    return { first_tile_coordinate_x + kTileVisualWidth_ * coordinate.x,
                first_tile_coordinate_y + 1 + kTileVisualHeight_ * coordinate.y};
}

void tbs::TurnBasedGame::battle_map_update_player_selection() {
    FrameCoordinate tile_center_coordinates{ battle_map_find_tile_center_frame_coordinate(player_coordinate_selection_) };
    
    int selection_visual_indent_height{ kPlayerSelectionVisualHeight_ / 2 },
        selection_visual_indent_width{ kPlayerSelectionVisualWidth_ / 2 };

    std::array<std::string, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() + tile_center_coordinates.y };

    //top
    std::string::pointer frame_coordinate_x_ptr{
        (frame_coordinate_y_ptr - selection_visual_indent_height)->data()
        + tile_center_coordinates.x + selection_visual_indent_width * (-1) };
    
    int player_selection_width_current{};
    for (; player_selection_width_current != kPlayerSelectionVisualWidth_;
        ++player_selection_width_current, ++frame_coordinate_x_ptr) {

        *frame_coordinate_x_ptr = kTileHorizontalSymbol_;
    }

    // left & right
    frame_coordinate_x_ptr = (frame_coordinate_y_ptr + selection_visual_indent_height)->data()
        + tile_center_coordinates.x + selection_visual_indent_width * (-1);

    for (player_selection_width_current = 0; player_selection_width_current != kPlayerSelectionVisualWidth_;
        ++player_selection_width_current, ++frame_coordinate_x_ptr) {

        *frame_coordinate_x_ptr = kTileHorizontalSymbol_;
    }

    // bottom
    frame_coordinate_y_ptr += ((selection_visual_indent_height - 1) * (-1));
    ++selection_visual_indent_width;

    for (int player_selection_height_current{}; player_selection_height_current != kPlayerSelectionVisualHeight_;
        ++player_selection_height_current, ++frame_coordinate_y_ptr) {

        *(frame_coordinate_y_ptr->data() + tile_center_coordinates.x - selection_visual_indent_width) = kTileVerticalSymbol_;
        *(frame_coordinate_y_ptr->data() + tile_center_coordinates.x + selection_visual_indent_width) = kTileVerticalSymbol_;
    }
    update_ui();
    ui_status[UI_Status::kCreatureSelected] =
        ((*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x].creature_ != nullptr) ? true : false;
}

void tbs::TurnBasedGame::battle_map_clear_old_player_selection() {
    FrameCoordinate tile_center_coordinates;
    
    if (player_coordinate_selection_old_.x != -1 && player_coordinate_selection_old_.y != -1) { // possibly unneeded check for 2 values, only 1 is enough
        tile_center_coordinates = battle_map_find_tile_center_frame_coordinate(player_coordinate_selection_old_);
    }
    else {
        tile_center_coordinates = battle_map_find_tile_center_frame_coordinate(player_coordinate_selection_);
    }

    int selection_visual_indent_height{ kPlayerSelectionVisualHeight_ / 2 },
        selection_visual_indent_width{ kPlayerSelectionVisualWidth_ / 2 };

    std::array<std::string, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() + tile_center_coordinates.y };

    //top
    std::string::pointer frame_coordinate_x_ptr{
        (frame_coordinate_y_ptr - selection_visual_indent_height)->data() // frame_coordinate_y
        + tile_center_coordinates.x + selection_visual_indent_width * (-1) };

    int player_selection_width_current{};
    for (; player_selection_width_current != kPlayerSelectionVisualWidth_;
        ++player_selection_width_current, ++frame_coordinate_x_ptr) {

        *frame_coordinate_x_ptr = ' ';
    }

    // left & right
    frame_coordinate_x_ptr = (frame_coordinate_y_ptr + selection_visual_indent_height)->data() // frame_coordinate_y
        + tile_center_coordinates.x + selection_visual_indent_width * (-1);

    for (player_selection_width_current = 0; player_selection_width_current != kPlayerSelectionVisualWidth_;
        ++player_selection_width_current, ++frame_coordinate_x_ptr) {

        *frame_coordinate_x_ptr = ' ';
    }

    // bottom
    frame_coordinate_y_ptr += ((selection_visual_indent_height - 1) * (-1));
    ++selection_visual_indent_width;

    for (int player_selection_height_current{}; player_selection_height_current != kPlayerSelectionVisualHeight_;
        ++player_selection_height_current, ++frame_coordinate_y_ptr) {

        *(frame_coordinate_y_ptr->data() + tile_center_coordinates.x - selection_visual_indent_width) = ' ';
        *(frame_coordinate_y_ptr->data() + tile_center_coordinates.x + selection_visual_indent_width) = ' ';
    }
}

void tbs::TurnBasedGame::frame_clear_string(std::string::iterator frame_coordinate_x_iter, std::string::iterator frame_coordinate_x_iter_end) { // can be overhead but raises readability
    std::fill(std::execution::par_unseq, frame_coordinate_x_iter, frame_coordinate_x_iter_end, ' ');
}

// needs better system for transporting words from UI's right end
std::string::iterator tbs::TurnBasedGame::add_string_to_ui(FrameCoordinate frame_coordinate, const std::string&& str_rvalue, int indent = 0) {
    std::string::iterator frame_coordinate_x_ptr = { frame_[frame_coordinate.y].begin() + frame_coordinate.x + indent };

    if (str_rvalue.size() > ui_window_string_width_) {
        runtime_logger::log_in_file("Error, string is too big to be added in one ui string", true);
        return frame_coordinate_x_ptr;
    }

    std::move(std::execution::par_unseq, str_rvalue.begin(), str_rvalue.end(), frame_coordinate_x_ptr);

    frame_coordinate_x_ptr += str_rvalue.size();
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_[frame_coordinate.y].begin() + ui_window_width_end_);
    }
    return frame_coordinate_x_ptr;
}

std::string::iterator tbs::TurnBasedGame::add_string_to_ui(FrameCoordinate frame_coordinate, const std::string* str_ptr, int indent = 0) {
    std::string::iterator frame_coordinate_x_ptr = { frame_[frame_coordinate.y].begin() + frame_coordinate.x + indent };

    if (str_ptr->size() > ui_window_string_width_) {
        runtime_logger::log_in_file("Error, string is too big to be added in one ui string", true);
        return frame_coordinate_x_ptr;
    }

    std::copy(std::execution::par_unseq, str_ptr->begin(), str_ptr->end(), frame_coordinate_x_ptr);

    frame_coordinate_x_ptr += str_ptr->size();
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_[frame_coordinate.y].begin() + ui_window_width_end_);
    }
    return frame_coordinate_x_ptr;
}

std::string::iterator tbs::TurnBasedGame::add_string_to_ui(FrameCoordinate frame_coordinate, const std::string_view* str_view_ptr, int indent = 0) {
    std::string::iterator frame_coordinate_x_ptr = { frame_[frame_coordinate.y].begin() + frame_coordinate.x + indent };

    if (str_view_ptr->size() > ui_window_string_width_) {
        runtime_logger::log_in_file("Error, string is too big to be added in one ui string", true);
        return frame_coordinate_x_ptr;
    }

    std::copy(std::execution::par_unseq, str_view_ptr->begin(), str_view_ptr->end(), frame_coordinate_x_ptr);

    frame_coordinate_x_ptr += str_view_ptr->size();
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_[frame_coordinate.y].begin() + ui_window_width_end_);
    }
    return frame_coordinate_x_ptr;
}

std::string::iterator tbs::TurnBasedGame::add_string_to_ui(FrameCoordinate frame_coordinate, const u_input::UserInputDescription* user_input_description_ptr) {
    std::string::iterator frame_coordinate_x_ptr{ frame_[frame_coordinate.y].begin() + frame_coordinate.x };
    const std::string* str{ &user_input_description_ptr->description_ };

    if (str->size() > ui_window_string_width_) {
        runtime_logger::log_in_file("Error, string is too big to be added in one ui string", true);
        return frame_coordinate_x_ptr;
    }

    *frame_coordinate_x_ptr++ = user_input_description_ptr->button_;
    *frame_coordinate_x_ptr++ = ' ';
    *frame_coordinate_x_ptr++ = '-';
    *frame_coordinate_x_ptr++ = ' ';

    std::copy(std::execution::par_unseq, str->cbegin(), str->cend(), frame_coordinate_x_ptr);

    frame_coordinate_x_ptr += str->size();
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_[frame_coordinate.y].begin() + ui_window_width_end_);
    }
    return frame_coordinate_x_ptr;
}

void tbs::TurnBasedGame::ui_input_help_turn_on(const std::vector<u_input::UserInputButton>& allowed_user_input) {
    if (ui_status[UI_Status::kCreatureStats]) { create_new_ui_window(); } // clear from ui stats, can be changed to more optimized variant

    u_input::load_user_input_database(file_database::ID::kUserInputDescription);

    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width, ui_window_height_start_ + ui_visual_indent_height };
    std::for_each(std::execution::seq, allowed_user_input.begin(), allowed_user_input.end(),
        [=, &coordinate](const u_input::UserInputButton allowed_user_input_description) {
            add_string_to_ui(coordinate, u_input::user_input_database_get_main_description(allowed_user_input_description));
            ++coordinate.y;
        });

    u_input::unload_user_input_database(file_database::ID::kUserInputDescription);

    tbs::global::add_string_to_ui_log("Turn in input help");
    ui_status[UI_Status::kUI_InputHelp] = true;
}

void tbs::TurnBasedGame::ui_input_help_turn_off(size_t allowed_user_input_size) {
    std::array<std::string, kWindowHeight_>::iterator ui_begin_iter{ frame_.begin() + ui_window_height_start_ + 2 };
    std::for_each(std::execution::par_unseq, ui_begin_iter, ui_begin_iter + allowed_user_input_size, // +2 because of visual indent
        [=](std::string& str) {
            std::fill(std::execution::par_unseq, str.begin() + ui_window_width_start_ + 3, str.begin() + ui_window_width_end_, ' '); // +1 because of VerticalSymbol and +2 because of visual indent
        });

    tbs::global::add_string_to_ui_log("Turn off input help");
    ui_status[UI_Status::kUI_InputHelp] = false;

    if (ui_status[UI_Status::kBattleMap]) {
        battle_map_create_basic_ui();
        if (ui_status[UI_Status::kCreatureSelected]) {
            battle_map_create_basic_ui_with_creature();
            update_ui();
        }
    }
}

void tbs::TurnBasedGame::ui_input_help_switch(const std::vector<u_input::UserInputButton>& allowed_user_input) {
    (!ui_status[UI_Status::kUI_InputHelp]) ? ui_input_help_turn_on(allowed_user_input) : ui_input_help_turn_off(allowed_user_input.size());
}

void tbs::TurnBasedGame::add_creature_stat_string_to_ui(FrameCoordinate frame_coordinate,
    creature::StatId creature_stat_id, int stat_value_current, int stat_value_max = 0) {

    std::string::iterator frame_coordinate_x_ptr = { frame_[frame_coordinate.y].begin() + frame_coordinate.x
        + static_cast<int>(creature::get_stat_naming_from_database(creature_stat_id)->size()) + 2 }; // +2 for indent from stat naming to stat numeric values
    
    std::string::iterator frame_coordinate_x_ptr_end{ frame_[frame_coordinate.y].begin() + ui_window_width_end_ };

    std::string::const_pointer str_ptr, str_ptr_end;

    std::string stat_value_current_str{ std::to_string(stat_value_current) };
    for (str_ptr = stat_value_current_str.data(), str_ptr_end = stat_value_current_str.data() + stat_value_current_str.size();
        str_ptr != str_ptr_end; ++str_ptr, ++frame_coordinate_x_ptr) {

        *frame_coordinate_x_ptr = *str_ptr;
    }

    if (stat_value_max != 0) {
        *(frame_coordinate_x_ptr++) = '/';
        std::string stat_value_max_str{ std::to_string(stat_value_max) };
        for (str_ptr = stat_value_max_str.data(), str_ptr_end = stat_value_max_str.data() + stat_value_max_str.size();
            str_ptr != str_ptr_end; ++str_ptr, ++frame_coordinate_x_ptr) {

            *frame_coordinate_x_ptr = *str_ptr;
        }
    }

    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_coordinate_x_ptr_end);
    }
}

void tbs::TurnBasedGame::battle_map_create_basic_ui() {
    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width, ui_window_height_start_ + ui_visual_indent_height };

    for (int battle_tile_parameter_iter{}; battle_tile_parameter_iter != static_cast<int>(battle_tile::TileParameters::kTileParametersMax);
        ++battle_tile_parameter_iter, ++coordinate.y) {

        *(add_string_to_ui(coordinate,
            battle_tile::get_tile_parameter_name_from_database(static_cast<battle_tile::TileParameters>(battle_tile_parameter_iter)))) = ':';
    }
}

void tbs::TurnBasedGame::battle_map_create_basic_ui_with_creature() {
    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width,
            ui_window_height_start_ + ui_visual_indent_height + static_cast<int>(battle_tile::TileParameters::kTileParametersMax) };

    *(add_string_to_ui(coordinate, "Name")) = ':';
    ++coordinate.y;
    *(add_string_to_ui(coordinate, "Race")) = ':';
    ++coordinate.y;

    for (int creature_stat_iter{}, creature_stat_iter_max{ static_cast<int>(creature::StatId::kStatMax) };
        creature_stat_iter != creature_stat_iter_max; ++creature_stat_iter, ++coordinate.y) {

        *(add_string_to_ui(coordinate,
            creature::get_stat_naming_from_database(static_cast<creature::StatId>(creature_stat_iter)))) = ':';
    }
}

void tbs::TurnBasedGame::update_ui() { // maybe should save in memory previus selection coordinates for possible action skips
    if (ui_status[UI_Status::kUI_InputHelp] == true) {
        create_new_ui_window(); // not ui_help_turn_off because it asks for allowed_input_size
        battle_map_create_basic_ui();
    }
    
    battle_tile::BattleTile* target{ &(*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x] };
    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width, ui_window_height_start_ + ui_visual_indent_height };

    // landscape
    add_string_to_ui(coordinate,
        &get_full_terrain_info_from_database(target->terrain_type_)->type_name_,
        static_cast<int>(battle_tile::get_tile_parameter_name_from_database(battle_tile::TileParameters::kLandscape)->size()) + 2);
    ++coordinate.y;

    // TO DO creature or landscape "image"?
    // creature ownership
    if (target->creature_ == nullptr) {
        add_string_to_ui(coordinate, "None",
            static_cast<int>(get_tile_parameter_name_from_database(battle_tile::TileParameters::kCreature)->size()) + 2);
        ++coordinate.y;

        if (ui_status[UI_Status::kCreatureStats]) {
            for (std::array<std::string, kWindowHeight_>::iterator frame_coordinate_y_ptr { frame_.begin() + coordinate.y },
                frame_coordinate_y_ptr_end{ frame_.begin() + ui_window_height_end_ };
                (*frame_coordinate_y_ptr)[coordinate.x] != ' ' && frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

                frame_clear_string(frame_coordinate_y_ptr->begin() + coordinate.x,
                    frame_coordinate_y_ptr->begin() + ui_window_width_end_);
            }
            ui_status[UI_Status::kCreatureStats] = false;
        }
        return;
    }

    add_string_to_ui(coordinate,
        (target->creature_->get_army_id() == 0) ? "Player" : "Enemy",
        static_cast<int>(battle_tile::get_tile_parameter_name_from_database(battle_tile::TileParameters::kCreature)->size()) + 2);
    ++coordinate.y;

    if (!ui_status[UI_Status::kCreatureStats]) {
        battle_map_create_basic_ui_with_creature();
        ui_status[UI_Status::kCreatureStats] = true;
    }

    // creature name
    add_string_to_ui(coordinate, target->creature_->get_name(), 6);
    ++coordinate.y;

    // creature race
    add_string_to_ui(coordinate,
        creature::get_race_naming_from_database(target->creature_->get_race()), 6);
    ++coordinate.y;

    // creature stats
    { 
        creature::Stat creature_stat;
        for (int stat_iter{}, stat_iter_end{ static_cast<int>(creature::StatId::kStatMax) };
            stat_iter != stat_iter_end; ++stat_iter, ++coordinate.y) {

            creature_stat = target->creature_->get_stat_current_and_max(static_cast<creature::StatId>(stat_iter));

            add_creature_stat_string_to_ui(coordinate,
                static_cast<creature::StatId>(stat_iter),
                creature_stat.current_, (creature_stat.current_ != creature_stat.max_) ? creature_stat.max_ : 0);
        }
    }
}

void tbs::TurnBasedGame::battle_map_clear_tile_from_creature_image(BattleMapCoordinate creature_battle_map_coordinate) {
    FrameCoordinate tile_center_coordinate{ battle_map_find_tile_center_frame_coordinate(creature_battle_map_coordinate) };

    std::string::pointer frame_coordinate_x_ptr{
        frame_[tile_center_coordinate.y].data() + tile_center_coordinate.x };

    *(frame_coordinate_x_ptr - 1) = ' ';
    *frame_coordinate_x_ptr = ' ';
    *(frame_coordinate_x_ptr + 1) = ' ';
}

