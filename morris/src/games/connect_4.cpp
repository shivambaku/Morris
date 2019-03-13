#include "../pch.hpp"
#include "connect_4.hpp"

namespace boardgame {

void Connect4State::Print() {

	std::cout << "Board:\n";

	for (unsigned y = 0; y < Connect4State::kHeight; ++y) {
		for (unsigned x = 0; x < Connect4State::kWidth; ++x) {
			if (board[x][y] == Player::kNone) {
				std::cout << " 0 ";
			}
			else {
				std::cout << (board[x][y] == Player::kLeftPlayer ? " L " : " R ");
			}
		}
		std::cout << "\n";
	}
	for (unsigned i = 0; i < Connect4State::kWidth; ++i) {
		std::cout << "---";
	}
	std::cout << "\n";
	for (unsigned i = 0; i < Connect4State::kWidth; ++i) {
		std::cout << " " + std::to_string(i) + " ";
	}
	std::cout << "\n";

	std::cout << (player == Player::kLeftPlayer ? "Player: Left\n" : "Player: Right\n");
}

std::tuple<bool, Player> Connect4::Winner(Connect4State const & state) {

	unsigned end_x = Connect4State::kWidth - Connect4State::kConnectCount;
	unsigned end_y = Connect4State::kHeight - Connect4State::kConnectCount;

	// check horizontal
	for (unsigned y = 0; y < Connect4State::kHeight; ++y) {
		for (unsigned x = 0; x <= end_x; ++x) {
			bool all_same = true;
			Player piece = state.board[x][y];
			if (piece == Player::kNone) continue;

			for (unsigned i = 1; i < Connect4State::kConnectCount; ++i) {
				if (state.board[x + i][y] != piece) {
					all_same = false;
					break;
				}
			}
			if (all_same) {
				return { false, piece };
			}
		}
	}

	// check vertical
	for (unsigned x = 0; x < Connect4State::kWidth; ++x) {
		for (unsigned y = 0; y <= end_y; ++y) {
			bool all_same = true;
			Player piece = state.board[x][y];
			if (piece == Player::kNone) continue;

			for (unsigned i = 1; i < Connect4State::kConnectCount; ++i) {
				if (state.board[x][y + i] != piece) {
					all_same = false;
					break;
				}
			}
			if (all_same) {
				return { false, piece };
			}
		}
	}

	// check diagonal forward
	for (unsigned x = 0; x <= end_x; ++x) {
		for (unsigned y = 0; y <= end_y; ++y) {
			bool all_same = true;
			Player piece = state.board[x][y];
			if (piece == Player::kNone) continue;

			for (unsigned i = 1; i < Connect4State::kConnectCount; ++i) {
				if (state.board[x + i][y + i] != piece) {
					all_same = false;
					break;
				}
			}
			if (all_same) {
				return { false, piece };
			}
		}
	}

	// check diagonal backwards
	for (unsigned x = 0; x <= end_x; ++x) {
		for (unsigned y = 0; y <= end_y; ++y) {
			bool all_same = true;
			Player piece = state.board[x + Connect4State::kConnectCount - 1][y];
			if (piece == Player::kNone) continue;

			for (unsigned i = 0; i < Connect4State::kConnectCount - 1; ++i) {
				if (state.board[x + i][y + Connect4State::kConnectCount - 1 - i] != piece) {
					all_same = false;
					break;
				}
			}
			if (all_same) {
				return { false, piece };
			}
		}
	}

	for (unsigned x = 0; x < Connect4State::kWidth; ++x) {
		if (state.board[x][0] == Player::kNone) {
			return kOnGoingGame;
		}
	}

	return { false, Player::kNone };
}

std::tuple<bool, std::array<double, 2>> Connect4::StateValue(Connect4State const &state, unsigned /*depth*/) {
	auto[on_going, winner] = Winner(state);

	if (winner == Player::kLeftPlayer) return { false, {1.0, 0.0} };
	else if (winner == Player::kRightPlayer) return { false, {0.0, 1.0} };

	// game ended in a draw or game has not ended yet
	return { on_going, {0.5, 0.5} };
}

std::vector<Connect4Move> Connect4::ListMoves(Connect4State const & state) {
	std::vector<Connect4Move> moves;
	for (unsigned x = 0; x < Connect4State::kWidth; ++x) {
		if (state.board[x][0] == Player::kNone) {
			moves.emplace_back(x);
		}
	}
	return moves;
}

bool Connect4::IsValidMove(Connect4State const & state, Connect4Move const & move) {
	return move.location >= 0
		&& move.location < Connect4State::kWidth
		&& state.board[move.location][0] == Player::kNone;
}

Connect4State Connect4::ApplyMove(Connect4State const & state, Connect4Move const & move) {
	Connect4State next_state(state);

	// find the first empty position on the most bottom
	for (int y = Connect4State::kHeight - 1; y >= 0; --y) {
		if (next_state.board[move.location][y] == Player::kNone) {
			next_state.board[move.location][y] = state.player;
			break;
		}
	}
	next_state.player = Opponent(state.player);
	return next_state;
}

Connect4State Connect4::SimulationPolicy(Connect4State const & state, std::mt19937_64 &random_engine) {
	auto moves = ListMoves(state);
	std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.size() - 1);
	return ApplyMove(state, moves[moves_distribution(random_engine)]);
}

}
