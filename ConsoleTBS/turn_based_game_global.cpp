
#include "turn_based_game_global.h"

#include "turn_based_game.h"
#include "logger.h"

TurnBasedGame* game_ptr; // for log strings

void initialize_global_ptr_to_game_object(TurnBasedGame* game_ptr_tmp) {
	game_ptr = game_ptr_tmp;
}

void add_string_to_ui_log(const std::string* str_ptr) {
    if (game_ptr->ui_log_window_height_current_ == game_ptr->kUserInterfaceLogWindowHeight_) {
#ifdef debug_log
        log_in_file("ERROR, log is too big.", true); // lazy check
#endif
    } // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! but when log message take more than 1 string - UB !!!!!!!!!!!!!!!!!!! needs fix

    std::string::const_pointer str_const_ptr{ str_ptr->data() },
        str_const_ptr_end{ str_const_ptr + str_ptr->size() };

    std::string::pointer frame_coordinate_x_ptr,
        frame_coordinate_x_ptr_end;

    for (std::array<std::string, game_ptr->kWindowHeight_>::pointer frame_coordinate_y_ptr{
        game_ptr->frame_.data() + game_ptr->ui_log_window_height_start_ + game_ptr->ui_log_window_height_current_ };

        str_const_ptr != str_const_ptr_end && game_ptr->ui_log_window_height_current_ != game_ptr->kUserInterfaceLogWindowHeight_;
        ++game_ptr->ui_log_window_height_current_, ++frame_coordinate_y_ptr) {

        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + game_ptr->ui_window_width_start_ + game_ptr->ui_visual_indent_width;
        frame_coordinate_x_ptr_end = frame_coordinate_y_ptr->data() + game_ptr->ui_window_width_end_ - (game_ptr->ui_visual_indent_width - 1);

        for (; str_const_ptr != str_const_ptr_end && frame_coordinate_x_ptr != frame_coordinate_x_ptr_end; ++str_const_ptr, ++frame_coordinate_x_ptr) {

            *frame_coordinate_x_ptr = *str_const_ptr;
        }

        if (str_const_ptr != str_const_ptr_end) {
            *frame_coordinate_x_ptr = '-';
        }
    }
}

void add_string_to_ui_log(const std::string str) {
#ifdef debug_log
    log_in_file("UI_Log: " + str);
#endif
    if (game_ptr->ui_log_window_height_current_ == game_ptr->kUserInterfaceLogWindowHeight_) {
#ifdef debug_log
        log_in_file("ERROR, log is too big.", true); // lazy check
#endif
    }

    std::string::const_pointer str_ptr{ str.data() },
        str_ptr_end{ str_ptr + str.size() };

    std::string::pointer frame_coordinate_x_ptr,
        frame_coordinate_x_ptr_end;

    for (std::array<std::string, game_ptr->kWindowHeight_>::pointer frame_coordinate_y_ptr{
        game_ptr->frame_.data() + game_ptr->ui_log_window_height_start_ + game_ptr->ui_log_window_height_current_ };

        str_ptr != str_ptr_end && game_ptr->ui_log_window_height_current_ != game_ptr->kUserInterfaceLogWindowHeight_;
        ++game_ptr->ui_log_window_height_current_, ++frame_coordinate_y_ptr) {

        frame_coordinate_x_ptr = frame_coordinate_y_ptr->data() + game_ptr->ui_window_width_start_ + game_ptr->ui_visual_indent_width;
        frame_coordinate_x_ptr_end = frame_coordinate_y_ptr->data() + game_ptr->ui_window_width_end_ - (game_ptr->ui_visual_indent_width - 1);

        for (; str_ptr != str_ptr_end && frame_coordinate_x_ptr != frame_coordinate_x_ptr_end; ++str_ptr, ++frame_coordinate_x_ptr) {

            *frame_coordinate_x_ptr = *str_ptr;
        }

        if (str_ptr != str_ptr_end) {
            *frame_coordinate_x_ptr = '-';
        }
    }
}