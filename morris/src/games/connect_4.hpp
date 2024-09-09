#ifndef MORRIS_GAMES_CONNECT_4_HPP_
#define MORRIS_GAMES_CONNECT_4_HPP_

#include "simulation.hpp"

namespace boardgame {

struct Connect4Move {
    Connect4Move(unsigned int location) : location(location) {}
    unsigned int location;
};

struct Connect4State {
    static const unsigned kHeight = 7;
    static const unsigned kWidth = 6;
    static const unsigned kConnectCount = 4;

    Connect4State(Player player) : player(player) {
        for (unsigned x = 0; x < Connect4State::kWidth; ++x) {
            for (unsigned y = 0; y < Connect4State::kHeight; ++y) {
                board[x][y] = Player::kNone;
            }
        }
    }

    void Print();

    Player player;
    std::array<std::array<Player, kHeight>, kWidth> board;
};

class Connect4 {
public:
    static std::tuple<bool, Player> Winner(Connect4State const & state);
    static std::tuple<bool, std::array<double, 2>> StateValue(Connect4State const & state, unsigned depth = 0);
    static std::vector<Connect4Move> ListMoves(Connect4State const & state);
    static bool IsValidMove(Connect4State const & state, Connect4Move const & move);
    static Connect4State ApplyMove(Connect4State const & state, Connect4Move const & move);
    static Connect4State SimulationPolicy(Connect4State const & state, std::mt19937_64 &random_engine);
};

} // namespace boardgame

#endif /* MORRIS_GAMES_CONNECT_4_HPP_ */
