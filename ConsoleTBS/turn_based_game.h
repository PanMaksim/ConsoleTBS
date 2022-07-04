#pragma once

#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <utility>

#include <chrono>

#include "user_input.h"
#include "battle_map_coordinate.h"
#include "frame_coordinate.h"
#include "random.h"
#include "user_input.h"
#include "terrain.h"
#include "battle_tile.h"
#include "creature.h"
#include "army.h"

/*
    ui - user interface
    pv - player view
*/

//enum class BattleMapSize {
//    SMALL = 0,
//    MEDIUM,
//    LARGE
//};

class TurnBasedGame {
public:

    TurnBasedGame() {
        create_new_main_game_window();
        calculate_window_borders();
        create_new_ui_window(); // currently at first launch doing only unneeded clearing
        //create_new_pv_window(); // currently at first launch doing only unneeded clearing
        ui_status[UI_Status::kPlayerViewWindow] = true;
    }

    ~TurnBasedGame() {
        battle_map_info_.reset();
    }

    void print_frame();

    void start() {
        const std::vector<UserInputButton> allowed_user_input { // unoptimized because will hold allowed input for main_menu when there is currently battle in proccess
            UserInputButton::kExit
        };

        std::string ask_user_input_str((kWindowWidth_ / 2) - 6, ' ');
        ask_user_input_str += " User input: ";

        bool new_frame{ true };
        char user_input{};
        do {
            if (new_frame) { 
                print_frame(); 
                clear_ui_log();
            }
            else { new_frame = true; }

            std::cout << ask_user_input_str;
            std::cin >> user_input;
            player_coordinate_selection_old_ = player_coordinate_selection_;
            switch (user_input) {
            case UserInputButton::kShowInputHelp:
                ui_input_help_switch(allowed_user_input);
                new_frame = true;
                break;
            case UserInputButton::kStartBattle: // should start another switch with another input switch to prevent improper input
                start_new_battle();
                battle_process();
                battle_map_clear();
                set_ui_status_flags_to_default();
                create_new_ui_window();
                create_new_pv_window();
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
            //direction_input.release();
            std::cout << std::endl;
        } while (true);
    }

    friend void add_string_to_ui_log(const std::string* str);
    friend void add_string_to_ui_log(const std::string str);

private:
    enum UI_Status {
        kWindowsAndInterfacesStatusMin,
        kPlayerViewWindow = kWindowsAndInterfacesStatusMin,
        kUI_Window,
        kUI_WindowLog,
        kUI_InputHelp,
        kBattleMap,
        kBattleMapTileNumeration,
        kCreatureStats,
        kCreatureSelected, // for situations when something will be shown over creature stats
        // kCreatureInteracted, // for future, example when interaction creates new "windows"
        kWindowsAndInterfacesStatusMax,
    };

    void create_new_main_game_window();
    void create_new_ui_window();
    void create_new_pv_window();

    void set_ui_status_flags_to_default();
    void calculate_window_borders();
    void frame_clear_string(std::string::iterator frame_coordinate_x_ptr, std::string::iterator frame_coordinate_x_ptr_end);

    void update_ui();
    std::string::iterator add_string_to_ui(FrameCoordinate coordinate, const std::string&& str, int indent);
    std::string::iterator add_string_to_ui(FrameCoordinate coordinate, const std::string* str, int indent);
    std::string::iterator add_string_to_ui(FrameCoordinate coordinate, const std::string_view* str, int indent);
    std::string::iterator add_string_to_ui(FrameCoordinate coordinate, const UserInputDescription* user_input_description);
    void add_creature_stat_string_to_ui(FrameCoordinate coordinate, CreatureStatId creature_stat, int stat_value_current, int stat_value_max);
    void ui_input_help_switch(const std::vector<UserInputButton>& allowed_user_input);
    void ui_input_help_turn_on(const std::vector<UserInputButton>& allowed_user_input);
    void ui_input_help_turn_off(size_t allowed_user_input_size);
    void clear_ui_log();

    void start_new_battle();
    void battle_process();

    void battle_map_clear();
    void generate_new_battle_map();

    void calculate_battle_map_visual();
    void show_battle_map();
    void battle_map_show_landscape();
    void battle_map_add_creature(Creature* creature, BattleMapCoordinate coordinate, BattleStartStatus battle_reason);
    void battle_map_add_army(Army* army, BattleStartStatus status);
    bool battle_map_tile_numeration_switch();
    bool battle_map_tile_numeration_turn_on();
    bool battle_map_tile_numeration_turn_off();
    void battle_map_create_basic_ui();
    void battle_map_create_basic_ui_with_creature();
    void battle_map_clear_tile_from_creature_image(BattleMapCoordinate creature_coordinate);
    void battle_map_kill_creature(BattleMapCoordinate killed_creature_coordinate);

    FrameCoordinate battle_map_find_tile_center_frame_coordinate(BattleMapCoordinate coordinate); // returns frame_[y,x] where creature_middle_symbol would be

    void battle_map_update_player_selection();
    void battle_map_clear_old_player_selection();

    bool player_coordinate_selection_move_by_coordinate_input();
    std::unique_ptr<std::vector<UserInputButton>> player_coordinate_selection_move_by_direction_input();

    bool interact_with_creature();
    bool creature_move_by_input(UserInputButton input_method);
    bool move_creature_by_coordinate(BattleMapCoordinate old_coordinate, BattleMapCoordinate new_coordinate);

    void check_possible_kill(BattleMapCoordinate creature_coordinate);

private:
    //static constexpr int kWindowWidth_{ 317 },  // two strings are not included in height: first positioned below window frame and used for user input, second positioned above window frame and used for commenting what is done by user input
    //                    kWindowHeight_{ 82 };
    static constexpr int kWindowWidth_{ 264 },  // tmp value for laptop
                         kWindowHeight_{ 66 };

    std::array<std::string, kWindowHeight_> frame_;
    // when accessing it frame[y][x]

    static constexpr char kGameWindowVerticalSymbol_{ '|' },
        kGameWindowHorizontalSymbol_{ '_' };

    static constexpr int kUserInterfaceLogWindowHeight_{ 14 };
    int ui_log_window_height_current_{ 1 };

    int pv_window_height_start_, // they are used for navigation through std::array frame, not showing actual interface window height/width
        pv_window_height_end_,
        pv_window_width_start_,
        pv_window_width_end_;

    int ui_window_height_start_,
        ui_window_height_end_,
        ui_window_width_start_,
        ui_window_width_end_;
    int ui_log_window_height_start_;

    std::array<bool, UI_Status::kWindowsAndInterfacesStatusMax> ui_status{ false }; // represent opened windows/interfaces

    std::unique_ptr<std::vector<std::vector<BattleTile>>> battle_map_info_ = nullptr; // used vector not array, cause for possibility in future to make dynamic map sizes (change it at generation or maybe even growing in started battle)
    // (*battle_map_info_)[y][x]

    static constexpr int kBattleMapSizeHeight_{ 7 }, // tmp value for laptop
        kBattleMapSizeWidth_{ 16 };
    //static constexpr int kBattleMapSizeHeight_{ 9 }, // size in tiles
    //    kBattleMapSizeWidth_{ 19 };

    int pv_visual_indent_height_{}, // for output at center of player view
        pv_visual_indent_width_{},
        ui_visual_indent_height{ 2 },
        ui_visual_indent_width{ 3 };


    static constexpr int kTileVisualHeight_{ 8 }, // length of each tile border, one tile in 'y' makes 10 symbols 1 horizontal (top) + 9 vertical (9 = 8 (left/right) + 1 (bottom))
        kTileVisualWidth_{ 12 };

    static constexpr char kTileHorizontalSymbol_{ '_' },
        kTileVerticalSymbol_{ '|' },
        kTileCornerSymbol1_{ '/' },
        kTileCornerSymbol2_{ '\\' };


    Army player_army_,
        ai_army_;

    static constexpr char kCreatureMiddleSymbol_{ '-' },
        kCreatureBackSymbol_{ 'I' },
        kCreaturePlayerHeadSymbol_{ '0' },
        kCreatureEnemyHeadSymbol_{ 'X' };

    BattleMapCoordinate player_coordinate_selection_{ 0, 0 },
                        player_coordinate_selection_old_{ 0, 0 };

    static constexpr int kPlayerSelectionVisualHeight_{ 4 }, // must % 2 == 0
                         kPlayerSelectionVisualWidth_{ 5 }; // must % 2 == 1 for 1 symbol at level of creature_middle_symbol

    // if map will be bigger than player_view
    //std::vector<std::vector<char>>* battle_map_visual;
};
