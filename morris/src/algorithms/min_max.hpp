#ifndef MORRIS_ALGORITHMS_MIN_MAX_HPP_
#define MORRIS_ALGORITHMS_MIN_MAX_HPP_

template<class GameType, class StateType, class MoveType>
class MinMax {
public:
	MinMax(unsigned max_depth = std::numeric_limits<unsigned>::max()) : max_depth_(max_depth) {
	}

	StateType Compute(StateType const & state) {
		auto state_and_value = Compute(state, static_cast<unsigned>(state.player), -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 0);
		return std::get<0>(state_and_value);
	}

private:
	std::tuple<StateType, double> Compute(StateType const & state, unsigned maximizing_player, double alpha, double beta, unsigned depth) {

		std::tuple<bool, std::array<double, 2>> state_value = GameType::StateValue(state, depth);
		bool on_going = std::get<0>(state_value);
		double value = std::get<1>(state_value)[static_cast<unsigned>(maximizing_player)];

		// base case, the game has ended
		if (!on_going) {
			return { state, value };
		}

		// for early termination, return whatever the current state value is
		if (depth >= max_depth_) {
			return { state, value };
		}

		double best_value;
		StateType best_state;

		// expand the game tree given all the next possible states
		std::vector<MoveType> moves = GameType::ListMoves(state);

		if (static_cast<unsigned>(state.player) == maximizing_player) {
			best_value = -std::numeric_limits<double>::max();
			for (auto & move : moves) {
				auto next_state = GameType::ApplyMove(state, move);
				value = std::get<1>(Compute(next_state, maximizing_player, alpha, beta, depth + 1));
				if (value > best_value) {
					best_value = value;
					best_state = next_state;
				}
				alpha = std::max(alpha, best_value);
				if (alpha >= beta) break;
			}
		}
		else {
			best_value = std::numeric_limits<double>::max();
			for (auto & move : moves) {
				auto next_state = GameType::ApplyMove(state, move);
				value = std::get<1>(Compute(next_state, maximizing_player, alpha, beta, depth + 1));
				if (value < best_value) {
					best_value = value;
					best_state = next_state;
				}
				beta = std::min(beta, best_value);
				if (alpha >= beta) break;
			}
		}
		return { best_state, best_value };
	}

	unsigned max_depth_;
};

#endif /* MORRIS_ALGORITHMS_MIN_MAX_HPP_ */


