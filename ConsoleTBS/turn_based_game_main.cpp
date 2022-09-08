#include "turn_based_game.h"

#include <iostream> // for cerr

#include "user_input.h"

void tbs::TurnBasedGame::start() {
    using namespace u_input;
    const std::bitset<u_input::kUserInputButtonMax> allowed_user_input_for_this_window( kExitID | kShowInputHelpID);

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
            ui_input_help_switch(allowed_user_input_for_this_window);
            new_frame = true;
            break;
        case UserInputButton::kStartBattle:
            ui_status.reset(UI_Status::kUI_InputHelp);
            start_new_battle();
            battle_process();
            battle_map_clear();
            ui_status.reset();
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