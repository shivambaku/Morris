#include "pch.hpp"
#include "games/simulation.hpp"
#include "games/tic_tac_toe.hpp"
#include "games/nine_men_morris.hpp"
#include "games/connect_4.hpp"
#include "algorithms/random_play.hpp"
#include "algorithms/min_max.hpp"
#include "algorithms/mcts.hpp"

using namespace boardgame;
using namespace algorithm;

template<class GameType, class StateType, class MoveType>
void Simulate(unsigned total_games = 100) {

    //auto l_algorithm = MinMax<GameType, StateType, MoveType>();
    //auto l_algorithm = RandomPlay<GameType, StateType>();
    auto l_algorithm = MonteCarloTreeSearch<GameType, StateType, 2>(1000);

    //auto r_algorithm = RandomPlay<GameType, StateType>();
    auto r_algorithm = MonteCarloTreeSearch<GameType, StateType, 2>(1000);

    auto initial_state = []() { return StateType(RandomPlayer()); };

    Simulation<GameType, StateType, MoveType, decltype(l_algorithm), decltype(r_algorithm)>
        simulation(initial_state(), l_algorithm, r_algorithm);

    size_t total_moves = 0;
    std::unordered_map<Player, unsigned> player_wins;
    for (unsigned i = 0; i < total_games; ++i) {
        simulation.Initialize(initial_state());
        auto winner = simulation.Run();
        player_wins[winner]++;
        total_moves += simulation.TotalMoves();
    }

    std::cout
        << "left player: " << player_wins[Player::kLeftPlayer] << ", "
        << "right player: " << player_wins[Player::kRightPlayer] << ", "
        << "draw: " << player_wins[Player::kNone] << '\n'
        << "total moves per game: " << total_moves / total_games << '\n';
}

int main(int /*argc*/, const char * /*argv*/[]) {

    // insert code here...
    std::cout << "Hello, World!\n";

    srand(static_cast<unsigned>(time(0)));

    auto start = std::chrono::high_resolution_clock::now();

    //Simulate<NineMenMorris, NineMenMorrisState, NineMenMorrisMove>(10);
    Simulate<TicTacToe, TicTacToeState, TicTacToeMove>(100);
    //Simulate<Connect4, Connect4State, Connect4Move>(100);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';

    std::cin.get();

    return 0;
}
