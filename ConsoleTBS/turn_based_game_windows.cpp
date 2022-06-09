
#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <utility>
#include <string_view>
#include <algorithm>

#include "turn_based_game.h"
#include "turn_based_game_global.h"
#include "random.h"
#include "army.h"
#include "terrain.h"
#include "battle_tile.h"

void TurnBasedGame::create_new_main_game_window() {
    std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() };
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr{ frame_coordinate_y_ptr->data() };

    //create upper border
    *frame_coordinate_x_ptr++ = ' ';
    *frame_coordinate_x_ptr++ = ' ';

    std::for_each(frame_coordinate_x_ptr, frame_coordinate_y_ptr->data() + kWindowWidth_ - 2, 
        [](char& symbol) { symbol = kGameWindowHorizontalSymbol_;});
    
    frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + kWindowWidth_ - 2;
    *frame_coordinate_x_ptr++ = ' ';
    *frame_coordinate_x_ptr = '\n';
    ++frame_coordinate_y_ptr;

    //create left and right borders
    for (std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr_end{ frame_.data() + kWindowHeight_ - 1 };
        frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data();

        *frame_coordinate_x_ptr++ = ' ';
        *frame_coordinate_x_ptr++ = kGameWindowVerticalSymbol_;

        std::for_each(frame_coordinate_x_ptr, frame_coordinate_y_ptr->data() + kWindowWidth_ - 2,
            [](char& symbol) { symbol = ' ';});

        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + kWindowWidth_ - 2;
        *frame_coordinate_x_ptr++ = kGameWindowVerticalSymbol_;
        *frame_coordinate_x_ptr = '\n';
    }

    //create lower border
    frame_coordinate_x_ptr = frame_coordinate_y_ptr->data();
    *frame_coordinate_x_ptr++ = ' ';
    *frame_coordinate_x_ptr++ = kGameWindowVerticalSymbol_;

    std::for_each(frame_coordinate_x_ptr, frame_coordinate_y_ptr->data() + kWindowWidth_ - 2,
        [](char& symbol) { symbol = kGameWindowHorizontalSymbol_;});

    frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + kWindowWidth_ - 2;
    *frame_coordinate_x_ptr++ = kGameWindowVerticalSymbol_;
    *frame_coordinate_x_ptr = '\n';
}

void TurnBasedGame::create_new_ui_window() {
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr;

    std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() + ui_window_height_start_ };
    for (std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr_end{ frame_.data() + ui_window_height_end_ };
        frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + ui_window_width_start_;
        *frame_coordinate_x_ptr++ = kGameWindowVerticalSymbol_;
         
        std::for_each(frame_coordinate_x_ptr, frame_coordinate_y_ptr->data() + ui_window_width_end_, 
            [](char& symbol) { symbol = ' ';});
    }

    *(frame_coordinate_y_ptr->data() + ui_window_width_start_) = kGameWindowVerticalSymbol_;

    // border for log window
    frame_coordinate_y_ptr = frame_.data() + ui_window_height_end_ - kUserInterfaceLogWindowHeight_;
    std::for_each(frame_coordinate_y_ptr->data() + ui_window_width_start_ + 1, frame_coordinate_y_ptr->data() + ui_window_width_end_,
        [](char& symbol) {symbol = kGameWindowHorizontalSymbol_;});

    ui_status[UI_Status::kUI_Window] = true;
}

void TurnBasedGame::create_new_pv_window() {
    for (std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() + pv_window_height_start_ },
        frame_coordinate_y_ptr_end{ frame_.data() + pv_window_height_end_ };
        frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

        std::for_each(frame_coordinate_y_ptr->data() + pv_window_width_start_, frame_coordinate_y_ptr->data() + pv_window_width_end_,
            [](char& symbol) { symbol = ' ';});
    }

    ui_status[UI_Status::kPlayerViewWindow] = true;
}

