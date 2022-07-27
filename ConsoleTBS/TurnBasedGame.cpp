
#include <iostream>
#include "turn_based_game.h"
#include "turn_based_game_global.h"
#include "logger.h"

int main()
{
    // std::ios_base::sync_with_stdio(false);
    std::chrono::time_point time_start{ std::chrono::high_resolution_clock::now() };

    TurnBasedGame* game = new TurnBasedGame;

    //std::chrono::time_point time_end{ std::chrono::high_resolution_clock::now() };
    //std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    //std::cout << "Execution time: " << duration.count() << "ms.\n";

    initialize_global_ptr_to_game_object(game);
    game->start();
    log_in_file("Close game");
    delete game;
    return 0;
}
