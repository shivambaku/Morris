#include "../pch.hpp"
#include "tic_tac_toe.hpp"

namespace boardgame {

const std::array<std::array<int, 3>, 8> TicTacToe::kWinCombos = { {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    {0, 4, 8},
    {6, 4, 2}
} };

std::tuple<bool, Player> TicTacToe::Winner(TicTacToeState const &state) {

    for (unsigned i = 0; i < TicTacToe::kWinCombos.size(); ++i) {

        // first piece in the mill
        auto piece = static_cast<Player>(state.board[kWinCombos[i][0]]);

        // if the piece is empty then there cannot be a mill
        if (piece != Player::kNone) {

            // all the pieces in combo indices are the same, therefore somebody completed a mill
            auto mill = std::all_of(std::begin(kWinCombos[i]), std::end(kWinCombos[i]), [&state, piece] (int i) {
                return state.board[i] == piece;
            });

            // {game has ended, the player who won}
            if (mill) return {false, piece};
        }
    }

    // if there are still empty spots available then the game is ongoing
    auto on_going = std::any_of(std::begin(state.board), std::end(state.board), [] (Player player) {
        return player == Player::kNone;
    });

    // {whether the game is still ongoing, in either case nobody won}
    return { on_going, Player::kNone };
}
    
std::tuple<bool, std::array<double, 2>> TicTacToe::StateValue(TicTacToeState const &state, unsigned /*depth*/) {
    auto [on_going, winner] = Winner(state);

    if (winner == Player::kLeftPlayer) return { false, {1.0, 0.0} };
    else if (winner == Player::kRightPlayer) return { false, {0.0, 1.0} };

    // game ended in a draw or game has not ended yet
    return { on_going, {0.5, 0.5} };
}

TicTacToeState TicTacToe::ApplyMove(TicTacToeState const &state, TicTacToeMove const &move) {
    TicTacToeState next_state(state);
    next_state.player = Opponent(state.player);
    next_state.board[move.destination] = state.player;
    return next_state;
}

std::vector<TicTacToeMove> TicTacToe::ListMoves(TicTacToeState const &state) {
    std::vector<TicTacToeMove> moves;
    for (unsigned i = 0; i < TicTacToeState::kBoardSize; ++i) {
        if (state.board[i] == Player::kNone) {
            moves.emplace_back(i);
        }
    }
    return moves;
}

TicTacToeState TicTacToe::SimulationPolicy(TicTacToeState const &state, std::mt19937_64 &random_engine) {
    auto moves = ListMoves(state);
    std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.size() - 1);
    return ApplyMove(state, moves[moves_distribution(random_engine)]);
}

} // namespace boardgame