void TurnBasedGame::calculate_window_borders() {
    pv_window_height_start_ = 1; // starting from 1 and ending at window_height - 3 cause of main game borders from both sides and numeration from 0
    pv_window_height_end_ = kWindowHeight_ - 1;

    ui_window_height_start_ = 1;
    ui_window_height_end_ = kWindowHeight_ - 1;

    int interface_main_actual_window_width = static_cast<int>((kWindowWidth_ * 0.20) - 2);
    //int player_view_actual_window_width = window_width - interface_main_actual_window_width - 2;

    ui_window_width_start_ = kWindowWidth_ - 3 - interface_main_actual_window_width;
    ui_window_width_end_ = kWindowWidth_ - 3;

    pv_window_width_start_ = 2;
    pv_window_width_end_ = ui_window_width_start_ - 1;

    ui_log_window_height_start_ = ui_window_height_end_ - kUserInterfaceLogWindowHeight_;
}

void TurnBasedGame::print_frame() {
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr,
        frame_coordinate_x_ptr_end;

    for (std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() },
        frame_coordinate_y_ptr_end{ frame_.data() + kWindowHeight_ };
        frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

        for (frame_coordinate_x_ptr = frame_coordinate_y_ptr->data(),
            frame_coordinate_x_ptr_end = frame_coordinate_y_ptr->data() + kWindowWidth_;
            frame_coordinate_x_ptr != frame_coordinate_x_ptr_end; ++frame_coordinate_x_ptr) {

            std::cout << *frame_coordinate_x_ptr;
        }
    }
}

void TurnBasedGame::clear_ui_log() {
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr;
    
    for (std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{
        frame_.data() + ui_log_window_height_start_ + 1 },
        frame_coordinate_y_ptr_end{ frame_.data() + ui_log_window_height_start_ + ui_log_window_height_current_ + 1};
        frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + ui_window_width_start_ + ui_visual_indent_width;

        if (*frame_coordinate_x_ptr != ' ' && *(frame_coordinate_x_ptr + 1) != ' ') {
            std::for_each(frame_coordinate_x_ptr,
                frame_coordinate_y_ptr->data() + ui_window_width_end_ - 1,
                [](char& symbol) { symbol = ' ';}); // not checking if it is already clear, because in this small log string will be faster just clear it all
        }
    }

    ui_log_window_height_current_ = 1;
}

void TurnBasedGame::battle_map_clear() {
    if (battle_map_info_ != nullptr) {
        if (ui_status[UI_Status::kBattleMapTileNumeration]) {
            battle_map_tile_numeration_turn_off();
        }
        player_coordinate_selection_ = { 0, 0 }; 

        pv_visual_indent_height_ = 0;
        pv_visual_indent_width_ = 0;

        battle_map_info_.reset();
        ui_status[UI_Status::kBattleMap] = false;
    }
}

void TurnBasedGame::generate_new_battle_map() {
    battle_map_clear();
    battle_map_info_ = std::make_unique<std::vector<std::vector<BattleTile>>>(kBattleMapSizeHeight_, std::vector<BattleTile>(kBattleMapSizeWidth_));

    //set random Terrain
    std::for_each(battle_map_info_->data(), battle_map_info_->data() + kBattleMapSizeHeight_,
        [](std::vector<BattleTile>& battle_tile_line) {
            std::for_each(battle_tile_line.data(), battle_tile_line.data() + kBattleMapSizeWidth_,
                [](BattleTile& battle_tile) { battle_tile.terrain_type_ = static_cast<TerrainType>
                (get_random_number(static_cast<int>(TerrainType::kPlain), static_cast<int>(TerrainType::kTerrainTypeMax) - 1));
                });
        });
}

