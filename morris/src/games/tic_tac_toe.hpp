#ifndef tic_tac_toe_hpp
#define tic_tac_toe_hpp

#include "simulation.hpp"

namespace boardgame {

struct TicTacToeMove {
	TicTacToeMove(int destination) :
		destination(destination) {}

	int destination = -1;
};

struct TicTacToeState {
    static const unsigned kBoardSize = 9;
    
    TicTacToeState(Player player) : player(player) {
		std::fill(board.begin(), board.end(), Player::kNone);
	}

	char PlayerToXO(Player player) {
		if (player == Player::kLeftPlayer) return 'X';
		else if (player == Player::kRightPlayer) return 'O';
		return '_';
	}

	void Print() {
		std::cout << "Board: \n";
		std::cout << PlayerToXO(board[0]) << " | " << PlayerToXO(board[1]) << " | " << PlayerToXO(board[2]) << '\n';
		std::cout << PlayerToXO(board[3]) << " | " << PlayerToXO(board[4]) << " | " << PlayerToXO(board[5]) << '\n';
		std::cout << PlayerToXO(board[6]) << " | " << PlayerToXO(board[7]) << " | " << PlayerToXO(board[8]) << '\n';
		std::cout << "Player: " << PlayerToXO(player) << '\n';
	}
    
    Player player;
	std::array<Player, kBoardSize> board;
};

class TicTacToe {
public:
    // returns whether the game is still on going and if not then who the winner is
    // winners include kLeftPlayer, kRightPlayer, and kNone
    static std::tuple<bool, Player> Winner(TicTacToeState const &state);

	// the function returns whether the game is going on and the value for the maximizing player
	// depth is optional, often winning sooner (smaller depth) has better value
	static std::tuple<bool, std::array<double, 2>> StateValue(TicTacToeState const &state, unsigned depth = 0);

    // finds all next possible moves 
	static std::vector<TicTacToeMove> ListMoves(TicTacToeState const &state);

    // get the next state using uniform random
    static TicTacToeState SimulationPolicy(TicTacToeState const &state, std::mt19937_64 &random_engine);

	// apply a move to a state
	static TicTacToeState ApplyMove(TicTacToeState const &state, TicTacToeMove const &move);

private:
    static const std::array<std::array<int, 3>, 8> kWinCombos;
};

} // namespace boardgame

#endif /* tic_tac_toe_hpp */


