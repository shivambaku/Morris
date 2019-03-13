#include "../pch.hpp"
#include "nine_men_morris.hpp"

namespace boardgame {

// std::vector<std::array<unsigned, 3>> NineMenMorris::kMills = {
// 	{ 0, 1, 2 },
// 	{ 0, 9, 21 },
// 	{ 2, 14, 23 },
// 	{ 21, 22, 23 },
// 	{ 3, 4, 5 },
// 	{ 3, 10, 18 },
// 	{ 18, 19, 20 },
// 	{ 5, 13, 20 },
// 	{ 6, 7, 8 },
// 	{ 6, 11, 15 },
// 	{ 8, 12, 17 },
// 	{ 15, 16, 17 },
// 	{ 9, 10, 11 },
// 	{ 1, 4, 7 },
// 	{ 16, 19, 22 },
// 	{ 12, 13, 14 }
// };

std::vector<std::array<unsigned, 2>> NineMenMorris::kMills = {
	{ 1, 2 }, { 9, 21 },
	{ 0, 2 }, { 4, 7 },
	{ 0, 1 }, { 14, 23 },
	{ 4, 5 }, { 10, 18 },
	{ 3, 5 }, { 1, 7 },
	{ 13, 20 }, { 3, 4 },
	{ 7, 8 }, { 11, 15 },
	{ 6, 8 },  { 1, 4 },
	{ 12, 17 }, { 6, 7 },
	{ 10, 11 }, { 0, 21 },
	{ 9, 11 }, { 3, 18 },
	{ 9, 10 }, { 6, 15 },
	{ 13, 14 }, { 8, 17 },
	{ 12, 14 }, { 5, 20 },
	{ 12, 13}, { 2, 23 },
	{ 16, 17 }, { 6, 11 },
	{ 19, 22 }, { 15, 17 },
	{ 15, 16 }, { 8, 12 },
	{ 19, 20 }, { 3, 10 },
	{ 18, 20 }, { 16, 22 },
	{ 18, 19 }, { 5, 13 },
	{ 22, 23 }, { 0, 9 },
	{ 21, 23 }, { 16, 19 },
	{ 21, 22 }, { 2, 14 }
};

std::vector<std::vector<unsigned>> NineMenMorris::kNeighbors = {
	{ 1, 9 },
	{ 0, 2, 4 },
	{ 1, 14 },
	{ 4, 10 },
	{ 3, 5, 1, 7 },
	{ 4, 13 },
	{ 11, 7 },
	{ 4, 6, 8 },
	{ 7, 12 },
	{ 0, 21, 10 },
	{ 3, 9, 18, 11 },
	{ 6, 10, 15 },
	{ 8, 17, 13 },
	{ 5, 12, 20, 14 },
	{ 2, 13, 23 },
	{ 11, 16 },
	{ 15, 17, 19 },
	{ 12, 16 },
	{ 10, 19 },
	{ 16, 18, 22, 20 },
	{ 13, 19 },
	{ 9, 22 },
	{ 19, 21, 23 },
	{ 14, 22 }
};

NineMenMorrisState::Phase NineMenMorris::GetStage(NineMenMorrisState const & state, Player player) {
	// in free move stage
	if (state.Stage(player) == NineMenMorrisState::Phase::kMovement && state.Remaining(player) <= 3) {
		return NineMenMorrisState::Phase::kFreeMovement;
	}
	// in moving stage
	else if (state.RemainingToPlay(player) == 0) {
		return NineMenMorrisState::Phase::kMovement;
	}
	// in placing stage
	else {
		return NineMenMorrisState::Phase::kPlacement;
	}
}

bool NineMenMorris::PartOfAMill(NineMenMorrisState const & state, unsigned destination, Player player) {
	unsigned index = destination * 2;
	return (state.board[kMills[index][0]] == player && state.board[kMills[index][1]] == player) ||
		(state.board[kMills[index + 1][0]] == player && state.board[kMills[index + 1][1]] == player);
}

NineMenMorrisState NineMenMorris::ApplyMove(NineMenMorrisState const & state, NineMenMorrisMove const & move) {
	NineMenMorrisState next_state(state);

	next_state.player = Opponent(state.player);

	// moving the piece from source position
	if (move.source != -1) {
		next_state.board[move.source] = Player::kNone;
	}
	// the piece is a new piece
	else {
		next_state.SetRemainingToPlay(state.player, next_state.RemainingToPlay(state.player) - 1);
	}

	// place the piece at the destination position
	next_state.board[move.destination] = state.player;

	// if the move is removing opponent's piece at deletion position
	if (move.deletion != -1) {
		next_state.board[move.deletion] = Player::kNone;
		next_state.SetRemaining(next_state.player, next_state.Remaining(next_state.player) - 1);
	}

	next_state.SetStage(state.player, GetStage(next_state, state.player));
	next_state.SetStage(next_state.player, GetStage(next_state, next_state.player));

	return next_state;
}

void NineMenMorris::DeletionMoves(NineMenMorrisState const & state, int source, int destination, std::vector<NineMenMorrisMove> & moves) {
	auto move_without_deletion = NineMenMorrisMove(source, destination, -1);
	auto next_state = ApplyMove(state, move_without_deletion);

	// if the move forms a mill, then find all possible opponent's pieces that can be removed
	bool forms_a_mill = PartOfAMill(next_state, destination, state.player);
	if (forms_a_mill) {
		bool found_deletion_moves = false;

		for (unsigned i = 0; i < next_state.board.size(); ++i) {
			if (next_state.board[i] == next_state.player) {
				// can only remove opponent's piece if it is not already part of a mill
				if (!PartOfAMill(next_state, i, next_state.player)) {
					moves.emplace_back(source, destination, i);
					found_deletion_moves = true;
				}
			}
		}

		// if did not find any move that allowed for deletion, then make a move without deletion
		if (!found_deletion_moves) {
			moves.push_back(move_without_deletion);
		}
	}
	else {
		moves.push_back(move_without_deletion);
	}
}

std::vector<NineMenMorrisMove> NineMenMorris::PlacementMoves(NineMenMorrisState const & state) {
	std::vector<NineMenMorrisMove> moves;
	moves.reserve(64);

	// find an empty spot on the board
	for (unsigned i = 0; i < state.board.size(); ++i) {
		if (state.board[i] == Player::kNone) {
			DeletionMoves(state, -1, i, moves);
		}
	}
	return moves;
}

std::vector<NineMenMorrisMove> NineMenMorris::MovementMoves(NineMenMorrisState const & state) {
	std::vector<NineMenMorrisMove> moves;
	moves.reserve(64);

	// get all moves from player pieces to available neighbor spot
	for (unsigned i = 0; i < state.board.size(); ++i) {
		if (state.board[i] == state.player) {
			for (auto & neighbor : kNeighbors[i]) {
				if (state.board[neighbor] == Player::kNone) {
					DeletionMoves(state, i, neighbor, moves);
				}
			}
		}
	}
	return moves;
}

std::vector<NineMenMorrisMove> NineMenMorris::FreeMovementMoves(NineMenMorrisState const & state) {
	std::vector<NineMenMorrisMove> moves;
	moves.reserve(64);

	// get all moves from all player pieces to available empty spots
	for (unsigned i = 0; i < state.board.size(); ++i) {
		if (state.board[i] == state.player) {
			for (unsigned j = 0; j < state.board.size(); ++j) {
				if (state.board[j] == Player::kNone) {
					DeletionMoves(state, i, j, moves);
				}
			}
		}
	}

	return moves;
}

std::vector<NineMenMorrisMove> NineMenMorris::ListMoves(NineMenMorrisState const & state) {
	auto player_stage = state.Stage(state.player);
	switch (player_stage) {
		case NineMenMorrisState::Phase::kFreeMovement: return FreeMovementMoves(state);
		case NineMenMorrisState::Phase::kMovement: return MovementMoves(state);
		default: return PlacementMoves(state);
	}
}

std::tuple<bool, Player> NineMenMorris::Winner(NineMenMorrisState const & state) {
	// if there are less than three pieces left for either player
	if (state.Remaining(Player::kLeftPlayer) < 3) {
		return { false, Player::kRightPlayer };
	} else if (state.Remaining(Player::kRightPlayer) < 3) {
		return { false, Player::kLeftPlayer };
	}

	// secondary win conditions
	if (state.Stage(state.player) == NineMenMorrisState::Phase::kMovement) {

		// check if there is an available move left for the current player
		for (unsigned i = 0; i < state.board.size(); ++i) {
			if (state.board[i] == state.player) {
				for (auto & neighbor : kNeighbors[i]) {
					if (state.board[neighbor] == Player::kNone) {
						return kOnGoingGame;
					}
				}
			}
		}
		// no more moves available for the current player, so the opponent wins
		return { false, Opponent(state.player) };
	}

	// the game is going on, nobody has won yet
	return kOnGoingGame;
}

std::tuple<bool, std::array<double, 2>> NineMenMorris::StateValue(NineMenMorrisState const & state, unsigned /*depth*/) {
	auto [on_going, winner] = Winner(state);

	if (winner == Player::kLeftPlayer) return { false, {1.0, 0.0} };
	else if (winner == Player::kRightPlayer) return { false, {0.0, 1.0} };

	// game ended in a draw or game has not ended yet
	return {on_going, {0.5, 0.5}};
}

NineMenMorrisState NineMenMorris::SimulationPolicy(NineMenMorrisState const & state, std::mt19937_64 & random_engine) {
	auto moves = ListMoves(state);

	std::uniform_int_distribution<std::size_t> moves_distribution(0, moves.size() - 1);

	auto best_move = moves[moves_distribution(random_engine)];

	return ApplyMove(state, best_move);
}

} // namespace boardgame