void TurnBasedGame::calculate_battle_map_visual() {
    // + 1 after battle_map_indent_height for visual at the bottom
    if ((kBattleMapSizeHeight_ * kTileVisualHeight_) + 1 + pv_visual_indent_height_ + 1 > pv_window_height_end_ - pv_window_height_start_) {
        std::cerr << "ERROR: battle_map_height is too large to be shown all at once.";
    }
    // + 2 after battle_map_indent_width for visual
    if ((kBattleMapSizeWidth_ * kTileVisualWidth_) + 1 + pv_visual_indent_width_ + 2 > pv_window_width_end_ - pv_window_width_start_) {
        std::cerr << "ERROR: battle_map_width is too large to be shown all at once.";
    }
    // it is just cout no actual STOP!!!, program will be stopped by system error in the proccess of adding battle map to frame_

    {
        int battle_map_visual_size_height{ kBattleMapSizeHeight_ * kTileVisualHeight_ + 1 },
            battle_map_visual_size_width{ kBattleMapSizeWidth_ * kTileVisualWidth_ };

        // +1 and +2 for visual
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

void TurnBasedGame::show_battle_map() {

    std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{
        frame_.data() + pv_window_height_start_ + pv_visual_indent_height_ };

    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr{
        frame_coordinate_y_ptr->data() + pv_window_width_start_ + pv_visual_indent_width_ };

    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr_end;

    int shown_tiles_width, 
        tile_border_visual_size_current;

    // create upper edge (different visual from others)
    for (shown_tiles_width = 0; shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width) {
        *frame_coordinate_x_ptr++ = ' '; // for visual

        for (tile_border_visual_size_current = 1 ; tile_border_visual_size_current != kTileVisualWidth_;
            ++tile_border_visual_size_current, ++frame_coordinate_x_ptr) {

            *frame_coordinate_x_ptr = kTileHorizontalSymbol_;
        }
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
        *frame_coordinate_x_ptr++ = kTileVerticalSymbol_;

        for (shown_tiles_width = 0; shown_tiles_width != kBattleMapSizeWidth_; ++shown_tiles_width, ++frame_coordinate_x_ptr) {
            for (frame_coordinate_x_ptr_end = frame_coordinate_x_ptr + kTileVisualWidth_ - 1;
                frame_coordinate_x_ptr != frame_coordinate_x_ptr_end; ++frame_coordinate_x_ptr) {

                *frame_coordinate_x_ptr = kTileHorizontalSymbol_;
            }
            *frame_coordinate_x_ptr = kTileVerticalSymbol_;
        }

        ++frame_coordinate_y_ptr;
    }

    ui_status[UI_Status::kBattleMap] = true;
}

void TurnBasedGame::battle_map_show_landscape() {
    std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{
        frame_.data() + pv_window_height_start_ + kTileVisualHeight_ - 1 + pv_visual_indent_height_ };

    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr;

    std::vector<std::vector<BattleTile>>::pointer battle_map_coordinate_y_ptr{ battle_map_info_ -> data() },
                                                battle_map_coordinate_y_ptr_end{ battle_map_info_->data() + kBattleMapSizeHeight_ };

    std::vector<BattleTile>::pointer battle_map_coordinate_x_ptr,
                                     battle_map_coordinate_x_ptr_end;

    for (; battle_map_coordinate_y_ptr != battle_map_coordinate_y_ptr_end; frame_coordinate_y_ptr += kTileVisualHeight_, ++battle_map_coordinate_y_ptr) {
        
        for (frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + pv_window_width_start_ + kTileVisualWidth_ - 2 + pv_visual_indent_width_,
            battle_map_coordinate_x_ptr = battle_map_coordinate_y_ptr->data(),
            battle_map_coordinate_x_ptr_end = battle_map_coordinate_y_ptr->data() + kBattleMapSizeWidth_;

            battle_map_coordinate_x_ptr != battle_map_coordinate_x_ptr_end; frame_coordinate_x_ptr += kTileVisualWidth_, ++battle_map_coordinate_x_ptr) {

            *frame_coordinate_x_ptr = terrain_database_get_full_info(battle_map_coordinate_x_ptr->terrain_type_)->symbol_;
        }
    }
}

bool TurnBasedGame::battle_map_tile_numeration_turn_on() {
    add_string_to_ui_log("Turn on tile numeration");
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

bool TurnBasedGame::battle_map_tile_numeration_turn_off() {
    add_string_to_ui_log("Turn off tile numeration");
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

bool TurnBasedGame::battle_map_tile_numeration_switch() {
    //if (!ui_status[UI_Status::kBattleMap]) {return false;}
    return (!ui_status[UI_Status::kBattleMapTileNumeration]) ? battle_map_tile_numeration_turn_on() 
                                                                : battle_map_tile_numeration_turn_off();
}

// returns [y,x] where creature_middle_symbol would be
FrameCoordinate TurnBasedGame::battle_map_find_tile_center_frame_coordinate(BattleMapCoordinate coordinate) { // enter val numeration from 0
    if (coordinate.y > kBattleMapSizeHeight_ || coordinate.x > kBattleMapSizeWidth_) {
        std::cerr << "ERROR: battle_map_find_tile_center_coordinate out of range\n";
        return { pv_window_width_start_ + pv_visual_indent_width_ + (kTileVisualWidth_ / 2), 
                    pv_window_height_start_ + pv_visual_indent_height_ + 2 }; // return [0, 0] tile
    }
    return { pv_window_width_start_ + pv_visual_indent_width_ + (kTileVisualWidth_ / 2) + kTileVisualWidth_ * coordinate.x,
                pv_window_height_start_ + pv_visual_indent_height_ + 3 + kTileVisualHeight_ * coordinate.y};
}

void TurnBasedGame::battle_map_update_player_selection() {
    FrameCoordinate tile_center_coordinates{ battle_map_find_tile_center_frame_coordinate(player_coordinate_selection_) };
    
    int selection_visual_indent_height{ kPlayerSelectionVisualHeight_ / 2 },
        selection_visual_indent_width{ kPlayerSelectionVisualWidth_ / 2 };

    std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() + tile_center_coordinates.y };

    //top
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr{ 
        (frame_coordinate_y_ptr - selection_visual_indent_height)->data() // frame_coordinate_y
        + tile_center_coordinates.x + selection_visual_indent_width * (-1) };
    
    int player_selection_width_current{};
    for (; player_selection_width_current != kPlayerSelectionVisualWidth_;
        ++player_selection_width_current, ++frame_coordinate_x_ptr) {

        *frame_coordinate_x_ptr = kTileHorizontalSymbol_;
    }

    // left & right
    frame_coordinate_x_ptr = (frame_coordinate_y_ptr + selection_visual_indent_height)->data() // frame_coordinate_y
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
        ((*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x].creature_ != nullptr)
        ? true : false;
}

void TurnBasedGame::battle_map_clear_old_player_selection() {
    FrameCoordinate tile_center_coordinates;
    
    if (player_coordinate_selection_old_.x != -1 && player_coordinate_selection_old_.y != -1) { // possibly unneeded check for 2 values, only 1 is enough
        tile_center_coordinates = battle_map_find_tile_center_frame_coordinate(player_coordinate_selection_old_);
    }
    else {
        tile_center_coordinates = battle_map_find_tile_center_frame_coordinate(player_coordinate_selection_);
    }

    int selection_visual_indent_height{ kPlayerSelectionVisualHeight_ / 2 },
        selection_visual_indent_width{ kPlayerSelectionVisualWidth_ / 2 };

    std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr{ frame_.data() + tile_center_coordinates.y };

    //top
    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr{
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

void TurnBasedGame::frame_clear_string(char* frame_coordinate_x_ptr, char* frame_coordinate_x_ptr_end) {
    while (true) {
        if (frame_coordinate_x_ptr == frame_coordinate_x_ptr_end ||
            (*frame_coordinate_x_ptr == ' ' && *(frame_coordinate_x_ptr + 1) == ' ')) {
            return;
        }
        *(frame_coordinate_x_ptr++) = ' ';
    }
}

// needs better system for transporting words from UI's right end
char* TurnBasedGame::add_string_to_ui(FrameCoordinate coordinate, const std::string str, int indent = 0) {
    char* frame_coordinate_x_ptr = { frame_[coordinate.y].data() + coordinate.x + indent },
        * frame_coordinate_x_ptr_end{ frame_[coordinate.y].data() + ui_window_width_end_ };

    for (std::string::const_pointer str_ptr{ str.data() }, str_ptr_end{ str.data() + str.size() };
        str_ptr != str_ptr_end && frame_coordinate_x_ptr != frame_coordinate_x_ptr_end;
        ++frame_coordinate_x_ptr, ++str_ptr) {

        *frame_coordinate_x_ptr = *str_ptr;
    }
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_coordinate_x_ptr_end);
    }
    return frame_coordinate_x_ptr;
}

char* TurnBasedGame::add_string_to_ui(FrameCoordinate coordinate, const std::string* str, int indent = 0) {
    char* frame_coordinate_x_ptr = { frame_[coordinate.y].data() + coordinate.x + indent },
        * frame_coordinate_x_ptr_end{ frame_[coordinate.y].data() + ui_window_width_end_ };

    for (std::string::const_pointer str_ptr{ str->data() }, str_ptr_end{ str->data() + str->size() };
        str_ptr != str_ptr_end && frame_coordinate_x_ptr != frame_coordinate_x_ptr_end;
        ++frame_coordinate_x_ptr, ++str_ptr) {

        *frame_coordinate_x_ptr = *str_ptr;
    }
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_coordinate_x_ptr_end);
    }
    return frame_coordinate_x_ptr;
}

char* TurnBasedGame::add_string_to_ui(FrameCoordinate coordinate, const std::string_view* str, int indent = 0) {
    char* frame_coordinate_x_ptr = { frame_[coordinate.y].data() + coordinate.x + indent },
        * frame_coordinate_x_ptr_end{ frame_[coordinate.y].data() + ui_window_width_end_ };

    for (std::string::const_pointer str_ptr{ str->data() }, str_ptr_end{ str->data() + str->size() };
        str_ptr != str_ptr_end && frame_coordinate_x_ptr != frame_coordinate_x_ptr_end;
        ++frame_coordinate_x_ptr, ++str_ptr) {

        *frame_coordinate_x_ptr = *str_ptr;
    }
    if (*frame_coordinate_x_ptr != ' ') {
        frame_clear_string(frame_coordinate_x_ptr, frame_coordinate_x_ptr_end);
    }
    return frame_coordinate_x_ptr;
}

void TurnBasedGame::ui_input_help_turn_on() {
    if (ui_status[UI_Status::kCreatureStats]) { create_new_ui_window(); } // clear from ui stats, should be changed to more optimized variant

    const std::vector<std::pair<char, std::string>>* input_button_description{ user_input_database_get_all_description() };
    const std::pair<char, std::string>* input_button_description_ptr_begin{ input_button_description->data() },
        *input_button_description_ptr_end{ input_button_description_ptr_begin + input_button_description->size() };

    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width, ui_window_height_start_ + ui_visual_indent_height };

    for (; input_button_description_ptr_begin != input_button_description_ptr_end; ++input_button_description_ptr_begin, ++coordinate.y) {
        add_string_to_ui(coordinate, std::string{ input_button_description_ptr_begin->first } + " - " + input_button_description_ptr_begin->second);
        // for unknown reason without manual conversion char to string throws away info before description.second
    }

    add_string_to_ui_log("Turn in input help");
    ui_status[UI_Status::kUI_InputHelp] = true;
}

