
#include <iostream>
#include "turn_based_game.h"
#include "turn_based_game_global.h"

int main()
{
    // std::ios_base::sync_with_stdio(false);

    TurnBasedGame* game = new TurnBasedGame;
    initialize_global_ptr_to_game_object(game);
    game->start();
    std::cout << "CLOSE GAME\n";
    delete game;
    return 0;
}
