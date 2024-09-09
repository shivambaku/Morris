#ifndef simulation_hpp
#define simulation_hpp

namespace boardgame {

enum class Player {
    kLeftPlayer = 0,
    kRightPlayer = 1,
    kNone = 2
};

// {game is ongoing, nobody won}
const auto kOnGoingGame = std::make_tuple(true, Player::kNone);

static Player Opponent(Player player) {
    return player == Player::kLeftPlayer ? Player::kRightPlayer : Player::kLeftPlayer;
}

static Player RandomPlayer() {
    return (std::rand() % 2) == 0 ? Player::kLeftPlayer : Player::kRightPlayer;
}

template<class GameType, class StateType, class MoveType, class LAlgorithmType, class RAlgorithmType>
class Simulation {
public:

    Simulation(StateType initial_state, LAlgorithmType l_algorithm, RAlgorithmType r_algorithm)
    : state_(initial_state), l_algorithm_(l_algorithm), r_algorithm_(r_algorithm) {
        history_ = std::vector<StateType> { state_ };
    }

    // initialize is separate as same simulation can be reused with different states
    // which is useful to test an algorithm's efficiency over multiple rounds of games
    // ie: some algorithms might improve each game
    void Initialize(StateType initial_state) {
        state_ = initial_state;
        history_ = std::vector<StateType> { state_ };
    }

    Player Run() {
        std::tuple<bool, Player> winner;
        while ((winner = Move()) == kOnGoingGame) continue;
        return std::get<1>(winner);
    }

    std::tuple<bool, Player> Move(MoveType const & move) {
        state_ = GameType::ApplyMove(state_, move);
        history_.push_back(state_);
        return GameType::Winner(state_);
    }

    std::tuple<bool, Player> Move() {

        // given the current state, play a move and get a new state
        state_ = state_.player == Player::kLeftPlayer ? l_algorithm_.Compute(state_) : r_algorithm_.Compute(state_);
        history_.push_back(state_);
        //state_.Print();
        return GameType::Winner(state_);
    }

    size_t TotalMoves() {
        return history_.size();
    }

    StateType State() {
        return state_;
    }
private:
    StateType state_;
    LAlgorithmType l_algorithm_;
    RAlgorithmType r_algorithm_;
    std::vector<StateType> history_;
};

} // namespace boardgame

#endif /* simulation_hpp */