void TurnBasedGame::ui_input_help_turn_off() {
    create_new_ui_window(); // should be changed to more optimized variant

    add_string_to_ui_log("Turn off input help");
    ui_status[UI_Status::kUI_InputHelp] = false;

    battle_map_create_basic_ui();
    if (ui_status[UI_Status::kCreatureSelected]) {
        battle_map_create_basic_ui_with_creature();
        update_ui();
    }
}

void TurnBasedGame::ui_input_help_switch() {
    (!ui_status[UI_Status::kUI_InputHelp]) ? ui_input_help_turn_on() : ui_input_help_turn_off();
}

void TurnBasedGame::add_creature_stat_string_to_ui(FrameCoordinate coordinate, 
        CreatureStatId creature_stat, int stat_value_current, int stat_value_max = 0) {

    char* frame_coordinate_x_ptr = { frame_[coordinate.y].data() + coordinate.x
        + static_cast<int>(creature_database_get_stat_naming(creature_stat)->size()) + 2 }, // +2 for indent from stat naming to stat numeric values
        * frame_coordinate_x_ptr_end{ frame_[coordinate.y].data() + ui_window_width_end_ };

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

void TurnBasedGame::battle_map_create_basic_ui() {
    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width, ui_window_height_start_ + ui_visual_indent_height };

    for (int battle_tile_parameter_iter{}; battle_tile_parameter_iter != static_cast<int>(BattleTileParameters::kBattleTileParametersMax);
        ++battle_tile_parameter_iter, ++coordinate.y) {

        *(add_string_to_ui(coordinate,
           battle_tile_database_get_parameter_name(static_cast<BattleTileParameters>(battle_tile_parameter_iter)))) = ':';
    }
}

void TurnBasedGame::battle_map_create_basic_ui_with_creature() {
    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width,
            ui_window_height_start_ + ui_visual_indent_height + static_cast<int>(BattleTileParameters::kBattleTileParametersMax) };

    *(add_string_to_ui(coordinate, "Name")) = ':';
    ++coordinate.y;
    *(add_string_to_ui(coordinate, "Race")) = ':';
    ++coordinate.y;

    for (int creature_stat_iter{}, creature_stat_iter_max{ static_cast<int>(CreatureStatId::kCreatureStatMax) };
        creature_stat_iter != creature_stat_iter_max; ++creature_stat_iter, ++coordinate.y) {

        *(add_string_to_ui(coordinate,
            creature_database_get_stat_naming(static_cast<CreatureStatId>(creature_stat_iter)))) = ':';
    }
}

void TurnBasedGame::update_ui() { // maybe should save in memory previus selection coordinates for possible action skips
    BattleTile* target{ &(*battle_map_info_)[player_coordinate_selection_.y][player_coordinate_selection_.x] };
    FrameCoordinate coordinate{ ui_window_width_start_ + ui_visual_indent_width, ui_window_height_start_ + ui_visual_indent_height };

    // landscape
    add_string_to_ui(coordinate,
        &terrain_database_get_full_info(target->terrain_type_)->type_name_,
        static_cast<int>(battle_tile_database_get_parameter_name(BattleTileParameters::kLandscape)->size()) + 2);
    ++coordinate.y;

    // TO DO creature or landscape "image"?
    // creature ownership
    if (target->creature_ == nullptr) {
        add_string_to_ui(coordinate, "None",
            static_cast<int>(battle_tile_database_get_parameter_name(BattleTileParameters::kCreature)->size()) + 2);
        ++coordinate.y;

        if (ui_status[UI_Status::kCreatureStats]) {
            for (std::array<std::array<char, kWindowWidth_>, kWindowHeight_>::pointer frame_coordinate_y_ptr { frame_.data() + coordinate.y },
                frame_coordinate_y_ptr_end{ frame_.data() + ui_window_height_end_ };
                (*frame_coordinate_y_ptr)[coordinate.x] != ' ' && frame_coordinate_y_ptr != frame_coordinate_y_ptr_end; ++frame_coordinate_y_ptr) {

                frame_clear_string(frame_coordinate_y_ptr->data() + coordinate.x,
                    frame_coordinate_y_ptr->data() + ui_window_width_end_);
            }
            ui_status[UI_Status::kCreatureStats] = false;
        }
        return;
    }

    add_string_to_ui(coordinate,
        (target->creature_->get_army_id() == 0) ? "Player" : "Enemy",
        static_cast<int>(battle_tile_database_get_parameter_name(BattleTileParameters::kCreature)->size()) + 2);
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
        creature_database_get_race_naming(target->creature_->get_race()), 6);
    ++coordinate.y;

    // creature stats
    { 
        CreatureStat creature_stat;
        for (int stat_iter{}, stat_iter_end{ static_cast<int>(CreatureStatId::kCreatureStatMax) };
            stat_iter != stat_iter_end; ++stat_iter, ++coordinate.y) {
            creature_stat = target->creature_->get_certain_stat_current_and_max(static_cast<CreatureStatId>(stat_iter));

            add_creature_stat_string_to_ui(coordinate,
                static_cast<CreatureStatId>(stat_iter),
                creature_stat.current, (creature_stat.current != creature_stat.max) ? creature_stat.max : 0);
        }
    }
}

void TurnBasedGame::battle_map_clear_tile_from_creature_image(BattleMapCoordinate creature_coordinate) {
    FrameCoordinate tile_center_coordinate{ battle_map_find_tile_center_frame_coordinate(creature_coordinate) };

    std::array<char, kWindowWidth_>::pointer frame_coordinate_x_ptr{ 
        frame_[tile_center_coordinate.y].data() + tile_center_coordinate.x };

    *(frame_coordinate_x_ptr - 1) = ' ';
    *frame_coordinate_x_ptr = ' ';
    *(frame_coordinate_x_ptr + 1) = ' ';
}

